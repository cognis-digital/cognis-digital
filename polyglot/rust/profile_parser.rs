use serde::{Deserialize, Serialize};
use std::fs;
use std::path::PathBuf;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct GitHubProfile {
    pub name: String,
    #[serde(default)]
    pub username: Option<String>,
    #[serde(default)]
    pub email: Option<String>,
    #[serde(default)]
    pub bio: Option<String>,
    #[serde(default)]
    pub website: Option<String>,
    #[serde(default)]
    pub location: Option<String>,
    #[serde(default)]
    pub avatar_url: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ProfileConfig {
    pub github: GitHubProfile,
    #[serde(default)]
    pub metadata: Metadata,
}

#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct Metadata {
    #[serde(default)]
    pub version: String,
    #[serde(default)]
    pub created_at: Option<String>,
    #[serde(default)]
    pub updated_at: Option<String>,
}

impl GitHubProfile {
    pub fn validate(&self) -> Vec<ValidationError> {
        let mut errors = Vec::new();

        if self.name.is_empty() {
            errors.push(ValidationError::Field("name".into(), "must not be empty"));
        }

        if let Some(ref username) = self.username {
            if username.chars().count() > 50 {
                errors.push(ValidationError::Field("username".into(), "max 50 characters"));
            }
        }

        if let Some(ref email) = self.email {
            if !email.contains('@') || !email.contains('.') {
                errors.push(ValidationError::Field("email".into(), "must look like a valid email"));
            }
        }

        errors
    }

    pub fn to_json(&self, pretty: bool) -> Result<String, serde_json::Error> {
        if pretty {
            serde_json::to_string_pretty(self)
        } else {
            serde_json::to_string(self)
        }
    }

    pub fn from_json(data: &str) -> Result<Self, serde_json::Error> {
        serde_json::from_str(data)
    }
}

#[derive(Debug)]
pub enum ValidationError {
    Field(String, String),
    Config(String),
}

impl std::fmt::Display for ValidationError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ValidationError::Field(field, msg) => write!(f, "field '{}' error: {}", field, msg),
            ValidationError::Config(msg) => write!(f, "config error: {}", msg),
        }
    }
}

impl std::error::Error for ValidationError {}

pub struct ProfileParser {
    config_path: Option<PathBuf>,
    auto_detect_format: bool,
}

impl Default for ProfileParser {
    fn default() -> Self {
        Self {
            config_path: None,
            auto_detect_format: true,
        }
    }
}

impl ProfileParser {
    pub fn new(config_path: Option<PathBuf>) -> Self {
        Self {
            config_path,
            auto_detect_format: true,
        }
    }

    pub fn with_auto_detect(mut self, detect: bool) -> Self {
        self.auto_detect_format = detect;
        self
    }

    pub fn parse(&self, source: &str) -> Result<ProfileConfig, ParseError> {
        let config: ProfileConfig = match serde_yaml::from_str(source) {
            Ok(c) => c,
            Err(_) => {
                return Err(ParseError::Format("YAML".into(), "failed to parse YAML"));
            }
        };

        if !config.github.name.is_empty() || config.github.username.is_some() {
            Ok(config)
        } else {
            Err(ParseError::EmptyProfile)
        }
    }

    pub fn load(&self, path: &PathBuf) -> Result<ProfileConfig, ParseError> {
        let source = fs::read_to_string(path).map_err(|e| {
            ParseError::Io(format!("failed to read file '{}': {}", path.display(), e))
        })?;

        self.parse(&source)
    }

    pub fn save(&self, config: &ProfileConfig, path: &PathBuf) -> Result<(), SaveError> {
        let json = config.github.to_json(true)?;
        fs::write(path, json).map_err(|e| {
            SaveError::Io(format!("failed to write '{}': {}", path.display(), e))
        })?;

        Ok(())
    }
}

#[derive(Debug)]
pub enum ParseError {
    Format(String, String),
    EmptyProfile,
    Io(String),
}

impl std::fmt::Display for ParseError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ParseError::Format(format_name, msg) => write!(f, "format '{}' error: {}", format_name, msg),
            ParseError::EmptyProfile => write!(f, "profile is empty or missing required fields"),
            ParseError::Io(msg) => write!(f, "{}", msg),
        }
    }
}

