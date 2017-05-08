class Shortcut
  attr_accessor :from, :to, :weight

  def initialize(from, to, weight)
    @from, @to, @weight = [from, to, weight]
  end

  def useless?
    Metrics.distance(@from, @to) <= @weight
  end

  def include?(other)
    other.from >= from && other.to <= to && weight < other.weight
  end
end

class Case
  attr_accessor :floors, :shortcuts

  def initialize(floors, shortcuts)
    @floors = floors
    @shortcuts = shortcuts
  end

  def self.read(input)
    floors, shortcuts = input.readline.chomp.split(" ").map(&:to_i)
    cuts = (1..shortcuts).map do
      from, to, weight = input.readline.chomp.split(" ").map(&:to_i)
      Shortcut.new(from, to, weight)
    end
    case_obj = Case.new(floors, cuts)
    case_obj.filter_shortcuts
    case_obj.sort_shortcuts
    case_obj
  end

  def sort_shortcuts
    @shortcuts.sort_by!(&:from)
  end

  def filter_shortcuts
    @shortcuts.reject!(&:useless?)
    filter_shortcuts_included
    @shortcuts
  end

  def filter_shortcuts_included
    cuts = []
    @shortcuts.each do |sc|
      cuts << sc unless @shortcuts.any? {|s| s.include?(sc)}
    end
    @shortcuts = cuts
  end

  def shortcuts_at(position)
    return [] unless shortcuts_by_from.key?(position)
    shortcuts_by_from[position]
  end

  def shortcuts_by_from
    return @shortcuts_by_from if @shortcuts_by_from
    @shortcuts_by_from = Hash.new {|hash, key| hash[key] = [] }
    @shortcuts.each {|s| @shortcuts_by_from[s.from] << s}
    @shortcuts_by_from
  end

  def call
    @result = Simulator.new(self).optimal_path
  end

  def to_s
    @result.to_s
  end
end
