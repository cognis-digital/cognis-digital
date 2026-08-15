package main

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"time"

	"gopkg.in/yaml.v3"
)

// Profile represents the GitHub profile configuration.
type Profile struct {
	Name        string   `json:"name,omitempty"`
	Bio         string   `json:"bio,omitempty"`
	Location    string   `json:"location,omitempty"`
	Website     string   `json:"website,omitempty"`
	Email       string   `json:"email,omitempty"`
	AvatarURL   string   `json:"avatar_url,omitempty"`
	SocialLinks []Social `json:"social_links,omitempty"`
	Metadata    Metadata `json:"metadata,omitempty"`
}

// Social represents a social media link.
type Social struct {
	Name  string `json:"name,omitempty"`
	URL   string `json:"url,omitempty"`
	Icon  string `json:"icon,omitempty"` // emoji or icon name
}

// Metadata contains auto-generated info.
type Metadata struct {
	GeneratedAt    time.Time `json:"generated_at"`
	Version        string    `json:"version"`
	Tool           string    `json:"tool"`
	InputFile      string    `json:"input_file,omitempty"`
}

const (
	defaultVersion = "1.0.0"
	defaultTool    = "cognis-digital/profile-generator"
)

// ConfigLoader handles reading profile data from various sources.
type ConfigLoader struct {
	Stdin   bool
	File    string
	Env     bool
}

// NewConfigLoader creates a new loader with defaults.
func NewConfigLoader() *ConfigLoader {
	return &ConfigLoader{
		Stdin:  false,
		File:   "",
		Env:    true,
	}
}

// Load reads profile data from the configured source.
func (l *ConfigLoader) Load() (*Profile, error) {
	var p Profile

	if l.Stdin {
		data, err := os.Stdin.ReadAll()
		if err != nil {
			return nil, fmt.Errorf("reading stdin: %w", err)
		}
		err = json.Unmarshal(data, &p)
		if err != nil {
			return nil, fmt.Errorf("parsing stdin JSON: %w", err)
		}
		return &p, nil
	}

	if l.File != "" {
		data, err := os.ReadFile(l.File)
		if err != nil {
			return nil, fmt.Errorf("reading file '%s': %w", l.File, err)
		}
		err = json.Unmarshal(data, &p)
		if err != nil {
			return nil, fmt.Errorf("parsing JSON: %w", err)
		}
		return &p, nil
	}

	if l.Env {
		p.Name = os.Getenv("GITHUB_NAME")
		p.Bio = os.Getenv("GITHUB_BIO")
		p.Location = os.Getenv("GITHUB_LOCATION")
		p.Website = os.Getenv("GITHUB_WEBSITE")
		p.Email = os.Getenv("GITHUB_EMAIL")
		p.AvatarURL = os.Getenv("GITHUB_AVATAR_URL")

		if p.Name == "" {
			return nil, fmt.Errorf("no name provided (try GITHUB_NAME env or --file)")
		}
	}

	return &p, nil
}

// Validate checks the profile for common issues.
func (p *Profile) Validate() error {
	var issues []string

	if p.Name == "" {
		issues = append(issues, "name is required")
	}

	if p.Bio != "" && len(p.Bio) > 500 {
		issues = append(issues, fmt.Sprintf("bio too long: %d chars (max 500)", len(p.Bio)))
	}

	if p.Website != "" && !isValidURL(p.Website) {
		issues = append(issues, "invalid website URL")
	}

	if p.Email != "" && !isEmailValid(p.Email) {
		issues = append(issues, "invalid email format")
	}

	for i, link := range p.SocialLinks {
		if link.URL == "" {
			issues = append(issues, fmt.Sprintf("social link %d missing URL", i+1))
		}
		if !isValidURL(link.URL) {
			issues = append(issues, fmt.Sprintf("social link %d has invalid URL", i+1))
		}
	}

	return nil
}

// isValidURL checks if a string is a valid HTTP/HTTPS URL.
func isValidURL(s string) bool {
	if s == "" {
		return true // allow empty for optional fields
	}
	s = strings.TrimSpace(s)
	if !strings.HasPrefix(strings.ToLower(s), "http") {
		return false
	}
	parts := strings.SplitN(s, "/", 3)
	if len(parts) < 2 || parts[1] == "" {
		return false
	}
	return true
}

// isEmailValid checks basic email format.
func isEmailValid(email string) bool {
	email = strings.TrimSpace(email)
	if email == "" {
		return true
	}
	parts := strings.SplitN(email, "@", 2)
	if len(parts) != 2 || parts[0] == "" || parts[1] == "" {
		return false
	}
	domain := parts[1]
	if !strings.Contains(domain, ".") {
		return false
	}
	return true
}

// OutputWriter handles writing output to various destinations.
type OutputWriter struct {
	Stdout  bool
	File    string
	Format  string // "json" or "yaml"
	Indent  int
}

