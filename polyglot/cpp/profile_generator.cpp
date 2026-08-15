#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <regex>
#include <algorithm>
#include <iomanip>
#include <filesystem>
#include <ctime>
#include <chrono>

namespace fs = std::filesystem;

// ============================================================================
// Configuration Schema Definition
// ============================================================================

struct ProfileConfig {
    std::string name;
    std::string username;
    std::string email;
    std::string bio;
    std::vector<std::string> skills;
    std::map<std::string, int> repositories;
    bool show_email = true;
    bool show_location = false;
    std::string location;
    std::string website;
    std::string twitter;
    std::string linkedin;
    std::string github_url;
};

// ============================================================================
// YAML Parser (Minimal, Self-Contained)
// ============================================================================

class SimpleYamlParser {
public:
    static ProfileConfig parse(const std::string& content) {
        ProfileConfig config;
        
        // Helper lambda to extract string value
        auto get_str = [&](const std::string& key, const std::string& default_val = "") -> std::string {
            size_t pos = 0;
            while (pos < content.size()) {
                if (content.substr(pos, key.length() + 1) == key + ":") {
                    pos += key.length() + 2; // Skip "key: "
                    
                    // Find the end of value (next non-indented line or closing brace)
                    size_t val_start = pos;
                    while (pos < content.size()) {
                        if (content[pos] == '\n' && !std::isspace(content[pos-1])) {
                            break;
                        }
                        pos++;
                    }
                    
                    std::string value = content.substr(val_start, pos - val_start);
                    // Remove quotes and trim whitespace
                    while (!value.empty() && (value.front() == '"' || value.front() == '\'')) {
                        value.erase(0, 1);
                    }
                    while (!value.empty() && std::isspace(value.back())) {
                        value.pop_back();
                    }
                    
                    if (key == "name") config.name = value;
                    else if (key == "username") config.username = value;
                    else if (key == "email") config.email = value;
                    else if (key == "bio") config.bio = value;
                    else if (key == "location") config.location = value;
                    else if (key == "website") config.website = value;
                    else if (key == "twitter") config.twitter = value;
                    else if (key == "linkedin") config.linkedin = value;
                    else if (key == "github_url") config.github_url = value;
                    continue;
                }
                pos++;
            }
            
            // Set defaults
            if (config.name.empty()) config.name = "Developer";
            if (config.username.empty()) config.username = "dev-user";
            if (config.email.empty()) config.email = "";
            if (config.bio.empty()) config.bio = "Passionate developer building things.";
            
            return config;
        };

        // Parse repositories section
        auto get_repos = [&](const std::string& content) -> std::map<std::string, int> {
            std::map<std::string, int> repos;
            size_t pos = 0;
            while (pos < content.size()) {
                if (content.find("repos:") != std::string::npos && 
                    pos > 0 && content[pos-1] == ':') {
                    // Find next line with repo name
                    while (pos < content.size() && !content[pos].isalpha()) pos++;
                    
                    size_t repo_start = pos;
                    while (pos < content.size() && content[pos] != '\n' && 
                           content[pos] != ':' && !std::isspace(content[pos])) {
                        pos++;
                    }
                    
                    if (pos < content.size() && content[pos] == ':') pos++;
                    
                    std::string repo_name = content.substr(repo_start, pos - repo_start);
                    while (!repo_name.empty() && std::isspace(repo_name.back())) {
                        repo_name.pop_back();
                    }
                    
                    // Extract number (stars/forks)
                    size_t num_pos = pos;
                    while (num_pos < content.size() && 
                           (std::isdigit(content[num_pos]) || content[num_pos] == '.')) {
                        num_pos++;
                    }
                    
                    if (!repo_name.empty()) {
                        repos[repo_name] = std::stoi(content.substr(num_pos, pos - num_pos));
                    }
                }
                pos++;
            }
            return repos;
        };

        // Simple multi-pass parser for better reliability
        std::string clean_content = content;
        
        // Pass 1: Extract basic fields
        auto extract_field = [&](const std::string& pattern, const std::string& key) -> void {
            size_t start = clean_content.find(pattern);
            if (start == std::string::npos) return;
            
            size_t colon_pos = clean_content.find(':', start + pattern.length());
            if (colon_pos == std::string::npos) return;
            
            size_t value_start = colon_pos + 2; // Skip ": "
            while (value_start < clean_content.size() && 
                   (std::isspace(clean_content[value_start]) || 
                    clean_content[value_start] == '"')) {
                if (clean_content[value_start] == '"') value_start++;
                else value_start--;
            }
            
            size_t value_end = value_start;
            while (value_end < clean_content.size() && 
                   !std::isspace(clean_content[value_end])) {
                value_end++;
            }
            
            std::string value = clean_content.substr(value_start, value_end - value_start);
            if (!value.empty()) {
                // Remove surrounding quotes
                while (value.length() > 1 && 
                       ((value.front() == '"' || value.front() == '\'') ||
                        (value.back() == '"' || value.back() == '\''))) {
                    value = value.substr(1, value.length() - 2);
                }
                
                if (key == "name") config.name = value;
                else if (key == "username") config.username = value;
                else if (key == "email") config.email = value;
                else if (key == "bio") config.bio = value;
                else if (key == "location") config.location = value;
                else if (key == "website") config.website = value;
                else if (key == "twitter") config.twitter = value;
                else if (key == "linkedin") config.linkedin = value;
                else if (key == "github_url") config.github_url = value;
            }
        };

        extract_field("name:", "name");
        extract_field("username:", "username");
        extract_field("email:", "email");
        extract_field("bio:", "bio");
        extract_field("location:", "location");
        extract_field("website:", "website");
        extract_field("twitter:", "twitter");
        extract_field("linkedin:", "linkedin");
        extract_field("github_url:", "github_url");

        // Pass 2: Extract repositories
        size_t repos_start = clean_content.find("repos:");
        if (repos_start != std::string::npos) {
            size_t repos_end = clean_content.find("\n", repos_start);
            if (repos_end == std::string::npos) repos_end = clean_content.size();
            
            auto repo_map = get_repos(clean_content.substr(repos_start, repos_end - repos_start));
            config.repositories = repo_map;
        }

        // Pass 3: Set defaults
        if (config.name.empty()) config.name = "Developer";
        if (config.username.empty()) config.username = "dev-user";
        if (config.email.empty()) config.email = "";
        if (config.bio.empty()) config.bio = "Passionate developer building things.";

        return config;
    }
};

