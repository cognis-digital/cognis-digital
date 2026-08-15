# frozen_string_literal: true

require 'yaml'
require 'json'
require 'fileutils'

module CognisDigital
  # Merges multiple GitHub profile configuration files intelligently.
  # Later values override earlier ones, but nested structures are deep-merged.
  class ProfileMerger
    DEFAULT_CONFIG_DIR = File.join(Dir.home, '.cognis-digital', 'profiles')
    
    # Priority order for conflict resolution (higher number = higher priority)
    PRIORITY_FIELDS = {
      name: 10,
      email: 10,
      bio: 9,
      avatar_url: 8,
      links: 7,
      social_links: 6,
      metadata: 5
    }.freeze

    # Default values for fields that might be missing
    DEFAULT_VALUES = {
      name: 'Anonymous',
      email: '',
      bio: '',
      avatar_url: nil,
      links: [],
      social_links: []
    }.freeze

    def self.merge_files(file_paths, output_path: nil)
      merged = {}
      
      file_paths.each do |path|
        config = read_config(path)
        next unless config
        
        merged.deep_merge!(config)
      end
      
      # Apply default values for any missing fields
      merged.deep_merge!(DEFAULT_VALUES.transform_values { |v| v.nil? ? '' : v })
      
      output_path ||= File.join(DEFAULT_CONFIG_DIR, 'merged_profile.yaml')
      write_config(output_path, merged)
      
      output_path
    end

    def self.read_config(path)
      return nil unless File.exist?(path)
      
      content = File.read(path).strip
      return nil if content.empty?
      
      # Try YAML first (more common for config files), then JSON
      begin
        YAML.safe_load(content, permitted_classes: [Date, Time])
      rescue Psych::SyntaxError, StandardError
        begin
          JSON.parse(content)
        rescue JSON::ParserError, StandardError
          nil
        end
      end
    end

    def self.write_config(path, data)
      FileUtils.mkdir_p(File.dirname(path)) unless File.exist?(path)
      
      content = YAML.dump(data)
      File.write(path, content)
      
      path
    end

    # Deep merge two hashes with smart conflict resolution
    def self.deep_merge!(base, overlay)
      return base if base.nil? || overlay.nil?
      
      result = base.dup
      
      overlay.each do |key, value|
        if PRIORITY_FIELDS.key?(key.to_sym) && key.to_sym != :metadata
          # High-priority fields: later wins completely
          result[key] = value
        elsif result.key?(key) && result[key].is_a?(Hash) && value.is_a?(Hash)
          # Nested hashes: deep merge
          result.deep_merge!(value)
        elsif result.key?(key) && (result[key].is_a?(Array) || value.is_a?(Array))
          # Arrays: append with deduplication
          existing = result[key]
          new_items = value.select { |item| !existing.include?(item.to_s) }
          result[key] = existing + new_items
        else
          # Regular fields: later wins
          result[key] = value
        end
      end
      
      result
    end

    def self.validate_merged!(merged, strict: false)
      errors = []
      
      if merged[:name].nil? || merged[:name].empty?
        errors << 'Name is required' unless strict
        merged[:name] = DEFAULT_VALUES[:name]
      end
      
      # Validate email format
      if !merged[:email].nil? && !merged[:email].empty?
        unless merged[:email].match?(/\A[\w\.-]+@[\w\.-]+\.\w+\z/)
          errors << 'Invalid email format' unless strict
        end
      end
      
      # Validate avatar URL if present
      if merged[:avatar_url] && !merged[:avatar_url].nil?
        unless merged[:avatar_url].match?(/\Ahttps?:\/\/.+\z/)
          errors << 'Avatar URL must be a valid HTTP(S) URL' unless strict
        end
      end
      
      # Validate links array
      if merged[:links] && !merged[:links].is_a?(Array)
        errors << 'Links should be an array of strings' unless strict
        merged[:links] = []
      end
      
      # Validate social_links structure
      if merged[:social_links] && !merged[:social_links].is_a?(Array)
        errors << 'Social links should be an array of hashes' unless strict
        merged[:social_links] = []
      end
      
      errors
    end

    def self.generate_preview(merged, max_length: 500)
      preview = {
        name: merged[:name],
        email: merged[:email],
        bio: merged[:bio].truncate(max_length, suffix: '...') if merged[:bio]
      }
      
      # Include links if any
      if merged[:links] && !merged[:links].empty?
        preview[:links_count] = merged[:links].length
        preview[:has_links] = true
      end
      
      preview.to_yaml
    end

    def self.create_sample_configs(output_dir: nil)
      output_dir ||= File.join(Dir.home, '.cognis-digital', 'sample_profiles')
      
      FileUtils.mkdir_p(output_dir) unless File.exist?(output_dir)
      
      samples = [
        {
          file: 'base_profile.yaml',
          content: <<~YAML
            name: John Doe
            email: john@example.com
            bio: Software engineer passionate about Ruby and open source.
            avatar_url: https://example.com/avatar.jpg
            links:
              - "https://github.com/johndoe"
              - "https://twitter.com/johndoe"
          YAML
        },
        {
          file: 'override_profile.yaml',
          content: <<~YAML
            email: john.doe@newcompany.com
            bio: Senior engineer at NewCo. Previously at TechCorp.
            links:
              - "https://linkedin.com/in/johndoe"
          YAML
        },
        {
          file: 'metadata.yaml',
          content: <<~YAML
            metadata:
              created_at: 2024-01-15
              last_updated: 2024-01-15
              version: "1.0"
          YAML
        }
      ]
      
      samples.each do |sample|
        full_path = File.join(output_dir, sample[:file])
        File.write(full_path, sample[:content])
      end
      
      output_dir
    end

    def self.run_demo!
      puts "=== Cognis Digital Profile Merger Demo ==="
      
      # Create sample configs if they don't exist
      create_sample_configs unless File.exist?(File.join(DEFAULT_CONFIG_DIR, 'base_profile.yaml'))
      
      # Merge the samples
      merged_path = merge_files([
        File.join(DEFAULT_CONFIG_DIR, 'base_profile.yaml'),
        File.join(DEFAULT_CONFIG_DIR, 'override_profile.yaml')
      ])
      
      puts "\nMerged config written to: #{merged_path}"
      puts "\n--- Merged Result ---"
      puts generate_preview(YAML.safe_load(File.read(merged_path)))
    end
  end

  # Run demo if executed directly
  ProfileMerger.run_demo! unless ARGV.any?
end