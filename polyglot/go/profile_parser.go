package main

import (
	"encoding/json"
	"fmt"
	"net/url"
	"os"
	"path/filepath"
	"regexp"
	"strings"
)

// Profile represents a GitHub profile configuration.
type Profile struct {
	Name        string   `json:"name,omitempty"`
	Username    string   `json:"username,omitempty"`
	Bio         string   `json:"bio,omitempty"`
	Location    string   `json:"location,omitempty"`
	Website     string   `json:"website,omitempty"`
	Email       string   `json:"email,omitempty"`
	AvatarURL   string   `json:"avatar_url,omitempty"`
	SocialLinks []SocialLink `json:"social_links,omitempty"`
	Repositories []Repository `json:"repositories,omitempty"`
}

// SocialLink represents a social media link.
type SocialLink struct {
	Name  string `json:"name,omitempty"`
	URL   string `json:"url,omitempty"`
	Type  string `json:"type,omitempty"` // e.g., "twitter", "linkedin"
	Icon  string `json:"icon,omitempty"`
}

// Repository represents a GitHub repository.
type Repository struct {
	Name        string `json:"name,omitempty"`
	Description string `json:"description,omitempty"`
	Stars       int    `json:"stars,omitempty"`
	Forks       int    `json:"forks,omitempty"`
	URL         string `json:"url,omitempty"`
}

// Config represents the full configuration file structure.
type Config struct {
	Version   string     `json:"version,omitempty" yaml:"version,omitempty"`
	Profile   Profile    `json:"profile,omitempty" yaml:"profile,omitempty"`
	Metadata  Metadata   `json:"metadata,omitempty" yaml:"metadata,omitempty"`
}

// Metadata contains optional metadata about the config file.
type Metadata struct {
	Author     string `json:"author,omitempty" yaml:"author,omitempty"`
	Created    string `json:"created,omitempty" yaml:"created,omitempty"`
	LastModified string `json:"last_modified,omitempty" yaml:"last_modified,omitempty"`
}

// DefaultProfile returns a Profile with default values.
func DefaultProfile() *Profile {
	return &Profile{
		Bio:         "Go developer and open source contributor",
		Location:    "San Francisco, CA",
		SocialLinks: []SocialLink{{Name: "GitHub", Type: "github"}},
	}
}

// ValidateEmail checks if the email is in a valid format.
func ValidateEmail(email string) bool {
	if email == "" {
		return true // Empty is allowed
	}
	pattern := `^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$`
	matched, _ := regexp.MatchString(pattern, email)
	return matched
}

// NormalizeURL removes trailing slashes and ensures http/https prefix.
func NormalizeURL(rawURL string) (string, error) {
	if rawURL == "" {
		return "", nil
	}
	
	trimmed := strings.TrimSpace(rawURL)
	
	// Add protocol if missing
	if !strings.HasPrefix(trimmed, "http://") && !strings.HasPrefix(trimmed, "https://") {
		trimmed = "https://" + trimmed
	}
	
	// Remove trailing slash (except for root domain)
	if len(trimmed) > 7 && trimmed[len(trimmed)-1] == '/' {
		trimmed = trimmed[:len(trimmed)-1]
	}
	
	return trimmed, nil
}

// ValidateSocialLink checks if a social link is valid.
func ValidateSocialLink(link SocialLink) error {
	if link.URL != "" {
		parsed, err := url.Parse(link.URL)
		if err != nil {
			return fmt.Errorf("invalid URL: %w", err)
		}
		
		if parsed.Scheme == "" || (parsed.Host == "" && parsed.Path == "") {
			return fmt.Errorf("URL must have a host or path")
		}
		
		// Check for common social domains
		validDomains := map[string]bool{
			"github.com": true, "twitter.com": true, "linkedin.com": true,
			"instagram.com": true, "youtube.com": true, "reddit.com": true,
			"discord.com": true, "twitch.tv": true,
		}
		
		if parsed.Host != "" {
			if !validDomains[parsed.Host] && !strings.Contains(parsed.Host, ".") {
				return fmt.Errorf("unknown social domain: %s", parsed.Host)
			}
		}
	}
	
	return nil
}

// ValidateRepository checks if a repository entry is valid.
func ValidateRepository(repo Repository) error {
	if repo.Name == "" {
		return fmt.Errorf("repository name cannot be empty")
	}
	
	if !strings.HasPrefix(repo.URL, "https://github.com/") && 
	   !strings.HasPrefix(repo.URL, "http://github.com/") {
		return fmt.Errorf("URL should point to a GitHub repository")
	}
	
	return nil
}

