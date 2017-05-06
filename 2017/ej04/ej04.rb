#!/usr/bin/ruby

input = STDIN
cases = input.readline.chomp.to_i
(1..cases).each do |num_case|
  sides = input.readline.chomp.split(" ").map(&:to_i)
  sides.shift
  min = nil
  sides.sort!
  (2..sides.length - 1).each do |i|
    break if !min.nil? && sides[i] > min
    (1..i-1).to_a.reverse.each do |j|
      (0..j-1).to_a.reverse.each do |k|
        break if sides[k] + sides[j] <= sides[i]
        perimeter = sides[k] + sides[j] + sides[i]
        if min.nil?
          min = perimeter
        elsif min > perimeter
          min = perimeter
        end
      end
    end
  end
  min = "IMPOSSIBLE" if min.nil?
  puts "Case ##{num_case}: #{min}"
end
