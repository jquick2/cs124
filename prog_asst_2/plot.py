<<<<<<< HEAD
import numpy as np
import pandas as pd 
import matplotlib.pyplot as plt

data = pd.read_csv("data.csv")
print(data)



cats = ["min","max","mean"]

for cat in cats:
	data[f"{cat}_dev"] = data[cat]/data["real"]



	plt.plot(data["prob"],data[f'{cat}_dev'],label=cat)

plt.legend(loc="upper right")
plt.title("Ratio to theoretical # of triangles for vaying probs")
plt.xlabel("Edge probability")
plt.ylabel("Ratio")
plt.show()
plt.clf()
=======
import matplotlib.pyplot as plt
import numpy as np

# Data for plotting STRASSEN'S Method
x = [2, 64, 128] # n
y = [0.0111, 0.53, 7.7] # times
 
plt.rcParams["figure.figsize"] = (12,7)
fig, ax = plt.subplots()
ax.plot(x, y, "-g", label="Strassen's Algorithm")

# Data for plotting NORMAL Method
a = [2, 64, 128] # n
b = [0.000001, 0.16, 20.7] # times

ax.plot(a, b, "-b", label="Normal Algorithm")

ax.set(xlabel='n', ylabel='Runtime (in sec)',
       title="n v. Strassen's algorithm, Normal Algorithm")
plt.legend(loc="upper left")
ax.grid()


fig.savefig("PS4Graph.png")
plt.show()
>>>>>>> 2bc900626fd873dd8b8f7b73688d668251f30498
