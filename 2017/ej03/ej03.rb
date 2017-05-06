#!/usr/bin/ruby

input = STDIN
cases = input.readline.chomp.to_i
(1..cases).each do |num_case|
  n = input.readline.chomp.to_i
  m = n.to_s(2)
  puts "Case ##{num_case}: #{m.length}"
end
