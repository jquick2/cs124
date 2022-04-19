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