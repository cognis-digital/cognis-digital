package polyglot.java;

import java.io.*;
import java.nio.file.*;
import java.time.LocalDate;
import java.util.*;
import java.util.stream.Collectors;

/**
 * GitHub Profile Config Generator
 * 
 * Reads user profile data and generates a properly formatted config file
 * for GitHub profile customization tools.
 */
public class ProfileGenerator {

    private static final String DEFAULT_CONFIG_PATH = ".github/profile-config.json";
    private static final int MAX_BIO_LENGTH = 160;
    
    // Default values
    private static final Map<String, Object> DEFAULTS = Map.of(
        "name", "",
        "bio", "",
        "location", "",
        "website", "",
        "email", "",
        "twitter", "",
        "github", "",
        "linkedin", "",
        "stackOverflow", "",
        "badges", List.of(),
        "analytics", Map.of("enabled", false, "provider", ""),
        "theme", "default"
    );

    public static void main(String[] args) {
        DemoRunner runner = new DemoRunner();
        
        if (args.length > 0 && "--help".equals(args[0])) {
            printHelp();
            return;
        }

        try {
            // Load from command line or environment, then defaults
            ProfileConfig config = runner.loadFromArgsOrEnv();
            
            // Validate and normalize
            ProfileValidator.validate(config);
            ProfileNormalizer.normalize(config);
            
            // Generate output
            String jsonOutput = JsonSerializer.serialize(config);
            
            System.out.println("=== Generated Config ===");
            System.out.println(jsonOutput);
            System.out.println("========================\n");
            
            // Write to file if path provided or default exists
            Path outputPath = args.length > 0 ? Paths.get(args[0]) : DEFAULT_CONFIG_PATH;
            Files.writeString(outputPath, jsonOutput + "\n");
            
            System.out.println("Written to: " + outputPath.toAbsolutePath());
            
        } catch (Exception e) {
            System.err.println("Error: " + e.getMessage());
            if (e.getCause() != null) {
                System.err.println("Caused by: " + e.getCause().getMessage());
            }
            System.exit(1);
        }
    }

    /**
     * Load profile data from command line arguments or environment variables.
     */
    static ProfileConfig loadFromArgsOrEnv() {
        Map<String, String> rawValues = new HashMap<>();

        // Command line args: --name=John --bio=Developer
        for (int i = 1; i < main().length; i++) {
            if (main()[i].startsWith("--")) {
                int eqIndex = main()[i].indexOf('=');
                String key, value;
                
                if (eqIndex > 0) {
                    key = main()[i].substring(1, eqIndex);
                    value = main()[i].substring(eqIndex + 1);
                } else {
                    // Flag without value - use next arg or default
                    key = main()[i].substring(1);
                    if (i + 1 < main().length) {
                        value = main()[i + 1];
                        i++;
                    } else {
                        continue;
                    }
                }

                rawValues.put(key.toLowerCase(), value);
            }
        }

        // Environment variables: GITHUB_NAME, BIO, etc.
        for (String envKey : Arrays.asList(
                "GITHUB_NAME", "BIO", "LOCATION", "WEBSITE", 
                "EMAIL", "TWITTER", "LINKEDIN", "STACKOVERFLOW")) {
            String envValue = System.getenv(envKey);
            if (envValue != null) {
                rawValues.put(envKey.toLowerCase(), envValue);
            }
        }

        // Merge with defaults, command line overrides environment, 
        // environment overrides defaults
        ProfileConfig.Builder builder = new ProfileConfig.Builder();
        
        for (Map.Entry<String, String> entry : DEFAULTS.entrySet()) {
            String key = entry.getKey().toLowerCase();
            String defaultValue = entry.getValue() instanceof List ? 
                ((List<?>)entry.getValue()).stream()
                    .map(Object::toString)
                    .collect(Collectors.joining(", ")) : 
                entry.getValue().toString();

            if (!rawValues.containsKey(key)) {
                builder.set(entry.getKey(), defaultValue);
            }
        }

        // Override with provided values
        for (Map.Entry<String, String> entry : rawValues.entrySet()) {
            builder.set(entry.getKey(), entry.getValue());
        }

        return builder.build();
    }

