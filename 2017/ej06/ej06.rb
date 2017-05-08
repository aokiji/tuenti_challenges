#!/usr/bin/env ruby
require 'pry'
require 'byebug'
require 'logger'

Dir['lib/*'].each {|f| require File.expand_path("../#{f}", __FILE__)}

LOG = Logger.new(STDOUT)
LOG.level = Logger::INFO

class SimulatorState
  attr_accessor :floor, :counter, :strategy

  def initialize(floor, counter, strategy)
    @floor, @counter, @strategy = [floor, counter, strategy]
  end
end

class Simulator
  attr_accessor :counter, :floor, :case

  def initialize(case_obj)
    @case = case_obj
  end

  def reset
    @counter, @floor = [0, 1]
    @last_strategy = MovementStrategy.new
    @min = Metrics.distance(1, @case.floors)
    @min_at_floor = Hash.new {|hash, key| hash[key] = @min }
    @state = [ ]
  end

  def current_state
    SimulatorState.new(@floor, @counter, @last_strategy.dup)
  end

  def restore_state(state)
    @counter = state.counter
    @floor = state.floor
    @last_strategy = state.strategy
    #logger.debug "Restoring #{@floor} #{@last_strategy.inspect}"
  end

  def can_move?
    # return true if next move already found
    if @rollback == true && !@next_move.nil?
      #logger.debug "SKIPING move generator because of rollback"
      @rollback = false
      return @next_move
    end
    find_next_move
  end

  def find_next_move
    #logger.debug "Finding next move"
    #logger.debug "LAST" if @last_strategy.last?
    return @next_move = nil if @last_strategy.last?
    @last_strategy.next(self)

    while !@last_strategy.last?
      strategy = @last_strategy.strategy
      strategy.set_context(self)
      return @next_move = strategy.apply if strategy.applicable?
      @last_strategy.next(self)
    end

    return @next_move = nil
  end

  def apply_next_movement
    @state << current_state
    @next_move.apply(self)
    @last_strategy.reset
    # reject if movement doesn't win us anything
    if @counter >= @min_at_floor[@floor]
      rollback
    else
      @min_at_floor[@floor] = @counter
    end
    #logger.debug "New move applied #{@next_move}"
    #logger.debug @state.inspect

  end

  def top_floor?
    @floor == @case.floors
  end

  def update_min
    @min = @counter if @min > @counter
  end

  def rollback
    return if @state.empty?
    #logger.debug "Rolling back"
    loop do
      restore_state(@state.pop)
      break if can_move? || @state.empty?
    end
    #logger.debug "Finished rollback"
    @rollback = true
  end

  def logger
    LOG
  end

  def optimal_path
    reset

    while can_move?
      apply_next_movement
      if top_floor?
        update_min
        rollback
      end
    end

    @min
  end
end

STDOUT.sync =true
input = STDIN
cases = input.readline.chomp.to_i
(1..cases).each do |num_case|
  case_obj = Case.read(input)
  case_obj.call
  puts "Case ##{num_case}: #{case_obj}"
end
