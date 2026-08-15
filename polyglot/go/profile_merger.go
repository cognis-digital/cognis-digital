package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"sort"
	"strings"

	"gopkg.in/yaml.v3"
)

// Profile represents a GitHub profile structure.
type Profile struct {
	Name         string            `json:"name,omitempty" yaml:"name,omitempty"`
	DisplayName  string            `json:"display_name,omitempty" yaml:"display_name,omitempty"`
	Bio          string            `json:"bio,omitempty" yaml:"bio,omitempty"`
	URL          string            `json:"url,omitempty" yaml:"url,omitempty"`
	Email        string            `json:"email,omitempty" yaml:"email,omitempty"`
	Location     string            `json:"location,omitempty" yaml:"location,omitempty"`
	Socials      map[string]string `json:"socials,omitempty" yaml:"socials,omitempty"`
	Tags         []string          `json:"tags,omitempty" yaml:"tags,omitempty"`
	Metadata     Metadata          `json:"metadata,omitempty" yaml:"metadata,omitempty"`
}

// Metadata holds internal/optional profile data.
type Metadata struct {
	SourceFiles   []string `json:"source_files,omitempty" yaml:"source_files,omitempty"`
	MergedAt      string   `json:"merged_at,omitempty" yaml:"merged_at,omitempty"`
	Version       string   `json:"version,omitempty" yaml:"version,omitempty"`
}

// MergeOptions controls merge behavior.
type MergeOptions struct {
	AppendArrays    bool // If true, arrays are appended instead of replaced
	DryRun          bool // If true, print to stdout only
	OutputFormat    string // "yaml", "json", or auto-detect
	SortTags        bool   // Sort tags alphabetically after merge
}

// DefaultOptions returns sensible defaults.
func DefaultOptions() MergeOptions {
	return MergeOptions{
		AppendArrays:  false,
		DryRun:        false,
		OutputFormat:  "yaml",
		SortTags:      true,
	}
}

// LoadProfile reads a profile from file (YAML or JSON).
func LoadProfile(path string) (*Profile, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("reading %s: %w", path, err)
	}

	var profile Profile
	contentType := detectContentType(data)

	switch contentType {
	case "yaml":
		err = yaml.Unmarshal(data, &profile)
	case "json":
		err = json.Unmarshal(data, &profile)
	default:
		return nil, fmt.Errorf("unknown format for %s", path)
	}

	if err != nil {
		return nil, fmt.Errorf("parsing %s as %s: %w", path, contentType, err)
	}

	// Preserve source file info if not present
	if profile.Metadata.SourceFiles == nil {
		profile.Metadata.SourceFiles = []string{path}
	} else {
		// Avoid duplicates
		existing := make(map[string]bool)
		for _, f := range profile.Metadata.SourceFiles {
			existing[f] = true
		}
		if !existing[path] {
			profile.Metadata.SourceFiles = append(profile.Metadata.SourceFiles, path)
		}
	}

	return &profile, nil
}

// detectContentType determines if content is YAML or JSON.
func detectContentType(data []byte) string {
	trimmed := strings.TrimSpace(string(data))
	if len(trimmed) == 0 {
		return "yaml" // default fallback
	}

	// Check for JSON indicators
	if strings.HasPrefix(trimmed, "{") || strings.HasPrefix(trimmed, "[") {
		var temp interface{}
		if err := json.Unmarshal(data, &temp); err == nil {
			return "json"
		}
	}

	// Default to YAML (more common for profiles)
	return "yaml"
}

// MergeProfiles combines multiple profiles into one.
func MergeProfiles(profiles []*Profile, opts MergeOptions) (*Profile, error) {
	if len(profiles) == 0 {
		return nil, fmt.Errorf("no profiles provided")
	}

	result := &Profile{}

	// Initialize result with defaults from first profile
	result = profiles[0]

	for _, p := range profiles[1:] {
		mergeInto(result, p, opts)
	}

	// Apply post-merge processing
	if opts.SortTags && len(result.Tags) > 0 {
		sort.Strings(result.Tags)
	}

	return result, nil
}