    /**
     * Validate the profile configuration.
     */
    static void validate(ProfileConfig config) {
        List<String> errors = new ArrayList<>();

        // Check required fields if any specified
        if (config.getBio() != null && config.getBio().length() > MAX_BIO_LENGTH) {
            errors.add("Bio exceeds " + MAX_BIO_LENGTH + " characters");
        }

        // Validate URLs
        for (String url : Arrays.asList(
                config.getWebsite(), 
                config.getTwitter(),
                config.getGithub(),
                config.getLinkedin())) {
            if (!url.isEmpty() && !isValidUrl(url)) {
                errors.add("Invalid URL: " + url);
            }
        }

        // Validate email format
        String email = config.getEmail();
        if (email != null && !email.isEmpty()) {
            if (!email.matches("^[A-Za-z0-9+_.-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$")) {
                errors.add("Invalid email format: " + email);
            }
        }

        // Validate analytics config
        Object analytics = config.getAnalytics();
        if (analytics instanceof Map) {
            @SuppressWarnings("unchecked")
            Map<String, Object> aMap = (Map<String, Object>) analytics;
            
            if ("true".equals(aMap.get("enabled"))) {
                String provider = (String) aMap.getOrDefault("provider", "");
                if (!isValidAnalyticsProvider(provider)) {
                    errors.add("Invalid analytics provider: " + provider);
                }
            }
        }

        // Validate badges
        Object badges = config.getBadges();
        if (badges instanceof List) {
            @SuppressWarnings("unchecked")
            List<String> bList = (List<String>) badges;
            
            for (String badge : bList) {
                if (!isValidBadge(badge)) {
                    errors.add("Invalid badge: " + badge);
                }
            }
        }

        if (!errors.isEmpty()) {
            throw new ProfileValidationException(
                String.join("; ", errors));
        }
    }

    /**
     * Normalize the profile configuration.
     */
    static void normalize(ProfileConfig config) {
        // Trim strings
        for (String key : Arrays.asList(
                "name", "bio", "location", "website", 
                "email", "twitter", "github", "linkedin")) {
            Object value = config.get(key);
            if (value instanceof String) {
                config.set(key, ((String) value).trim());
            }
        }

        // Remove empty strings
        for (String key : Arrays.asList(
                "website", "email", "twitter", "github", 
                "linkedin", "stackoverflow")) {
            Object value = config.get(key);
            if (value instanceof String && ((String) value).isBlank()) {
                config.set(key, "");
            }
        }

        // Normalize analytics enabled flag
        Object analytics = config.getAnalytics();
        if (analytics instanceof Map) {
            @SuppressWarnings("unchecked")
            Map<String, Object> aMap = (Map<String, Object>) analytics;
            
            String enabledStr = (String) aMap.getOrDefault("enabled", "false");
            aMap.put("enabled", Boolean.parseBoolean(enabledStr));
        }

        // Normalize badges to lowercase
        Object badges = config.getBadges();
        if (badges instanceof List) {
            @SuppressWarnings("unchecked")
            List<String> bList = (List<String>) badges;
            
            bList.replaceAll(String::toLowerCase);
        }
    }

    /**
     * Serialize the profile configuration to JSON.
     */
    static String serialize(ProfileConfig config) {
        try {
            return new JsonEncoder().encode(config);
        } catch (JsonException e) {
            throw new ProfileSerializationException(
                "Failed to serialize: " + e.getMessage());
        }
    }

    // ==================== Inner Classes ====================

    /**
     * Main entry point for command-line arguments.
     */
    private static String[] main() {
        return System.getProperty("java.class.path").split(":");
    }

    /**
     * Builder pattern for constructing ProfileConfig instances.
     */
    static class Builder {
        private final Map<String, Object> data = new LinkedHashMap<>();

        public Builder set(String key, Object value) {
            this.data.put(key.toLowerCase(), value);
            return this;
        }

        public ProfileConfig build() {
            // Convert to immutable map with sorted keys for consistent output
            Map<String, Object> sortedData = new LinkedHashMap<>();
            
            String[] orderedKeys = {
                "name", "bio", "location", "website", 
                "email", "twitter", "github", "linkedin",
                "stackoverflow", "badges", "analytics", "theme"
            };

            for (String key : orderedKeys) {
                if (data.containsKey(key)) {
                    sortedData.put(key, data.get(key));
                } else {
                    // Use defaults from DEFAULTS map
                    Object defaultVal = DEFAULTS.getOrDefault(
                        key, null);
                    
                    if (defaultVal != null && !(defaultVal instanceof List)) {
                        sortedData.put(key, defaultVal.toString());
                    }
                }
            }

            return new ProfileConfig(sortedData);
        }
    }

    /**
     * Immutable profile configuration holder.
     */
    static class ProfileConfig {
        private final Map<String, Object> data;

        public ProfileConfig(Map<String, Object> data) {
            this.data = Collections.unmodifiableMap(data);
        }

        @SuppressWarnings("unchecked")
        public <T> T get(String key) {
            return (T) data.get(key);
        }

        public String getName() {
            return getString("name");
        }

        public String getBio() {
            return getString("bio");
        }

        public String getLocation() {
            return getString("location");
        }

        public String getWebsite() {
            return getString("website");
        }

        public String getEmail() {
            return getString("email");
        }

        public String getTwitter() {
            return getString("twitter");
        }

        public String getGithub() {
            return getString("github");
        }

        public String getLinkedin() {
            return getString("linkedin");
        }

        public String getStackOverflow() {
            return getString("stackoverflow");
        }

        @SuppressWarnings("unchecked")
        public List<String> getBadges() {
            Object b = data.get("badges");
            if (b instanceof List) {
                return (List<String>) b;
            }
            return new ArrayList<>();
        }