// ValidateConfig performs comprehensive validation of the config.
func (c *Config) Validate() error {
	var issues []string
	
	// Check profile name
	if c.Profile.Name == "" && c.Profile.Username == "" {
		issues = append(issues, "profile: at least one of 'name' or 'username' is required")
	}
	
	// Check email format if provided
	if c.Profile.Email != "" && !ValidateEmail(c.Profile.Email) {
		issues = append(issues, fmt.Sprintf("profile: invalid email format: %s", c.Profile.Email))
	}
	
	// Validate website URL
	if c.Profile.Website != "" {
		parsed, err := url.Parse(c.Profile.Website)
		if err == nil && parsed.Host != "" {
			c.Profile.Website = parsed.String() // Normalize it
		} else if err != nil {
			issues = append(issues, fmt.Sprintf("profile: invalid website URL: %s", c.Profile.Website))
		}
	}
	
	// Validate social links
	for i, link := range c.Profile.SocialLinks {
		if link.URL == "" && link.Name != "" {
			continue // Name-only is fine
		}
		
		err := ValidateSocialLink(link)
		if err != nil {
			issues = append(issues, fmt.Sprintf("social_links[%d]: %v", i, err))
		}
	}
	
	// Validate repositories
	for i, repo := range c.Profile.Repositories {
		err := ValidateRepository(repo)
		if err != nil {
			issues = append(issues, fmt.Sprintf("repositories[%d]: %v", i, err))
		}
	}
	
	if len(issues) > 0 {
		return fmt.Errorf("validation failed:\n\t%s", strings.Join(issues, "\n\t"))
	}
	
	return nil
}

// ApplyDefaults fills in missing optional fields with sensible defaults.
func (c *Config) ApplyDefaults() {
	if c.Profile.Name == "" && c.Profile.Username == "" {
		c.Profile.Name = "Anonymous"
	}
	
	if c.Profile.Bio == "" {
		c.Profile.Bio = DefaultProfile().Bio
	}
	
	if c.Profile.Location == "" {
		c.Profile.Location = DefaultProfile().Location
	}
	
	// Normalize all URLs
	if err := normalizeURLs(c); err != nil {
		fmt.Printf("Warning: %v\n", err)
	}
}

func normalizeURLs(c *Config) error {
	var normalized []string
	
	if c.Profile.Website != "" {
		n, _ := NormalizeURL(c.Profile.Website)
		c.Profile.Website = n
		normalized = append(normalized, "website")
	}
	
	for i, link := range c.Profile.SocialLinks {
		if link.URL != "" {
			n, err := NormalizeURL(link.URL)
			if err == nil {
				c.Profile.SocialLinks[i].URL = n
				normalized = append(normalized, fmt.Sprintf("social_links[%d]", i))
			} else {
				return err
			}
		}
	}
	
	for i, repo := range c.Profile.Repositories {
		if repo.URL != "" {
			n, _ := NormalizeURL(repo.URL)
			c.Profile.Repositories[i].URL = n
		}
	}
	
	if len(normalized) > 0 {
		fmt.Printf("Normalized %d URL(s)\n", len(normalized))
	}
	
	return nil
}

// ParseConfig reads and parses a configuration file.
func ParseConfig(path string, format string) (*Config, error) {
	var data []byte
	
	// Determine if we need to read the file
	if path == "-" || path == "stdin" {
		data = make([]byte, 0)
		if _, err := fmt.Fscan(&data); err != nil {
			return nil, err
		}
		path = "<stdin>"
	} else if filepath.Ext(path) == ".json" {
		var err error
		data, err = os.ReadFile(path)
		if err != nil {
			return nil, fmt.Errorf("reading JSON file: %w", err)
		}
	} else {
		var err error
		data, err = os.ReadFile(path)
		if err != nil {
			return nil, fmt.Errorf("reading YAML/JSON file: %w", err)
		}
	}
	
	// Try JSON first, then fall back to YAML if needed
	var config Config
	
	// Attempt JSON parsing
	err := json.Unmarshal(data, &config)
	if err == nil {
		return &config, nil
	}
	
	// Fall back to YAML (using a simple parser for common cases)
	config = parseYAMLFallback(string(data))
	return &config, nil
}

