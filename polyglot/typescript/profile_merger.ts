import { existsSync, readFileSync } from 'fs';
import { join, parse } from 'path';

// ============================================================================
// TYPES & SCHEMA
// ============================================================================

export interface ProfileConfig {
  name: string;
  email: string;
  bio: string;
  location: string;
  website?: string;
  socials: SocialLinks;
  repositories: Repository[];
  featured: boolean;
}

export interface SocialLinks {
  twitter?: string;
  linkedin?: string;
  github?: string;
  portfolio?: string;
}

export interface Repository {
  name: string;
  description: string;
  stars: number;
  forks: number;
  url: string;
  featured: boolean;
}

// ============================================================================
// UTILITY TYPES
// ============================================================================

type MergeStrategy = 'deep' | 'shallow';

interface MergeOptions {
  strategy?: MergeStrategy;
  overrideOrder: ProfileConfig[];
  defaultProfile?: ProfileConfig;
  onConflict?: (key: string, existing: any, incoming: any) => any;
}

// ============================================================================
// CORE MERGE LOGIC
// ============================================================================

function createDefaultProfile(): ProfileConfig {
  return {
    name: '',
    email: '',
    bio: '',
    location: '',
    website: undefined,
    socials: {},
    repositories: [],
    featured: false,
  };
}

function isObject(value: any): value is Record<string, unknown> {
  return typeof value === 'object' && !Array.isArray(value) && value !== null;
}

function deepMerge<T extends object>(target: T, source: Partial<T>): T {
  const result = { ...target };

  for (const key in source) {
    if (!Object.prototype.hasOwnProperty.call(source, key)) continue;

    const targetValue = result[key];
    const sourceValue = source[key as keyof T];

    // Handle nested objects recursively
    if (isObject(targetValue) && isObject(sourceValue)) {
      result[key] = deepMerge(targetValue, sourceValue);
    } else if (Array.isArray(targetValue) && Array.isArray(sourceValue)) {
      result[key] = [...targetValue, ...sourceValue];
    } else {
      result[key] = sourceValue;
    }
  }

  return result;
}

function shallowMerge<T extends object>(target: T, source: Partial<T>): T {
  return { ...target, ...source };
}

// ============================================================================
// FILE LOADER
// ============================================================================

interface LoaderOptions {
  rootPath?: string;
  extensions: string[];
}

function loadProfilesFromFiles(
  options: LoaderOptions = {}
): ProfileConfig[] {
  const { rootPath = process.cwd(), extensions = ['.json'] } = options;

  if (!existsSync(rootPath)) return [];

  const files = extensions.flatMap(ext => 
    [...new Set(extensions.map(e => e === ext ? '' : e))]
  );

  const profiles: ProfileConfig[] = [];

  for (const file of files) {
    const filePath = join(rootPath, file);
    
    if (!existsSync(filePath)) continue;

    try {
      const content = readFileSync(filePath, 'utf-8');
      const profile = JSON.parse(content);
      
      // Validate and sanitize
      const sanitized = validateProfile(profile);
      profiles.push(sanitized);
    } catch (error) {
      console.warn(`Warning: Failed to load ${filePath}: ${(error as Error).message}`);
    }
  }

  return profiles;
}

function validateProfile(profile: any): ProfileConfig {
  const defaults = createDefaultProfile();
  
  // Apply defaults for missing fields
  return deepMerge(defaults, profile) as ProfileConfig;
}

// ============================================================================
// MAIN MERGER CLASS
// ============================================================================

export class ProfileMerger {
  private defaultProfile: ProfileConfig;
  private loadedProfiles: ProfileConfig[] = [];

  constructor(options?: MergeOptions) {
    this.defaultProfile = options?.defaultProfile || createDefaultProfile();
    this.loadedProfiles = options?.overrideOrder || [];
  }

  loadFromFiles(extensions: string[] = ['.json']) {
    const loaded = loadProfilesFromFiles({ extensions });
    this.loadedProfiles.push(...loaded);
    return this;
  }

  addOverride(profile: ProfileConfig) {
    this.loadedProfiles.unshift(profile);
    return this;
  }

  merge(options?: MergeOptions): ProfileConfig {
    let result = deepMerge(this.defaultProfile, {});

    // Apply loaded profiles in order (later overrides earlier)
    for (const profile of [...this.loadedProfiles].reverse()) {
      result = deepMerge(result, profile);
    }

    // Apply custom options if provided
    if (options?.overrideOrder) {
      for (const profile of options.overrideOrder.reverse()) {
        result = deepMerge(result, profile);
      }
    }

    return result;
  }

