import { readFileSync, writeFileSync } from 'node:fs';
import { join, dirname } from 'node:path';
import * as process from 'node:process';

// ============================================================================
// TYPES & INTERFACES
// ============================================================================

interface ProfileConfig {
  name: string;
  bio: string;
  avatarUrl?: string;
  socialLinks: SocialLink[];
  repositories: RepositorySettings;
  badges: Badge[];
}

interface SocialLink {
  platform: 'twitter' | 'discord' | 'linkedin' | 'website' | 'email';
  url: string;
  username?: string;
}

interface RepositorySettings {
  private: boolean;
  defaultBranch: string;
  autoMerge: boolean;
  squashMerges: boolean;
}

interface Badge {
  name: string;
  description: string;
  url: string;
  color?: string;
}

// ============================================================================
// CONSTANTS & DEFAULTS
// ============================================================================

const DEFAULT_CONFIG_PATH = '.github/config.json';
const DEFAULT_AVATAR = 'https://avatars.githubusercontent.com/u/0?v=4';
const DEFAULT_BIO = 'TypeScript developer passionate about clean code and type safety.';

const SOCIAL_DEFAULTS: Record<string, string> = {
  twitter: 'https://twitter.com/',
  discord: 'https://discord.com/users/',
  linkedin: 'https://linkedin.com/in/',
  website: '',
  email: 'mailto:',
};

// ============================================================================
// VALIDATION HELPERS
// ============================================================================

function validateUrl(url: string, fieldName: string): string | null {
  if (!url || url.trim() === '') return null;
  
  const trimmed = url.trim();
  if (trimmed.startsWith('http://') || trimmed.startsWith('https://')) {
    return trimmed;
  }
  
  // Allow relative paths or usernames for platforms with defaults
  if (SOCIAL_DEFAULTS[trimmed.toLowerCase()]) {
    return SOCIAL_DEFAULTS[trimmed.toLowerCase()] + trimmed;
  }
  
  return null;
}

function validateEmail(email: string): string | null {
  if (!email || email.trim() === '') return null;
  
  const trimmed = email.trim();
  const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
  
  if (trimmed.startsWith('mailto:')) {
    return trimmed.slice(7);
  }
  
  if (!emailRegex.test(trimmed)) {
    return null;
  }
  
  return trimmed;
}

function validateName(name: string): string | null {
  if (!name || name.trim() === '') return null;
  return name.trim();
}

// ============================================================================
// CONFIG LOADER
// ============================================================================

interface LoadedConfig extends ProfileConfig {
  sourcePath?: string;
  loadedAt: Date;
}

function loadExistingConfig(path: string): LoadedConfig | null {
  try {
    const content = readFileSync(path, 'utf-8');
    return JSON.parse(content);
  } catch (error) {
    if (error instanceof Error && error.message.includes('ENOENT')) {
      return null;
    }
    throw error;
  }
}

// ============================================================================
// CONFIG MERGER & NORMALIZER
// ============================================================================

function mergeWithDefaults(config: Partial<ProfileConfig>): ProfileConfig {
  const merged: ProfileConfig = {
    name: config.name || '',
    bio: config.bio || DEFAULT_BIO,
    avatarUrl: config.avatarUrl || DEFAULT_AVATAR,
    socialLinks: config.socialLinks || [],
    repositories: {
      private: false,
      defaultBranch: 'main',
      autoMerge: true,
      squashMerges: false,
    },
    badges: config.badges || [],
  };

  // Normalize and validate each field
  merged.name = validateName(merged.name) || '';
  
  if (config.avatarUrl) {
    const validated = validateUrl(config.avatarUrl, 'avatar');
    merged.avatarUrl = validated || DEFAULT_AVATAR;
  }

  // Process social links
  const normalizedSocials: SocialLink[] = [];
  for (const link of config.socialLinks || []) {
    let url = validateUrl(link.url, `social.${link.platform}`);
    
    if (!url) continue;
    
    // Add platform prefix if missing
    if (!SOCIAL_DEFAULTS[link.platform]) {
      url = SOCIAL_DEFAULTS[link.platform] + url;
    }
    
    normalizedSocials.push({
      platform: link.platform,
      url,
      username: link.username || '',
    });
  }
  
  merged.socialLinks = normalizedSocials;

  // Normalize email if provided
  const emailLink = config.socialLinks?.find(l => l.platform === 'email');
  if (emailLink && !SOCIAL_DEFAULTS['email']) {
    emailLink.url = validateEmail(emailLink.url) || '';
  }

  return merged;
}

