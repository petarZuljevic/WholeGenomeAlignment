import sys

file1 = open("Homo_sapiens.GRCh37.75.dna.chromosome.18.fa", "r")
file2 = open("Mus_musculus.GRCm38.75.dna.chromosome.18.fa", "r")

lines1 = file1.readlines()
lines2 = file2.readlines()

new_str = ""

for i in range(1, len(lines1)):
    new_str += lines1[i].replace("\n","")

file3 = open("Homo_Mus_18", "w")

new_str += "$"

for i in range(1, len(lines2)):
    new_str += lines2[i].replace("\n","")

file3.write(new_str)

file3.close()
file1.close()
file2.close()
