"""
polyglot/python/profile_parser.py

GitHub Profile Configuration Parser

Parses various config formats (YAML, JSON, TOML) for GitHub profile data.
Auto-detects format from extension or content. Validates common fields.
"""

import json
import os
from pathlib import Path
from typing import Any, Dict, Optional, Tuple, Union

try:
    import tomllib  # Python 3.11+
except ImportError:
    try:
        import tomli as tomllib  # Backport for older versions
    except ImportError:
        tomllib = None


class ProfileParseError(Exception):
    """Raised when profile parsing fails."""

    def __init__(self, message: str, raw_data: Optional[Dict] = None):
        super().__init__(message)
        self.raw_data = raw_data


class GitHubProfileParser:
    """
    Parser for GitHub profile configuration files.

    Supports YAML, JSON, and TOML formats with auto-detection.
    Extracts common fields like username, name, bio, location, website, etc.
    """

    # Common GitHub profile fields we care about
    KNOWN_FIELDS = {
        'username', 'name', 'bio', 'location', 'website', 
        'email', 'avatar_url', 'twitter_username', 'github_id'
    }

    def __init__(self, strict: bool = False):
        """
        Initialize parser.

        Args:
            strict: If True, raise errors on unknown fields instead of warning.
        """
        self.strict = strict
        self.warnings: list[str] = []

    def _detect_format(self, filepath: str | Path) -> str:
        """Auto-detect config format from extension or content."""
        path = Path(filepath)
        
        # Check extension first
        ext_map = {
            '.yaml': 'yaml',
            '.yml': 'yaml',
            '.json': 'json',
            '.toml': 'toml',
        }

        if str(path.suffix).lower() in ext_map:
            return ext_map[str(path.suffix).lower()]

        # Fallback to content detection (first 2KB)
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                sample = f.read(2048)
            
            if sample.strip().startswith('{'):
                return 'json'
            elif sample.strip().startswith('['):
                # Could be JSON or YAML array - try both
                pass
        except (OSError, UnicodeDecodeError):
            pass

        # Default to YAML as it's most flexible for configs
        return 'yaml'

    def _parse_json(self, data: Union[str, bytes], filepath: str | Path) -> Dict:
        """Parse JSON content."""
        try:
            if isinstance(data, (str, bytes)):
                decoded = data.decode('utf-8') if isinstance(data, bytes) else data
                return json.loads(decoded)
        except json.JSONDecodeError as e:
            raise ProfileParseError(f"JSON parse error in {filepath}: {e}", 
                                   self._read_raw(filepath))

    def _parse_yaml(self, data: Union[str, bytes], filepath: str | Path) -> Dict:
        """Parse YAML content."""
        try:
            import yaml
            if isinstance(data, (str, bytes)):
                decoded = data.decode('utf-8') if isinstance(data, bytes) else data
                return yaml.safe_load(decoded) or {}
        except ImportError:
            raise ProfileParseError(
                "PyYAML not installed. Run: pip install pyyaml", filepath)
        except yaml.YAMLError as e:
            raise ProfileParseError(f"YAML parse error in {filepath}: {e}", 
                                   self._read_raw(filepath))

    def _parse_toml(self, data: Union[str, bytes], filepath: str | Path) -> Dict:
        """Parse TOML content."""
        if tomllib is None:
            raise ProfileParseError(
                "tomllib not available. Install: pip install tomli", filepath)

        try:
            if isinstance(data, (str, bytes)):
                decoded = data.decode('utf-8') if isinstance(data, bytes) else data
                return tomllib.loads(decoded)
        except Exception as e:
            raise ProfileParseError(f"TOML parse error in {filepath}: {e}", 
                                   self._read_raw(filepath))

    def _read_raw(self, filepath: str | Path) -> Union[str, bytes]:
        """Read file content safely."""
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                return f.read()
        except (OSError, UnicodeDecodeError) as e:
            raise ProfileParseError(f"Failed to read {filepath}: {e}")

    def _extract_profile_data(self, data: Dict) -> Dict[str, Any]:
        """Extract relevant profile fields from parsed config."""
        result = {}

        # Flatten nested keys (common GitHub profile structure)
        flatten_dict = lambda d, parent='': 
            {k: v for k, v in self._flatten(d, parent).items()}

        flat_data = flatten_dict(data)

        # Extract known fields directly
        for field in self.KNOWN_FIELDS:
            if field in flat_data and flat_data[field]:
                result[field] = flat_data[field]

        # Handle common nested structures
        if 'github' in data:
            github = data['github']
            if isinstance(github, dict):
                for key in self.KNOWN_FIELDS:
                    if key in github and github[key]:
                        result[key] = github[key]

        # Normalize some field names
        normalizations = {
            'user': 'username',
            'handle': 'username',
            'display_name': 'name',
            'full_name': 'name',
            'profile_url': 'website',
            'blog': 'website',
            'twitter': 'twitter_username',
        }

        for old, new in normalizations.items():
            if old in flat_data and flat_data[old]:
                result[new] = flat_data[old]

        return result

    def _flatten(self, data: Any, parent: str = '') -> Dict[str, Any]:
        """Flatten nested dictionary with dot notation keys."""
        items = {}
        
        if isinstance(data, dict):
            for key, value in data.items():
                new_key = f"{parent}.{key}" if parent else key
                if isinstance(value, (dict, list)):
                    items.update(self._flatten(value, new_key))
                elif not isinstance(value, (str, int, float, bool, type(None))):
                    # Convert other types to string representation
                    items[new_key] = str(value)
                else:
                    items[new_key] = value
        elif isinstance(data, list):
            for i, item in enumerate(data):
                if isinstance(item, dict):
                    items.update(self._flatten(item, f"{parent}[{i}]"))
                else:
                    items[f"{parent}[{i}]"] = str(item)

        return items

    def _validate_profile(self, data: Dict) -> list[str]:
        """Validate extracted profile data."""
        errors = []
        warnings_list = self.warnings.copy()

        # Check for required fields if strict mode
        if self.strict and not any(data.get(k) for k in ['username', 'name']):
            errors.append("No username or name found (strict mode)")

        # Validate URL formats
        website = data.get('website')
        if website:
            from urllib.parse import urlparse
            try:
                parsed = urlparse(website)
                if not parsed.netloc:
                    warnings_list.append(f"Website may be incomplete: {website}")
            except Exception:
                pass

        # Validate email format
        email = data.get('email')
        if email and '@' not in email:
            warnings_list.append("Email may lack @ symbol")

        return errors, warnings_list

    def parse(self, filepath: str | Path) -> Tuple[Dict[str, Any], list[str]]:
        """
        Parse a GitHub profile config file.

        Args:
            filepath: Path to the configuration file.

        Returns:
            Tuple of (extracted_profile_data, warnings).
        """
        self.warnings = []
        
        # Auto-detect format
        fmt = self._detect_format(filepath)
        
        # Parse based on detected format
        if fmt == 'json':
            raw = self._read_raw(filepath)
            data = self._parse_json(raw, filepath)
        elif fmt == 'yaml':
            raw = self._read_raw(filepath)
            data = self._parse_yaml(raw, filepath)
        elif fmt == 'toml':
            raw = self._read_raw(filepath)
            data = self._parse_toml(raw, filepath)
        else:
            raise ProfileParseError(f"Unsupported format detected: {fmt}")

        # Extract profile data
        profile = self._extract_profile_data(data)

        # Validate and collect warnings
        errors, more_warnings = self._validate_profile(profile)

        if errors:
            for err in errors:
                self.warnings.append(err)

        return profile, self.warnings

    def parse_string(self, content: str, 
                     filename: Optional[str] = None) -> Tuple[Dict[str, Any], list[str]]:
        """Parse a config string directly."""
        if not filename:
            filename = '<string>'

        fmt = self._detect_format(filename or 'string')
        
        if fmt == 'json':
            data = self._parse_json(content, filename)
        elif fmt == 'yaml':
            data = self._parse_yaml(content, filename)
        elif fmt == 'toml':
            data = self._parse_toml(content, filename)
        else:
            raise ProfileParseError(f"Unsupported format: {fmt}")

        profile = self._extract_profile_data(data)
        errors, warnings = self._validate_profile(profile)

        return profile, warnings


