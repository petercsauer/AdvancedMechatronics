import csv
import matplotlib.pyplot as plt
import numpy as np


t = [] # column 0
data1 = [] # column 1
data_ma_filter = []
data_lp_filter = []

A = .9
B = 1-A
#A: 1000 | B: 200 | C: 1 | D: 10
X = 10

with open('sigD.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        data1.append(float(row[1])) # second column

for i in range(len(data1)):
    sum = 0
    for k in range(X):
        j = len(data1)-i-k-1
        if j >= 0:
            sum+=data1[j]
        else:
            sum+=0;
    avg = sum/X
    data_ma_filter.insert(0,avg)

for i in range(len(data1)):
    if i-1>=0:
        data_lp_filter.append(A * data_ma_filter[i-1] + B * data1[i])
    else:
        data_lp_filter.append(0 + B * data1[i])

plt.title("A: "+str(A)+" B: "+str(B))
plt.plot(t,data1,'black') # plotting the fft
# plt.plot(t,data_ma_filter,'r')
print(data_lp_filter)
plt.plot(t,data_lp_filter,'r')

plt.xlabel('Time')
plt.ylabel('Amplitude')

plt.show()

