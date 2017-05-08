class MovementStrategy
  NEXT_HASH = {
    none: :shortcut,
    shortcut: :goback,
    goback: :forward,
    forward: nil
  }

  def initialize
    reset
  end

  def next(context)
    return @type if next_iteration
    @type = NEXT_HASH[@type]
    @value = nil
    start_shortcut_iterations(context) if @type == :shortcut
    start_goback_iterations(context) if @type == :goback
  end

  def last?
    @type.nil?
  end

  def strategy
    Object.const_get("#{@type.to_s.capitalize}Strategy").new(@value)
  end

  def reset
    @type = :none
    @value = nil
  end

  private

  def next_iteration
    if ! @value.nil? && @value > 0
      @value -= 1
      return true
    end
    return false
  end

  def start_goback_iterations(context)
    st = strategy
    st.set_context(context)
    @value = st.previous_shortcuts.length - 1
  end

  def start_shortcut_iterations(context)
    st = strategy
    st.set_context(context)
    @value = st.forward_shortcuts.length - 1
  end

end

class BaseStrategy
  attr_reader :context, :iterations

  def initialize(iterations)
    @iterations = iterations
  end

  def set_context(new_context)
    @context = new_context
  end
end

class ForwardStrategy < BaseStrategy
  def applicable?
    context.case.shortcuts_at(context.floor).none? {|s| s.to - s.from == 1}
  end

  def apply
    next_shortcut = context.case.shortcuts.bsearch { |s| s.from > context.floor }
    next_floor = (next_shortcut && next_shortcut.from) || context.case.floors
    ForwardMovement.new(next_floor)
  end
end

class ShortcutStrategy < BaseStrategy
  def applicable?
    !forward_shortcuts.empty?
  end

  def apply
    ShortcutMovement.new(forward_shortcuts[iterations])
  end

  def forward_shortcuts
    @shortcuts ||= context.case.shortcuts.select do |shortcut|
      shortcut.from == context.floor
    end
  end
end

class GobackStrategy < BaseStrategy
  def applicable?
    !previous_shortcuts.empty?
  end

  def apply
    ShortcutMovement.new(previous_shortcuts[iterations])
  end

  def previous_shortcuts
    shortcuts = context.case.shortcuts || []
    @shortcuts ||= shortcuts.reject do |shortcut|
      shortcut.from >= context.floor ||
      shortcut.to <= context.floor ||
      Metrics.distance(context.floor, shortcut.to) <= shortcut.weight
    end
  end
end
