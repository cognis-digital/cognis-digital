using System;
using System.Collections.Generic;
using System.IO;
using System.Text.Json;
using System.Threading.Tasks;

namespace polyglot.csharp;

/// <summary>
/// Constants for default configuration paths and formats.
/// </summary>
internal static class ProfileConstants
{
    public const string DefaultOutputDir = ".github";
    public const string DefaultConfigFile = "profile.config.json";
    public const string DefaultOutputFile = "profile.generated.json";
    
    // Common GitHub profile fields with their required status
    private static readonly Dictionary<string, bool> RequiredFields = new()
    {
        ["name"] = true,
        ["email"] = false,
        ["bio"] = false,
        ["location"] = false,
        ["website"] = false,
        ["twitter"] = false,
        ["linkedin"] = false,
        ["github"] = false,
    };

    public static bool IsRequired(string fieldName) => RequiredFields.TryGetValue(fieldName, out var required) && required;
}

/// <summary>
/// Represents the GitHub profile configuration model.
/// </summary>
public class ProfileConfig
{
    // Core identity fields
    public string? Name { get; set; }
    public string? Email { get; set; }
    public string? Bio { get; set; }
    
    // Location and contact
    public string? Location { get; set; }
    public string? Website { get; set; }
    public string? Twitter { get; set; }
    public string? LinkedIn { get; set; }
    public string? GitHub { get; set; }
    
    // Metadata and flags
    public bool PublicProfile { get; set; } = true;
    public bool IncludeTimestamps { get; set; } = false;
    public int? MaxBioLength { get; set; } = 160;
    
    // Nested configuration for advanced features
    public ProfileMetadata? Metadata { get; set; }
}

/// <summary>
/// Metadata stored alongside the profile data.
/// </summary>
public class ProfileMetadata
{
    public string? GeneratedBy { get; set; } = "cognis-digital";
    public DateTime? GeneratedAt { get; set; }
    public string? SourceFile { get; set; }
}

/// <summary>
/// Validation result containing success status and any error messages.
/// </summary>
public class ValidationResult
{
    public bool IsValid { get; set; } = true;
    public List<string> Errors { get; set; } = new();
    
    public static ValidationResult CreateValid() => new() { IsValid = true };
    
    public static ValidationResult CreateInvalid(string message) 
        => new() { IsValid = false, Errors = [message] };
}

/// <summary>
/// Service for validating profile configuration before generation.
/// </summary>
public class ProfileValidator
{
    private readonly ProfileConfig _config;
    
    public ProfileValidator(ProfileConfig config)
    {
        _config = config ?? throw new ArgumentNullException(nameof(config));
    }

    /// <summary>
    /// Validates the profile configuration and returns any errors found.
    /// </summary>
    public ValidationResult Validate()
    {
        var result = ValidationResult.CreateValid();
        
        if (string.IsNullOrWhiteSpace(_config.Name))
        {
            result.Errors.Add("Name is required but was empty or null.");
        }

        // Optional field validation with helpful hints
        if (!string.IsNullOrEmpty(_config.Email) && !IsValidEmail(_config.Email))
        {
            result.Errors.Add($"Invalid email format: {_config.Email}");
        }

        if (!string.IsNullOrWhiteSpace(_config.Bio) && _config.Bio.Length > ProfileConstants.MaxBioLength)
        {
            result.Errors.Add($"Bio exceeds maximum length of {ProfileConstants.MaxBioLength}. Current: {_config.Bio.Length}");
        }

        // Check for duplicate social links pointing to the same URL
        var urls = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        foreach (var field in _config.GetType().GetProperties())
        {
            if (field.PropertyType == typeof(string) && !string.IsNullOrEmpty(field.GetValue(_config)?.ToString()))
            {
                // Extract base URL for comparison
                string? urlBase = null;
                
                switch (field.Name)
                {
                    case "Website":
                        urlBase = _config.Website?.TrimEnd('/');
                        break;
                    case "Twitter":
                        if (!string.IsNullOrEmpty(_config.Twitter))
                            urlBase = $"https://twitter.com/{_config.Twitter}".TrimEnd('/');
                        break;
                    case "LinkedIn":
                        if (!string.IsNullOrEmpty(_config.LinkedIn))
                            urlBase = $"https://linkedin.com/in/{_config.LinkedIn}".TrimEnd('/');
                        break;
                }

                if (urlBase != null && urls.Contains(urlBase))
                {
                    result.Errors.Add($"Duplicate URL detected: {urlBase}");
                }
                
                if (urlBase != null)
                    urls.Add(urlBase);
            }
        }

        return result;
    }