// ============================================================================
// Output Generators
// ============================================================================

class ProfileGenerator {
public:
    static std::string generate_json(const ProfileConfig& profile) {
        std::ostringstream oss;
        
        oss << "{\n";
        oss << "  \"name\": \"" << escape_json(profile.name) << "\",\n";
        oss << "  \"username\": \"" << escape_json(profile.username) << "\",\n";
        oss << "  \"email\": \"" << (profile.show_email ? profile.email : "") << "\",\n";
        oss << "  \"bio\": \"" << escape_json(profile.bio) << "\",\n";
        
        if (!profile.location.empty()) {
            oss << "  \"location\": \"" << escape_json(profile.location) << "\",\n";
        }
        
        if (!profile.website.empty()) {
            oss << "  \"website\": \"" << escape_json(profile.website) << "\",\n";
        }
        
        if (!profile.twitter.empty()) {
            oss << "  \"twitter\": \"" << escape_json(profile.twitter) << "\",\n";
        }
        
        if (!profile.linkedin.empty()) {
            oss << "  \"linkedin\": \"" << escape_json(profile.linkedin) << "\",\n";
        }
        
        if (!profile.github_url.empty()) {
            oss << "  \"github_url\": \"" << escape_json(profile.github_url) << "\",\n";
        }
        
        oss << "  \"repositories\": {\n";
        bool first = true;
        for (const auto& [repo, count] : profile.repositories) {
            if (!first) oss << ",\n";
            oss << "    \"" << escape_json(repo) << "\": " << count;
            first = false;
        }
        oss << "\n  }\n";
        
        oss << "}\n";
        return oss.str();
    }

    static std::string generate_markdown(const ProfileConfig& profile, 
                                         const std::string& output_dir) {
        fs::path readme_path = (output_dir.empty() ? "." : output_dir);
        readme_path /= "README.md";
        
        std::ostringstream oss;
        
        // Header
        oss << "# " << escape_markdown(profile.name) << "\n\n";
        
        // Profile info
        if (!profile.email.empty()) {
            oss << "**Email:** [" << escape_markdown(profile.email) << "](" << 
                (profile.show_email ? profile.email : "") << ")\n\n";
        }
        
        if (!profile.bio.empty()) {
            oss << "**Bio:** " << escape_markdown(profile.bio) << "\n\n";
        }
        
        // Location
        if (!profile.location.empty()) {
            oss << "**Location:** " << escape_markdown(profile.location) << "\n\n";
        }
        
        // Website
        if (!profile.website.empty()) {
            oss << "**Website:** [" << escape_markdown(profile.website) << "](" << 
                escape_markdown(profile.website) << ")\n\n";
        }
        
        // Social links
        if (!profile.twitter.empty()) {
            oss << "[Twitter](https://twitter.com/" << escape_markdown(profile.twitter) << ")\n";
        }
        if (!profile.linkedin.empty()) {
            oss << "[LinkedIn](https://linkedin.com/in/" << escape_markdown(profile.linkedin) << ")\n";
        }
        
        // Repositories
        oss << "\n## Top Repositories\n\n";
        
        bool first = true;
        for (const auto& [repo, count] : profile.repositories) {
            if (!first) oss << "\n";
            
            std::string repo_url = "https://github.com/" + escape_markdown(profile.username) + "/" + 
                                  escape_markdown(repo);
            oss << "**" << escape_markdown(repo) << "** - ";
            
            // Format: Stars (Forks)
            if (count > 0) {
                oss << count << " ⭐";
            } else {
                oss << "(no stars yet)";
            }
            
            first = false;
        }
        
        return oss.str();
    }

