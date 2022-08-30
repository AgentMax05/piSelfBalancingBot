import numpy
import matplotlib
import matplotlib.pyplot as plt

# matplotlib.use("Agg")
filename = "./robotData.csv"

numGraph = 0

def plot(keys, data, xName, yName):
    global numGraph

    axs[numGraph].plot(data[keys.index(xName)][1:], data[keys.index(yName)][1:], linewidth=0.6)

    # ratio = 0.5
    # x_left, x_right = ax.get_xlim()
    # y_low, y_high = ax.get_ylim()
    # ax.set_aspect(abs((x_right-x_left)/(y_low-y_high)) * ratio)

    # plt.xlabel(xName)
    # plt.ylabel(yName)
    # plt.title(f"{yName.capitalize()} vs. {xName.capitalize()}")
    numGraph += 1

with open(filename, "r") as dataFile:
    rawData = [i.split(",") for i in dataFile.read().split("\n")];
    keys = [i[0] for i in rawData]

data = numpy.genfromtxt(filename, delimiter=",")

fig, axs = plt.subplots(len(keys) - 1, sharex=True)
fig.suptitle("Robot Data")

for i in keys[:-1]:
    plot(keys, data, "time", i)

fig.add_subplot(111, frameon=False)
plt.tick_params(labelcolor="none", top=False, bottom=False, left=False, right=False)
plt.grid(False)
plt.xlabel("time")

for i in range(len(keys[:-1])):
    plt.setp(axs[i], ylabel=keys[i])

plt.subplots_adjust(wspace=0, hspace=0)

# plt.savefig("./test_graph.png", dpi=500)
plt.show()
