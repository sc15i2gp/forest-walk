eval_log = open("eval_log", "r")
string_gen_time = 0

for line in eval_log:
    l = line.split(",")
    string_gen_time += int(l[1])

eval_log.close()
print("Time spent generating model string = " + str(string_gen_time) + "ms")
