#!/usr/bin/ruby
require 'yaml'

Dir.chdir(File.dirname(__FILE__))

def get_default(option)
    if option['type'] == 'boolean'
        option['default'].to_s.upcase
    else
        option['default']
    end
end
def get_readme_typestring(option)
    type = case option['type']
    when 'integer'
        if option.has_key? 'valid_values' and option['valid_values'].is_a? Array
            # We should eventually support e.g. ranges and such.
            option['valid_values'].join '|'
        else
            'NUMBER'
        end
    when 'boolean'
        'TRUE|FALSE'
    when 'character'
        'CHAR'
    else
        option['type'].upcase
    end
    if option.has_key? 'can_disable' and option['can_disable']
        type += '|DISABLED'
    end
    
    type
end
def get_config_printf(name, info, max_length)
    string = "printf(\"#define #{sprintf "% -#{max_length}s", name} %s\\n\", "
    if info['type'] == 'boolean'
        string += "(#{name} == FALSE ? \"FALSE\" : \"TRUE\")"
    else
        string += "stringify(#{name})"
    end
    string += ");"
end

config = YAML::load_file 'config.yaml'

max_length = config['options'].keys.max {|a,b| a.length <=> b.length }.length
max_length_dist = config['dist_options'].max {|a,b| a.to_s.length <=> b.to_s.length }.length
max_length_err = config['err_options'].max {|a,b| a.to_s.length <=> b.to_s.length }.length

File.open('../README.ZSCONFIG', 'w') do |file|
    file.puts config['readme_header']
    config['options'].sort.each do |name, info|
        file.puts "#{name} <#{get_readme_typestring info}>", info['comment'].gsub(/^/, "\t")
        file.puts "\tDefault: #{get_default info}", ''
    end
end
File.open('../zipscript/include/zsconfig.defaults.h', 'w') do |file|
    file.puts config['defaults_header']
    config['options'].sort.each do |name, info|
        next if config['err_options'].include? name
        file.puts "#ifndef #{name}"
        file.puts "#define #{name}_is_defaulted"
        file.puts "#define #{sprintf "% -#{max_length}s", name} #{get_default info}"
        file.puts "#endif", ''
    end
end
File.open('../zipscript/include/errors.h', 'w') do |file|
    config['err_options'].each do |name|
        info = config['options'][name]
        file.puts "#ifndef #{name}"
        file.puts "#define #{name}_is_defaulted"
        file.puts "#define #{sprintf "% -#{max_length_err}s", name} #{get_default info}"
        file.puts "#endif", ''
    end
end
File.open('../zipscript/conf/zsconfig.h.dist', 'w') do |file|
    file.puts config['dist_header']
    config['dist_options'].each do |name|
        if name.nil?
            file.puts ''
        else
            info = config['options'][name]
            file.puts "#define #{sprintf "% -#{max_length_dist}s", name} #{get_default info}"
        end
    end
end

File.open('../zipscript/src/print_config.c', 'w') do |file|
    file.puts '#include <stdio.h>', ''
    file.puts '#include "constants.h"'
    file.puts '#include "print_config.h"', ''
    file.puts '#include "../conf/zsconfig.h"'
    file.puts '#include "zsconfig.defaults.h"', ''
    file.puts '#include "errors.h"'

    file.puts '/* Hardcore preprocessor hackery, thanks to runehol/#scene.no. */'
    file.puts '#define expand(x) x'
    file.puts '#define stringize(x) expand(#x)'
    file.puts '#define stringify(x) stringize(x)'

    file.puts 'void print_nondefault_config(void)', '{'
    config['options'].sort.each do |name, info|
        file.puts "#ifndef #{name}_is_defaulted"
        file.puts get_config_printf(name, info, max_length)
        file.puts "#endif"
    end
    file.puts '}', ''

    file.puts 'void print_full_config(void)', '{'
    config['options'].sort.each do |name, info|
        file.puts get_config_printf(name, info, max_length)
    end
    file.puts '}'
end
