import matplotlib.pyplot as plt

eval_log = open("eval_log", "r")
number_of_derivations = []
str_len = []

for line in eval_log:
    l = line.split(",")
    del l[3]
    number_of_derivations.append(int(l[0]))
    str_len.append(int(l[2]))

eval_log.close()
plt.plot(number_of_derivations, str_len, 'r-', label="String Length")
plt.xlabel("Number of Simulation Steps")
plt.ylabel("Length of Tree String (bytes)")
plt.legend()
plt.show()
