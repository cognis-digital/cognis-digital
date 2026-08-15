#!/usr/bin/env python3
"""
polyglot/python/profile_merger.py

Merges multiple GitHub profile configuration files into a single, valid output.
Supports JSON, YAML, and TOML formats with deep merge semantics.
"""

import argparse
import json
import os
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Dict, List, Optional

try:
    import yaml
except ImportError:
    yaml = None

try:
    import toml
except ImportError:
    toml = None


@dataclass
class MergeConfig:
    """Configuration for the merge operation."""
    
    input_files: List[str] = field(default_factory=list)
    output_file: str = "merged_profile.json"
    format: str = "json"  # json, yaml, or toml
    overwrite: bool = False
    deep_merge: bool = True
    conflict_strategy: str = "last_wins"  # last_wins, first_wins, custom
    
    def __post_init__(self):
        if not self.input_files:
            raise ValueError("At least one input file must be specified")


def load_yaml_file(filepath: Path) -> Dict[str, Any]:
    """Load a YAML file and return its contents as a dictionary."""
    with open(filepath, 'r', encoding='utf-8') as f:
        content = yaml.safe_load(f)
        if content is None:
            return {}
        return content


def load_toml_file(filepath: Path) -> Dict[str, Any]:
    """Load a TOML file and return its contents as a dictionary."""
    with open(filepath, 'r', encoding='utf-8') as f:
        content = toml.load(f)
        if not isinstance(content, dict):
            return {}
        return content


def load_json_file(filepath: Path) -> Dict[str, Any]:
    """Load a JSON file and return its contents as a dictionary."""
    with open(filepath, 'r', encoding='utf-8') as f:
        content = json.load(f)
        if not isinstance(content, dict):
            return {}
        return content


def detect_format(filepath: Path) -> str:
    """Detect the format of a file based on extension."""
    ext = filepath.suffix.lower()
    mapping = {'.yaml': 'yaml', '.yml': 'yaml', 
               '.json': 'json', '.toml': 'toml'}
    return mapping.get(ext, 'json')


def deep_merge(base: Dict[str, Any], override: Dict[str, Any], 
               strategy: str = "last_wins") -> Dict[str, Any]:
    """
    Perform a deep merge of two dictionaries.
    
    Args:
        base: The base dictionary (existing profile)
        override: The overriding dictionary (new values)
        strategy: Conflict resolution strategy
        
    Returns:
        A new merged dictionary
    """
    result = dict(base)
    
    for key, value in override.items():
        if key not in result:
            # New key - just add it
            result[key] = value
        elif isinstance(result[key], dict) and isinstance(value, dict):
            # Both are dicts - recurse deeper
            result[key] = deep_merge(result[key], value, strategy)
        else:
            # Conflict - apply strategy
            if strategy == "last_wins":
                result[key] = value
            elif strategy == "first_wins":
                pass  # Keep original
            elif strategy == "custom":
                raise ValueError("Custom strategy requires a callback function")
    
    return result


def validate_profile(profile: Dict[str, Any]) -> List[str]:
    """
    Validate that the profile contains required fields.
    
    Returns a list of validation errors (empty if valid).
    """
    errors = []
    
    # Required top-level keys for GitHub profiles
    required_keys = ['username', 'name']
    
    missing = [key for key in required_keys if key not in profile]
    if missing:
        errors.append(f"Missing required fields: {', '.join(missing)}")
    
    # Validate types
    if 'email' in profile and not isinstance(profile['email'], str):
        errors.append("'email' must be a string")
    
    if 'bio' in profile and (not isinstance(profile['bio'], str) or 
                            len(profile['bio']) > 2000):
        pass  # Allow bio to be optional
    
    return errors


def load_all_files(config: MergeConfig) -> List[Dict[str, Any]]:
    """Load all input files and return their contents."""
    loaded = []
    
    for filepath in config.input_files:
        path = Path(filepath)
        
        if not path.exists():
            print(f"Warning: File not found - {path}")
            continue
        
        fmt = detect_format(path)
        
        try:
            if fmt == 'yaml':
                data = load_yaml_file(path)
            elif fmt == 'toml':
                data = load_toml_file(path)
            else:
                data = load_json_file(path)
            
            loaded.append(data)
            print(f"Loaded: {path} ({fmt})")
        except Exception as e:
            print(f"Error loading {path}: {e}")
    
    return loaded


def merge_profiles(loaded: List[Dict[str, Any]], 
                   config: MergeConfig) -> Dict[str, Any]:
    """Merge all loaded profiles into one."""
    if not loaded:
        return {}
    
    result = dict(loaded[0])
    
    for data in loaded[1:]:
        result = deep_merge(result, data, 
                          config.conflict_strategy)
    
    return result


def save_profile(profile: Dict[str, Any], output_path: Path, 
                 fmt: str) -> None:
    """Save the merged profile to a file."""
    with open(output_path, 'w', encoding='utf-8') as f:
        if fmt == 'yaml':
            yaml.dump(profile, f, default_flow_style=False, sort_keys=True)
        elif fmt == 'toml':
            import tomli_w
            toml_str = tomli_w.dumps(profile)
            f.write(toml_str)
        else:  # json
            json.dump(profile, f, indent=2, ensure_ascii=False, sort_keys=True)


def main() -> int:
    """Main entry point for the CLI."""
    parser = argparse.ArgumentParser(
        description="Merge multiple GitHub profile configuration files."
    )
    
    parser.add_argument("files", nargs="+", 
                       help="Input config files to merge")
    parser.add_argument("-o", "--output", default="merged_profile.json",
                       help="Output file path (default: merged_profile.json)")
    parser.add_argument("--format", choices=["json", "yaml", "toml"],
                       default="json", help="Output format")
    parser.add_argument("--no-deep-merge", action="store_true",
                       help="Shallow merge instead of deep merge")
    parser.add_argument("-v", "--verbose", action="store_true",
                       help="Enable verbose output")
    
    args = parser.parse_args()
    
    config = MergeConfig(
        input_files=args.files,
        output_file=args.output,
        format=args.format,
        deep_merge=not args.no_deep_merge,
    )
    
    # Load all files
    loaded = load_all_files(config)
    
    if not loaded:
        print("No valid profiles found to merge.")
        return 1
    
    # Merge them
    merged = merge_profiles(loaded, config)
    
    # Validate
    errors = validate_profile(merged)
    if errors:
        print("\nValidation warnings:")
        for error in errors:
            print(f"  - {error}")
    
    # Save output
    output_path = Path(args.output)
    save_profile(merged, output_path, config.format)
    
    print(f"\nMerged profile saved to: {output_path.resolve()}")
    
    if args.verbose and errors:
        return 1
    
    return 0


if __name__ == "__main__":
    sys.exit(main() or 0)