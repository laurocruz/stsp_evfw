import sys

INFLATION = 1000

def read_input():
  if len(sys.argv) < 2:
    print('Usage: <input_matlab_file> [<output_tsp_file>]')

  matlab_file = sys.argv[1]
  if not matlab_file.endswith('.txt'):
    input_file_name += '.txt'

  stp_file = ''
  if len(sys.argv) > 2:
    stp_file = sys.argv[2]
  else:
    stp_file = matlab_file.split('.')[0]

  if not stp_file.endswith('.stp'):
    stp_file += '.stp'

  return matlab_file, stp_file

def convert_w(w):
  return int(INFLATION * float(w))

def read_matlab_file(f):
  K = int(f.readline())
  V = int(f.readline())

  pr = []
  for i in range(K):
    pr.append(float(f.readline()))

  LOWER_ROW_V = (V * (V - 1)) // 2

  edges = [0 for _ in range(LOWER_ROW_V)]
  pr_edges = [[0 for _ in range(LOWER_ROW_V)] for _ in range(K)]

  i = 0
  for u in range(V):
    line = [convert_w(n) for n in f.readline().split(',')]
    for v in range(u):
      edges[i] = line[v]
      i += 1

  for k in range(K):
    i = 0
    for u in range(V):
      line = [convert_w(n) for n in f.readline().split(',')]
      for v in range(u):
        pr_edges[k][i] = line[v]
        i += 1

  return (V, pr, edges, pr_edges)

def write_stp_file(f, X):
  V = X[0]
  pr = X[1]
  edges = X[2]
  pr_edges = X[3]

    # Comment section.
  f.write('SECTION Comment\n')
  f.write('Auto Generated file.\n')
  name = f.name.split('/')[-1][:-4]
  f.write('Name \"{:s}\"\n'.format(name))
  f.write('Problem \"Stochastic Traveling Salesman Problem\"\n')
  f.write('Dimension: {:d}\n'.format(V))
  f.write('Scenarios: {:d}\n'.format(len(pr)))
  f.write('END\n\n')

  f.write('SECTION Graph\n')
  f.write('Nodes {:d}\n'.format(V))
  f.write('Edges {:d}\n'.format(len(edges)))
  f.write('Scenarios {:d}\n'.format(len(pr)))
  f.write('Root 0\n')
  i = 0
  for u in range(1, V):
    for v in range(u):
      f.write('E {:d} {:d} {:d}\n'.format(u + 1, v + 1, edges[i]))
      i += 1
  f.write('END\n\n')

  f.write('SECTION StochasticProbabilities\n')
  probability_string = ' '.join(map(str, pr))
  f.write('SP {:s}\n'.format(probability_string))
  f.write('END\n\n')

  f.write('SECTION StochasticWeights\n')
  for i in range(len(edges)):
    stochastic_edges = [pr_edges[k][i] for k in range(len(pr))]
    stochastic_edges_string = ' '.join(map(str, stochastic_edges))
    f.write('SE {:s}\n'.format(stochastic_edges_string))
  f.write('END\n\n')

  f.write('SECTION StochasticTerminals\n')
  # Particular case where all vertices are terminal in all scenarios.
  terminal_string = ' '.join(['1' for _ in range(len(pr))])
  for i in range(1, V + 1):
    f.write('ST {:d} {:s}\n'.format(i, terminal_string))
  f.write('END\n\n')

  f.write('EOF\n')


def main():
  matlab_file_name, stp_file_name = read_input()

  with open(matlab_file_name, 'r') as f:
    X = read_matlab_file(f)

  with open(stp_file_name, 'w') as f:
    write_stp_file(f, X)


if __name__ == '__main__':
  main()
