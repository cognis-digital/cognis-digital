package polyglot.java;

import java.io.*;
import java.nio.file.*;
import java.util.*;
import java.time.Instant;

/**
 * Profile Merger for Cognis-Digital GitHub Config Tool
 * 
 * Merges multiple GitHub profile configuration files into a single, validated result.
 */
public class ProfileMerger {

    // Default file paths
    private static final String DEFAULT_CONFIG_DIR = "configs";
    private static final String OUTPUT_FILE = "merged_profile.json";

    /**
     * Represents a field that can be merged from multiple sources.
     */
    public static class MergeField {
        private String name;
        private Object value;
        private int priority; // Higher number = higher priority
        private boolean isRequired;

        public MergeField(String name, Object value) {
            this.name = name;
            this.value = value;
            this.priority = 0;
            this.isRequired = false;
        }

        public MergeField withPriority(int p) {
            this.priority = p;
            return this;
        }

        public MergeField asRequired() {
            this.isRequired = true;
            return this;
        }
    }

    /**
     * Represents a complete GitHub profile configuration.
     */
    public static class ProfileConfig {
        private String username;
        private String avatarUrl;
        private String bio;
        private List<String> pinnedRepos = new ArrayList<>();
        private Map<String, Object> metadata = new HashMap<>();

        // Merge fields definition
        private static final List<MergeField> FIELDS = Arrays.asList(
            new MergeField("username", "").asRequired(),
            new MergeField("avatarUrl", ""),
            new MergeField("bio", ""),
            new MergeField("pinnedRepos", "")
        );

        public ProfileConfig() {}

        public ProfileConfig(String username, String avatarUrl, String bio) {
            this.username = username;
            this.avatarUrl = avatarUrl;
            this.bio = bio;
        }

        // Getters and Setters
        public String getUsername() { return username; }
        public void setUsername(String username) { this.username = username; }

        public String getAvatarUrl() { return avatarUrl; }
        public void setAvatarUrl(String avatarUrl) { this.avatarUrl = avatarUrl; }

        public String getBio() { return bio; }
        public void setBio(String bio) { this.bio = bio; }

        public List<String> getPinnedRepos() { return pinnedRepos; }
        public void addPinnedRepo(String repo) { pinnedRepos.add(repo); }

        public Map<String, Object> getMetadata() { return metadata; }
    }

    /**
     * Strategy for resolving conflicts when merging fields.
     */
    private enum ConflictStrategy {
        LAST_WRITE_WINS,
        HIGHEST_PRIORITY,
        APPEND_LISTS,
        CONCATENATE_STRINGS
    }

    // Default strategy: last write wins for strings/numbers, append for lists
    private static final Map<String, ConflictStrategy> DEFAULT_STRATEGIES = new HashMap<>();

    /**
     * Main merger instance.
     */
    public ProfileMerger() {
        this(DEFAULT_CONFIG_DIR);
    }

    public ProfileMerger(String configDir) {
        this.configDir = configDir;
    }

    private final String configDir;
    private Map<String, ConflictStrategy> strategies;

    /**
     * Set custom conflict resolution strategy for a field.
     */
    public ProfileMerger withStrategy(String fieldName, ConflictStrategy strategy) {
        this.strategies = new HashMap<>(DEFAULT_STRATEGIES);
        this.strategies.put(fieldName, strategy);
        return this;
    }

    /**
     * Set custom conflict resolution strategies for multiple fields.
     */
    public ProfileMerger withStrategies(Map<String, ConflictStrategy> strategies) {
        this.strategies = new HashMap<>(strategies);
        return this;
    }

