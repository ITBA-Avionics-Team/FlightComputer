import sys
import os
import matplotlib.pyplot as plt

metricsFilename = 'pressure_data.csv'
outputGraphsDir = 'tuvi.png'

with open(metricsFilename, "r") as reader:
  line = reader.readline()
  x_data1 = []
  x_data2 = []
  y_data = []
  for i in range(139):
    y_data.append(i*0.01)
  line = reader.readline()
  while line != '':
    splittedLine = line.split(',')
    x_data1.append(float(splittedLine[0]))
    x_data2.append(float(splittedLine[1]))
    line = reader.readline()
    print(line)

  # filename = metricsFilename.split('.')[0] + '.png'
  plt.scatter(y_data, x_data1, s=0.6)
  plt.ylim((90000, 105000)) 
  plt.scatter(y_data, x_data2, s=0.6)
  plt.xlabel('time (s)')
  plt.ylabel('Pressure (p)')
  # outputFilepath = os.path.join(outputDirectory, filename)
  plt.show()
  plt.clf()