// NewOutputWriter creates a new writer with defaults.
func NewOutputWriter() *OutputWriter {
	return &OutputWriter{
		Stdout: true,
		Format: "json",
		Indent: 2,
	}
}

// Write outputs the profile to the configured destination.
func (w *OutputWriter) Write(p *Profile) error {
	var data []byte
	var err error

	switch w.Format {
	case "yaml":
		data, err = yaml.Marshal(p)
	default:
		data, err = json.MarshalIndent(p, "", strings.Repeat(" ", w.Indent))
	}

	if err != nil {
		return fmt.Errorf("serializing profile: %w", err)
	}

	var out io.Writer
	if w.File != "" {
		out, err = os.Create(w.File)
		if err != nil {
			return fmt.Errorf("creating file '%s': %w", w.File, err)
		}
		defer out.Close()
		fmt.Fprintf(out, "%s\n", string(data))
		fmt.Printf("Written to: %s\n", w.File)
	} else if w.Stdout {
		fmt.Print(string(data))
	}

	return nil
}

// ProfileGenerator is the main orchestrator.
type ProfileGenerator struct {
	Loader  *ConfigLoader
	Writer  *OutputWriter
	Profile *Profile
	Meta    Metadata
}

// NewProfileGenerator creates a new generator instance.
func NewProfileGenerator() *ProfileGenerator {
	return &ProfileGenerator{
		Loader:  NewConfigLoader(),
		Writer:  NewOutputWriter(),
		Meta:    Metadata{Version: defaultVersion, Tool: defaultTool},
	}
}

// Generate creates a complete profile with metadata and outputs it.
func (g *ProfileGenerator) Generate() error {
	p, err := g.Loader.Load()
	if err != nil {
		return fmt.Errorf("loading config: %w", err)
	}

	g.Profile = p
	g.Meta.GeneratedAt = time.Now().UTC()
	g.Meta.InputFile = g.Loader.File

	err = g.Profile.Validate()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Validation warnings:\n%s\n", err)
	} else if len(g.Profile.SocialLinks) == 0 && g.Profile.Bio == "" {
		fmt.Fprintln(os.Stderr, "Tip: Add a bio and social links for a richer profile.")
	}

	return g.Writer.Write(p)
}

// ApplyDefaults fills in sensible defaults when fields are empty.
func (p *Profile) ApplyDefaults() {
	if p.Name == "" {
		p.Name = os.Getenv("USER") + " (from hostname)"
	}
	if p.Bio == "" {
		bio := fmt.Sprintf("Go developer | %s", strings.TrimSpace(p.Location))
		p.Bio = bio
	}
	if p.AvatarURL == "" {
		hostname, _ := os.Hostname()
		p.AvatarURL = fmt.Sprintf("https://github.com/%s.png", hostname)
	}
}

// Merge combines multiple profiles with later ones taking precedence.
func (p *Profile) Merge(other *Profile) {
	if other.Name != "" {
		p.Name = other.Name
	}
	if other.Bio != "" {
		p.Bio = other.Bio
	}
	if other.Location != "" {
		p.Location = other.Location
	}
	if other.Website != "" {
		p.Website = other.Website
	}
	if other.Email != "" {
		p.Email = other.Email
	}
	if other.AvatarURL != "" {
		p.AvatarURL = other.AvatarURL
	}

	for _, link := range other.SocialLinks {
		found := false
		for i, existing := range p.SocialLinks {
			if existing.Name == link.Name {
				p.SocialLinks[i] = link
				found = true
				break
			}
		}
		if !found {
			p.SocialLinks = append(p.SocialLinks, link)
		}
	}
}

// Preview shows a formatted preview of the profile.
func (p *Profile) Preview() string {
	var b strings.Builder
	b.WriteString("=== Profile Preview ===\n\n")
	b.WriteString(fmt.Sprintf("Name: %s\n", p.Name))
	b.WriteString(fmt.Sprintf("Bio: %s\n", p.Bio))
	b.WriteString(fmt.Sprintf("Location: %s\n", p.Location))
	b.WriteString(fmt.Sprintf("Website: %s\n", p.Website))
	b.WriteString(fmt.Sprintf("Email: %s\n", p.Email))
	b.WriteString(fmt.Sprintf("Avatar: %s\n\n", p.AvatarURL))

	if len(p.SocialLinks) > 0 {
		b.WriteString("Social Links:\n")
		for _, link := range p.SocialLinks {
			b.WriteString(fmt.Sprintf("  • %s: %s\n", link.Name, link.URL))
		}
	}

	return b.String()
}

func main() {
	gen := NewProfileGenerator()

	// Allow command-line overrides via environment or flags
	nameOverride := os.Getenv("GITHUB_NAME")
	if nameOverride != "" {
		os.Setenv("USER", nameOverride)
	}

	fmt.Println("cognis-digital/profile-generator v" + defaultVersion)
	fmt.Println("Generating GitHub profile configuration...")

	err := gen.Generate()
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}

	fmt.Println("\nDone. Use 'GITHUB_NAME' env to override name.")
}