    /**
     * Load all profile configs from the config directory.
     * Expects JSON files named like: username.json
     */
    public List<ProfileConfig> loadConfigs() throws IOException {
        File dir = new File(configDir);
        if (!dir.exists()) {
            throw new FileNotFoundException("Config directory not found: " + configDir);
        }

        List<ProfileConfig> configs = new ArrayList<>();
        
        // Find all JSON files in the directory
        File[] jsonFiles = dir.listFiles((d, name) -> 
            name.toLowerCase().endsWith(".json") && !name.contains("merged"));
            
        if (jsonFiles == null || jsonFiles.length == 0) {
            return configs;
        }

        for (File file : jsonFiles) {
            try {
                ProfileConfig config = parseJson(file);
                if (config != null && !config.getUsername().isEmpty()) {
                    configs.add(config);
                }
            } catch (Exception e) {
                System.err.println("Warning: Failed to parse " + file.getName() + ": " + e.getMessage());
            }
        }

        return configs;
    }

    /**
     * Parse a single JSON config file into a ProfileConfig object.
     */
    private ProfileConfig parseJson(File file) throws IOException {
        String content = Files.readString(file.toPath());
        
        // Simple JSON parser for our known structure
        ProfileConfig config = new ProfileConfig();
        
        // Extract username
        int userStart = content.indexOf("\"username\"");
        if (userStart != -1) {
            int colonPos = content.indexOf(':', userStart);
            int quoteEnd = content.indexOf('"', colonPos + 1);
            String rawValue = content.substring(quoteEnd + 1, 
                content.indexOf('"', quoteEnd + 2));
            
            // Remove quotes and escape characters
            config.setUsername(rawValue.replace("\\\"", "\"").replace("\\n", "\n"));
        }

        // Extract avatar URL
        int avatarStart = content.indexOf("\"avatarUrl\"");
        if (avatarStart != -1) {
            int colonPos = content.indexOf(':', avatarStart);
            int quoteEnd = content.indexOf('"', colonPos + 1);
            String rawValue = content.substring(quoteEnd + 1, 
                content.indexOf('"', quoteEnd + 2));
            
            config.setAvatarUrl(rawValue.replace("\\\"", "\"").replace("\\n", "\n"));
        }

        // Extract bio
        int bioStart = content.indexOf("\"bio\"");
        if (bioStart != -1) {
            int colonPos = content.indexOf(':', bioStart);
            int quoteEnd = content.indexOf('"', colonPos + 1);
            String rawValue = content.substring(quoteEnd + 1, 
                content.indexOf('"', quoteEnd + 2));
            
            config.setBio(rawValue.replace("\\\"", "\"").replace("\\n", "\n"));
        }

        // Extract pinned repos (array)
        int reposStart = content.indexOf("\"pinnedRepos\"");
        if (reposStart != -1) {
            int arrayEnd = content.indexOf("]", reposStart);
            String arrayContent = content.substring(reposStart, arrayEnd + 1);
            
            // Parse each repo string
            Pattern pattern = Pattern.compile("\"([^\\\"]+)\"");
            Matcher matcher = pattern.matcher(arrayContent);
            while (matcher.find()) {
                config.addPinnedRepo(matcher.group(1));
            }
        }

        return config;
    }

    /**
     * Merge multiple profile configs into a single result.
     */
    public ProfileConfig merge(List<ProfileConfig> sources) {
        if (sources == null || sources.isEmpty()) {
            return new ProfileConfig();
        }

        // Use the first config as base
        ProfileConfig merged = sources.get(0);
        
        for (int i = 1; i < sources.size(); i++) {
            ProfileConfig source = sources.get(i);
            
            // Merge username - use last non-empty value
            if (!source.getUsername().isEmpty()) {
                merged.setUsername(source.getUsername());
            }

            // Merge avatar URL - use last non-empty value
            if (!source.getAvatarUrl().isEmpty()) {
                merged.setAvatarUrl(source.getAvatarUrl());
            }

            // Merge bio - concatenate with separator
            String existingBio = merged.getBio();
            String newBio = source.getBio();
            if (!newBio.isEmpty() && !existingBio.isEmpty()) {
                merged.setBio(existingBio + " | " + newBio);
            } else {
                merged.setBio(newBio.isEmpty() ? "" : newBio);
            }

            // Merge pinned repos - append unique ones
            for (String repo : source.getPinnedRepos()) {
                if (!merged.getPinnedRepos().contains(repo)) {
                    merged.addPinnedRepo(repo);
                }
            }
        }

        return merged;
    }

