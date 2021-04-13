import csv
import matplotlib.pyplot as plt
import numpy as np


t = [] # column 0
data1 = [] # column 1
data_ma_filter = []

X = 1000

with open('sigA.csv') as f:
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




for i in range(len(t)):
    # print the data to verify it was read
    print(str(t[i]) + ", " + str(data1[i]))

dt = t[-1]/len(t)
t = np.arange(0.0, t[-1], dt) # 10s
# a constant plus 100Hz and 1000Hz
s = 4.0 * np.sin(2 * np.pi * 100 * t) + 0.25 * np.sin(2 * np.pi * 1000 * t) + 25

Fs = len(t)/t[-1]
Ts = t[-1]/Fs; # sampling interval
ts = np.arange(0,t[-1],Ts) # time vector
y = data1
n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

# fig, (ax1, ax2) = plt.subplots(2, 1)
# ax1.plot(t,data_ma_filter,'b')
# ax1.set_xlabel('Time')
# ax1.set_ylabel('Amplitude')
# ax2.loglog(frq,abs(Y),'b') # plotting the fft
# ax2.set_xlabel('Freq (Hz)')
# ax2.set_ylabel('|Y(freq)|')
# plt.show()

plt.title(str(X)+" Samples Averaged")
plt.plot(t,data1,'black') # plotting the fft
plt.plot(t,data_ma_filter,'r')
plt.xlabel('Time')
plt.ylabel('Amplitude')

plt.show()

