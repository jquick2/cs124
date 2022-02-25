import pandas as pd
import numpy as np 
import matplotlib.pyplot as plt
import math

data = pd.read_csv("results.csv")


def general_np_log(x,base):
	return np.log2(x)/math.log2(base)


subdata = []
for i in range(4):
	subdata.append(data[data.dimension == (i+1)])

for i in range(4):
	plt.plot(subdata[i].nodes,subdata[i].average,label=f"Dimension {i+1}")


x = np.array([2**i for i in range(7,19)])
g1_fill = np.array([1.2 for _ in range(7,19)])


plt.plot(x,g1_fill,label="Guess 1",linestyle=(0,(5,5)))
plt.plot(x,x**(1/2.1496),label="Guess 2",linestyle=(0,(1,1)))
plt.plot(x,x**(1/1.5822),label="Guess 3",linestyle=(0,(3,5,1,5,1,5)))
plt.plot(x,x**(1/1.3892),label="Guess 4",linestyle=(0,(5,1)))

plt.xlabel("Number of Nodes in Tree (N)")
plt.ylabel("Average Size of MST (log axis)")
plt.title("MST Size by Number of Tree Nodes (N)")
plt.yscale("log")
plt.legend()
plt.show()
plt.clf()