    private static bool IsValidEmail(string email)
    {
        // Simple but effective email validation regex
        var pattern = @"^[^@\s]+@[^@\s]+\.[^@\s]+$";
        return System.Text.RegularExpressions.Regex.IsMatch(email, pattern);
    }
}

/// <summary>
/// Service for generating output files from a validated profile configuration.
/// </summary>
public class ProfileGenerator
{
    private readonly ProfileConfig _config;
    private readonly string? _outputDirectory;
    
    public ProfileGenerator(ProfileConfig config, string? outputDir = null)
    {
        _config = config ?? throw new ArgumentNullException(nameof(config));
        _outputDirectory = outputDir?.TrimEnd('/') ?? ProfileConstants.DefaultOutputDir;
    }

    /// <summary>
    /// Generates the profile configuration file in JSON format.
    /// </summary>
    public async Task<GenerationResult> GenerateAsync()
    {
        var validator = new ProfileValidator(_config);
        var validation = validator.Validate();

        if (!validation.IsValid)
        {
            return await HandleValidationErrors(validation);
        }

        // Prepare the output data with metadata
        var outputPath = Path.Combine(_outputDirectory, ProfileConstants.DefaultOutputFile);
        
        var outputData = CreateOutputObject(validation);
        
        // Ensure output directory exists
        if (!Directory.Exists(_outputDirectory))
        {
            Directory.CreateDirectory(_outputDirectory);
        }

        // Write the file with proper formatting and encoding
        await File.WriteAllTextAsync(outputPath, SerializeToJson(outputData), Encoding.UTF8);

        return CreateSuccessResult(outputPath, outputData);
    }

    /// <summary>
    /// Generates both JSON and YAML versions of the profile.
    /// </summary>
    public async Task<GenerationResult[]> GenerateMultiFormatAsync()
    {
        var validator = new ProfileValidator(_config);
        var validation = validator.Validate();

        if (!validation.IsValid)
        {
            return [await HandleValidationErrors(validation)];
        }

        var outputPath = Path.Combine(_outputDirectory, ProfileConstants.DefaultOutputFile);
        
        // Ensure output directory exists
        if (!Directory.Exists(_outputDirectory))
        {
            Directory.CreateDirectory(_outputDirectory);
        }

        var results = new List<GenerationResult>();

        // Generate JSON version (primary)
        var jsonPath = outputPath;
        var jsonData = CreateOutputObject(validation);
        await File.WriteAllTextAsync(jsonPath, SerializeToJson(jsonData), Encoding.UTF8);
        
        results.Add(CreateSuccessResult(jsonPath, jsonData));

        // Generate YAML version for compatibility with tools like OctoDeploy
        var yamlPath = outputPath + ".yaml";
        var yamlContent = CreateYamlContent(validation);
        await File.WriteAllTextAsync(yamlPath, yamlContent, Encoding.UTF8);
        
        results.Add(CreateSuccessResult(yamlPath, new { Path = yamlPath, Format = "YAML" }));

        return results.ToArray();
    }

