class Movement
  def apply(simulator)
    raise 'NotImplemented'
  end
end

class ForwardMovement < Movement
  def initialize(to)
    @to = to
  end

  def apply(simulator)
    simulator.counter += Metrics.distance(simulator.floor, @to)
    simulator.floor = @to
  end

  def to_s
    "Forward movement to floor #{@to}"
  end
end

class ShortcutMovement < Movement
  def initialize(shortcut)
    @shortcut = shortcut
  end

  def apply(simulator)
    simulator.counter += @shortcut.weight
    simulator.floor = @shortcut.to
  end

  def to_s
    "Shortcut movement via #{@shortcut.inspect}"
  end
end