def create_default_config() -> Dict[str, Any]:
    """Create a sample GitHub profile config for reference."""
    return {
        'github': {
            'username': 'johndoe',
            'name': 'John Doe',
            'bio': 'Python developer and open source enthusiast',
            'location': 'San Francisco, CA',
            'website': 'https://example.com/blog',
            'email': 'john@example.com',
            'twitter_username': '@johndoe_dev',
        }
    }


def main():
    """Demo: Parse a sample config and show results."""
    import sys

    parser = GitHubProfileParser(strict=False)

    # Example 1: Parse from file (if provided as argument)
    if len(sys.argv) > 1:
        filepath = sys.argv[1]
        try:
            profile, warnings = parser.parse(filepath)
            
            print(f"=== Parsed Profile ===")
            for key in ['username', 'name', 'bio', 'location', 'website']:
                if key in profile:
                    print(f"  {key}: {profile[key]}")
            
            if warnings:
                print(f"\nWarnings:")
                for w in warnings:
                    print(f"  - {w}")

        except ProfileParseError as e:
            print(f"Error: {e}")
    else:
        # Example 2: Parse a string directly
        sample = """
github:
  username: demo_user
  name: Demo User
  bio: Testing the parser
  location: Remote
"""
        profile, warnings = parser.parse_string(sample)

        print("=== Sample Parse Result ===")
        for key in ['username', 'name', 'bio', 'location']:
            if key in profile:
                print(f"  {key}: {profile[key]}")


if __name__ == '__main__':
    main()