    /// <summary>
    /// Creates the output object structure for JSON serialization.
    /// </summary>
    private static Dictionary<string, object> CreateOutputObject(ValidationResult validation)
    {
        var result = new Dictionary<string, object>();

        // Add core profile data
        if (!string.IsNullOrEmpty(_config.Name))
            result["name"] = _config.Name;
        
        if (!string.IsNullOrEmpty(_config.Email))
            result["email"] = _config.Email;
        
        if (!string.IsNullOrWhiteSpace(_config.Bio))
        {
            // Truncate bio if needed
            var truncatedBio = _config.Bio.Length > ProfileConstants.MaxBioLength 
                ? _config.Bio.Substring(0, ProfileConstants.MaxBioLength) + "..." 
                : _config.Bio;
            result["bio"] = truncatedBio;
        }

        // Add optional fields only if populated
        if (!string.IsNullOrEmpty(_config.Location))
            result["location"] = _config.Location;
        
        if (!string.IsNullOrEmpty(_config.Website))
            result["website"] = _config.Website;
        
        if (!string.IsNullOrEmpty(_config.Twitter))
            result["twitter"] = $"https://twitter.com/{_config.Twitter}";
        
        if (!string.IsNullOrEmpty(_config.LinkedIn))
            result["linkedin"] = $"https://linkedin.com/in/{_config.LinkedIn}";
        
        if (!string.IsNullOrEmpty(_config.GitHub))
            result["github"] = $"https://github.com/{_config.GitHub}";

        // Add metadata with generation info
        var metadata = new ProfileMetadata
        {
            GeneratedBy = "cognis-digital",
            GeneratedAt = DateTime.UtcNow,
            SourceFile = ProfileConstants.DefaultConfigFile
        };

        result["metadata"] = new
        {
            metadata.GeneratedBy,
            metadata.GeneratedAt,
            metadata.SourceFile,
            validation.IsValid,
            validation.Errors.Count > 0 ? new[] { validation.Errors } : null
        };

        return result;
    }

    /// <summary>
    /// Creates YAML content for tools that prefer YAML format.
    /// </summary>
    private static string CreateYamlContent(ValidationResult validation)
    {
        var lines = new List<string>();
        
        // Header comment
        lines.Add("# GitHub Profile Configuration");
        lines.Add($"# Generated by: cognis-digital at {_config.IncludeTimestamps ? DateTime.UtcNow.ToString("o") : "unknown"}");
        lines.Add("");

        // Core fields
        if (!string.IsNullOrEmpty(_config.Name))
            lines.Add($"name: {_config.EscapeYamlValue(_config.Name)}");
        
        if (!string.IsNullOrEmpty(_config.Email))
            lines.Add($"email: {_config.EscapeYamlValue(_config.Email)}");
        
        if (!string.IsNullOrWhiteSpace(_config.Bio))
        {
            var truncatedBio = _config.Bio.Length > ProfileConstants.MaxBioLength 
                ? _config.Bio.Substring(0, ProfileConstants.MaxBioLength) + "..." 
                : _config.Bio;
            lines.Add($"bio: {_config.EscapeYamlValue(truncatedBio)}");
        }

        // Optional fields
        if (!string.IsNullOrEmpty(_config.Location))
            lines.Add($"location: {_config.EscapeYamlValue(_config.Location)}");
        
        if (!string.IsNullOrEmpty(_config.Website))
            lines.Add($"website: {_config.EscapeYamlValue(_config.Website)}");
        
        if (!string.IsNullOrEmpty(_config.Twitter))
            lines.Add($"twitter: https://twitter.com/{_config.Twitter}");
        
        if (!string.IsNullOrEmpty(_config.LinkedIn))
            lines.Add($"linkedin: https://linkedin.com/in/{_config.LinkedIn}");
        
        if (!string.IsNullOrEmpty(_config.GitHub))
            lines.Add($"github: https://github.com/{_config.GitHub}");

        // Metadata section
        lines.Add("");
        lines.Add("# Metadata");
        lines.Add($"metadata:");
        lines.Add($"  generatedBy: cognis-digital");
        lines.Add($"  generatedAt: {_config.IncludeTimestamps ? DateTime.UtcNow.ToString("o") : "unknown"}");
        
        if (!validation.IsValid)
        {
            lines.Add($"  validationStatus: warning");
            foreach (var error in validation.Errors)
            {
                lines.Add($"    - {error}");
            }
        }

        return string.Join("\n", lines);
    }

