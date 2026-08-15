package polyglot.java;

import java.io.*;
import java.nio.file.*;
import java.util.*;
import java.util.regex.*;

/**
 * Profile Parser for GitHub configuration files.
 * Supports YAML and JSON formats commonly used in .github/config.yaml or similar setups.
 */
public class profile_parser {

    // ==================== DATA MODEL ====================

    public record ProfileConfig(
        String name,
        String bio,
        String location,
        String website,
        List<String> socialLinks,
        Map<String, Object> metadata
    ) {}

    // ==================== PARSER FACTORY ====================

    private static final class ParserFactory {
        public static ProfileConfig parse(String content) throws IOException {
            if (content == null || content.trim().isEmpty()) {
                throw new IllegalArgumentException("Empty input");
            }

            String trimmed = content.trim();
            
            // Detect format by header or structure
            if (trimmed.startsWith("---") && trimmed.contains(":")) {
                return parseYaml(content);
            } else if (trimmed.startsWith("{") || trimmed.startsWith("[{")) {
                return parseJson(content);
            } else {
                throw new IOException("Unknown format: " + trimmed.substring(0, Math.min(50, trimmed.length())));
            }
        }

        private static ProfileConfig parseYaml(String content) throws IOException {
            // Simple YAML parser for common profile fields
            ProfileConfig config = new ProfileConfig(null, null, null, null, null, nullMap());
            
            try (StringReader reader = new StringReader(content)) {
                String line;
                while ((line = reader.readLine()) != null) {
                    line = line.trim();
                    if (line.isEmpty() || line.startsWith("#") || line.startsWith("---")) {
                        continue;
                    }

                    // Parse key: value pairs
                    int colonIndex = line.indexOf(':');
                    if (colonIndex > 0) {
                        String key = line.substring(0, colonIndex).trim();
                        String value = line.substring(colonIndex + 1).trim();

                        switch (key.toLowerCase()) {
                            case "name":
                                config.name = unquote(value);
                                break;
                            case "bio" || case "description":
                                config.bio = unquote(value);
                                break;
                            case "location":
                            case "based_in":
                                config.location = unquote(value);
                                break;
                            case "website" || case "homepage":
                                config.website = unquote(value);
                                break;
                            case "social_links" || case "links":
                                parseSocialLinks(config, value);
                                break;
                            default:
                                // Store unknown keys in metadata
                                if (config.metadata == nullMap()) {
                                    config.metadata = new LinkedHashMap<>();
                                }
                                config.metadata.put(key.toLowerCase(), unquote(value));
                        }
                    } else if (line.startsWith("- ")) {
                        // Handle list items like: - https://twitter.com/username
                        String item = line.substring(2).trim();
                        if (config.socialLinks == nullList()) {
                            config.socialLinks = new ArrayList<>();
                        }
                        config.socialLinks.add(unquote(item));
                    } else if (!line.startsWith("-")) {
                        // Handle nested structures or list items without dash prefix
                        String item = line.trim();
                        if (config.socialLinks == nullList()) {
                            config.socialLinks = new ArrayList<>();
                        }
                        config.socialLinks.add(unquote(item));
                    }
                }
            }

            return config;
        }

        private static ProfileConfig parseJson(String content) throws IOException {
            // Simple JSON parser for profile fields
            ProfileConfig config = new ProfileConfig(null, null, null, null, nullList(), nullMap());
            
            try (StringReader reader = new StringReader(content)) {
                int braceDepth = 0;
                StringBuilder currentKey = new StringBuilder();
                StringBuilder currentValue = new StringBuilder();
                
                for (int i = 0; i < content.length(); i++) {
                    char c = content.charAt(i);

                    if (c == '{') {
                        braceDepth++;
                        continue;
                    } else if (c == '}') {
                        braceDepth--;
                        if (braceDepth == 0) break;
                        continue;
                    }

                    // Handle key-value pairs
                    if (c == '"' && !currentKey.isEmpty()) {
                        String key = currentKey.toString();
                        String val = currentValue.toString().trim();

                        switch (key.toLowerCase()) {
                            case "name":
                                config.name = unquote(val);
                                break;
                            case "bio" || case "description":
                                config.bio = unquote(val);
                                break;
                            case "location":
                            case "based_in":
                                config.location = unquote(val);
                                break;
                            case "website" || case "homepage":
                                config.website = unquote(val);
                                break;
                            case "social_links" || case "links":
                                parseJsonSocialLinks(config, val);
                                break;
                            default:
                                if (config.metadata == nullMap()) {
                                    config.metadata = new LinkedHashMap<>();
                                }
                                config.metadata.put(key.toLowerCase(), unquote(val));
                        }

                        currentKey.setLength(0);
                        currentValue.setLength(0);
                    } else if (c != ' ' && c != '\t' && c != ':' && c != ',' && c != '[' && c != ']') {
                        currentValue.append(c);
                    } else if (c == '"' && !currentKey.isEmpty()) {
                        // End of key or value string
                        String key = currentKey.toString();
                        String val = currentValue.toString().trim();

                        switch (key.toLowerCase()) {
                            case "name":
                                config.name = unquote(val);
                                break;
                            case "bio" || case "description":
                                config.bio = unquote(val);
                                break;
                            case "location":
                            case "based_in":
                                config.location = unquote(val);
                                break;
                            case "website" || case "homepage":
                                config.website = unquote(val);
                                break;
                            case "social_links" || case "links":
                                parseJsonSocialLinks(config, val);
                                break;
                            default:
                                if (config.metadata == nullMap()) {
                                    config.metadata = new LinkedHashMap<>();
                                }
                                config.metadata.put(key.toLowerCase(), unquote(val));
                        }

                        currentKey.setLength(0);
                        currentValue.setLength(0);
                    } else if (c != ' ' && c != '\t' && c != ':' && c != ',' && c != '[' && c != ']') {
                        currentValue.append(c);
                    }
                }
            }

            return config;
        }

