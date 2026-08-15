use serde::{Deserialize, Serialize};
use std::collections::BTreeMap;
use std::fs;
use std::path::PathBuf;

/// GitHub profile configuration structure
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct ProfileConfig {
    pub name: Option<String>,
    pub email: Option<String>,
    pub bio: Option<String>,
    pub location: Option<String>,
    pub website: Option<String>,
    pub twitter: Option<String>,
    pub linkedin: Option<String>,
    pub github: Option<String>,
    pub avatar_url: Option<String>,
    pub pronouns: Option<String>,
}

/// Merge strategy configuration
#[derive(Debug, Clone, Default)]
pub struct MergeStrategy {
    /// Fields to always override (later wins)
    pub override_fields: Vec<&'static str>,
    /// Fields that are additive (concatenated with separator)
    pub add_fields: Vec<&'static str>,
    /// Separator for additive fields
    pub add_separator: &'static str,
}

impl Default for MergeStrategy {
    fn default() -> Self {
        Self {
            override_fields: vec![
                "name", "email", "bio", "location", "website",
                "twitter", "linkedin", "github", "avatar_url", "pronouns"
            ],
            add_fields: vec![],
            add_separator: ", ",
        }
    }
}

/// Result of a merge operation
#[derive(Debug)]
pub struct MergeResult {
    pub merged: ProfileConfig,
    pub sources_merged: Vec<PathBuf>,
    pub warnings: Vec<String>,
    pub errors: Vec<(PathBuf, String)>,
}

impl MergeResult {
    /// Returns true if merge was successful with no critical issues
    pub fn is_success(&self) -> bool {
        self.errors.is_empty() && !self.warnings.iter().any(|w| w.contains("missing"))
    }
    
    /// Prints a human-readable summary of the result
    pub fn print_summary(&self) {
        println!("=== Merge Result Summary ===");
        
        if self.is_success() {
            println!("\n✓ SUCCESS: Merged {} source files", self.sources_merged.len());
        } else {
            println!("\n⚠ PARTIAL: Some issues occurred during merge");
        }
        
        if !self.warnings.is_empty() {
            println!("\nWarnings ({})", self.warnings.len());
            for w in &self.warnings {
                println!("  - {}", w);
            }
        }
        
        if !self.errors.is_empty() {
            println!("\nErrors ({})", self.errors.len());
            for (path, msg) in &self.errors {
                println!("  [{}] {}", path.display(), msg);
            }
        }
    }
}

/// Load a profile config from a file
fn load_config(path: &PathBuf) -> Result<ProfileConfig, String> {
    let content = fs::read_to_string(&path).map_err(|e| format!("Failed to read {}: {}", path.display(), e))?;
    
    let mut config: ProfileConfig = serde_yaml::from_str(&content)
        .or_else(|_| serde_json::from_str(&content))
        .map_err(|e| format!("Parse error in {}: {}", path.display(), e))?;
    
    // Fill missing fields with defaults from environment or previous config
    if config.name.is_none() {
        config.name = std::env::var("GITHUB_NAME").ok();
    }
    if config.email.is_none() {
        let home_dir = dirs::home_dir().map(|d| format!("{}/", d.display()));
        config.email = std::env::var("GITHUB_EMAIL").or(home_dir).ok();
    }
    
    Ok(config)
}

/// Merge multiple configs into one using the specified strategy
pub fn merge_configs(
    paths: &[PathBuf],
    strategy: &MergeStrategy,
) -> MergeResult {
    let mut merged = ProfileConfig::default();
    let mut sources_merged = Vec::new();
    let mut warnings = Vec::new();
    let mut errors = Vec::new();

    for path in paths.iter() {
        match load_config(path.clone()) {
            Ok(config) => {
                // Apply merge strategy
                apply_merge_strategy(&mut merged, &config, strategy);
                
                sources_merged.push(path.clone());
                
                if config.name.is_some() || config.email.is_some() {
                    warnings.push(format!("Overriding existing values from {}", path.display()));
                }
            },
            Err(e) => {
                errors.push((path.clone(), e));
            }
        }
    }

    MergeResult {
        merged,
        sources_merged,
        warnings,
        errors,
    }
}

/// Apply the merge strategy between two configs
fn apply_merge_strategy(
    target: &mut ProfileConfig,
    source: &ProfileConfig,
    strategy: &MergeStrategy,
) {
    // Override fields: later value wins
    for field in &strategy.override_fields {
        match *field {
            "name" => target.name = source.name.clone(),
            "email" => target.email = source.email.clone(),
            "bio" => target.bio = source.bio.clone(),
            "location" => target.location = source.location.clone(),
            "website" => target.website = source.website.clone(),
            "twitter" => target.twitter = source.twitter.clone(),
            "linkedin" => target.linkedin = source.linkedin.clone(),
            "github" => target.github = source.github.clone(),
            "avatar_url" => target.avatar_url = source.avatar_url.clone(),
            "pronouns" => target.pronouns = source.pronouns.clone(),
            _ => {}
        }
    }

    // Additive fields: concatenate with separator
    for field in &strategy.add_fields {
        match *field {
            "bio" if let (Some(ref a), Some(ref b)) = (&target.bio, &source.bio) => {
                target.bio = format!("{}{}{}", a, strategy.add_separator, b);
            },
            _ => {}
        }
    }
}

