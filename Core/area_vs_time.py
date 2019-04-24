import matplotlib.pyplot as plt

eval_log = open("eval_log", "r")
number_of_derivations = []
pine_area = []
birch_area = []
rowan_area = []

for line in eval_log:
    l = line.split(",")
    del l[13]
    number_of_derivations.append(int(l[0]))
    pine_area.append(float(l[9]))
    birch_area.append(float(l[10]))
    rowan_area.append(float(l[11]))

eval_log.close()
plt.plot(number_of_derivations, pine_area, 'r-', label="Pine")
plt.plot(number_of_derivations, birch_area, 'g-', label="Birch")
plt.plot(number_of_derivations, rowan_area, 'm-', label="Rowan")
plt.xlabel("Number of Simulation Steps")
plt.ylabel("Area covered by species in $m^2$")
plt.legend()
plt.show()
