import re

dataStr = []
dataNum = []
dataBool = []

patternLabel = re.compile('"(.*?)"')
patternFloat = re.compile('[+-]?([0-9]*[.])?[0-9]+')
patternBool = re.compile ('(TRUE|FALSE)+')

for i, line in enumerate(open('../InOut/Inp_FOV.txt')):
    for match in re.finditer(patternLabel, line):
        dataStr.append(match.group(0))
    for match in re.finditer(patternFloat, line):
        dataNum.append(match.group(0))
    for match in re.finditer(patternBool, line):
        dataBool.append(match.group(1))

print(dataNum) # 17 per FOV
print(dataStr) # 1 per FOV
print(dataBool) # 2 per FOV
