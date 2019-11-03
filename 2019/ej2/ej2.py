#!/usr/bin/env python

import sys
import networkx
from networkx.classes.function import info, nodes

def readGraph(input):
  graph = networkx.DiGraph()
  nodes = int(input.readline())
  for node in range(nodes):
    node_name, connections = input.readline().rstrip().split(':')
    for other_node in connections.split(','):
      graph.add_edge(node_name, other_node)
  return graph

input = sys.stdin
cases = int(input.readline())
for case in range(cases):
  graph = readGraph(input)
  paths = networkx.all_simple_paths(graph, source='Galactica', target='New Earth')
  number_of_paths = sum(1 for path in paths)
  print(f"Case #{case+1}: {number_of_paths}")
