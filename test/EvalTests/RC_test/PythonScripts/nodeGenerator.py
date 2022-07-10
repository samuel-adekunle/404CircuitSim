# Node generating script
import sys
for num_nodes in range(50,1000, 50):
    f = open("netlist" + str(num_nodes) + ".cir", "a")
    f.write("*" + str(num_nodes))
    f.write("V1 1 0 SINE(0 100 1)")
    for i in range(1,num_nodes):
        resistorSmall = "R" + str(i) + " " + str(i) + " " + str(i + 1) + " 1"
        resistorLarge = "R" + str(i+num_nodes) + " " + str(i) + " " + str(0) + " 1000"
        f.write(resistorSmall)
        f.write(resistorLarge)
    f.write(".tran 0 3 0 0.001")
    f.write(".end")
    f.close()

    
