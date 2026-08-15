"""
cognis-digital: GitHub Profile Generator

A self-contained tool to generate configuration files and documentation
for your GitHub profile from structured data or environment variables.

Usage:
    python -m polyglot.python.profile_generator --help
    
Or set environment variables and run:
    export GITHUB_NAME="Jane Doe"
    export GITHUB_BIO="Python developer..."
    python -m polyglot.python.profile_generator
"""

import argparse
import json
import os
import sys
from dataclasses import asdict, dataclass, field
from pathlib import Path
from typing import Optional


@dataclass
class SocialLink:
    platform: str
    url: str
    
    def to_dict(self) -> dict:
        return {"platform": self.platform, "url": self.url}


@dataclass
class Project:
    name: str
    description: str = ""
    url: Optional[str] = None
    tags: list[str] = field(default_factory=list)
    
    def to_dict(self) -> dict:
        result = {"name": self.name, "description": self.description}
        if self.url:
            result["url"] = self.url
        if self.tags:
            result["tags"] = self.tags
        return result


@dataclass
class ProfileData:
    name: str = ""
    bio: str = ""
    website: Optional[str] = None
    email: Optional[str] = None
    location: Optional[str] = None
    avatar_url: Optional[str] = None
    
    # Social links
    twitter: Optional[SocialLink] = None
    linkedin: Optional[SocialLink] = None
    github: Optional[SocialLink] = None
    
    # Projects
    projects: list[Project] = field(default_factory=list)
    
    # Skills/tech stack
    skills: list[str] = field(default_factory=list)
    
    # Metadata
    created_at: str = ""
    
    def to_dict(self) -> dict:
        result = {k: v for k, v in asdict(self).items() if v is not None}
        
        # Convert nested objects
        if self.twitter:
            result["twitter"] = self.twitter.to_dict()
        if self.linkedin:
            result["linkedin"] = self.linkedin.to_dict()
        if self.github:
            result["github"] = self.github.to_dict()
        
        return result


def get_env_default(name: str, default: str) -> str:
    """Get value from environment or use default."""
    env_name = f"GITHUB_{name.upper().replace('.', '_')}"
    return os.environ.get(env_name, default)


def load_existing_profile(path: Path) -> Optional[ProfileData]:
    """Load profile data from a JSON file if it exists."""
    try:
        with open(path, "r", encoding="utf-8") as f:
            data = json.load(f)
        
        # Reconstruct nested objects
        result = ProfileData()
        for key in ["name", "bio", "website", "email", "location", 
                    "avatar_url", "created_at"]:
            if key in data and data[key]:
                setattr(result, key, data[key])
        
        # Handle social links
        for platform in ["twitter", "linkedin", "github"]:
            if platform in data:
                link_data = data[platform]
                result.__setattr__(platform, SocialLink(
                    platform=link_data.get("platform", ""),
                    url=link_data["url"]
                ))
        
        # Handle projects
        if "projects" in data:
            for proj_data in data["projects"]:
                result.projects.append(Project(**proj_data))
        
        # Handle skills
        if "skills" in data:
            result.skills = data["skills"]
        
        return result
    except (FileNotFoundError, json.JSONDecodeError):
        return None


def create_profile_from_env() -> ProfileData:
    """Create a profile using environment variables."""
    profile = ProfileData(
        name=get_env_default("NAME", ""),
        bio=get_env_default("BIO", ""),
        website=get_env_default("WEBSITE", ""),
        email=get_env_default("EMAIL", ""),
        location=get_env_default("LOCATION", ""),
        avatar_url=get_env_default("AVATAR_URL", ""),
    )
    
    # Social links
    if twitter := get_env_default("TWITTER", ""):
        profile.twitter = SocialLink(platform="twitter", url=twitter)
    if linkedin := get_env_default("LINKEDIN", ""):
        profile.linkedin = SocialLink(platform="linkedin", url=linkedin)
    
    # Projects from environment
    project_count = int(get_env_default("PROJECT_COUNT", "0"))
    for i in range(project_count):
        name = f"Project {i+1}"
        desc = get_env_default(f"PROJECT_{i+1}_DESC", "")
        url = get_env_default(f"PROJECT_{i+1}_URL", "")
        tags_str = get_env_default(f"PROJECT_{i+1}_TAGS", "")
        
        if name:  # Only add non-empty projects
            profile.projects.append(Project(
                name=name,
                description=desc,
                url=url,
                tags=[t.strip() for t in tags_str.split(",")] if tags_str else []
            ))
    
    return profile


def generate_readme(profile: ProfileData) -> str:
    """Generate a Markdown README for the GitHub profile."""
    lines = [
        f"# {profile.name or 'GitHub Profile'}",
        "",
        "## About",
        "" if profile.bio else None,
        f"### Bio\n{profile.bio}" if profile.bio else None,
        "",
        "### Contact",
        "",
    ]
    
    # Add contact info
    contacts = []
    if profile.email:
        contacts.append(f"- **Email**: {profile.email}")
    if profile.website:
        contacts.append(f"- **Website**: [{profile.website}]({profile.website})")
    if profile.location:
        contacts.append(f"- **Location**: {profile.location}")
    
    # Add social links
    for platform, link in [
        ("Twitter", profile.twitter),
        ("LinkedIn", profile.linkedin),
        ("GitHub", profile.github),
    ]:
        if link and link.url:
            contacts.append(
                f"- **{platform}**: [{link.platform}]({link.url})"
            )
    
    lines.extend(contacts)
    lines.extend(["", "### Projects"])
    
    # Add projects
    for proj in profile.projects:
        lines.append(f"\n#### {proj.name}")
        if proj.description:
            lines.append(proj.description)
        if proj.url:
            lines.append(f"[{proj.name}]({proj.url})")
        if proj.tags:
            lines.append(f"*Tags*: {', '.join(proj.tags)}")
    
    # Add skills section
    if profile.skills:
        lines.extend(["", "### Skills"])
        for skill in profile.skills:
            lines.append(f"- {skill}")
    
    # Footer
    lines.extend([
        "",
        "---",
        f"*Generated on {profile.created_at}*",
    ])
    
    return "\n".join(line for line in lines if line)


