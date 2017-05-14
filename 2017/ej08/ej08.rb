#!/usr/bin/env ruby
require 'pry'
require 'byebug'
require "unicode/numeric_value"
require 'unicode/name'
require 'unicode/categories'

def is_space?(line)
  char = line.encode(Encoding::UTF_8)
  Unicode::Categories.of(char).include?("Zs") || Unicode::Name.of(char) =~ /\bSPACE\b/
end

def left_trim(line)
  line_array = line.chars
  while !line_array.empty? && is_space?(line_array[0])
    line_array.shift
  end
  line_array.join
end

def right_trim(line)
  left_trim(line.reverse).reverse
end

NUMBER_REGEX = Regexp.new("^[[:digit:]]+$".encode("UTF-16LE"),
                          Regexp::FIXEDENCODING)

def extract_number(line)
  line = left_trim(line)
  line = right_trim(line)
  match = NUMBER_REGEX.match(line)
  return nil unless match
  match[0].chars.map do |x|
    Unicode::NumericValue.of x.encode(Encoding::UTF_8)
  end.join.to_i
end

STDOUT.sync =true
input = File.open('submitInput', 'rb:UTF-16LE')
cases = extract_number(input.readline.chomp)

(1..cases).each do |num_case|
  line = input.readline.chomp
  number = extract_number(line)
  number = number.nil? ? "N/A" : number.to_s(16)
  puts "Case ##{num_case}: #{number}"
end