// parseYAMLFallback is a minimal YAML parser for common GitHub profile configs.
func parseYAMLFallback(content string) Config {
	var config Config
	
	// Simple key-value extraction for basic YAML support
	lines := strings.Split(strings.TrimSpace(content), "\n")
	
	for _, line := range lines {
		line = strings.TrimSpace(line)
		
		if line == "" || strings.HasPrefix(line, "#") {
			continue
		}
		
		parts := strings.SplitN(line, ":", 2)
		if len(parts) != 2 {
			continue
		}
		
		key := strings.TrimSpace(parts[0])
		value := strings.TrimSpace(parts[1])
		
		// Extract nested keys for profile section
		if key == "profile" {
			config.Profile.Name = extractYAMLValue(value, "name")
			config.Profile.Username = extractYAMLValue(value, "username")
			config.Profile.Bio = extractYAMLValue(value, "bio")
			config.Profile.Location = extractYAMLValue(value, "location")
			config.Profile.Website = extractYAMLValue(value, "website")
			config.Profile.Email = extractYAMLValue(value, "email")
		} else if key == "social_links" {
			// Parse social links array (simplified)
			if strings.HasPrefix(value, "[") && strings.HasSuffix(value, "]") {
				links := parseSocialLinksArray(value)
				config.Profile.SocialLinks = append(config.Profile.SocialLinks, links...)
			} else if value != "" {
				link := SocialLink{URL: value}
				config.Profile.SocialLinks = append(config.Profile.SocialLinks, link)
			}
		} else if key == "repositories" || key == "repos" {
			if strings.HasPrefix(value, "[") && strings.HasSuffix(value, "]") {
				repos := parseRepositoriesArray(value)
				config.Profile.Repositories = append(config.Profile.Repositories, repos...)
			} else if value != "" {
				repo := Repository{Name: value}
				config.Profile.Repositories = append(config.Profile.Repositories, repo)
			}
		}
	}
	
	return config
}

func extractYAMLValue(nested string, field string) string {
	pattern := fmt.Sprintf(`"%s"\s*:\s*"([^"]*)"`+`|`+fmt.Sprintf(`'%s'\s*:\s*'([^']*)'`, field, field)
	matched, _ := regexp.MatchString(pattern, nested)
	if matched {
		return "" // Found it in complex format
	}
	
	// Simple extraction: find "field": "value" pattern
	pattern = fmt.Sprintf(`"%s"\s*:\s*"([^"]*)"`+`|`+fmt.Sprintf(`'%s'\s*:\s*'([^']*)'`, field, field)
	matches := regexp.FindAllStringSubmatch(pattern, -1)
	
	for _, match := range matches {
		if len(match) >= 3 && match[2] != "" {
			return match[2]
		}
	}
	
	return ""
}

func parseSocialLinksArray(arrayStr string) []SocialLink {
	var links []SocialLink
	
	// Simple parser: look for "name": "url" patterns within the array
	pattern := `"\s*name\s*:\s*"([^"]*)"`+`|`+`'\s*name\s*:\s*'([^']*)'`
	matches := regexp.FindAllStringSubmatch(arrayStr, -1)
	
	for _, match := range matches {
		if len(match) >= 3 && (match[2] != "" || match[3] != "") {
			name := strings.TrimSpace(match[2])
			if name == "" {
				name = strings.TrimSpace(match[3])
			}
			
			link := SocialLink{Name: name, Type: "github"}
			links = append(links, link)
		}
	}
	
	return links
}

func parseRepositoriesArray(arrayStr string) []Repository {
	var repos []Repository
	
	pattern := `"\s*name\s*:\s*"([^"]*)"`+`|`+`'\s*name\s*:\s*'([^']*)'`
	matches := regexp.FindAllStringSubmatch(arrayStr, -1)
	
	for _, match := range matches {
		if len(match) >= 3 && (match[2] != "" || match[3] != "") {
			name := strings.TrimSpace(match[2])
			if name == "" {
				name = strings.TrimSpace(match[3])
			}
			
			repo := Repository{Name: name, Description: "Repository"}
			repos = append(repos, repo)
		}
	}
	
	return repos
}

// PrintProfile outputs the profile in a formatted way.
func (p *Profile) Print() {
	fmt.Printf("=== GitHub Profile ===\n")
	fmt.Printf("Name:    %s\n", p.Name)
	if p.Username != "" {
		fmt.Printf("Username:%s\n", p.Username)
	}
	fmt.Printf("Bio:     %s\n", p.Bio)
	fmt.Printf("Location: %s\n", p.Location)
	
	if p.Website != "" {
		fmt.Printf("Website: %s\n", p.Website)
	}
	
	if p.Email != "" {
		fmt.Printf("Email:   %s\n", p.Email)
	}
	
	if p.AvatarURL != "" {
		fmt.Printf("Avatar:  %s\n", p.AvatarURL)
	}
	
	if len(p.SocialLinks) > 0 {
		fmt.Println("\nSocial Links:")
		for _, link := range p.SocialLinks {
			if link.URL != "" {
				fmt.Printf("  • %s: %s\n", link.Name, link.URL)
			} else if link.Name != "" {
				fmt.Printf("  • %s (type: %s)\n", link.Name, link.Type)
			}
		}
	}
	
	if len(p.Repositories) > 0 {
		fmt.Println("\nRepositories:")
		for _, repo := range p.Repositories {
			desc := ""
			if repo.Description != "" {
				desc = fmt.Sprintf(" | %s", repo.Description)
			}
			stars := ""
			forks := ""
			if repo.Stars > 0 {
				stars = fmt.Sprintf(" (⭐ %d)", repo.Stars)
			}
			if repo.Forks > 0 {
				forks = fmt.Sprintf(" (🔀 %d)", repo.Forks)
			}
			fmt.Printf("  • %s