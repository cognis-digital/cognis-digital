require 'json'
require 'yaml'
require 'fileutils'
require 'open3'

module CognisDigital
  module ProfileGenerator
    DEFAULTS = {
      name: '',
      bio: '',
      location: '',
      website: '',
      email: '',
      twitter: '',
      linkedin: '',
      github: '',
      stackoverflow: '',
      npm: '',
      dockerhub: ''
    }.freeze

    class << self
      def generate(output_dir:, input: {})
        merged = DEFAULTS.merge(input)
        merged[:github] ||= merged[:name].gsub(/\s+/, '').downcase.gsub(/[^a-z0-9]/, '')
        
        File.write(File.join(output_dir, 'profile.yaml'), YAML.dump(merged))
        File.write(File.join(output_dir, 'profile.json'), JSON.pretty_generate(merged))
        generate_readme(merged) if merged[:name] || merged[:bio]
        
        merged
      end

      def validate!(input = {})
        errors = []
        input.each do |key, value|
          next unless DEFAULTS.key?(key)
          
          case key
          when :name
            errors << "Name '#{value}' is too short (min 2 chars)" if value.length < 2
          when :bio
            errors << "Bio must not exceed 160 characters" if value.length > 160
          when :email
            errors << "Invalid email format: #{value}" unless value.match?(/\A[\w\.-]+@[\w\.-]+\.\w+\z/)
          else
            errors << "#{key} must be a string, got #{value.class.name}" unless value.is_a?(String)
          end
        end
        
        raise "Validation failed: #{errors.join(', ')}" if errors.any?
        true
      rescue => e
        puts "[WARN] Validation warning: #{e.message}"
        false
      end

      def generate_readme(profile = {})
        lines = [
          "# GitHub Profile Configuration",
          "",
          "## Summary",
          "",
          "| Field | Value |",
          "|-------|-------|"
        ]
        
        profile.each do |key, value|
          next if value.empty?
          label = key.to_s.capitalize.sub(/([a-z])([A-Z])/, '\1\2') # camelCase to Title Case
          lines << "| #{label} | #{value} |"
        end
        
        File.write(File.join('README.md'), lines.join("\n")) if profile.any? { |_, v| !v.empty? }
      rescue => e
        puts "[WARN] Failed to generate README: #{e.message}"
        false
      end

      def preview(profile = {})
        output_dir = File.expand_path('../output', __dir__)
        FileUtils.mkdir_p(output_dir) unless File.exist?(output_dir)
        
        validate!(profile) || (puts "Running with defaults..." && profile.merge(DEFAULTS))
        generate(output_dir: output_dir, input: profile)
        puts "\nGenerated files:"
        puts "  #{File.join(output_dir, 'profile.yaml')}"
        puts "  #{File.join(output_dir, 'profile.json')}"
        
        if File.exist?(File.join(output_dir, 'README.md'))
          puts "  #{File.join(output_dir, 'README.md')}"
        end
        
        puts "\nPreview of profile.yaml:"
        puts "=" * 40
        puts YAML.dump(profile)
      rescue => e
        puts "[ERROR] Preview failed: #{e.message}"
        puts e.backtrace.first(5).join("\n")
      end
    end
  end
end

if __FILE__ == $0
  require 'optparse'
  
  parser = OptionParser.new do |opts|
    opts.banner = "Usage: ruby profile_generator.rb [--name=...] [--bio=...] [-p]"
    
    DEFAULTS.keys.each do |key|
      opts.on("-#{key}", "--#{key}=", "#{key.to_s.capitalize} (default: '#{DEFAULTS[key]}')") { |v| @profile ||= {}; @profile[key] = v }
    end
    
    opts.on('-p', '--preview', 'Generate and show preview') do
      @preview = true
    end
    
    opts.on('-o', '--output=DIR', 'Output directory (default: ./output)') { |v| @output_dir = v }
    
    opts.on_tail('-h', '--help') { puts parser; exit }
  end
  
  profile = {}
  
  begin
    parser.parse!
  rescue OptionParser::InvalidArgument => e
    puts "[ERROR] #{e.message}"
    puts parser
    exit 1
  end
  
  @profile ||= {}
  @output_dir ||= File.expand_path('output', __dir__)
  
  if @preview
    ProfileGenerator.preview(@profile)
  else
    result = ProfileGenerator.generate(output_dir: @output_dir, input: @profile)
    puts "\nProfile generated successfully!"
    puts "Output directory: #{@output_dir}"
    puts "Files created:"
    Dir.glob("#{@output_dir}/*").each { |f| puts "  - #{File.basename(f)}" }
  end
end