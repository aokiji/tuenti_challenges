#!/usr/bin/ruby
class FrameScore
  attr_reader :bonus, :score, :scores

  def initialize(*scores)
    @scores = scores
    @bonus = 0
    @score = scores.inject(0) {|sum, p| sum + p}
    if scores.length == 1
      @bonus = 2
    else
      @bonus = 1 if @score == 10
    end
  end

  def to_s
    score.to_s
  end

  def scored(other)
    other.scores.each do |score|
      return unless @bonus > 0
      @bonus-= 1
      @score += score
    end
  end
end

class ListenersQueue < SimpleDelegator
  def initialize(num = 3)
    @size = num
    super Array.new
  end

  def push(value)
    __getobj__.shift if __getobj__.size >= @size
    __getobj__.push(value)
  end

  def <<(value)
    push(value)
  end

end

class FramesParser
  attr_accessor :frames

  def initialize
    @buffer = []
    @frames = []
  end

  def <<(value)
    @buffer << value.to_i
    flush if @buffer.length == 2 || @buffer.first == 10
  end

  def flush
    frames << @buffer.dup unless @buffer.empty?
    @buffer.clear
  end

  def frame_scores
    flush
    frames.map {|values| FrameScore.new(*values) }
  end
end

class ScoreCalculator
  def initialize(knocked_down_pins)
    @knocked_down_pins = knocked_down_pins
  end

  def score
    fp = FramesParser.new
    @knocked_down_pins.each {|v| fp << v }
    queue = ListenersQueue.new
    sum = 0
    frame_scores = fp.frame_scores
    frame_scores.each do |fs|
      queue.each { |listener| listener.scored(fs) }
      queue << fs
    end.map { |x| sum+=x.score }.first(10).join(' ')
  end
end

input = STDIN
cases = input.readline.chomp.to_i
(1..cases).each do |num_case|
  input.readline
  knocked_down_pins = input.readline.chomp.split(" ")
  calculator = ScoreCalculator.new(knocked_down_pins)
  puts "Case ##{num_case}: #{calculator.score}"
end