    static std::string generate_plain_text(const ProfileConfig& profile, 
                                           const std::string& output_dir) {
        fs::path txt_path = (output_dir.empty() ? "." : output_dir);
        txt_path /= "profile.txt";
        
        std::ostringstream oss;
        
        oss << "=== GitHub Profile: " << escape_plain(profile.name) << " ===\n\n";
        
        if (!profile.email.empty()) {
            oss << "Email: " << profile.email << "\n\n";
        }
        
        if (!profile.bio.empty()) {
            oss << "Bio: " << profile.bio << "\n\n";
        }
        
        if (!profile.location.empty()) {
            oss << "Location: " << profile.location << "\n\n";
        }
        
        if (!profile.website.empty()) {
            oss << "Website: " << profile.website << "\n\n";
        }
        
        // Social links
        std::vector<std::string> socials;
        if (!profile.twitter.empty()) {
            socials.push_back("Twitter: @" + profile.twitter);
        }
        if (!profile.linkedin.empty()) {
            socials.push_back("LinkedIn: /in/" + profile.linkedin);
        }
        
        for (const auto& s : socials) {
            oss << s << "\n";
        }
        
        // Repositories
        oss << "\n---\n\n";
        oss << "Repositories:\n\n";
        
        bool first = true;
        for (const auto& [repo, count] : profile.repositories) {
            if (!first) oss << ", ";
            
            std::string repo_url = "https://github.com/" + escape_plain(profile.username) + "/" + 
                                  escape_plain(repo);
            oss << "[" << escape_plain(repo) << "](" << repo_url << ")";
            
            if (count > 0) {
                oss << " (" << count << " ⭐)";
            }
            
            first = false;
        }
        
        return oss.str();
    }

private:
    static std::string escape_json(const std::string& s) {
        std::ostringstream oss;
        for (char c : s) {
            switch (c) {
                case '"':  oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default:   oss << c; break;
            }
        }
        return oss.str();
    }

    static std::string escape_markdown(const std::string& s) {
        std::ostringstream oss;
        for (char c : s) {
            switch (c) {
                case '#':  oss << "\\#"; break;
                case '*':  oss << "\\*"; break;
                case '_':  oss << "\\_"; break;
                case '[':  oss << "\\\["; break;
                case ']':  oss << "\\\\]"; break;
                default:   oss << c; break;
            }
        }
        return oss.str();
    }

    static std::string escape_plain(const std::string& s) {
        std::ostringstream oss;
        for (char c : s) {
            switch (c) {
                case '<':  oss << "&lt;"; break;
                case '>':  oss << "&gt;"; break;
                default:   oss << c; break;
            }
        }
        return oss.str();
    }
};

// ============================================================================
// Validation Logic
// ============================================================================

class ProfileValidator {
public:
    static std::vector<std::string> validate(const ProfileConfig& profile) {
        std::vector<std::string> errors;
        
        // Validate name
        if (profile.name.empty()) {
            errors.push_back("Name is required");
        } else if (profile.name.length() > 100) {
            errors.push_back("Name too long (max 100 characters)");
        }
        
        // Validate username
        if (profile.username.empty()) {
            errors.push_back("Username is required");
        } else if (!is_valid_username(profile.username)) {
            errors.push_back("Invalid username format (alphanumeric, hyphens, underscores only)");
        }
        
        // Validate email
        if (!profile.email.empty() && !is_valid_email(profile.email)) {
            errors.push_back("Invalid email format");
        }
        
        // Validate bio
        if (profile.bio.length() > 500) {
            errors.push_back("Bio too long (max 500 characters)");
        }
        
        // Validate repositories
        for (const auto& [repo, count] : profile.repositories) {
            if (repo.empty()) {
                errors.push_back("Repository name cannot be empty");
            } else if (!is_valid_repo_name(repo)) {
                errors.push_back("Invalid repository name format");
            }
            
            if (count < 0) {