    /// <summary>
    /// Serializes the output object to a properly formatted JSON string.
    /// </summary>
    private static string SerializeToJson(object data)
    {
        var options = new JsonSerializerOptions
        {
            WriteIndented = true,
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
            DefaultIgnoreCondition = System.Text.Json.Serialization.JsonIgnoreCondition.WhenWritingNull,
            Encoder = System.Text.Encodings.Web.JavaScriptEncoder.UnsafeRelaxedJsonEscaping
        };

        return JsonSerializer.Serialize(data, options);
    }

    /// <summary>
    /// Handles validation errors and returns an appropriate result.
    /// </summary>
    private async Task<GenerationResult> HandleValidationErrors(ValidationResult validation)
    {
        var errorPath = Path.Combine(_outputDirectory, "profile.errors.json");
        
        // Create an error report object
        var errorReport = new
        {
            Timestamp = DateTime.UtcNow,
            SourceFile = ProfileConstants.DefaultConfigFile,
            Errors = validation.Errors,
            ErrorCount = validation.Errors.Count,
            IsCritical = !validation.IsValid && (string.IsNullOrEmpty(_config.Name) || 
                string.IsNullOrEmpty(_config.Email))
        };

        // Write error report
        await File.WriteAllTextAsync(errorPath, SerializeToJson(errorReport), Encoding.UTF8);

        return new GenerationResult
        {
            Success = false,
            PrimaryOutputPath = null,
            Outputs = [new { Path = errorPath, Format = "Error Report" }],
            Errors = validation.Errors,
            Warnings = []
        };
    }

    /// <summary>
    /// Creates a result object indicating the outcome of generation.
    /// </summary>
    private static GenerationResult CreateSuccessResult(string primaryPath, object data)
    {
        return new GenerationResult
        {
            Success = true,
            PrimaryOutputPath = primaryPath,
            Outputs = [new { Path = primaryPath, Format = "JSON" }],
            Errors = [],
            Warnings = []
        };
    }

    /// <summary>
    /// Escapes a value for safe inclusion in YAML content.
    /// </summary>
    private static string EscapeYamlValue(string? value)
    {
        if (string.IsNullOrEmpty(value))
            return "null";
        
        // Check if quoting is needed
        var needsQuoting = value.Contains(":") || 
                          value.Contains("#") || 
                          value.Contains("\"") || 
                          value.Contains("\n");

        if (!needsQuoting)
            return value;

        // Quote the value and escape internal quotes
        var quotedValue = $"\"{value.Replace("\"", "\\\"")}\"";
        
        // Handle special YAML types (dates, booleans)
        if (DateTime.TryParse(value, out _))
            return $"\"{value}\"";
        
        if (bool.TryParse(value, out _) || 
            int.TryParse(value, out _) || 
            double.TryParse(value, out _))
            return value;

        return quotedValue;
    }
}

/// <summary>
/// Represents the result of a generation operation.
/// </summary>
public class GenerationResult
{
    public bool Success { get; set; }
    
    /// <summary>The primary output file path (for JSON format).</summary>
    public string? PrimaryOutputPath { get; set; }
    
    /// <summary>All generated files with their formats.</summary>
    public List<object> Outputs { get; set; } = new();
    
    /// <summary>Any errors encountered during generation.</summary>
    public List<string> Errors { get; set; } = new();
    
    /// <summary>Any warnings (non-critical issues).</summary>
    public List<string> Warnings { get; set; } = new();

    public static GenerationResult CreateSuccess(string primaryPath) 
        => new() { Success = true, PrimaryOutputPath = primaryPath };
}

/// <summary>
/// Extension methods for the ProfileConfig class.
/// </summary>
public static class ProfileConfigExtensions
{
    /// <summary>
    /// Creates a default configuration with common defaults applied.
    /// </summary>
    public static ProfileConfig CreateDefault() 
        =>