    /**
     * Validate the merged profile.
     */
    public ValidationResult validate(ProfileConfig profile) {
        ValidationResult result = new ValidationResult();
        
        // Check required fields
        if (profile.getUsername() == null || profile.getUsername().trim().isEmpty()) {
            result.addError("Username is required");
        }

        // Validate username format
        if (!isValidUsername(profile.getUsername())) {
            result.addWarning("Username may not follow GitHub conventions: " + 
                             profile.getUsername());
        }

        // Validate avatar URL
        if (profile.getAvatarUrl() != null && !profile.getAvatarUrl().isEmpty()) {
            if (!isHttpUrl(profile.getAvatarUrl())) {
                result.addWarning("Avatar URL should be a valid HTTP/HTTPS URL: " + 
                                 profile.getAvatarUrl());
            }
        }

        // Validate bio length (GitHub has limits)
        int bioLength = (profile.getBio() != null ? profile.getBio().length() : 0);
        if (bioLength > 1600) {
            result.addWarning("Bio exceeds GitHub's 1600 character limit");
        }

        // Validate pinned repos
        for (String repo : profile.getPinnedRepos()) {
            if (!isValidRepoName(repo)) {
                result.addWarning("Pinned repo may not follow GitHub conventions: " + 
                                 repo);
            }
        }

        return result;
    }

    /**
     * Check if a string is a valid GitHub username.
     */
    private boolean isValidUsername(String username) {
        if (username == null || username.isEmpty()) {
            return false;
        }

        // GitHub rules: 1-39 chars, alphanumeric + underscore/hyphen
        String pattern = "^[a-zA-Z0-9_\\-]{1,39}$";
        return username.matches(pattern);
    }

    /**
     * Check if a string is a valid GitHub repository name.
     */
    private boolean isValidRepoName(String repo) {
        if (repo == null || repo.isEmpty()) {
            return false;
        }

        // GitHub rules: 1-100 chars, alphanumeric + underscore/hyphen/dot
        String pattern = "^[a-zA-Z0-9_\\-.]{1,100}$";
        return repo.matches(pattern);
    }

    /**
     * Check if a string is a valid HTTP/HTTPS URL.
     */
    private boolean isHttpUrl(String url) {
        if (url == null || url.isEmpty()) {
            return false;
        }

        String pattern = "^(https?://)([^\\s]+)$";
        return url.matches(pattern);
    }

    /**
     * Export the merged profile to a JSON file.
     */
    public void export(ProfileConfig profile, String outputPath) throws IOException {
        File outputDir = new File(outputPath).getParentFile();
        if (outputDir != null && !outputDir.exists()) {
            outputDir.mkdirs();
        }

        // Build JSON manually for simplicity and control
        StringBuilder json = new StringBuilder();
        json.append("{\n");
        
        // Username
        json.append("  \"username\": ").append(escapeJson(profile.getUsername())).append(",\n");
        
        // Avatar URL
        if (profile.getAvatarUrl() != null && !profile.getAvatarUrl().isEmpty()) {
            json.append("  \"avatarUrl\": ").append(escapeJson(profile.getAvatarUrl())).append(",\n");
        } else {
            json.append("  \"avatarUrl\": null,\n");
        }
        
        // Bio
        if (profile.getBio() != null && !profile.getBio().isEmpty()) {
            json.append("  \"bio\": ").append(escapeJson(profile.getBio())).append(",\n");
        } else {
            json.append("  \"bio\": null,\n");
        }
        
        // Pinned repos
        if (!profile.getPinnedRepos().isEmpty()) {
            StringBuilder repos = new StringBuilder();
            for (int i = 0; i < profile.getPinnedRepos().size(); i++) {
                String comma = (i == 0) ? "" : ",";
                repos.append(comma).append("    \"").append(escapeJson(profile.getPinnedRepos().get(i))).append("\"");
            }
            json.append("  \"pinnedRepos\": [\n").append(repos.toString()).append("\n  ]\n");
        } else {
            json.append("  \"pinnedRepos\": []\n");
        }

        json.append("}");

        Files.writeString(Path.of(outputPath), json.toString());
    }