impl std::error::Error for ParseError {}

#[derive(Debug)]
pub enum SaveError {
    Io(String),
}

impl std::fmt::Display for SaveError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            SaveError::Io(msg) => write!(f, "{}", msg),
        }
    }
}

impl std::error::Error for SaveError {}

fn detect_format(source: &str) -> Option<&'static str> {
    if source.starts_with("{") || source.contains(": ") {
        Some("JSON")
    } else if source.starts_with("-" | source.lines().any(|l| l.trim_start().starts_with("-"))) {
        Some("YAML")
    } else {
        None
    }
}

pub fn detect_format_from_source(source: &str) -> Option<&'static str> {
    if let Some(format) = detect_format(source) {
        return Some(format);
    }

    // Fallback to YAML as default for GitHub configs
    Some("YAML")
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_valid_profile() {
        let profile = ProfileConfig {
            github: GitHubProfile {
                name: "Jane Doe".into(),
                username: Some("janedoe".into()),
                email: Some("jane@example.com".into()),
                bio: Some("Rust enthusiast".into()),
                website: Some("https://example.com".into()),
                location: Some("San Francisco, CA".into()),
                avatar_url: None,
            },
            metadata: Metadata {
                version: "1.0.0".into(),
                created_at: Some("2024-01-01T00:00:00Z".into()),
                updated_at: Some("2024-01-01T00:00:00Z".into()),
            },
        };

        let errors = profile.github.validate();
        assert!(errors.is_empty());
    }

    #[test]
    fn test_empty_name_error() {
        let profile = ProfileConfig {
            github: GitHubProfile {
                name: String::new(),
                ..Default::default()
            },
            metadata: Metadata::default(),
        };

        let errors = profile.github.validate();
        assert!(errors.iter().any(|e| e.to_string().contains("name")));
    }

    #[test]
    fn test_json_roundtrip() {
        let profile = GitHubProfile {
            name: "Test User".into(),
            ..Default::default()
        };

        let json = profile.to_json(true).unwrap();
        let parsed: GitHubProfile = serde_json::from_str(&json).unwrap();
        
        assert_eq!(parsed.name, "Test User");
    }

    #[test]
    fn test_parse_error_display() {
        let err = ParseError::Format("YAML".into(), "invalid syntax".into());
        assert!(err.to_string().contains("format"));
    }
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("cognis-digital: GitHub Profile Parser");
    println!("=====================================");

    // Demo: Create a sample profile
    let sample_profile = ProfileConfig {
        github: GitHubProfile {
            name: "Example Developer".into(),
            username: Some("example-dev".into()),
            email: Some("dev@example.com".into()),
            bio: Some("Full-stack developer passionate about Rust and web technologies.".into()),
            website: Some("https://github.com/example-dev".into()),
            location: Some("Remote, Worldwide".into()),
            avatar_url: None,
        },
        metadata: Metadata {
            version: "1.0.0".into(),
            created_at: Some(format!(
                "{}",
                chrono::Utc::now().format("%Y-%m-%dT%H:%M:%SZ")
            )),
            updated_at: None,
        },
    };

    println!("\n--- Sample Profile ---");
    println!("{}", sample_profile.github.to_json(true)?);

    // Demo: Validate the profile
    let errors = sample_profile.github.validate();
    if errors.is_empty() {
        println!("\n✓ Validation passed!");
    } else {
        for error in &errors {
            println!("✗ {}", error);
        }
    }

    // Demo: Parse from JSON string
    let json_input = r#"{
        "github": {
            "name": "Another User",
            "username": "another-user"
        },
        "metadata": {}
    }"#;

    println!("\n--- Parsing JSON Input ---");
    match sample_profile.github.from_json(json_input) {
        Ok(parsed) => println!("✓ Parsed successfully: {}", parsed.name),
        Err(e) => println!("✗ Parse error: {}", e),
    }

    // Demo: Detect format automatically
    let yaml_input = r#"- github:
    name: YAML User
    username: yaml-user
- metadata: {}"#;

    println!("\n--- Format Detection ---");
    if let Some(detected) = detect_format_from_source(yaml_input) {
        println!("✓ Detected format: {}", detected);
    } else {
        println!("? Could not auto-detect format, defaulting to YAML");
    }

    Ok(())
}