/// Write the merged config to a file
pub fn write_merged(merged: &ProfileConfig, path: &PathBuf) -> Result<(), String> {
    let content = serde_yaml::to_string(merged).map_err(|e| format!("Serialization error: {}", e))?;
    
    fs::write(path, &content).map_err(|e| format!("Write error to {}: {}", path.display(), e))
}

/// Demo / Main entry point with example usage
#[cfg(test)]
mod tests {
    use super::*;
    use std::io::Write;
    use tempfile::NamedTempFile;

    fn create_test_file(path: &PathBuf, content: &str) -> PathBuf {
        let mut file = fs::File::create(path).unwrap();
        file.write_all(content.as_bytes()).unwrap();
        path.to_path_buf()
    }

    #[test]
    fn test_basic_merge() {
        // Create two test config files
        let base_config = r#"
name: "Base User"
bio: "Base bio text"
email: "base@example.com"
"#;
        
        let override_config = r#"
name: "Override User"
bio: "Override bio text"
website: "https://example.com"
"#;

        let temp_dir = tempfile::tempdir().unwrap();
        let base_path = create_test_file(
            &temp_dir.path().join("base.yaml"), 
            base_config
        );
        
        let override_path = create_test_file(
            &temp_dir.path().join("override.yaml"),
            override_config
        );

        // Perform merge
        let result = merge_configs(&[base_path, override_path], &MergeStrategy::default());
        
        assert!(result.is_success());
        assert_eq!(result.merged.name.as_deref(), Some("Override User"));
        assert_eq!(result.merged.bio.as_deref(), Some("Override bio text"));
        assert_eq!(result.merged.website.as_deref(), Some("https://example.com"));
    }

    #[test]
    fn test_partial_merge() {
        let config = r#"
name: "Test User"
email: "test@example.com"
"#;

        let temp_dir = tempfile::tempdir().unwrap();
        let path = create_test_file(&temp_dir.path().join("config.yaml"), config);

        let result = merge_configs(&[path], &MergeStrategy::default());
        
        assert!(result.is_success());
        assert_eq!(result.merged.name.as_deref(), Some("Test User"));
    }

    #[test]
    fn test_parse_error_handling() {
        let invalid_yaml = r#"not valid yaml: [
"#;

        let temp_dir = tempfile::tempdir().unwrap();
        let path = create_test_file(&temp_dir.path().join("invalid.yaml"), invalid_yaml);

        let result = merge_configs(&[path], &MergeStrategy::default());
        
        assert!(!result.is_success());
        assert_eq!(result.errors.len(), 1);
    }
}

fn main() {
    println!("cognis-digital: Profile Merger");
    println!("==============================\n");

    // Default demo configs
    let base_config = r#"---
name: "Jane Doe"
email: "jane@example.com"
bio: "Software engineer passionate about Rust and systems programming."
location: "San Francisco, CA"
website: "https://janedoe.dev"
twitter: "@janedoe_dev"
github: "janedoe"
avatar_url: "https://example.com/avatar.jpg"
pronouns: "she/her"
"#;

    let override_config = r#"---
name: "Jane Doe"
bio: "Software engineer passionate about Rust and systems programming. Also loves hiking."
website: "https://janedoe.dev/blog"
"#;

    // Create temporary files for demo
    let temp_dir = tempfile::tempdir().unwrap();
    let base_path = create_test_file(
        &temp_dir.path().join("profile.base.yaml"), 
        base_config
    );
    
    let override_path = create_test_file(
        &temp_dir.path().join("profile.override.yaml"),
        override_config
    );

    // Merge with default strategy (override fields take precedence)
    println!("Merging 2 config files...");
    let result = merge_configs(&[base_path, override_path], &MergeStrategy::default());

    // Print summary
    result.print_summary();

    // Show merged result
    println!("\n=== Merged Profile ===\n");
    if let Some(name) = &result.merged.name {
        println!("Name: {}", name);
    }
    if let Some(email) = &result.merged.email {
        println!("Email: {}", email);
    }
    if let Some(bio) = &result.merged.bio {
        println!("\nBio:\n  \"{}\"", bio);
    }

    // Save merged result to output file
    let output_path = temp_dir.path().join("profile_merged.yaml");
    match write_merged(&result.merged, &output_path) {
        Ok(_) => println!("\n✓ Merged profile saved to: {}", output_path.display()),
        Err(e) => eprintln!("Error saving merged profile: {}", e),
    }

    // Run unit tests (optional - uncomment if you want them in main run)
    // tests::test_basic_merge();
    // tests::test_partial_merge();
    // tests::test_parse_error_handling();
}