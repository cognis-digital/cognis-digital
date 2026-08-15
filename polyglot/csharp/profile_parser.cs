using System;
using System.Collections.Generic;
using System.IO;
using System.Text.Json;
using System.Text.RegularExpressions;

namespace cognis.digital
{
    /// <summary>
    /// Configuration model for GitHub profile customization.
    /// </summary>
    public class ProfileConfig
    {
        // Core identity fields
        public string? Username { get; set; }
        public string? DisplayName { get; set; }
        public string? Bio { get; set; }

        // Visual settings
        public string? AvatarUrl { get; set; }
        public int? AvatarSize { get; set; } = 128;

        // Links and social
        public List<ProfileLink> Links { get; set; } = new();

        // Technical metadata
        public ProfileMetadata Metadata { get; set; } = new();

        // Customization flags
        public bool EnableDarkMode { get; set; } = true;
        public bool ShowMetrics { get; set; } = false;
    }

    /// <summary>
    /// External links for the profile.
    /// </summary>
    public class ProfileLink
    {
        public string? Label { get; set; }
        public string? Url { get; set; }
        public string? Icon { get; set; }

        public bool IsValid => !string.IsNullOrEmpty(Url);
    }

    /// <summary>
    /// Metadata about the profile configuration.
    /// </summary>
    public class ProfileMetadata
    {
        public string? ToolVersion { get; set; } = "1.0.0";
        public DateTime CreatedAt { get; set; } = DateTime.UtcNow;
        public int? ThemeId { get; set; }
        public bool AutoUpdate { get; set; } = true;
    }

    /// <summary>
    /// Result of the parsing operation.
    /// </summary>
    public class ParseResult
    {
        public ProfileConfig? Config { get; set; }
        public List<string> Warnings { get; set; } = new();
        public List<string> Errors { get; set; } = new();

        public bool IsValid => !Errors.Any() && (Config == null || !string.IsNullOrEmpty(Config.Username));

        public string? ErrorSummary => Errors.Count > 0 ? string.Join("; ", Errors) : null;
    }

    /// <summary>
    /// Main parser for GitHub profile configuration files.
    /// </summary>
    public static class ProfileParser
    {
        private const string DefaultToolName = "cognis-digital";

        /// <summary>
        /// Parses a JSON config file and returns the result.
        /// </summary>
        public static ParseResult ParseFile(string path)
        {
            if (!File.Exists(path))
            {
                return new ParseResult
                {
                    Errors = { $"Config file not found: {path}" },
                    Warnings = { "No configuration loaded" }
                };
            }

            try
            {
                var content = File.ReadAllText(path);
                return ParseContent(content, path);
            }
            catch (JsonException ex)
            {
                return new ParseResult
                {
                    Errors = { $"JSON parsing error: {ex.Message}" },
                    Warnings = { "Check file encoding and format" }
                };
            }
        }

        /// <summary>
        /// Parses a JSON string directly.
        /// </summary>
        public static ParseResult Parse(string json)
        {
            try
            {
                var config = JsonSerializer.Deserialize<ProfileConfig>(json);
                return ValidateAndBuild(config, null);
            }
            catch (JsonException ex)
            {
                return new ParseResult
                {
                    Errors = { $"JSON parsing error: {ex.Message}" },
                    Warnings = { "Verify JSON syntax" }
                };
            }
        }

        /// <summary>
        /// Parses content with optional file path for better error messages.
        /// </summary>
        private static ParseResult ParseContent(string json, string? sourcePath)
        {
            var config = JsonSerializer.Deserialize<ProfileConfig>(json);
            return ValidateAndBuild(config, sourcePath);
        }

        /// <summary>
        /// Validates the parsed configuration and builds the final result.
        /// </summary>
        private static ParseResult ValidateAndBuild(ProfileConfig? raw, string? sourcePath)
        {
            var result = new ParseResult();

            // Handle null input gracefully
            if (raw == null)
            {
                result.Errors.Add("Empty or invalid configuration");
                return result;
            }

            // Validate core fields with helpful messages
            if (string.IsNullOrWhiteSpace(raw.Username))
            {
                result.Errors.Add("Username is required. Set 'username' in your config.");
            }

            if (!IsValidUrl(raw.AvatarUrl, "Avatar URL"))
            {
                result.Warnings.Add($"Avatar URL may be invalid: {raw.AvatarUrl}");
            }

            // Validate and normalize links
            var validLinks = new List<ProfileLink>();
            foreach (var link in raw.Links)
            {
                if (!string.IsNullOrEmpty(link.Url))
                {
                    if (!IsValidUrl(link.Url, "Link URL"))
                    {
                        result.Warnings.Add($"Invalid link: {link.Label ?? "Unnamed"}");
                    }
                    else
                    {
                        validLinks.Add(link);
                    }
                }
            }

            // Only keep valid links
            if (validLinks.Count != raw.Links.Count)
            {
                result.Warnings.Add($"Filtered out {raw.Links.Count - validLinks.Count} invalid link(s)");
            }

            raw.Links = validLinks;

            // Set defaults for optional fields
            if (!raw.Metadata.CreatedAt.HasValue)
            {
                raw.Metadata.CreatedAt = DateTime.UtcNow;
            }

            result.Config = raw;
            return result;
        }

