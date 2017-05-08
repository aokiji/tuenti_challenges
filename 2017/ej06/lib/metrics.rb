module Metrics
  extend self

  def self.distance(from,to)
    ((from + to - 1) * (to - from)) / 2
  end
end
