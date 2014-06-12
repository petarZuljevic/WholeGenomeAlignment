import sys
import random

a = open("primjer10.txt", "w")

inpt = ""

for i in range(0, 10000000):
    inpt += random.choice(['A', 'C', 'T', 'G'])
inpt += "$"

for i in range(0, 10000000):
    inpt += random.choice(['A', 'C', 'T', 'G'])

a.write(inpt + "\n")
