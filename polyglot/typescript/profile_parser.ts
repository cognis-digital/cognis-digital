import * as fs from 'fs';
import * as yaml from 'yaml';

export interface ProfileConfig {
  user: UserSettings;
  repositories?: Repository[];
  links?: Link[];
  badges?: Badge[];
  metadata?: Metadata;
}

interface UserSettings {
  name: string;
  bio: string;
  location: string;
  avatar_url: string;
  hide_email?: boolean;
  email?: string;
  twitter_username?: string;
  linkedin_username?: string;
  website?: string;
}

interface Repository {
  name: string;
  description: string;
  url: string;
  featured?: boolean;
  pinned?: boolean;
  tags?: string[];
}

interface Link {
  title: string;
  url: string;
  icon?: string;
}

interface Badge {
  name: string;
  description: string;
  url: string;
  color?: string;
}

interface Metadata {
  version: string;
  last_updated: string;
  author: string;
}

export class ProfileParserError extends Error {
  constructor(message: string, public readonly code: string) {
    super(message);
    this.name = 'ProfileParserError';
  }
}

class ProfileParser {
  private static readonly VALID_FIELDS = new Set([
    'name', 'bio', 'location', 'avatar_url', 'hide_email', 
    'email', 'twitter_username', 'linkedin_username', 'website'
  ]);

  constructor(private readonly configPath: string) {}

  parse(): ProfileConfig {
    let rawContent: string;
    
    try {
      rawContent = fs.readFileSync(this.configPath, 'utf-8');
    } catch (error) {
      if (error instanceof Error && error.code === 'ENOENT') {
        throw new ProfileParserError(
          `Config file not found at "${this.configPath}"`,
          'FILE_NOT_FOUND'
        );
      }
      throw new ProfileParserError(`Failed to read config file: ${error}`, 'READ_ERROR');
    }

    if (!rawContent.trim()) {
      throw new ProfileParserError('Config file is empty', 'EMPTY_FILE');
    }

    let parsed: any;
    try {
      parsed = yaml.parse(rawContent);
    } catch (error) {
      throw new ProfileParserError(
        `YAML parsing failed: ${(error as Error).message}`,
        'PARSE_ERROR'
      );
    }

    return this.validateAndNormalize(parsed);
  }

  private validateAndNormalize(data: any): ProfileConfig {
    if (!data.user) {
      throw new ProfileParserError('Missing required "user" section', 'MISSING_USER');
    }

    const user = data.user;
    
    // Validate and normalize user fields
    this.normalizeUser(user);

    // Normalize repositories
    if (data.repositories) {
      data.repositories = this.normalizeRepositories(data.repositories as any[]);
    }

    // Normalize links
    if (data.links) {
      data.links = this.normalizeLinks(data.links as any[]);
    }

    // Normalize badges
    if (data.badges) {
      data.badges = this.normalizeBadges(data.badges as any[]);
    }

    return data;
  }

  private normalizeUser(user: UserSettings): void {
    // Ensure required fields exist
    if (!user.name || user.name.trim() === '') {
      throw new ProfileParserError('User "name" is required and cannot be empty', 'MISSING_NAME');
    }

    if (!user.bio) {
      throw new ProfileParserError('User "bio" is required', 'MISSING_BIO');
    }

    // Validate URL fields
    this.validateUrl(user.avatar_url, 'avatar_url');
    this.validateUrl(user.website, 'website');
    
    // Normalize email format if present
    if (user.email) {
      const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
      if (!emailRegex.test(user.email)) {
        throw new ProfileParserError(`Invalid email format: "${user.email}"`, 'INVALID_EMAIL');
      }
    }

    // Validate social handles
    this.validateHandle(user.twitter_username, 'twitter_username');
    this.validateHandle(user.linkedin_username, 'linkedin_username');
  }

  private validateUrl(field: string | undefined, fieldName: string): void {
    if (field && !/^https?:\/\//i.test(field)) {
      throw new ProfileParserError(
        `Invalid URL format for "${fieldName}": must start with http:// or https://`,
        'INVALID_URL'
      );
    }
  }

  private validateHandle(handle: string | undefined, fieldName: string): void {
    if (handle && !/^[a-zA-Z0-9_]{1,50}$/.test(handle)) {
      throw new ProfileParserError(
        `Invalid handle format for "${fieldName}": must be alphanumeric with underscores`,
        'INVALID_HANDLE'
      );
    }
  }

  private normalizeRepositories(repos: any[]): Repository[] {
    return repos.map((repo, index) => {
      if (!repo.name || !repo.description) {
        throw new ProfileParserError(
          `Repository at index ${index} is missing "name" or "description"`,
          'MISSING_REPO_FIELD'
        );
      }

      this.validateUrl(repo.url, `repository[${index}].url`);

      return {
        name: repo.name.trim(),
        description: repo.description.trim(),
        url: repo.url,
        featured: !!repo.featured,
        pinned: !!repo.pinned,
        tags: Array.isArray(repo.tags) ? repo.tags : []
      };
    });
  }

  private normalizeLinks(links: any[]): Link[] {
    return links.map((link, index) => {
      if (!link.title || !link.url) {
        throw new ProfileParserError(
          `Link at index ${index} is missing "title" or "url"`,
          'MISSING_LINK_FIELD'
        );
      }

      this.validateUrl(link.url, `link[${index}].url`);

      return {
        title: link.title.trim(),
        url: link.url,
        icon: link.icon || undefined
      };
    });
  }

  private normalizeBadges(badges: any[]): Badge[] {
    return badges.map((badge, index) => {
      if (!badge.name || !badge.description || !badge.url) {
        throw new ProfileParserError(
          `Badge at index ${index} is missing required fields`,
          'MISSING_BADGE_FIELD'
        );
      }

      this.validateUrl(badge.url, `badge[${index}].url`);

      return {
        name: badge.name.trim(),
        description: badge.description.trim(),
        url: badge.url,
        color: badge.color || undefined
      };
    });
  }

  static fromFile(path: string): ProfileConfig {
    const parser = new ProfileParser(path);
    return parser.parse();
  }
}

// Export a default instance for convenience
export const profileParser = new ProfileParser('.github/profile.yml');

// Demo / Entry Point
if (require.main === module) {
  async function main() {
    try {
      console.log('GitHub Profile Parser - Demo\n');
      console.log(`Looking for config at: ${profileParser.configPath}\n`);

      const config = profileParser.parse();

      console.log('Parsed Configuration:\n');
      console.log(JSON.stringify(config, null, 2));

      // Quick summary
      console.log('\n--- Summary ---');
      console.log(`Name: ${config.user.name}`);
      console.log(`Bio: ${config.user.bio}`;
      console.log(`Location: ${config.user.location}`);
      console.log(`Repos configured: ${config.repositories?.length || 0}`);
      console.log(`Links configured: ${config.links?.length || 0}`);
      console.log(`Badges configured: ${config.badges?.length || 0}`);

    } catch (error) {
      if (error instanceof ProfileParserError) {
        console.error(`Error (${error.code}): ${error.message}`);
        process.exit(1);
      } else {
        console.error('Unexpected error:', error);
        process.exit(1);
      }
    }
  }

  main();
}

export default ProfileParser;