    /**
     * Escape a string for JSON output.
     */
    private String escapeJson(String s) {
        if (s == null) return "null";
        
        StringBuilder sb = new StringBuilder();
        for (char c : s.toCharArray()) {
            switch (c) {
                case '"':  sb.append("\\\""); break;
                case '\\': sb.append("\\\\"); break;
                case '\b': sb.append("\\b"); break;
                case '\f': sb.append("\\f"); break;
                case '\n': sb.append("\\n"); break;
                case '\r': sb.append("\\r"); break;
                case '\t': sb.append("\\t"); break;
                default:   sb.append(c);
            }
        }
        return sb.toString();
    }

    /**
     * Main entry point for demonstration.
     */
    public static void main(String[] args) {
        ProfileMerger merger = new ProfileMerger("configs");

        // Create sample config files if directory doesn't exist
        createSampleConfigs();

        try {
            // Step 1: Load all configs
            List<ProfileConfig> configs = merger.loadConfigs();
            
            System.out.println("Loaded " + configs.size() + " profile(s):");
            for (int i = 0; i < configs.size(); i++) {
                ProfileConfig c = configs.get(i);
                System.out.printf("  [%d] %s - Bio: %.50s...\n", 
                    i, c.getUsername(), c.getBio());
            }

            // Step 2: Merge profiles
            ProfileConfig merged = merger.merge(configs);
            
            System.out.println("\n--- Merged Result ---");
            System.out.printf("Username: %s\n", merged.getUsername());
            System.out.printf("Avatar URL: %s\n", merged.getAvatarUrl() != null ? merged.getAvatarUrl() : "null");
            System.out.printf("Bio: %.100s...\n", merged.getBio());
            System.out.println("Pinned Repos: " + merged.getPinnedRepos());

            // Step 3: Validate the result
            ValidationResult validation = merger.validate(merged);
            
            if (!validation.isValid()) {
                System.out.println("\n--- Validation Issues ---");
                for (ValidationIssue issue : validation.getIssues()) {
                    System.out.printf("  %s - %s\n", 
                        issue.getType(), issue.getMessage());
                }
            } else {
                System.out.println("\n✓ Merged profile is valid!");
            }

            // Step 4: Export to file
            String outputPath = "merged_profile.json";
            merger.export(merged, outputPath);
            System.out.printf("Exported merged profile to: %s\n", outputPath);

        } catch (Exception e) {
            System.err.println("Error during merge operation:");
            e.printStackTrace();
        }
    }

    /**
     * Create sample configuration files for demonstration.
     */
    private static void createSampleConfigs() throws IOException {
        File configDir = new File("configs");
        
        if (!configDir.exists()) {
            System.out.println("Creating sample configs directory...");
            configDir.mkdirs();
            
            // Sample profile 1
            String profile1 = """
                {
                    "username": "johndoe",
                    "avatarUrl": "https://avatars.githubusercontent.com/u/12345678?v=4",
                    "bio": "Full-stack Java developer passionate about clean code and architecture.",
                    "pinnedRepos": ["cognis-digital/toolkit", "johndoe/sample-project"]
                }
                """;
            
            // Sample profile 2 (partial - will be merged)
            String profile2 = """
                {
                    "username": "johndoe",
                    "bio": "