def generate_json_config(profile: ProfileData, output_path: Path) -> None:
    """Generate a JSON configuration file."""
    data = profile.to_dict()
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2, ensure_ascii=False)


def generate_yaml_profile(profile: ProfileData, output_path: Path) -> None:
    """Generate a YAML profile file using simple string formatting."""
    lines = [
        "# GitHub Profile Configuration",
        "",
        "name:",
        f"  name: {profile.name or ''}",
        f"  bio: {profile.bio or ''}",
        f"  website: {profile.website or ''}",
        f"  email: {profile.email or ''}",
        f"  location: {profile.location or ''}",
        "",
    ]
    
    # Social links
    if profile.twitter and profile.twitter.url:
        lines.extend([
            "social:",
            f"  twitter: {profile.twitter.url}",
        ])
    if profile.linkedin and profile.linkedin.url:
        lines.extend([
            "  linkedin: ",
            f"    url: {profile.linkedin.url}",
        ])
    
    # Projects
    if profile.projects:
        lines.extend(["", "projects:",])
        for proj in profile.projects:
            lines.append(f"  - name: {proj.name}")
            if proj.description:
                lines.append(f"    description: {proj.description}")
            if proj.url:
                lines.append(f"    url: {proj.url}")
    
    # Skills
    if profile.skills:
        lines.extend(["", "skills:",])
        for skill in profile.skills:
            lines.append(f"  - {skill}")
    
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Generate GitHub profile configuration files.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:

  # Use environment variables
  export GITHUB_NAME="Jane Doe"
  export GITHUB_BIO="Python developer..."
  python -m polyglot.python.profile_generator
  
  # Override specific fields
  python -m polyglot.python.profile_generator --name "John Smith" \
    --bio "Full-stack developer"
  
  # Output to custom paths
  python -m polyglot.python.profile_generator \\
    --output-dir ./config/
        """
    )
    
    parser.add_argument(
        "--name", "-n",
        help="Profile name (overrides GITHUB_NAME env var)"
    )
    parser.add_argument(
        "--bio", "-b",
        help="Short bio (overrides GITHUB_BIO env var)"
    )
    parser.add_argument(
        "--website", "-w",
        help="Website URL"
    )
    parser.add_argument(
        "--email", "-e",
        help="Email address"
    )
    parser.add_argument(
        "--location", "-l",
        help="Location/City/Region"
    )
    parser.add_argument(
        "--output-dir", "-o",
        default=Path.cwd(),
        help="Output directory (default: current directory)"
    )
    parser.add_argument(
        "--json-file", "-j",
        help="Custom JSON output file path"
    )
    parser.add_argument(
        "--yaml-file", "-y",
        help="Custom YAML output file path"
    )
    parser.add_argument(
        "--readme-only",
        action="store_true",
        help="Only generate README.md, skip config files"
    )
    parser.add_argument(
        "--load-existing",
        default=Path.home() / ".cognis_profile.json",
        help="Load existing profile from this path (default: ~/.cognis_profile.json)"
    )
    
    args = parser.parse_args()
    
    # Build output paths
    output_dir = Path(args.output_dir).expanduser().resolve()
    json_path = args.json_file or output_dir / "config.json"
    yaml_path = args.yaml_file or output_dir / ".github/profile.yaml"
    readme_path = output_dir / "README.md"
    
    # Load existing profile if requested
    existing_profile = load_existing_profile(args.load_existing)
    
    # Build profile data
    if existing_profile:
        profile = existing_profile
    else:
        profile = create_profile_from_env()
    
    # Override with command-line arguments
    if args.name:
        profile.name = args.name
    if args.bio:
        profile.bio = args.bio
    if args.website:
        profile.website = args.website
    if args.email:
        profile.email = args.email
    if args.location:
        profile.location = args.location
    
    # Set creation timestamp
    import datetime
    profile.created_at = datetime.datetime.now().isoformat()
    
    # Generate outputs
    generated_files = []
    
    if not args.readme_only:
        generate_json_config(profile, json_path)
        generated_files.append(json_path.name)
        
        generate_yaml_profile(profile, yaml_path)
        generated_files.append(yaml_path.name)
    
    readme_content = generate_readme(profile)
    with open(readme_path, "w", encoding="utf-8") as f:
        f.write(readme_content)
    generated_files.append("README.md")
    
    # Print summary
    print(f"Profile generated successfully!")
    print(f"  Name: {profile.name or '(not set)'}")
    print(f"  Bio: {profile.bio[:50] + '...' if profile.bio else '(empty)'}")
    print(f"\nGenerated files:")
    for path in [json_path, yaml_path, readme_path]:
        rel = path.relative_to(output_dir)
        size = len(path.read_text(encoding="utf-8"))
        print(f"  ✓ {rel} ({size:,} bytes)")
    
    if generated_files:
        print(f"\nRun `python -m polyglot.python.profile_generator --help` for more options.")
    
    return 0


if __name__ == "__main__":
    sys.exit(main())