import matplotlib.pyplot as plt
import numpy as np
import csv

data = []
colors = ['b', 'r', 'g', 'y']
labels = ["uTensor", "xTensor", "OpenCV"]

with open('initMeasurementsDebug.txt', 'r') as f:

    lines = f.readlines()

    for i in range(0, len(lines[0].split('\t')), 1):
        i_column = [row[i] for row in csv.reader(lines,delimiter='\t')]
        if i == 0:
            data.append([str(it) for it in i_column])
        else:
            data.append([float(it) for it in i_column])

print(data)
assert(len(data) > 1)
assert(len(data) - 1 <= len(colors))

for i in range(1, len(data), 1):
    if labels:
        plt.plot(data[0], data[i], color=colors[i -1], label=labels[i-1])
    else:
        plt.plot(data[0], data[i], color=colors[i -1])

plt.ylabel('Time (milliseconds)')
plt.xlabel('Size message (bytes)')

if labels:
    plt.legend()

plt.show()