// ============================================================================
// OUTPUT GENERATORS
// ============================================================================

function generateJsonOutput(config: ProfileConfig): string {
  const output: Record<string, unknown> = {
    name: config.name,
    bio: config.bio,
    avatarUrl: config.avatarUrl,
    socialLinks: config.socialLinks.map(({ platform, url }) => ({
      platform,
      url,
    })),
    repositories: config.repositories,
    badges: config.badges,
  };

  return JSON.stringify(output, null, 2);
}

function generateTOMLOutput(config: ProfileConfig): string {
  const lines: string[] = [];
  
  // Header comment
  lines.push('# GitHub Profile Configuration');
  lines.push(`name = "${config.name}"`);
  lines.push(`bio = "${config.bio.replace(/"/g, '\\"')}"`);
  lines.push(`avatar_url = "${config.avatarUrl}"`);
  
  // Social links
  for (const link of config.socialLinks) {
    const key = `social.${link.platform}`;
    if (SOCIAL_DEFAULTS[link.platform]) {
      const urlWithoutPrefix = link.url.replace(SOCIAL_DEFAULTS[link.platform], '');
      lines.push(`${key} = "${urlWithoutPrefix}"`);
    } else {
      lines.push(`${key} = "${link.url}"`);
    }
  }

  // Repositories
  const repo = config.repositories;
  lines.push(`repositories.private = ${repo.private}`);
  lines.push(`repositories.default_branch = "${repo.defaultBranch}"`);
  lines.push(`repositories.auto_merge = ${repo.autoMerge}`);
  lines.push(`repositories.squash_merges = ${repo.squashMerges}`);

  // Badges
  if (config.badges.length > 0) {
    lines.push('\n# Badges');
    for (const badge of config.badges) {
      const safeName = badge.name.replace(/[^a-zA-Z0-9]/g, '_').toLowerCase();
      lines.push(`[[badges]]`);
      lines.push(`name = "${safeName}"`);
      lines.push(`description = "${badge.description.replace(/"/g, '\\"')}"`);
      lines.push(`url = "${badge.url}"`);
      if (badge.color) {
        lines.push(`color = "${badge.color}"`);
      }
    }
  }

  return lines.join('\n');
}

function generateYamlOutput(config: ProfileConfig): string {
  const output: Record<string, unknown> = {
    name: config.name,
    bio: config.bio,
    avatarUrl: config.avatarUrl,
    socialLinks: config.socialLinks.map(({ platform, url }) => ({
      platform,
      url,
    })),
    repositories: config.repositories,
    badges: config.badges,
  };

  return `---\n${JSON.stringify(output, null, 2)}`;
}

// ============================================================================
// FILE WRITER
// ============================================================================

interface WriteOptions {
  path?: string;
  format: 'json' | 'toml' | 'yaml';
  append?: boolean;
  createDirs?: boolean;
}

function writeConfig(config: ProfileConfig, options: WriteOptions): void {
  const content = 
    options.format === 'json' ? generateJsonOutput(config) :
    options.format === 'toml' ? generateTOMLOutput(config) :
    generateYamlOutput(config);

  const fullPath = options.path || DEFAULT_CONFIG_PATH;
  
  if (options.createDirs && !fullPath.startsWith('http')) {
    const dir = dirname(fullPath);
    try {
      process.chdir(dir);
    } catch {}
  }

  const mode = options.append ? 'a' : 'w';
  writeFileSync(fullPath, content + '\n', { mode });
}

// ============================================================================
// CLI INTERFACE
// ============================================================================