        // ==================== HELPER METHODS ====================

        private static void parseSocialLinks(ProfileConfig config, String value) throws IOException {
            if (config.socialLinks == nullList()) {
                config.socialLinks = new ArrayList<>();
            }
            
            // Handle both inline list and string representation
            if (value.startsWith("[")) {
                try {
                    String[] items = value.substring(1, value.length() - 1).split(",");
                    for (String item : items) {
                        config.socialLinks.add(unquote(item.trim()));
                    }
                } catch (Exception e) {
                    // Fallback: treat as single string
                    config.socialLinks.add(unquote(value));
                }
            } else {
                config.socialLinks.add(unquote(value));
            }
        }

        private static void parseJsonSocialLinks(ProfileConfig config, String value) throws IOException {
            if (config.socialLinks == nullList()) {
                config.socialLinks = new ArrayList<>();
            }
            
            // Handle JSON array string like: ["https://twitter.com/x", "https://linkedin.com/in/y"]
            if (value.startsWith("[")) {
                try {
                    String[] items = value.substring(1, value.length() - 1).split(",");
                    for (String item : items) {
                        config.socialLinks.add(unquote(item.trim()));
                    }
                } catch (Exception e) {
                    config.socialLinks.add(unquote(value));
                }
            } else {
                config.socialLinks.add(unquote(value));
            }
        }

        private static String unquote(String value) {
            if (value == null || value.isEmpty()) return "";
            
            // Remove surrounding quotes
            if ((value.startsWith("\"") && value.endsWith("\"")) || 
                (value.startsWith("'") && value.endsWith("'"))) {
                return value.substring(1, value.length() - 1);
            }
            
            return value;
        }

        private static <T> T nullList() {
            return null;
        }

        private static <K, V> Map<K, V> nullMap() {
            return null;
        }

        // ==================== FILE OPERATIONS ====================

        public static ProfileConfig parseFile(String path) throws IOException {
            if (path == null || path.trim().isEmpty()) {
                throw new IllegalArgumentException("Path cannot be empty");
            }

            Path p = Paths.get(path);
            if (!Files.exists(p)) {
                throw new FileNotFoundException("Profile file not found: " + path);
            }

            String content;
            try (BufferedReader reader = Files.newBufferedReader(p, StandardCharsets.UTF_8)) {
                StringBuilder sb = new StringBuilder();
                String line;
                while ((line = reader.readLine()) != null) {
                    sb.append(line).append('\n');
                }
                content = sb.toString();
            }

            return parse(content);
        }

        public static ProfileConfig parseFile(Path path) throws IOException {
            if (path == null || !Files.exists(path)) {
                throw new FileNotFoundException("Profile file not found: " + path);
            }

            String content;
            try (BufferedReader reader = Files.newBufferedReader(path, StandardCharsets.UTF_8)) {
                StringBuilder sb = new StringBuilder();
                String line;
                while ((line = reader.readLine()) != null) {
                    sb.append(line).append('\n');
                }
                content = sb.toString();
            }

            return parse(content);
        }

        // ==================== VALIDATION ====================

        public static boolean validate(ProfileConfig config, String purpose) {
            if (config == null) {
                System.err.println("Validation failed: Config is null");
                return false;
            }

            List<String> errors = new ArrayList<>();

            // Check required fields based on purpose
            switch (purpose.toLowerCase()) {
                case "public":
                case "default":
                    if (config.name == null || config.name.trim().isEmpty()) {
                        errors.add("Name is required for public profiles");
                    }
                    break;
                
                case "minimal":
                case "internal":
                    // Minimal validation
                    break;

                default:
                    // Default to minimal
                    break;
            }

            if (!errors.isEmpty()) {
                System.err.println("Validation errors for '" + purpose + "' profile:");
                for (String error : errors) {
                    System.err.println("  - " + error);
                }
                return false;
            }

            System.out.println("Validation passed for '" + purpose + "' profile.");
            return true;
        }

        // ==================== OUTPUT ====================

