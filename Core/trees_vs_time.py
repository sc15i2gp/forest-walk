import matplotlib.pyplot as plt

eval_log = open("eval_log", "r")
number_of_derivations = []
number_of_pines = []
number_of_birches = []
number_of_rowans = []
number_of_trees = []

for line in eval_log:
    l = line.split(",")
    del l[13]
    number_of_derivations.append(int(l[0]))
    number_of_pines.append(int(l[1]))
    number_of_birches.append(int(l[2]))
    number_of_rowans.append(int(l[3]))
    number_of_trees.append(int(l[4]))

eval_log.close()
plt.plot(number_of_derivations, number_of_trees, 'b-', label='Number of trees')
plt.plot(number_of_derivations, number_of_pines, 'r-', label='Number of pines') 
plt.plot(number_of_derivations, number_of_birches, 'g-', label='Number of birches')
plt.plot(number_of_derivations, number_of_rowans, 'm-', label='Number of rowans')
plt.xlabel("Number of Simulation Steps")
plt.ylabel("Number of Trees")
plt.legend()
plt.show()
