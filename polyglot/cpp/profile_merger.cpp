#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ============================================================================
// CONFIGURATION STRUCTURES
// ============================================================================

struct ProfileMeta {
    std::string name;
    std::string source_file;
    std::chrono::system_clock::time_point timestamp;
};

struct ProfileConfig {
    ProfileMeta meta;
    
    // GitHub OAuth settings
    struct OAuth {
        std::string client_id;
        std::string client_secret;
        bool auto_refresh = true;
    } oauth;
    
    // API rate limiting
    struct RateLimit {
        int requests_per_minute = 300;
        int burst_size = 50;
    } rate_limit;
    
    // Profile data to merge
    std::string username;
    std::string display_name;
    std::string bio;
    std::vector<std::string> repositories;
    std::vector<std::string> organizations;
    bool show_private = false;
};

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

std::string escape_json_string(const std::string& str) {
    static const char* escapes[] = {
        "\\\"", "\\\\", "\\/", "\b", "\f", "\n", "\r", "\t"
    };
    static const char* esc_chars = "\"\\/bfnrt";
    
    std::ostringstream oss;
    for (char c : str) {
        auto it = std::find(esc_chars, esc_chars + 10, c);
        if (it != esc_chars + 10) {
            oss << "\\" << *it;
        } else {
            oss << c;
        }
    }
    return oss.str();
}

std::string format_timestamp(const std::chrono::system_clock::time_point& tp) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    auto tm = *std::localtime(&time);
    
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(buffer);
}

// ============================================================================
// PROFILE MERGER CORE LOGIC
// ============================================================================

class ProfileMerger {
public:
    static const int DEFAULT_PRIORITY = 0;
    
private:
    std::vector<ProfileConfig> profiles_;
    bool auto_priority_ = true;
    
public:
    ProfileMerger() = default;
    
    void add_profile(const ProfileConfig& config) {
        if (auto_priority_) {
            // Auto-generate timestamp for priority resolution
            config.meta.timestamp = std::chrono::system_clock::now();
        }
        profiles_.push_back(config);
    }
    
    void set_auto_priority(bool enabled) {
        auto_priority_ = enabled;
    }
    
    ProfileConfig merge() const {
        if (profiles_.empty()) {
            throw std::runtime_error("No profiles to merge");
        }
        
        // Sort by timestamp (newest first for priority)
        std::sort(profiles_.begin(), profiles_.end(),
                  [](const ProfileConfig& a, const ProfileConfig& b) {
                      return a.meta.timestamp < b.meta.timestamp;
                  });
        
        // Start with the newest profile as base
        ProfileConfig result = profiles_[0];
        
        for (size_t i = 1; i < profiles_.size(); ++i) {
            const auto& source = profiles_[i];
            
            // Merge OAuth settings - newer wins unless disabled
            if (!source.oauth.client_id.empty()) {
                result.oauth = source.oauth;
            } else if (result.oauth.auto_refresh && !result.oauth.client_id.empty()) {
                // Keep existing OAuth, but mark as auto-refresh enabled
            }
            
            // Merge string fields - newer wins
            if (!source.username.empty()) {
                result.username = std::move(source.username);
            }
            if (!source.display_name.empty()) {
                result.display_name = std::move(source.display_name);
            }
            if (!source.bio.empty()) {
                result.bio = std::move(source.bio);
            }
            
            // Merge arrays - union of all, newest takes precedence for duplicates
            merge_arrays(result.repositories, source.repositories);
            merge_arrays(result.organizations, source.organizations);
            
            // Boolean flags - newer wins
            if (source.show_private) {
                result.show_private = true;
            }
        }
        
        return result;
    }
    
private:
    void merge_arrays(std::vector<std::string>& dest, 
                      const std::vector<std::string>& source) {
        for (const auto& item : source) {
            if (!dest.empty() && !dest.back().empty()) {
                // Check if last element matches - newer wins
                if (dest.back() == item) {
                    continue;
                }
            }
            dest.push_back(item);
        }
    }
};

// ============================================================================
// I/O OPERATIONS
// ============================================================================

class ConfigIO {
public:
    static ProfileConfig read_from_file(const std::string& filepath, 
                                        const std::string& source_name) {
        std::ifstream file(filepath);
        
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open config file: " + filepath);
        }
        
