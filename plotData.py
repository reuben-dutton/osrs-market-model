import numpy as np
import sys
import matplotlib
# so it works on getafix (the default matplotlib backend doesn't)
matplotlib.use('agg')
import matplotlib.pyplot as plt

# path = "sim.dat"
path = sys.argv[1]
itemName = sys.argv[2]
data = np.fromfile(path, dtype=np.int32)

# timestep, prey pop, pred pop read as integers
t = data[::3]
prey = data[1::3]
pred = data[2::3]

plt.plot(t, prey, label="buy", linewidth=0.5)
plt.plot(t, pred, label="sell", linewidth=0.5)
plt.title(itemName)
plt.xlabel("Timestep")
plt.ylabel("gp")
plt.legend(loc="center right")
plt.savefig(f"plots/{itemName}.png")

# btw, the output plot will look different depending on whether it's run on
# getfix or your local machine (or at least it does on mine)