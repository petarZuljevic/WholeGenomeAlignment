import sys

file1 = open(sys.argv[1], "r")
file2 = open(sys.argv[2], "r")

lines1 = file1.readlines()
lines2 = file2.readlines()

new_str = ""

for i in range(1, len(lines1)):
    new_str += lines1[i].replace("\n","")

file3 = open("INPUT.txt", "w")

new_str += "$"

for i in range(1, len(lines2)):
    new_str += lines2[i].replace("\n","").replace("N", "M")

file3.write(new_str)

file3.close()
file1.close()
file2.close()
