import pandas as pd
import matplotlib.pyplot as plot
import re as re

def compare(lt,out):

    rVal = re.split("(?:[a-zA-Z]=)(\d+)", lt)
    rVal = float(rVal[1])
    a = pd.read_csv(lt)
    print(out)
    b = pd.read_csv(out)
        
    a.columns = a.columns.str.lower()
    b.columns = b.columns.str.lower()
    a.set_index('time')
    b.set_index('time')
    c = a.copy()
    means = list()
    means.append(rVal)
    upTo = int((rVal*10e-6)/1e-5)
    print(upTo)
    a = a.iloc[:upTo]
    b = b.iloc[:upTo]
    header = list()
    header.append("Resistor Value")
    for i in a.columns:
        if i == "time" or i=="Time":
            continue
        else:
            header.append(i)
            c[i] = 100*(abs(a[i]-b[i])/a[i])
            means.append(abs(c[i].mean()))

    return (c,means,header)


from os import listdir
from os.path import isfile, join

folder = "RC_test"

LTfol = "LT.csvs"
fol404 = "out.csvs"
ltFiles= [f for f in listdir(LTfol) if isfile(join(LTfol, f))]
spiceFiles = [f for f in listdir(fol404) if isfile(join(fol404, f))]

ltFiles = [x for x in ltFiles if "new" in x]
means = list()
header = list()
for ltFile in ltFiles:
    file404 = ltFile.replace('new','')
    resultTuple = compare("../LT.csvs/"+ltFile,"../out.csvs/"+file404)
    means.append(resultTuple[1])
    a = resultTuple[0]
    header = resultTuple[2]
    b = a.plot(kind='scatter',marker='x',x='time',y='v(n001)',color='red')
    b.grid(color='black', linestyle='-', linewidth=0.4)
    b.set(xlabel="Time /s", ylabel="Percentage Error in Voltage /%",title="Pecentage Error of Voltage across resistor\n vs Time /s " + file404.replace(',.csv','')) 

x = pd.DataFrame.from_records(means)
x.columns = header
x.set_index('Resistor Value')
print(x)

for i in x.columns[1:]:
    b = x.plot(kind='scatter',marker='x',x='Resistor Value',y=i,color='red')
    b.grid(color='black', linestyle='-', linewidth=0.4)
    b.set(xlabel="Resistor Value /Ω", ylabel="Mean Percentage Error in " +i + "/%",title="Average Percentage Error in " + i + "/%" + "\nIn first time constant for various resistor values /Ω") 