function parseArgs(): Partial<ProfileConfig> & WriteOptions {
  const args: Partial<ProfileConfig> & WriteOptions = { format: 'json' };

  for (const [key, value] of Object.entries(process.argv.slice(2))) {
    if (value === 'true') args[key as string] = true;
    else if (value === 'false') args[key as string] = false;
    else if (!isNaN(Number(value)) && !['name', 'bio'].includes(key)) {
      // Could be a number config option
    } else {
      (args as Record<string, unknown>)[key] = value;
    }
  }

  return args;
}

function runCli(): void {
  const args = parseArgs();
  
  if (!args.name) {
    console.log('GitHub Profile Generator');
    console.log('Usage: node profile_generator.ts --name "Your Name" [--bio "..."] [--format json|toml|yaml]');
    process.exit(1);
  }

  const config = mergeWithDefaults(args as Partial<ProfileConfig>);
  
  // Use default path if no custom path provided
  const outputPath = args.path || DEFAULT_CONFIG_PATH;
  console.log(`Writing to: ${outputPath}`);
  
  writeConfig(config, { format: args.format as 'json' | 'toml' | 'yaml', createDirs: true });
  
  console.log('Done!');
}

// ============================================================================
// DEMO / RUNNABLE ENTRY POINT
// ============================================================================

function runDemo(): void {
  // Sample profile data
  const sampleProfile: Partial<ProfileConfig> = {
    name: 'Jane Doe',
    bio: 'Senior TypeScript Engineer | Open Source Contributor | Coffee Enthusiast',
    avatarUrl: 'https://avatars.githubusercontent.com/u/123456?v=4',
    socialLinks: [
      { platform: 'twitter', url: 'jane_dev', username: 'JaneDev' },
      { platform: 'linkedin', url: 'janedoe' },
      { platform: 'website', url: 'https://example.com/jane' },
      { platform: 'email', url: 'hello@example.com' },
    ],
    repositories: {
      private: false,
      defaultBranch: 'main',
      autoMerge: true,
      squashMerges: true,
    },
    badges: [
      { name: 'TypeScript', description: 'TypeScript Developer', url: 'https://img.shields.io/badge/typescript-blue', color: '#3178C6' },
      { name: 'Open Source', description: 'Contributor to 50+ projects', url: 'https://github.com/janedoe/contributions' },
    ],
  };

  // Generate all formats
  console.log('=== JSON Output ===');
  console.log(generateJsonOutput(mergeWithDefaults(sampleProfile)));

  console.log('\n=== TOML Output ===');
  console.log(generateTOMLOutput(mergeWithDefaults(sampleProfile)));

  console.log('\n=== YAML Output ===');
  console.log(generateYamlOutput(mergeWithDefaults(sampleProfile)));

  // Write to file
  const outputPath = join(process.cwd(), '.github', 'config.json');
  
  try {
    writeConfig(mergeWithDefaults(sampleProfile), { 
      path: outputPath, 
      format: 'json',
      createDirs: true,
    });
    console.log(`\n✓ Written to: ${outputPath}`);
    
    // Verify it was written correctly
    const content = readFileSync(outputPath, 'utf-8');
    console.log('Verification - File contents:');
    console.log(content);
  } catch (error) {
    if (error instanceof Error) {
      console.error(`Error writing file: ${error.message}`);
    }
  }

  // Load and display what we just wrote back
  console.log('\n=== Loaded Back ===');
  const loaded = loadExistingConfig(outputPath);
  if (loaded) {
    console.log('Successfully loaded config!');
    console.log(`Name: ${loaded.name}`);
    console.log(`Bio: ${loaded.bio.substring(0, 50)}...`);
  }
}

// ============================================================================
// EXPORTS & ENTRY POINT
// ============================================================================

export { 
  ProfileConfig, 
  SocialLink, 
  RepositorySettings, 
  Badge,
  LoadedConfig,
  mergeWithDefaults,
  generateJsonOutput,
  generateTOMLOutput,
  generateYamlOutput,
  writeConfig,
  loadExistingConfig,
};

// Run demo when executed directly
if (import.meta.url === `file://${process.argv[1]}`) {
  runDemo();
}

// Also support Node.js module entry point
runDemo();