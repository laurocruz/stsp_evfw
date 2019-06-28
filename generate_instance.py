from math import sqrt
import random
import sys
import re

UNSPECIFIED = -1

# EDGE_WEIGHT_TYPE values.
[EXPLICIT, EUC_2D] = range(2)

# EDGE_WEIGHT_FORMAT values.
[FULL_MATRIX, UPPER_ROW, LOWER_ROW, UPPER_DIAG_ROW, LOWER_DIAG_ROW, \
    UPPER_COL, LOWER_COL, UPPER_DIAG_COL, LOWER_DIAG_COL] = range(9)

def distance(p1, p2):
  return int(sqrt((p1[0] - p2[0])**2 + (p1[1] - p2[1])**2))

def generate_probabilities(scenarios):
  probabilities = [random.randint(1, 100) for _ in range(scenarios)]
  soma = sum(probabilities)
  probabilities = [((p * 100) // soma) for p in probabilities]

  soma = sum(probabilities)
  if soma != 100:
    probabilities[0] += 100 - soma
    probabilities = random.sample(probabilities, k = len(probabilities))

  probabilities = [(p / 100) for p in probabilities]

  return probabilities

def read_input():
  if len(sys.argv) < 5:
    print('Usage: <input_tsp_file> <output_stp_file> <#_of_scenarios> <max_inflation_factor>')
    return '', '', -1, -1
  input_file_name = sys.argv[1]
  output_file_name = sys.argv[2]
  scenarios = int(sys.argv[3])
  factor_max = float(sys.argv[4])

  return input_file_name, output_file_name, scenarios, factor_max

def read_header(f):
  dimension = UNSPECIFIED
  edge_weight_type = UNSPECIFIED
  edge_weight_format = UNSPECIFIED

  while True:
    line = f.readline()
    if line.startswith('TYPE'):
      s = line.split()
      t = ''
      if s[1] == ':':
        t = s[2]
      else:
        t = s[1]
      if t != 'TSP':
        raise Exception('TYPE ' + t + ' not supported.')

    elif line.startswith('DIMENSION'):
      dimension = int(line.split()[-1])

    elif line.startswith('EDGE_WEIGHT_TYPE'):
      t = line.split()[-1]
      if t == 'EXPLICIT':
        edge_weight_type = EXPLICIT
      elif t == 'EUC_2D':
        edge_weight_type = EUC_2D
      else:
        raise Exception('EDGE_WEIGHT_TYPE ' + t + ' not supported.')

    elif line.startswith('EDGE_WEIGHT_FORMAT'):
      t = line.split()[-1]
      if t == 'FULL_MATRIX':
        edge_weight_format = FULL_MATRIX
      elif t == 'UPPER_ROW':
        edge_weight_format = UPPER_ROW
      elif t == 'LOWER_ROW':
        edge_weight_format = LOWER_ROW
      elif t == 'UPPER_DIAG_ROW':
        edge_weight_format = UPPER_DIAG_ROW
      elif t == 'LOWER_DIAG_ROW':
        edge_weight_format = LOWER_DIAG_ROW
      elif t == 'UPPER_COL':
        edge_weight_format = UPPER_COL
      elif t == 'LOWER_COL':
        edge_weight_format = LOWER_COL
      elif t == 'UPPER_DIAG_COL':
        edge_weight_format = UPPER_DIAG_COL
      elif t == 'LOWER_DIAG_COL':
        edge_weight_format = LOWER_DIAG_COL
      else:
        raise Exception('EDGE_WEIGHT_FORMAT ' + t + ' not supported.')

    elif line.startswith('EDGE_WEIGHT_SECTION') or line.startswith('NODE_COORD_SECTION'):
      break

  return dimension, edge_weight_type, edge_weight_format

# Edge position for LOWER_ROW representation.
def encode_edge(u, v):
  if u == v:
    return -1
  if u < v:
    u, v = v, u

  return ((u * (u - 1)) // 2) + v

# If there is a letter in the line that means we are not reading edges
# or points anymore.
def finished_reading(line):
  pattern = re.compile(r'[A-Z_]')
  if len(line) == 0 or line == '\n' or pattern.findall(line):
    return True
  return False

def read_explicit(input_file, dimension, edge_weight_format):
  file_edges = []

  while True:
    line = input_file.readline()
    if finished_reading(line):
      break
    for edge in map(int, line.split()):
      file_edges.append(edge)

  i = 0
  edges = [0 for _ in range((dimension * (dimension - 1)) // 2)]
  if edge_weight_format == FULL_MATRIX:
    for u in range(dimension):
      for v in range(dimension):
        if u != v:
          edges[encode_edge(u, v)] = file_edges[i]
        i += 1
  elif edge_weight_format == LOWER_ROW:
    for u in range(1, dimension):
      for v in range(u):
        edges[encode_edge(u, v)] = file_edges[i]
        i += 1
  elif edge_weight_format == LOWER_DIAG_ROW:
    for u in range(dimension):
      for v in range(u + 1):
        if u != v:
          edges[encode_edge(u, v)] = file_edges[i]
        i += 1
  elif edge_weight_format == UPPER_ROW:
    for u in range(dimension - 1):
      for v in range(u + 1, dimension):
        edges[encode_edge(u, v)] = file_edges[i]
        i += 1
  elif edge_weight_format == UPPER_DIAG_ROW:
    for u in range(dimension):
      for v in range(u, dimension):
        if u != v:
          edges[encode_edge(u, v)] = file_edges[i]
        i += 1
  elif edge_weight_format == LOWER_COL:
    for v in range(dimension - 1):
      for u in range(v + 1, dimension):
        edges[encode_edge(u, v)] = file_edges[i]
        i += 1
  elif edge_weight_format == LOWER_DIAG_COL:
    for v in range(dimension):
      for u in range(v, dimension):
        if u != v:
          edges[encode_edge(u, v)] = file_edges[i]
        i += 1
  elif edge_weight_format == UPPER_COL:
    for v in range(1, dimension):
      for u in range(v):
        edges[encode_edge(u, v)] = file_edges[i]
        i += 1
  elif edge_weight_format == UPPER_DIAG_COL:
    for v in range(dimension):
      for u in range(v + 1):
        if u != v:
          edges[encode_edge(u, v)] = file_edges[i]
        i += 1
  else:
    raise Exception('EDGE_WEIGHT_FORMAT not specified.')

  return edges

def read_euc_2d(input_file, dimension):
  # Read points.
  points = []
  while True:
    line = input_file.readline()
    if finished_reading(line):
      break
    index, x, y = line.split()
    points.append((float(x), float(y)))

  edges = []
  for u in range(1, dimension):
    for v in range(u):
      edges.append(distance(points[u], points[v]))

  return edges

def read_edges(input_file, dimension, edge_weight_type, edge_weight_format):
  edges = []

  if edge_weight_type is EXPLICIT:
    edges = read_explicit(input_file, dimension, edge_weight_format)
  elif edge_weight_type is EUC_2D:
    edges = read_euc_2d(input_file, dimension)
  else:
    raise Exception('EDGE_WEIGHT_TYPE not specified')

  return edges


def inflate_edge(edge, scenarios, factor_max):
  return [int(edge * ((random.random() * factor_max) + 1)) for _ in range(scenarios)]

def write_output_file(output_file, dimension, edges, probabilities, factor_max):
    # Comment section.
  output_file.write('SECTION Comment\n')
  output_file.write('Auto Generated file.\n')
  name = output_file.name.split('/')[-1][:-4]
  output_file.write('Name \"{:s}\"\n'.format(name))
  output_file.write('Problem \"Stochastic Traveling Salesman Problem\"\n')
  output_file.write('Dimension: {:d}\n'.format(dimension))
  output_file.write('Scenarios: {:d}\n'.format(len(probabilities)))
  output_file.write('END\n\n')

  output_file.write('SECTION Graph\n')
  output_file.write('Nodes {:d}\n'.format(dimension))
  output_file.write('Edges {:d}\n'.format(len(edges)))
  output_file.write('Scenarios {:d}\n'.format(len(probabilities)))
  output_file.write('Root 0\n')
  i = 0
  for u in range(1, dimension):
    for v in range(u):
      output_file.write('E {:d} {:d} {:d}\n'.format(u + 1, v + 1, edges[i]))
      i += 1
  output_file.write('END\n\n')

  output_file.write('SECTION StochasticProbabilities\n')
  probability_string = ' '.join(map(str, probabilities))
  output_file.write('SP {:s}\n'.format(probability_string))
  output_file.write('END\n\n')

  output_file.write('SECTION StochasticWeights\n')
  for edge in edges:
    stochastic_edges = inflate_edge(edge, len(probabilities), factor_max)
    stochastic_edges_string = ' '.join(map(str, stochastic_edges))
    output_file.write('SE {:s}\n'.format(stochastic_edges_string))
  output_file.write('END\n\n')

  output_file.write('SECTION StochasticTerminals\n')
  # Particular case where all vertices are terminal in all scenarios.
  terminal_string = ' '.join(['1' for _ in range(len(probabilities))])
  for i in range(1, dimension + 1):
    output_file.write('ST {:d} {:s}\n'.format(i, terminal_string))
  output_file.write('END\n\n')

  output_file.write('EOF\n')

def main():
  input_file_name, output_file_name, scenarios, factor_max = read_input()
  if input_file_name == '':
    return

  # Adds extensions.
  if not input_file_name.endswith('.tsp'):
    input_file_name += '.tsp'

  if not output_file_name.endswith('.stp'):
    output_file_name += '.stp'

  input_file = open(input_file_name, 'r')

  probabilities = generate_probabilities(scenarios)
  dimension, edge_weight_type, edge_weight_format = read_header(input_file)
  edges = read_edges(input_file, dimension, edge_weight_type, edge_weight_format)
  input_file.close()

  output_file = open(output_file_name, 'w')
  write_output_file(output_file, dimension, edges, probabilities, factor_max)
  output_file.close()

if __name__ == '__main__':
  main()
