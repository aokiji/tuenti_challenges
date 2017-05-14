#!/usr/bin/env ruby
require 'websocket-eventmachine-client'
require 'open-uri'
require 'nokogiri'
require 'base64'
require 'json'
require 'logger'
require 'v8'
require 'byebug'
require 'pry'

LOG = Logger.new(STDOUT)
LOG.level = Logger::DEBUG

class Explorer
  attr_accessor :row, :col, :rows, :cols, :movements

  def initialize(row, col, rows, cols)
    @row, @col, @rows, @cols = [row, col, rows, cols]
    @movements = []
  end

  def move
    movements.flatten.all? { |m| m.new(self).next }
  end
end

class WordCoordinates
  attr_accessor :word, :from_row, :from_col, :to_row, :to_col

  def initialize(word, frow, fcol, trow, tcol)
    @word = word
    @from_row, @from_col, @to_row, @to_col = [frow, fcol, trow, tcol]
  end
end

BaseMove = SimpleDelegator

class LeftMove < BaseMove
  def next
    return false if col == 0
    self.col -= 1
  end
end

class RightMove < BaseMove
  def next
    return false if col == cols - 1
    self.col += 1
  end
end

class DownMove < BaseMove
  def next
    return false if row == rows - 1
    self.row += 1
  end
end

class UpMove < BaseMove
  def next
    return false if row == 0
    self.row -= 1
  end
end

UpLeftMove = [UpMove, LeftMove]
UpRightMove = [UpMove, RightMove]
DownLeftMove = [DownMove, LeftMove]
DownRightMove = [DownMove, RightMove]

Directions = [
  LeftMove, RightMove, UpMove, DownMove, UpLeftMove,
  UpRightMove, DownRightMove, DownLeftMove
]

class Game
  attr_reader :words

  def initialize(board, words)
    @board = board
    @rows = @board.length
    @columns = @board[0].length
    @words = words
  end

  def find_word(word)
    (0..@rows-1).each do |row|
      (0..@columns-1).each do |col|
        found = word_at?(word, row, col)
        return WordCoordinates.new(word, row, col, found.row, found.col) if found
      end
    end
    byebug
    raise 'NotFound'
  end

  def word_at?(word, row, col)
    return unless @board[row][col] == word[0]
    Directions.each do |direction|
      explorer = Explorer.new(row, col, @rows, @columns)
      explorer.movements << direction
      found = (1..word.length-1).all? do |position|
        char = word[position]
        explorer.move && @board[explorer.row][explorer.col] == char
      end
      return explorer if found
    end
    return false
  end

end

STDOUT.sync =true

EM.run do
  def ws
    @ws ||= WebSocket::EventMachine::Client.connect(:uri => 'ws://52.49.91.111:3636/word-soup-challenge')
  end

  ws.onopen do
    logger.debug "Connected"
  end

  ws.onmessage do |msg, type|
    msg = Base64.decode64(msg)
    logger.debug "Received message: #{msg}"
    logger.debug "Type #{type}"

    if msg =~ /GAME OVER/ then
      logger.info "GAME OVER"
      next
    end

    @game ||= parse_game(msg)
    word = @game.words.shift
    if word
      logger.debug "Finding solution"
      solution = @game.find_word(word)
      send_solution(solution)
    elsif !msg.start_with?("d.selectWord")
      logger.debug "Finishing game"
      send_msg "play hard"
      @game = nil
    end
  end

  ws.onclose do |code, reason|
    logger.info "Disconnected with status code: #{code} #{reason}"
    exit 0
  end

  def send_msg(msg)
    logger.debug "Sending message: #{msg}"
    ws.send msg
  end

  def send_solution(solution)
    msg = [solution.from_col, solution.from_row, solution.to_col, solution.to_row].join('-')
    cxt = V8::Context.new
    code = %Q{
            function compute(t) {
              var n, e, o, s = 0;
              var c = t + "-saltbae";
              if (!c.length) return s;
              for (n = 0, o = c.length; n < o; n++) e = c.charCodeAt(n), s = (s << 5) - s + e, s |= 0;
              return Math.abs(s)
            }
            compute("#{msg}")
        }
    s = cxt.eval code
    msg = [msg, s].join('-')
    logger.debug msg
    msg = Base64.encode64(msg)
    send_msg(msg)
  end

  def parse_game(msg)
    game = JSON.parse(msg[/s=\[\[.*\]\]/].sub(/^s=/, ''))
    words = JSON.parse(msg[/,c=\[[^\]]*\]/].sub(/^,c=/, ''))

    Game.new(game, words)
  end

  def logger
    LOG
  end

end
