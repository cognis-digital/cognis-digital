use reqwest::Client;
use serde::{Deserialize, Serialize};
use std::env;
use std::fs;
use std::path::PathBuf;
use std::process;

/// GitHub API response structure
#[derive(Debug, Deserialize)]
struct ApiResponse {
    login: String,
    name: Option<String>,
    email: Option<String>,
    bio: Option<String>,
    location: Option<String>,
    avatar_url: String,
    html_url: String,
}

/// Generated profile configuration
#[derive(Debug, Serialize)]
struct ProfileConfig {
    tool: String,
    github: GitHubProfile,
    #[serde(default)]
    metadata: Metadata,
}

#[derive(Debug, Serialize)]
struct GitHubProfile {
    username: String,
    name: Option<String>,
    email: Option<String>,
    bio: Option<String>,
    location: Option<String>,
    avatar_url: String,
    html_url: String,
}

#[derive(Debug, Serialize)]
struct Metadata {
    generated_by: String,
    version: String,
}

impl ProfileConfig {
    fn new(username: &str) -> Self {
        ProfileConfig {
            tool: "cognis-digital".to_string(),
            github: GitHubProfile {
                username: username.to_string(),
                name: None,
                email: None,
                bio: None,
                location: None,
                avatar_url: String::new(),
                html_url: format!("https://github.com/{}", username),
            },
            metadata: Metadata {
                generated_by: "cognis-digital".to_string(),
                version: env!("CARGO_PKG_VERSION").to_string(),
            },
        }
    }

    async fn fetch_from_github(&mut self, client: &Client) -> Result<(), Box<dyn std::error::Error>> {
        let url = format!("{}/{}", self.github.html_url, "api/v1");
        
        match client.get(&url).send().await {
            Ok(response) => {
                if response.status().is_success() {
                    let api_response: ApiResponse = response.json().await?;
                    
                    self.github.name = api_response.name.or(api_response.login.clone());
                    self.github.email = api_response.email;
                    self.github.bio = api_response.bio;
                    self.github.location = api_response.location;
                    self.github.avatar_url = api_response.avatar_url;
                } else {
                    println!("GitHub API returned status: {}", response.status());
                }
            }
            Err(e) => {
                eprintln!("Failed to fetch GitHub data: {}", e);
            }
        }

        Ok(())
    }

    fn generate_yaml(&self, output_path: &PathBuf) -> Result<(), Box<dyn std::error::Error>> {
        let yaml_content = serde_yaml::to_string(self)?;
        fs::write(output_path, yaml_content)?;
        println!("Profile generated at: {:?}", output_path);
        Ok(())
    }

    fn generate_json(&self, output_path: &PathBuf) -> Result<(), Box<dyn std::error::Error>> {
        let json_content = serde_json::to_string_pretty(self)?;
        fs::write(output_path, json_content)?;
        println!("Profile generated at: {:?}", output_path);
        Ok(())
    }

    fn print_summary(&self) {
        println!("\n=== Profile Summary ===");
        println!("Username: {}", self.github.username);
        
        if let Some(name) = &self.github.name {
            println!("Name: {}", name);
        } else {
            println!("Name: (not found)");
        }

        if let Some(email) = &self.github.email {
            println!("Email: {}", email);
        } else {
            println!("Email: (not found)");
        }

        if let Some(bio) = &self.github.bio {
            println!("Bio: {}", bio);
        } else {
            println!("Bio: (not set)");
        }

        if let Some(location) = &self.github.location {
            println!("Location: {}", location);
        } else {
            println!("Location: (not set)");
        }

        println!("\nAvatar URL: {}", self.github.avatar_url);
    }
}

fn default_output_path() -> PathBuf {
    let home = env::var("HOME").unwrap_or(env::var("USERPROFILE").unwrap_or_default());
    let config_dir = format!("{}/.config/cognis-digital", home);
    PathBuf::from(config_dir)
}

async fn run(username: &str, output_format: OutputFormat) {
    let client = Client::new();
    
    // Fetch real data from GitHub API
    println!("Fetching profile for: {}", username);
    let mut config = ProfileConfig::new(username);
    if let Err(e) = config.fetch_from_github(&client).await {
        eprintln!("Warning: {}", e);
    }

    // Print summary to console
    config.print_summary();

    // Generate output file
    match output_format {
        OutputFormat::Yaml => {
            if let Err(e) = config.generate_yaml(&default_output_path()) {
                eprintln!("Error generating YAML: {}", e);
                process::exit(1);
            }
        },
        OutputFormat::Json => {
            if let Err(e) = config.generate_json(&default_output_path()) {
                eprintln!("Error generating JSON: {}", e);
                process::exit(1);
            }
        },
    }

    println!("\nDone!");
}

#[derive(Debug, Clone, Copy)]
enum OutputFormat {
    Yaml,
    Json,
}

fn parse_args() -> Result<(String, OutputFormat), String> {
    let args: Vec<String> = env::args().collect();
    
    if args.len() < 2 {
        return Err("Usage: cognis-digital <username> [--yaml | --json]".to_string());
    }

    let username = &args[1];
    let format = match args.get(2).map(|s| s.as_str()) {
            Some("--yaml") => OutputFormat::Yaml,
            Some("--json") => OutputFormat::Json,
            _ => OutputFormat::Yaml, // default
        };

    Ok((username.to_string(), format))
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let (username, output_format) = parse_args()?;
    
    println!("cognis-digital profile generator");
    println!("=================================");
    println!();
    
    // Run the async logic
    tokio::runtime::Runtime::new()?.block_on(run(&username, output_format));

    Ok(())
}