        @SuppressWarnings("unchecked")
        public Map<String, Object> getAnalytics() {
            Object a = data.get("analytics");
            if (a instanceof Map) {
                return (Map<String, Object>) a;
            }
            return Map.of("enabled", false);
        }

        public String getTheme() {
            return getString("theme");
        }

        @SuppressWarnings("unchecked")
        public <T> T set(String key, T value) {
            this.data.put(key.toLowerCase(), value);
            return (T) data.get(key.toLowerCase());
        }
    }

    // ==================== Utility Classes ====================

    /**
     * JSON encoder using minimal dependencies.
     */
    static class JsonEncoder {
        
        private static final String QUOTE = "\"";
        private static final String ESCAPE = "\\";
        private static final String TRUE_STR = "true";
        private static final String FALSE_STR = "false";
        private static final String NULL_STR = "null";

        public String encode(Object obj) throws JsonException {
            if (obj == null) {
                return NULL_STR;
            } else if (obj instanceof String) {
                return escapeString((String) obj);
            } else if (obj instanceof Boolean) {
                return ((Boolean) obj).booleanValue() ? TRUE_STR : FALSE_STR;
            } else if (obj instanceof Number) {
                return obj.toString();
            } else if (obj instanceof Map) {
                return encodeMap((Map<?, ?>) obj);
            } else if (obj instanceof List) {
                return encodeList((List<?>) obj);
            } else {
                throw new JsonException("Unsupported type: " + obj.getClass().getName());
            }
        }

        private String escapeString(String s) throws JsonException {
            StringBuilder sb = new StringBuilder();
            
            for (int i = 0; i < s.length(); i++) {
                char c = s.charAt(i);
                
                switch (c) {
                    case '"':
                        sb.append(ESCAPE).append(QQUOTE);
                        break;
                    case '\\':
                        sb.append(ESCAPE).append(ESCAPE);
                        break;
                    case '\b':
                        sb.append(ESCAPE).append("b");
                        break;
                    case '\t':
                        sb.append(ESCAPE).append("t");
                        break;
                    case '\n':
                        sb.append(ESCAPE).append("n");
                        break;
                    default:
                        sb.append(c);
                }
            }

            return QUOTE + sb.toString() + QUOTE;
        }

        private String encodeMap(Map<?, ?> map) throws JsonException {
            if (map.isEmpty()) {
                return "{}";
            }

            StringBuilder sb = new StringBuilder();
            sb.append("{");

            int i = 0;
            for (Map.Entry<?, ?> entry : map.entrySet()) {
                if (i > 0) {
                    sb.append(", ");
                }
                
                String key = escapeString(entry.getKey().toString());
                Object value = encode(entry.getValue());
                
                sb.append(key).append(": ").append(value);
                i++;
            }

            sb.append("}");
            return sb.toString();
        }

        private String encodeList(List<?> list) throws JsonException {
            if (list.isEmpty()) {
                return "[]";
            }

            StringBuilder sb = new StringBuilder();
            sb.append("[");

            int i = 0;
            for (Object item : list) {
                if (i > 0) {
                    sb.append(", ");
                }
                
                Object encoded = encode(item);
                sb.append(encoded);
                i++;
            }

            sb.append("]");
            return sb.toString();
        }
    }

    /**
     * JSON serialization exception.
     */
    static class JsonException extends Exception {
        public JsonException(String message) {
            super(message);
        }
    }

    /**
     * Profile validation exception.
     */
    static class ProfileValidationException extends Exception {
        public ProfileValidationException(String message) {
            super(message);
        }
    }

    /**
     * Profile serialization exception.
     */
    static class ProfileSerializationException extends Exception {
        public ProfileSerializationException(String message) {
            super(message);
        }
    }

    // ==================== Demo Runner ====================

    static class DemoRunner {
        
        private String[] args;

        public DemoRunner() {
            this.args = new String[0];
        }

        public ProfileConfig loadFromArgsOrEnv() {
            return ProfileGenerator.loadFromArgsOrEnv();
        }

        public void printHelp() {
            System.out.println("Usage: java polyglot.java.ProfileGenerator [options]");
            System.out.println("");
            System.out.println("Options:");
            System.out.println("  --name=<value>    Full name (default: empty)");
            System.out.println("  --bio=<value>     Short bio, max " + MAX_BIO_LENGTH + " chars");
            System.out.println("  --location=<value> Location/city");
            System.out.println("  --website=<url>   Personal website URL");
            System.out.println("  --email=<addr>    Email address");
            System.out.println("  --twitter=<handle> Twitter/X handle (without @)");
            System.out.println("  --github=<user>   GitHub username");
            System.out.println("  --linkedin=<url>  LinkedIn profile URL");
            System.out.println("  --stackoverflow=<url> Stack Overflow profile URL");
            System.out.println("  --theme=<name>    Theme name (default: default)");
            System.out.println("");
            System.out.println("Environment variables: GITHUB_NAME, BIO, LOCATION, etc.");
            System.out.println("");
            System.out.println("Example:");
            System.out.println("  java polyglot.java.ProfileGenerator --name=Jane Doe