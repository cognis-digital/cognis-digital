require 'json'
require 'yaml'
require 'pathname'

module CognisDigital
  module ProfileParser
    ERROR_CODES = {
      missing_file: :missing_file,
      invalid_yaml: :invalid_yaml,
      invalid_json: :invalid_json,
      empty_config: :empty_config,
      missing_user: :missing_user,
      missing_name: :missing_name,
      missing_email: :missing_email,
      unknown_format: :unknown_format
    }.freeze

    class << self
      def parse(file_path)
        file_path = Pathname.new(file_path).expand_path
        raise ERROR_CODES[:missing_file], "File not found: #{file_path}" unless file_path.exist?

        content = File.read(file_path, encoding: 'UTF-8')
        return nil if content.strip.empty?

        result = parse_content(content)
        raise ERROR_CODES[:empty_config] if result.nil? || result.empty?

        validate!(result)
        result
      end

      def parse_content(content)
        formats = {
          'yaml' => YAML,
          'json' => JSON
        }

        content.strip!
        return nil if content.nil?

        # Try to detect format by extension first
        ext = Pathname.new(content).extension.downcase
        parser = formats[ext] || (content.include?('---') ? YAML : JSON)

        begin
          data = parser.load(content)
          return data unless data.is_a?(Hash)

          # Normalize nested keys for consistent access
          normalize_keys(data)
        rescue ArgumentError, Psych::SyntaxError => e
          raise ERROR_CODES[:invalid_yaml], "YAML parse error: #{e.message}" if ext == 'yaml'
          raise ERROR_CODES[:invalid_json], "JSON parse error: #{e.message}" if ext == 'json'

          # Fallback: try the other parser
          fallback = ext == 'yaml' ? JSON : YAML
          begin
            data = fallback.load(content)
            normalize_keys(data)
          rescue => e2
            raise ERROR_CODES[:unknown_format], "Unknown or malformed format: #{e2.message}"
          end
        end

        # Extract user info from normalized structure
        extract_user_info(data)
      end

      def validate!(data, options = {})
        errors = []

        required_fields = {
          name: [:name],
          email: [:email]
        }

        required_fields.each do |field, keys|
          value = data.deep_get(keys)
          if value.nil? || value.to_s.strip.empty?
            errors << ERROR_CODES[:missing_#{field}]
          end
        end

        # Validate email format
        email = data.deep_get(:email)
        if email && !email.match?(/\A[^@\s]+@[^@\s]+\.[^@\s]+\z/)
          errors << :invalid_email_format
        end

        { valid: errors.empty?, warnings: [], data: data, errors: errors }
      rescue => e
        { valid: false, warnings: [e.message], data: nil, errors: [:parse_error] }
      end

      def deep_get(hash, *keys)
        keys.each do |k|
          return nil unless hash.is_a?(Hash) && hash.key?(k)
          hash = hash[k]
        end
        hash
      rescue => _e
        nil
      end

      def normalize_keys(data)
        # Normalize common key variations
        data.transform_values do |v|
          v.is_a?(String) ? v.strip : v
        end

        # Handle nested user info patterns
        if data.key?('user') && data['user'].is_a?(Hash)
          data.merge!(data.delete('user'))
        elsif data.key?('profile') && data['profile'].is_a?(Hash)
          data.merge!(data.delete('profile'))
        end

        # Normalize boolean strings
        data.transform_values do |v|
          if v.is_a?(String)
            case v.downcase
            when 'true', 'yes' then true
            when 'false', 'no'  then false
            else v
            end
          else
            v
          end
        end

        data
      rescue => _e
        data
      end

      def extract_user_info(data)
        {
          name: data.deep_get(:name, :full_name, :username),
          email: data.deep_get(:email),
          username: data.deep_get(:username, :user, :handle),
          avatar_url: data.deep_get(:avatar_url, :avatar),
          bio: data.deep_get(:bio, :about, :description)
        }
      rescue => _e
        {}
      end

      def format_output(data, options = {})
        output = {
          valid: true,
          user: {},
          warnings: [],
          errors: []
        }

        if data.is_a?(Hash)
          output[:user] = extract_user_info(data)
          result = validate!(data)
          output.merge!(result) unless result[:valid]
        else
          output[:errors] << :invalid_data_type
        end

        # Apply formatting options
        output[:user].transform_values! do |v|
          v.is_a?(String) ? v.strip : v
        end if output[:user]

        output
      rescue => e
        { valid: false, user: {}, warnings: [e.message], errors: [:format_error] }
      end
    end
  end
end

# Demo / Entry Point
if __FILE__ == $0
  require 'pp'

  # Example usage patterns
  demo_configs = {
    yaml_example: <<~YAML,
      ---
      name: "Jane Doe"
      email: jane@example.com
      username: janedoe
      avatar_url: https://example.com/avatar.png
      bio: "Ruby enthusiast and developer"
    YAML

    json_example: <<~JSON,
      {
        "name": "John Smith",
        "email": "john@smith.io",
        "username": "jsmith",
        "avatar_url": "https://example.com/jsmith.png",
        "bio": "Full-stack developer"
      }
    JSON

    nested_example: <<~YAML,
      ---
      profile:
        name: "Nested User"
        email: nested@example.com
        username: nested_user
    YAML

    minimal_example: <<~YAML,
      ---
      name: "Minimal"
    YAML
  }

  puts "=" * 60
  puts "COGNIS-DIGITAL PROFILE PARSER - DEMO"
  puts "=" * 60
  puts

  demo_configs.each do |label, content|
    puts "Testing: #{label}"
    puts "-" * 40

    begin
      result = CognisDigital::ProfileParser.parse_content(content)
      validation = CognisDigital::ProfileParser.validate!(result)

      if validation[:valid]
        puts "  ✓ Valid configuration"
        puts "  User: #{validation[:data].fetch(:name, 'Unknown')}"
        puts "  Email: #{validation[:data].fetch(:email, 'Not set')}"
      else
        puts "  ✗ Validation errors:"
        validation[:errors].each { |e| puts "    - #{e}" }
      end

    rescue CognisDigital::ProfileParser::ERROR_CODES.keys.any? => e
      puts "  ⚠ Error: #{e.message}"
    rescue StandardError => e
      puts "  ⚠ Unexpected error: #{e.class} - #{e.message}"
    end

    puts
  end

  # Real file parsing demo (commented out for safety)
  # puts "=" * 60
  # puts "REAL FILE PARSING"
  # puts "=" * 60
  #
  # config_file = ARGV.first || 'polyglot/ruby/profile_parser.yml'
  # begin
  #   result = CognisDigital::ProfileParser.parse(config_file)
  #   if result && result[:valid]
  #     puts "Successfully parsed: #{config_file}"
  #     puts "User: #{result[:data][:name]}"
  #   else
  #     puts "Parse issues:"
  #     result[:errors].each { |e| puts "  - #{e}" } if result[:errors]
  #   end
  # rescue => e
  #   puts "Error: #{e.message}"
  # end
end