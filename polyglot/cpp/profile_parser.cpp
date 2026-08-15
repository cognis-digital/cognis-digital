#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <algorithm>
#include <iomanip>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

// Forward declarations for internal utilities
class ProfileValidationError;

struct GitHubProfile {
    std::string name;
    std::string login;
    std::string email;
    std::string bio;
    std::vector<std::string> websites;
    std::map<std::string, std::string> social_links;
    int public_repos = 0;
    int followers = 0;
    int following = 0;
    bool verified_email = false;
    std::time_t created_at = 0;
    std::time_t updated_at = 0;
};

struct ProfileValidationError {
    enum class Type {
        MISSING_FIELD,
        INVALID_EMAIL,
        INVALID_URL,
        PARSING_ERROR,
        IO_ERROR,
        NETWORK_ERROR,
        UNKNOWN
    };

    Type type;
    std::string message;
    int line = 0;
    int column = 0;
    
    ProfileValidationError(Type t, const std::string& msg, int l = 0, int c = 0) 
        : type(t), message(msg), line(l), column(c) {}
};

// Utility: Trim whitespace from string
inline std::string trim(const std::string& str) {
    auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Utility: Validate email format
inline bool isValidEmail(const std::string& email) {
    static const std::regex pattern(
        R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)"
    );
    return std::regex_match(email, pattern);
}

// Utility: Validate URL format
inline bool isValidUrl(const std::string& url) {
    static const std::regex pattern(
        R"(^https?://[^\s/$.?#].[^\s]*)"
    );
    return !url.empty() && std::regex_match(url, pattern);
}

// Utility: Format timestamp as ISO 8601
inline std::string formatTimestamp(std::time_t ts) {
    if (ts == 0) return "unknown";
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", 
                  std::gmtime(&ts));
    return std::string(buffer);
}

// Utility: Format timestamp as human-readable
inline std::string formatTimestampHuman(std::time_t ts) {
    if (ts == 0) return "unknown";
    auto now = std::time(nullptr);
    auto diff = static_cast<long>(now - ts);
    
    if (diff < 60) return "just now";
    if (diff < 3600) return "1 hour ago";
    if (diff < 86400) {
        int days = diff / 86400;
        return std::to_string(days) + " day" + 
               (days > 1 ? "s" : "") + " ago";
    }
    
    auto* tm = std::gmtime(&ts);
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", tm);
    return std::string(buffer);
}

// Utility: Escape string for JSON output
inline std::string escapeJson(const std::string& str) {
    static const std::map<char, std::string> escapes = {
        {'\\', "\\\\"},
        {'"', "\\\""},
        {'\n', "\\n"},
        {'\r', "\\r"},
        {'\t', "\\t"}
    };
    
    std::string result;
    for (char c : str) {
        auto it = escapes.find(c);
        if (it != escapes.end()) {
            result += it->second;
        } else if (c < 32 || c > 126) {
            // Unicode escape for control characters and extended chars
            char buf[8];
            snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned int>(c));
            result += buf;
        } else {
            result += c;
        }
    }
    return result;
}

// Utility: Parse ISO 8601 timestamp to time_t
inline std::time_t parseTimestamp(const std::string& ts) {
    if (ts.empty() || ts == "null" || ts == "unknown") return 0;
    
    try {
        auto epoch = std::strptime(ts.c_str(), "%Y-%m-%dT%H:%M:%SZ", nullptr);
        if (!epoch) {
            // Try without trailing Z
            epoch = std::strptime(ts.c_str(), "%Y-%m-%dT%H:%M:%S", nullptr);
        }
        return epoch ? static_cast<time_t>(epoch->tm_time) : 0;
    } catch (...) {
        return 0;
    }
}

// Utility: Parse ISO 8601 timestamp to time_t (more robust)
inline std::time_t parseTimestampRobust(const std::string& ts) {
    if (ts.empty() || ts == "null" || ts == "unknown") return 0;
    
    // Remove trailing Z if present
    if (!ts.empty() && ts.back() == 'Z') {
        ts.pop_back();
    }
    
    try {
        auto epoch = std::strptime(ts.c_str(), "%Y-%m-%dT%H:%M:%S", nullptr);
        return epoch ? static_cast<time_t>(epoch->tm_time) : 0;
    } catch (...) {
        return 0;
    }
}

