eval_log = open("eval_log", "r")
dist_gen_time = 0

for line in eval_log:
    l = line.split(",")
    dist_gen_time += int(l[12])

eval_log.close()
print("Time spent generating distribution = " + str(dist_gen_time) + "ms")
