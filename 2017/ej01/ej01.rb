#!/usr/bin/ruby
cases = STDIN.readline.chomp.to_i
(1..cases).each do |num_case|
  STDIN.readline
  total_slices = STDIN.readline.chomp.split(" ").inject(0) do |sum, current|
    sum + current.to_i
  end
  pizzas = (total_slices - 1) / 8 + 1
  puts "Case ##{num_case}: #{pizzas}"
end

