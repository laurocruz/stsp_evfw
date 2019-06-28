import os
import sys

def append_data(ans_f, csv_f):
  ans = ans_f.readlines()
  ans_name = ans_f.name.split('/')[-1].split('.')[0]
  try:
    nodes = int(ans[2].split()[-1])
    edges = int(ans[3].split()[-1])
    scenarios = int(ans[4].split()[-1])

    probabilities = [0.0 for _ in range(scenarios)]
    ps_tmp = ans[6].split(' | ')
    for i in range(len(ps_tmp) - 1):
      probabilities[i] = float(ps_tmp[i].split()[-1])

    max_inflation = float(ans_name.split('_')[2].split('.')[0]) / 10
    cost = float(ans[-1].split()[-1])
    time = float(ans[-2].split()[-1])
    all_equal = 0
    if ans[-4].strip('\n') == 'ALL EQUAL':
      all_equal = 1
  except Exception as e:
    print('Error in file {:s}'.format(ans_name))
    print(e)
    print()
    return False
  # name,nodes,edges,scenarios,max_inflation,average_inflation,cost,time,all_equal
  line = '{:s},{:d},{:d},{:d},{:f},{:f},{:f},{:d}'.format(ans_name, nodes, edges, scenarios, max_inflation, cost, time, all_equal)

  for p in probabilities:
    line += ',{:f}'.format(p)
  line += '\n'

  csv_f.write(line)

  return True

def main():
  if len(sys.argv) < 3:
    print('Usage: <outputs_folder> <output_csv_file>')
  outputs_folder = sys.argv[1]
  output_csv_file = sys.argv[2]

  # Adds extension.
  if not output_csv_file.endswith('.csv'):
    output_csv_file += '.csv'

  with open(output_csv_file, 'w') as output_file:
    for answer_file_name in os.listdir(outputs_folder):
      if answer_file_name.endswith('.out'):
        with open(outputs_folder + '/' + answer_file_name, 'r') as answer_file:
          append_data(answer_file, output_file)

if __name__ == '__main__':
  main()
