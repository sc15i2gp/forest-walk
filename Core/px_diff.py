import matplotlib.pyplot as plt

def load_ppm(filename):
    ppm = open(filename,"r")
    data = []
    for line in ppm:
        l = line[:len(line)-1].split(" ")
        if len(l) == 3:
            data.append([int(l[0]), int(l[1]), int(l[2])])
    ppm.close()
    return data

print("Loading no_lod")
no_lod = load_ppm("image_no_lod.ppm")
print("Loading lod")
lod = load_ppm("image_lod.ppm")

num_pixels = len(lod)

print("Taking difference")
for i in range(num_pixels):
    for j in range(3):
        lod[i][j] = ((lod[i][j] - no_lod[i][j])**2)**0.5

print("Writing output image")
final_ppm = open("image_diff.ppm", "w")
final_ppm.write("P3\n")
final_ppm.write(str(int(num_pixels**0.5)) + " " + str(int(num_pixels**0.5)) + "\n")
final_ppm.write("255\n")
for pixel in lod:
    final_ppm.write(str(int(pixel[0])) + " " + str(int(pixel[1])) + " " + str(int(pixel[2])) + "\n")
final_ppm.close()
print("Done")