  getMergedProfile(options?: MergeOptions): ProfileConfig {
    const merged = this.merge(options);
    
    // Post-processing: ensure socials are properly formatted
    if (merged.socials && typeof merged.socials === 'object') {
      merged.socials = Object.entries(merged.socials)
        .filter(([_, value]) => value !== undefined)
        .reduce((acc, [key, value]: [string, string]) => ({
          ...acc,
          [key.toLowerCase() as keyof SocialLinks]: value,
        }), {});
    }

    return merged;
  }

  generateMarkdown(): string {
    const profile = this.getMergedProfile();
    
    let md = `# ${profile.name || 'GitHub Profile'}\n\n`;
    
    if (profile.bio) {
      md += `${profile.bio}\n\n`;
    }
    
    if (profile.location) {
      md += `📍 **Location:** ${profile.location}\n\n`;
    }
    
    if (profile.website) {
      md += `[Website](${profile.website})\n\n`;
    }
    
    // Social links
    const socials = profile.socials;
    const socialItems: string[] = [];
    
    if (socials.twitter) socialItems.push(`[Twitter](https://twitter.com/${socials.twitter.replace('@', '')})`);
    if (socials.linkedin) socialItems.push(`[LinkedIn](${socials.linkedin})`);
    if (socials.portfolio) socialItems.push(`[Portfolio](${socials.portfolio})`);
    
    if (socialItems.length > 0) {
      md += `**Social:** ${socialItems.join(' | ')}\n\n`;
    }
    
    // Repositories
    const featuredRepos = profile.repositories.filter(r => r.featured).sort((a, b) => 
      b.stars - a.stars || b.forks - a.forks
    );
    
    if (featuredRepos.length > 0) {
      md += `## Featured Projects\n\n`;
      
      for (const repo of featuredRepos.slice(0, 5)) {
        const stars = repo.stars.toLocaleString();
        const forks = repo.forks.toLocaleString();
        
        md += `\n### ${repo.name}\n`;
        md += `${repo.description}\n\n`;
        md += `⭐ **Stars:** ${stars} | 🍴 **Forks:** ${forks}\n\n`;
      }
    }

    return md;
  }

  generateJSON(): string {
    const profile = this.getMergedProfile();
    
    // Clean up undefined values for JSON output
    const clean: any = {};
    
    Object.keys(profile).forEach(key => {
      if (profile[key] !== undefined) {
        clean[key] = profile[key];
      }
    });

    return JSON.stringify(clean, null, 2);
  }

  exportToFile(format: 'markdown' | 'json', filename?: string): void {
    const content = format === 'markdown' 
      ? this.generateMarkdown() 
      : this.generateJSON();
    
    const path = filename || `profile.${format}`;
    writeFileSync(path, content);
    console.log(`Exported to: ${path}`);
  }
}

// ============================================================================
// CLI INTERFACE
// ============================================================================

function runCLI(): void {
  const merger = new ProfileMerger();

  // Load from current directory
  merger.loadFromFiles(['.github/profile.json', '.profile.json']);

  // Check for environment variables
  if (process.env.GITHUB_PROFILE_NAME) {
    merger.addOverride({ name: process.env.GITHUB_PROFILE_NAME });
  }

  const merged = merger.getMergedProfile();

  console.log('=== Merged Profile ===');
  console.log(JSON.stringify(merged, null, 2));

  // Auto-export based on format preference
  if (process.argv.includes('--md')) {
    merger.exportToFile('markdown', 'profile.md');
  } else if (process.argv.includes('--json')) {
    merger.exportToFile('json', 'profile.json');
  } else {
    console.log('\n=== Markdown Preview ===\n' + merger.generateMarkdown());
  }
}

// ============================================================================
// EXPORTED API
// ============================================================================

export const createMerger = (options?: MergeOptions) => new ProfileMerger(options);

// ============================================================================
// DEMO / RUNNABLE ENTRY POINT
// ============================================================================

if (require.main === module) {
  runCLI();
}

// ============================================================================
// TYPE EXPORTS FOR MODULE USAGE
// ============================================================================

export type { ProfileConfig, SocialLinks, Repository, MergeOptions };