        public static String format(ProfileConfig config) {
            if (config == null) return "null";

            StringBuilder sb = new StringBuilder();
            sb.append("Profile Configuration:\n");
            sb.append("=====================\n\n");

            appendIfPresent(sb, "Name", config.name);
            appendIfPresent(sb, "Bio/Description", config.bio);
            appendIfPresent(sb, "Location", config.location);
            appendIfPresent(sb, "Website", config.website);

            if (config.socialLinks != nullList()) {
                sb.append("\nSocial Links:\n");
                for (String link : config.socialLinks) {
                    sb.append("  - ").append(link).append('\n');
                }
            }

            if (config.metadata != nullMap() && !config.metadata.isEmpty()) {
                sb.append("\nMetadata:\n");
                for (var entry : config.metadata.entrySet()) {
                    sb.append("  ").append(entry.getKey()).append(": ")
                      .append(entry.getValue()).append('\n');
                }
            }

            return sb.toString();
        }

        private static void appendIfPresent(StringBuilder sb, String label, Object value) {
            if (value != null && !Objects.equals(value, "")) {
                sb.append(label).append(": ").append(value).append('\n');
            }
        }

        // ==================== DEMO / ENTRY POINT ====================

        public static void main(String[] args) throws Exception {
            System.out.println("GitHub Profile Parser - Demo\n");
            System.out.println("=".repeat(40));

            // Sample YAML config (typical .github/config.yaml format)
            String sampleYaml = """
            ---
            name: "Jane Doe"
            bio: "Senior Java Engineer | Open Source Contributor"
            location: "San Francisco, CA"
            website: "https://janedoe.dev"
            
            social_links:
              - "https://twitter.com/janedoe"
              - "https://github.com/janedoe"
              - "https://linkedin.com/in/janedoe"
              
            metadata:
              email: "jane@example.com"
              timezone: "America/Los_Angeles"
            """;

            // Sample JSON config
            String sampleJson = """
            {
                "name": "John Smith",
                "bio": "Full-Stack Developer",
                "location": "New York, NY",
                "website": "https://johnsmith.io",
                "social_links": [
                    "https://twitter.com/johnsmith",
                    "https://github.com/johnsmith"
                ],
                "metadata": {
                    "email": "john@example.com",
                    "timezone": "America/New_York"
                }
            }
            """;

            // Demo 1: Parse YAML string
            System.out.println("\n[Demo 1] Parsing YAML String...");
            try {
                ProfileConfig yamlProfile = parseYaml(sampleYaml);
                System.out.println(format(yamlProfile));
            } catch (Exception e) {
                System.err.println("Error: " + e.getMessage());
            }

            // Demo 2: Parse JSON string
            System.out.println("\n[Demo 2] Parsing JSON String...");
            try {
                ProfileConfig jsonProfile = parseJson(sampleJson);
                System.out.println(format(jsonProfile));
            } catch (Exception e) {
                System.err.println("Error: " + e.getMessage());
            }

            // Demo 3: Parse from file (create temp file for demo)
            System.out.println("\n[Demo 3] Parsing from File...");
            try {
                Path tempFile = Files.createTempFile("profile-", ".yaml");
                Files.writeString(tempFile, sampleYaml);
                
                ProfileConfig fileProfile = parseFile(tempFile.toString());
                System.out.println(format(fileProfile));
                
                // Cleanup
                Files.deleteIfExists(tempFile);
            } catch (Exception e) {
                System.err.println("Error: " + e.getMessage());
            }

            // Demo 4: Validation
            System.out.println("\n[Demo 4] Validating Profile...");
            try {
                ProfileConfig testProfile = parseYaml(sampleYaml);
                validate(testProfile, "public");
            } catch (Exception e) {
                System.err.println("Error: " + e.getMessage());
            }

            // Demo 5: Edge cases
            System.out.println("\n[Demo 5] Edge Cases...");
            
            // Empty input
            try {
                ProfileConfig empty = parse("");
                System.out.println("Empty string result: " + format(empty));
            } catch (Exception e) {
                System.err.println("Empty string error: " + e.getMessage());
            }

            // Whitespace only
            try {
                ProfileConfig whitespace = parse("   \n\n  ");
                System.out.println("Whitespace result: " + format(whitespace));
            } catch (Exception e) {
                System.err.println("Whitespace error: " + e.getMessage());
            }

            // Malformed JSON
            try {
                ProfileConfig malformed = parseJson("{invalid json}");
                System.out.println("Malformed result: " + format(malformed));
            } catch (Exception e) {
                System.err.println("Malformed error: " + e.getMessage());
            }

            // Demo 6: Real-world usage pattern
            System.out.println("\n[Demo 6] Real-World Usage Pattern...");
            try {
                // Simulate reading from a config directory
                String[] samplePaths = {"/tmp/profile.yaml", "/home/user/.github/config.json"};
                
                for (String path : samplePaths) {
                    System.out.println("\nChecking: " + path);
                    if (Files.exists(Paths.get(path))) {
                        ProfileConfig profile = parseFile(path);
                        boolean valid = validate(profile, "public");
                        if (valid) {
                            System.out.println("  Status: Ready for deployment");
                        } else {
                            System.out.println("  Status: Needs fixes before deployment");
                        }