// mergeInto performs a deep merge of two profiles.
func mergeInto(base, overlay *Profile, opts MergeOptions) {
	// Name fields: last one wins
	if overlay.Name != "" {
		base.Name = overlay.Name
	}
	if overlay.DisplayName != "" {
		base.DisplayName = overlay.DisplayName
	}

	// Bio: append if non-empty and not already set
	if overlay.Bio != "" && base.Bio == "" {
		base.Bio = overlay.Bio
	} else if overlay.Bio != "" && base.Bio != "" {
		// If both have bios, overlay wins (or could concatenate)
		base.Bio = overlay.Bio
	}

	// URL: last one wins
	if overlay.URL != "" {
		base.URL = overlay.URL
	}

	// Email: last one wins
	if overlay.Email != "" {
		base.Email = overlay.Email
	}

	// Location: last one wins
	if overlay.Location != "" {
		base.Location = overlay.Location
	}

	// Socials: deep merge maps
	if base.Socials == nil && overlay.Socials != nil {
		base.Socials = make(map[string]string)
		for k, v := range overlay.Socials {
			base.Socials[k] = v
		}
	} else if base.Socials != nil && overlay.Socials != nil {
		for k, v := range overlay.Socials {
			if _, exists := base.Socials[k]; !exists || opts.AppendArrays {
				base.Socials[k] = v
			}
		}
	}

	// Tags: smart merge based on options
	if len(overlay.Tags) > 0 {
		if opts.AppendArrays && len(base.Tags) == 0 {
			base.Tags = append([]string(nil), overlay.Tags...)
		} else if !opts.AppendArrays || len(base.Tags) == 0 {
			base.Tags = append([]string(nil), overlay.Tags...)
		} else {
			// Append only new tags, avoid duplicates
			existing := make(map[string]bool)
			for _, t := range base.Tags {
				existing[t] = true
			}
			for _, t := range overlay.Tags {
				if !existing[t] {
					base.Tags = append(base.Tags, t)
					existing[t] = true
				}
			}
		}
	}

	// Metadata: merge source files and timestamps
	if base.Metadata.SourceFiles == nil && overlay.Metadata.SourceFiles != nil {
		base.Metadata.SourceFiles = make([]string, 0, len(overlay.Metadata.SourceFiles))
		for _, f := range overlay.Metadata.SourceFiles {
			base.Metadata.SourceFiles = append(base.Metadata.SourceFiles, f)
		}
	} else if base.Metadata.SourceFiles != nil && overlay.Metadata.SourceFiles != nil {
		existing := make(map[string]bool)
		for _, f := range base.Metadata.SourceFiles {
			existing[f] = true
		}
		for _, f := range overlay.Metadata.SourceFiles {
			if !existing[f] {
				base.Metadata.SourceFiles = append(base.Metadata.SourceFiles, f)
				existing[f] = true
			}
		}
	}

	// Set merge timestamp if not present
	if base.Metadata.MergedAt == "" {
		import "time"
		base.Metadata.MergedAt = time.Now().Format(time.RFC3339)
	}
}

