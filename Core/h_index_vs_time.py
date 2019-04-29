import matplotlib.pyplot as plt

eval_log = open("eval_log", "r")
number_of_derivations = []
total_h_index = []
pine_h_index = []
birch_h_index = []
rowan_h_index = []

for line in eval_log:
    l = line.split(",")
    del l[13]
    number_of_derivations.append(int(l[0]))
    total_h_index.append(float(l[5]))
    pine_h_index.append(float(l[6]))
    birch_h_index.append(float(l[7]))
    rowan_h_index.append(float(l[8]))

eval_log.close()
plt.plot(number_of_derivations, total_h_index, 'b-', label="All")
plt.plot(number_of_derivations, pine_h_index, 'r-', label="Pine")
plt.plot(number_of_derivations, birch_h_index, 'g-', label="Birch")
plt.plot(number_of_derivations, rowan_h_index, 'm-', label="Rowan")
plt.xlabel("Number of Simulation Steps")
plt.ylabel("Hopkin's Index Value")
plt.legend()
plt.show()