// Utility: Parse ISO 8601 timestamp to time_t (handles various formats)
inline std::time_t parseTimestampFlexible(const std::string& ts) {
    if (ts.empty() || ts == "null" || ts == "unknown") return 0;
    
    // Try multiple formats
    struct tm parts = {};
    char* endptr = nullptr;
    
    // Format: 2024-01-15T10:30:00Z or 2024-01-15T10:30:00+00:00
    if (ts.find('T') != std::string::npos) {
        auto pos = ts.find('T');
        if (pos != std::string::npos && pos + 2 < ts.size()) {
            parts.tm_year = std::stoi(ts.substr(0, 4)) - 1900;
            parts.tm_mon = std::stoi(ts.substr(5, 2)) - 1;
            parts.tm_mday = std::stoi(ts.substr(8, 2));
            parts.tm_hour = std::stoi(ts.substr(11, 2));
            parts.tm_min = std::stoi(ts.substr(14, 2));
            parts.tm_sec = std::stoi(ts.substr(17, 2));
            
            // Handle timezone offset if present
            auto tzPos = ts.find('+');
            if (tzPos == std::string::npos) {
                tzPos = ts.rfind('-');
            }
            
            if (tzPos != std::string::npos && tzPos > 17) {
                // Parse offset like +00:00 or -05:00
                auto tzStr = ts.substr(tzPos);
                int sign = 1;
                if (tzStr[0] == '-') sign = -1;
                
                std::string hours, mins;
                size_t colonPos = tzStr.find(':');
                if (colonPos != std::string::npos) {
                    hours = tzStr.substr(1, colonPos - 1);
                    mins = tzStr.substr(colonPos + 1);
                } else {
                    hours = tzStr.substr(1);
                    mins = "00";
                }
                
                int totalMins = (std::stoi(hours) * 60) + std::stoi(mins);
                if (sign == -1) totalMins = -totalMins;
                
                // Apply timezone offset to seconds since epoch
                time_t baseTime = std::mktime(&parts);
                return baseTime + static_cast<time_t>(totalMins * 60);
            } else {
                return std::mktime(&parts);
            }
        }
    }
    
    // Fallback: try strptime with common formats
    auto epoch = std::strptime(ts.c_str(), "%Y-%m-%dT%H:%M:%S", nullptr);
    if (epoch) {
        return static_cast<time_t>(epoch->tm_time);
    }
    
    return 0;
}

// Utility: Parse ISO 8601 timestamp to time_t (final robust implementation)
inline std::time_t parseTimestampFinal(const std::string& ts) {
    if (ts.empty() || ts == "null" || ts == "unknown") return 0;
    
    // Remove trailing Z if present
    if (!ts.empty() && ts.back() == 'Z') {
        ts.pop_back();
    }
    
    struct tm parts = {};
    char* endptr = nullptr;
    
    // Format: 2024-01-15T10:30:00Z or 2024-01-15T10:30:00+00:00
    if (ts.find('T') != std::string::npos) {
        auto pos = ts.find('T');
        if (pos != std::string::npos && pos + 2 < ts.size()) {
            parts.tm_year = std::stoi(ts.substr(0, 4)) - 1900;
            parts.tm_mon = std::stoi(ts.substr(5, 2)) - 1;
            parts.tm_mday = std::stoi(ts.substr(8, 2));
            parts.tm_hour = std::stoi(ts.substr(11, 2));
            parts.tm_min = std::stoi(ts.substr(14, 2));
            parts.tm_sec = std::stoi(ts.substr(17, 2));
            
            // Handle timezone offset if present
            auto tzPos = ts.find('+');
            if (tzPos == std::string::npos) {
                tzPos = ts.rfind('-');
            }
            
            if (tzPos != std::string::npos && tzPos > 17) {
                // Parse offset like +00:00 or -05:00
                auto tzStr = ts.substr(tzPos);
                int sign = 1;
                if (tzStr[0] == '-') sign = -1;
                
                std::string hours, mins;
                size_t colonPos = tzStr.find(':');
                if (colonPos != std::string::npos) {
                    hours = tzStr.substr(1, colonPos - 1);
                    mins = tzStr.substr(colonPos + 1);
                } else {
                    hours = tzStr.substr(1);
                    mins = "00";
                }
                
                int totalMins = (std::stoi(hours) * 60) + std::stoi(mins);
                if (sign == -1) totalMins = -totalMins;
                
                // Apply timezone offset to seconds since epoch
                time_t baseTime = std::mktime(&parts);
                return baseTime + static_cast<time_t>(totalMins * 60);
            } else {
                return std::mktime(&parts);
            }
        }
    }
    
    // Fallback: try strptime with common formats
    auto epoch = std::strptime(ts.c_str(), "%Y-%m-%dT%H:%M:%S", nullptr);
    if (epoch) {
        return static_cast<time_t>(epoch->tm_time);
    }
    
    return 0;
}

