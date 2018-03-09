"""
This script will turn each of the xyz files in "molecules" into the C++ commands
to make said molecule in a source file
"""
import os
from helper_fxns import *

class molecule:
    def __init__(self,name):
        self.name = name
        self.carts = []
        self.atoms = []
    def cxxify(self, f):
        f.write("    molecule {:s};\n".format(self.name))
        indent = "        "
        for i in range(len(self.atoms)):
            f.write("    {\n")
            f.write(indent + "auto temp = wikipedia.atomic_info(\"{"
                             ":s}\");\n".format(self.atoms[i]))
            f.write(indent + "temp.coord = std::array<double,3>( \n")
            f.write(indent + "    {")
            for j in range(2):
                f.write("{:s},".format(self.carts[3*i+j]))
            f.write("{:s};\n".format(self.carts[3*i+2]+'})'))
            f.write(indent + "{:s}.insert(temp);\n".format(self.name))
            f.write("    }\n")
        f.write("    rv[\"{0:s}\"] = {0:s};\n".format(self.name))

def main():
    mols = ["water"]
    my_path = os.path.dirname(os.path.realpath(__file__))
    data_dir = os.path.join(os.path.dirname(my_path), "SDE",
                            "Defaults")
    with open(os.path.join(data_dir, "DefaultMols.cpp"),'w') as f:
        write_header(__file__, f)
        f.write("using molecule = typename ChemistryRuntime::molecule_type;\n")
        f.write("using return_t = typename "
                "ChemistryRuntime::molecule_lut_type;\n")
        f.write("return_t default_molecules() {\n")
        f.write("    return_t rv;\n")
        for m in mols:
            mol = molecule(m)
            with open("molecules/{:s}.xyz".format(m),'r') as g:
                for i,l in enumerate(g):
                if i<2:
                    continue
                f.write("")
                line = l.split()
                mol.atoms.append(line[0])
                for j in range(1,4):
                    mol.carts.append(line[j])
        mol.cxxify(f)