// Import: need to fix the import above - let me restructure
func main() {
	var opts MergeOptions = DefaultOptions()
	var inputs []string
	var output string

	// Parse command-line arguments
	for _, arg := range os.Args[1:] {
		if strings.HasPrefix(arg, "--") {
			switch arg {
			case "--help", "-h":
				printUsage()
				os.Exit(0)
			case "--dry-run", "-d":
				opts.DryRun = true
			case "--format=":
				format := strings.TrimPrefix(arg, "--format=")
				if format != "yaml" && format != "json" {
					fmt.Fprintf(os.Stderr, "Invalid format. Use 'yaml' or 'json'.\n")
					os.Exit(1)
				}
				opts.OutputFormat = format
			case "--sort-tags":
				opts.SortTags = true
			case "--no-sort-tags":
				opts.SortTags = false
			default:
				fmt.Fprintf(os.Stderr, "Unknown option: %s\n", arg)
				printUsage()
				os.Exit(1)
			}
		} else if strings.HasPrefix(arg, "-") {
			switch arg {
			case "--help":
				printUsage()
				os.Exit(0)
			case "-d":
				opts.DryRun = true
			default:
				inputs = append(inputs, arg)
			}
		} else if !strings.HasPrefix(arg, "-") && !opts.DryRun {
			if output == "" {
				output = arg
			} else {
				fmt.Fprintf(os.Stderr, "Multiple output files not supported yet.\n")
				os.Exit(1)
			}
		} else if opts.DryRun {
			inputs = append(inputs, arg)
		}
	}

	if len(inputs) == 0 {
		fmt.Fprintf(os.Stderr, "No input files specified.\n\n")
		printUsage()
		os.Exit(1)
	}

	if output == "" && !opts.DryRun {
		output = "merged_profile.yaml"
	}

	// Load all profiles
	var profiles []*Profile
	for _, path := range inputs {
		absPath, err := filepath.Abs(path)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error resolving %s: %v\n", path, err)
			os.Exit(1)
		}

		p, err := LoadProfile(absPath)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Warning: %v\n", err)
			continue // Continue with remaining files
		}

		profiles = append(profiles, p)
		fmt.Printf("Loaded: %s\n", absPath)
	}

	if len(profiles) == 0 {
		fmt.Fprintf(os.Stderr, "No valid profiles found.\n")
		os.Exit(1)
	}

	// Merge profiles
	merged, err := MergeProfiles(profiles, opts)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Merge error: %v\n", err)
		os.Exit(1)
	}

	// Output result
	var outData []byte
	switch opts.OutputFormat {
	case "json":
		outData, _ = json.MarshalIndent(merged, "", "  ")
	default: // yaml
		outData, _ = yaml.Marshal(merged)
	}

	if opts.DryRun {
		fmt.Println(string(outData))
	} else {
		err = os.WriteFile(output, outData, 0644)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error writing output: %v\n", err)
			os.Exit(1)
		}
		fmt.Printf("Merged profile saved to: %s\n", output)

		// Print summary
		printSummary(merged)
	}
}

func printUsage() {
	fmt.Println(`
cognis-digital profile-merger - Merge GitHub profile configurations

Usage:
  cognis-digital merge [options] <file1.yaml> <file2.json> ...

Options:
  -d, --dry-run       Print merged result to stdout without writing
  --format=TYPE       Output format: yaml (default) or json
  --sort-tags         Sort tags alphabetically after merge
  --no-sort-tags      Keep original tag order
  -h, --help          Show this help message

Examples:
  # Merge two YAML files and save output
  cognis-digital merge profile1.yaml profile2.yaml > merged.yaml

  # Dry run to preview result
  cognis-digital merge -d profile1.yaml profile2.yaml

  # Merge with JSON output
  cognis-digital merge --format=json profile1.yaml profile2.yaml
`)
}

func printSummary(p *Profile) {
	fmt.Println("\n=== MERGED PROFILE SUMMARY ===")
	if p.Name != "" {
		fmt.Printf("Name: %s\n", p.Name)
	}
	if p.DisplayName != "" {
		fmt.Printf("Display Name: %s\n", p.DisplayName)
	}
	if p.Bio != "" {
		fmt.Printf("Bio: %s\n", p.Bio)
	}
	if p.URL != "" {
		fmt.Printf("URL: %s\n", p.URL)
	}
	if p.Email != "" {
		fmt.Printf("Email: %s\n", p.Email)
	}
	if p.Location != "" {
		fmt.Printf("Location: %s\n", p.Location)
	}

	if len(p.Socials) > 0 {
		fmt.Println("\nSocial Links:")
		for k, v := range p.Socials {
			fmt.Printf("  - %s: %s\n", k, v)
		}
	}

	if len(p.Tags) > 0 {
		fmt.Printf("\nTags (%d): ", len(p.Tags))
		fmt.Print(strings.Join(p.Tags, ", "))
	}

	if !p.Metadata.MergedAt == "" {
		fmt.Printf("\nMerged at: %s\n", p.Metadata.MergedAt)
	}

	fmt.Println("==============================")
}