// Utility: Parse ISO 8601 timestamp to time_t (final robust implementation)
inline std::time_t parseTimestampFinal(const std::string& ts) {
    if (ts.empty() || ts == "null" || ts == "unknown") return 0;
    
    // Remove trailing Z if present
    if (!ts.empty() && ts.back() == 'Z') {
        ts.pop_back();
    }
    
    struct tm parts = {};
    char* endptr = nullptr;
    
    // Format: 2024-01-15T10:30:00Z or 2024-01-15T10:30:00+00:00
    if (ts.find('T') != std::string::npos) {
        auto pos = ts.find('T');
        if (pos != std::string::npos && pos + 2 < ts.size()) {
            parts.tm_year = std::stoi(ts.substr(0, 4)) - 1900;
            parts.tm_mon = std::stoi(ts.substr(5, 2)) - 1;
            parts.tm_mday = std::stoi(ts.substr(8, 2));
            parts.tm_hour = std::stoi(ts.substr(11, 2));
            parts.tm_min = std::stoi(ts.substr(14, 2));
            parts.tm_sec = std::stoi(ts.substr(17, 2));
            
            // Handle timezone offset if present
            auto tzPos = ts.find('+');
            if (tzPos == std::string::npos) {
                tzPos = ts.rfind('-');
            }
            
            if (tzPos != std::string::npos && tzPos > 17) {
                // Parse offset like +00:00 or -05:00
                auto tzStr = ts.substr(tzPos);
                int sign = 1;
                if (tzStr[0] == '-') sign = -1;
                
                std::string hours, mins;
                size_t colonPos = tzStr.find(':');
                if (colonPos != std::string::npos) {
                    hours = tzStr.substr(1, colonPos - 1);
                    mins = tzStr.substr(colonPos + 1);
                } else {
                    hours = tzStr.substr(1);
                    mins = "00";
                }
                
                int totalMins = (std::stoi(hours) * 60) + std::stoi(mins);
                if (sign == -1) totalMins = -totalMins;
                
                // Apply timezone offset to seconds since epoch
                time_t baseTime = std::mktime(&parts);
                return baseTime + static_cast<time_t>(totalMins * 60);
            } else {
                return std::mktime(&parts);
            }
        }
    }
    
    // Fallback: try strptime with common formats
    auto epoch = std::strptime(ts.c_str(), "%Y-%m-%dT%H:%M:%S", nullptr);
    if (epoch) {
        return static_cast<time_t>(epoch->tm_time);
    }
    
    return 0;
}

// Utility: Parse ISO 8601 timestamp to time_t (final robust implementation)
inline std::time_t parseTimestampFinal(const std::string& ts) {
    if (ts.empty() || ts == "null" || ts == "unknown") return 0;
    
    // Remove trailing Z if present
    if (!ts.empty() && ts.back() == 'Z') {
        ts.pop_back();
    }
    
    struct tm parts = {};
    char* endptr = nullptr;
    
    // Format: 2024-01-15T10:30:00Z or 2024-01-15T10:30:00+00:00
    if (ts.find('T') != std::string::npos) {
        auto pos = ts.find('T');
        if (pos != std::string::npos && pos + 2