        json j;
        try {
            j = json::parse(file);
        } catch (const json::parse_error& e) {
            throw std::runtime_error("JSON parse error in " + filepath + ": " + 
                                     std::string(e.what()));
        }
        
        ProfileConfig config;
        config.meta.name = source_name;
        config.meta.source_file = filepath;
        
        // Extract fields with defaults
        if (j.contains("oauth")) {
            auto& o = j["oauth"];
            config.oauth.client_id = o.value<std::string>("client_id", "");
            config.oauth.client_secret = o.value<std::string>("client_secret", "");
            config.oauth.auto_refresh = o.value<bool>("auto_refresh", true);
        }
        
        if (j.contains("rate_limit")) {
            auto& r = j["rate_limit"];
            config.rate_limit.requests_per_minute = 
                r.value<int>("requests_per_minute", 300);
            config.rate_limit.burst_size = 
                r.value<int>("burst_size", 50);
        }
        
        if (j.contains("username")) {
            config.username = j["username"];
        }
        
        if (j.contains("display_name")) {
            config.display_name = j["display_name"];
        }
        
        if (j.contains("bio")) {
            config.bio = j["bio"];
        }
        
        if (j.contains("repositories")) {
            auto& arr = j["repositories"];
            for (const auto& repo : arr) {
                config.repositories.push_back(repo.get<std::string>());
            }
        }
        
        if (j.contains("organizations")) {
            auto& arr = j["organizations"];
            for (const auto& org : arr) {
                config.organizations.push_back(org.get<std::string>());
            }
        }
        
        if (j.contains("show_private")) {
            config.show_private = j["show_private"].get<bool>();
        }
        
        return config;
    }
    
    static std::vector<ProfileConfig> read_all_files(
        const std::string& directory, 
        const std::string& pattern = "*.json") {
        
        std::vector<ProfileConfig> configs;
        
        // Simple glob implementation (works for most cases)
        std::ifstream dir_stream(directory);
        if (!dir_stream.is_open()) {
            return configs;
        }
        
        std::string line;
        while (std::getline(dir_stream, line)) {
            if (line.find(pattern) != std::string::npos || 
                line.find("profile") != std::string::npos) {
                
                // Extract filename from path
                size_t last_slash = line.rfind('/');
                std::string filename = (last_slash == std::string::npos) ? 
                                      line : line.substr(last_slash + 1);
                
                ProfileConfig config = read_from_file(line, filename);
                configs.push_back(config);
            }
        }
        
        return configs;
    }
    
    static bool write_config(const ProfileConfig& config, 
                            const std::string& filepath) {
        std::ofstream file(filepath);
        
        if (!file.is_open()) {
            std::cerr << "Error: Failed to open output file for writing\n";
            return false;
        }
        
        json j;
        
        // OAuth section
        if (config.oauth.client_id.empty() || config.oauth.client_secret.empty()) {
            j["oauth"] = {{"auto_refresh", config.oauth.auto_refresh}};
        } else {
            j["oauth"] = {
                {"client_id", config.oauth.client_id},
                {"client_secret", config.oauth.client_secret},
                {"auto_refresh", config.oauth.auto_refresh}
            };
        }
        
        // Rate limit section
        if (config.rate_limit.requests_per_minute != 300 || 
            config.rate_limit.burst_size != 50) {
            j["rate_limit"] = {
                {"requests_per_minute", config.rate_limit.requests_per_minute},
                {"burst_size", config.rate_limit.burst_size}
            };
        }
        
        // Profile data
        if (!config.username.empty()) {
            j["username"] = config.username;
        }
        if (!config.display_name.empty()) {
            j["display_name"] = config.display_name;
        }
        if (!config.bio.empty()) {
            j["bio"] = config.bio;
        }
        
        // Arrays - only include if non-empty or explicitly set
        if (!config.repositories.empty() || !config.organizations.empty()) {
            json repos, orgs;
            
            for (const auto& repo : config.repositories) {
                repos.push_back(repo);
            }
            
            for (const auto& org : config.organizations) {
                orgs.push_back(org);
            }
            
            j["repositories"] = repos;
            j["organizations"] = orgs;
        }
        
        if (config.show_private) {
            j["show_private"] = true;
        }
        
        // Metadata - optional but useful for debugging
        j["_meta"] = {
            {"source", config.meta.source_file},
            {"merged_at", format_timestamp(config.meta.timestamp)}
        };
        
        file << j.dump(2) << "\n";
        return true;
    }
};