        /// <summary>
        /// Validates a URL string.
        /// </summary>
        private static bool IsValidUrl(string? url, string fieldName)
        {
            if (string.IsNullOrWhiteSpace(url))
                return true; // Empty is valid (user might want to disable feature)

            var pattern = @"^https?://(?:www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}";
            pattern += @".[-a-zA-Z0-9()]{1,256}";
            pattern += @".[-a-zA-Z0-9()]{1,256}\.[-a-zA-Z0-9()@:%_\+.~#?&//=]*$";

            return Regex.IsMatch(url, pattern);
        }

        /// <summary>
        /// Generates a sample configuration for quick start.
        /// </summary>
        public static string GenerateSampleConfig(string username)
        {
            var config = new ProfileConfig
            {
                Username = username,
                DisplayName = $"GitHub User ({username})",
                Bio = "Software Engineer | Open Source Contributor",
                AvatarUrl = "https://github.com/${{USERNAME}}.png",
                Links = new List<ProfileLink>
                {
                    new ProfileLink
                    {
                        Label = "Portfolio",
                        Url = "https://${{USERNAME}}.com"
                    },
                    new ProfileLink
                    {
                        Label = "LinkedIn",
                        Url = "https://linkedin.com/in/${{USERNAME}}"
                    }
                },
                Metadata = new ProfileMetadata
                {
                    ToolVersion = "1.0.0",
                    AutoUpdate = true,
                    ThemeId = 1
                }
            };

            return JsonSerializer.Serialize(config, new JsonSerializerOptions
            {
                WriteIndented = true,
                Encoder = System.Text.Encodings.Web.JavaScriptEncoder.UnsafeRelaxedJsonEscaping
            });
        }

        /// <summary>
        /// Main entry point for demonstration.
        /// </summary>
        public static void Main(string[] args)
        {
            Console.WriteLine($"=== {DefaultToolName} Profile Parser ===\n");

            // Demo 1: Parse a sample config string
            var sampleJson = GenerateSampleConfig("johndoe");
            Console.WriteLine("[Demo 1] Sample Config Generated:");
            Console.WriteLine(sampleJson);
            Console.WriteLine();

            var result = Parse(sampleJson);
            Console.WriteLine($"[Demo 1] Parse Result - Valid: {result.IsValid}");
            if (!string.IsNullOrEmpty(result.ErrorSummary))
                Console.WriteLine($"  Errors: {result.ErrorSummary}");
            foreach (var warning in result.Warnings)
                Console.WriteLine($"  Warning: {warning}");
            Console.WriteLine();

            // Demo 2: Parse with missing required field
            var badJson = @"{ ""DisplayName"": ""Test"", ""Links"": [] }";
            var badResult = Parse(badJson);
            Console.WriteLine("[Demo 2] Missing Username Field:");
            Console.WriteLine($"  Valid: {badResult.IsValid}");
            foreach (var error in badResult.Errors)
                Console.WriteLine($"  Error: {error}");
            Console.WriteLine();

            // Demo 3: Parse from file path
            if (args.Length > 0 && File.Exists(args[0]))
            {
                var fileResult = ParseFile(args[0]);
                Console.WriteLine("[Demo 3] File Parsing Result:");
                Console.WriteLine($"  Valid: {fileResult.IsValid}");
                foreach (var error in fileResult.Errors)
                    Console.WriteLine($"  Error: {error}");
            }

            // Demo 4: Edge cases
            var edgeCases = new[]
            {
                "",              // Empty string
                "null",          // Null JSON
                "{",             // Incomplete JSON
                @"{""Username"": ""test"", ""AvatarUrl"": ""invalid""}",  // Invalid URL
                @"{""Username"": ""test"", ""Links"": [{}]}"               // Malformed links
            };

            Console.WriteLine("[Demo 4] Edge Cases:");
            foreach (var test in edgeCases)
            {
                var r = Parse(test);
                Console.WriteLine($"  Input: [{test.Length} chars], Valid: {r.IsValid}, Errors: {r.Errors.Count}");
            }

            Console.WriteLine("\n=== Done ===");
        }
    }
}