// ============================================================================
// VALIDATION
// ============================================================================

class ConfigValidator {
public:
    static bool validate(const ProfileConfig& config, 
                        std::string& error_msg = "") {
        
        // Check OAuth if present
        if (!config.oauth.client_id.empty()) {
            if (config.oauth.client_id.length() < 8) {
                error_msg += "OAuth client_id too short\n";
            }
            
            if (!config.oauth.client_secret.empty()) {
                if (config.oauth.client_secret.length() < 10) {
                    error_msg += "OAuth client_secret too short\n";
                }
            }
        }
        
        // Check username
        if (!config.username.empty()) {
            if (config.username.length() > 50) {
                error_msg += "Username exceeds 50 character limit\n";
            }
            
            // Validate GitHub username format
            bool valid = true;
            for (char c : config.username) {
                if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
                    valid = false;
                    break;
                }
            }
            
            if (!valid) {
                error_msg += "Invalid username format\n";
            }
        }
        
        // Check rate limits
        if (config.rate_limit.requests_per_minute < 10 || 
            config.rate_limit.requests_per_minute > 5000) {
            error_msg += "Rate limit outside acceptable range (10-5000)\n";
        }
        
        return error_msg.empty();
    }
};

// ============================================================================
// COMMAND LINE INTERFACE
// ============================================================================

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS] <input_files...> <output_file>\n\n"
              << "Merge multiple GitHub profile configurations into one.\n\n"
              << "Options:\n"
              << "  -p, --priority=timestamp   Use timestamp for merge priority (default)\n"
              << "  -p, --priority=file        Use filename order instead of timestamp\n"
              << "  -v, --verbose             Show detailed merge information\n"
              << "  -d, --dry-run            Print merged result without writing\n"
              << "  -h, --help                Show this help message\n\n"
              << "Examples:\n"
              << "  " << program_name << " profile_a.json profile_b.json output.json\n"
              << "  " << program_name << " -v -d *.json merged.json\n";
}

int main(int argc, char* argv[]) {
    std::string input_files;
    std::string output_file = "merged_profile.json";
    bool verbose = false;
    bool dry_run = false;
    ProfileMerger::PriorityStrategy priority_strategy = 
        ProfileMerger::DEFAULT_PRIORITY;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            return 0;
        } else if ((arg == "-v" || arg == "--verbose") && !input_files.empty()) {
            verbose = true;
        } else if ((arg == "-d" || arg == "--dry-run") && !input_files.empty()) {
            dry_run = true;
        } else if (arg == "-p" || arg == "--priority") {
            ++i;
            if (i < argc) {
                priority_strategy = 
                    (std::string(argv[i]) == "file") ? 
                    ProfileMerger::DEFAULT_PRIORITY : 
                    ProfileMerger::DEFAULT_PRIORITY;
            }
        } else if (!input_files.empty()) {
            input_files += " " + std::string(arg);
        }
    }
    
    // Need at least one input file and output specified
    if (input_files.empty() || output_file.empty()) {
        print_usage(argv[0]);
        return 1;
    }
    
    // Read all profiles
    std::vector<ProfileConfig> configs;
    try {
        std::istringstream iss(input_files);
        std::string file_path;
        
        while (iss >> file_path) {
            ProfileConfig config = ConfigIO::read_from_file(file_path, 
                                                             file_path);
            configs.push_back(config);
            
            if (verbose) {
                std::cout << "Loaded: " << file_path << "\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading input files: " << e.what() << "\n";
        return 1;
    }
    
    if (configs.empty()) {
        std::cerr << "No valid profile configurations found\n";
        return 1;
    }
    
    // Merge profiles
    ProfileConfig merged;
    try {
        merged = configs[0];
        
        for (size_t i = 1; i < configs.size(); ++i) {
            const auto& source = configs[i];
            
            std::cout << "Merging from: " << source.meta.source_file << "\n";
            std::cout << "  Username: " << (source.username.empty() ? "(empty)" : 
                                         source.username) << "\n";
            std::cout << "  Repositories: " << source.repositories.size() << "\n";
            
            // Perform actual merge
            merged = configs[0];
            
            for (size_t j = 1; j < configs.size(); ++j) {
                const auto& src = configs[j];
                
                if