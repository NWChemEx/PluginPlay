"""
This script will loop over a series of basis sets and write out a file that will
fill them in.  The format of the resulting basis sets is suitable for use with
the BasisSetExchange class
"""
import os
import fnmatch
import re
from helper_fxns import *

class Shell:
    def __init__(self, l):
        self.l = l
        self.exp=[]
        self.coefs=[]
        self.gen = 0
    def add_prim(self, exp, coefs):
        self.exp.append(exp)
        self.coefs.append(coefs)
        self.gen = max(len(coefs), self.gen)
    def cxxify(self, f):
        f.write("    shell_t{\n      ")
        f.write("LibChemist::ShellType::SphericalGaussian, ")
        f.write("sym2l.at(\"{}\"), {}, \n    {{".format(self.l, self.gen))
        for a in self.exp:
            f.write("{}, ".format(a.replace('D', 'E').replace('d', 'e')))
        f.write("}, \n    {")
        for i in range(self.gen):
            for cs in self.coefs:
                f.write("{}, ".format(cs[i].replace('D', 'E').replace('d', 'e')))
        f.write("}\n    }")

def write_am(out_dir):
    with open(os.path.join(out_dir, "DefaultAM.cpp"), 'w') as f:
        letters = 'spdfghijklmnoqrtuvwxyzabce'
        write_header(__file__, f)
        f.write("using return_t = typename "
                "ChemistryRuntime::am_sym_lut_type;\n")
        f.write("return_t default_am() {\n")
        f.write("    return_t rv;\n")
        f.write("    rv[\"sp\"] = -1;\n")
        for i,l in zip(range(26), letters):
            f.write("    rv[\"{}\"] = {};\n".format(l, i))
        write_footer(f)

def write_bases(out_dir, bases):
    with open(os.path.join(out_dir,"DefaultBases.cpp"),'w') as f:
        write_header(__file__, f)
        f.write("using indexed_atom_type = typename ")
        f.write("ChemistryRuntime::indexed_atom_type;\n")
        f.write("using return_t = typename "
                "ChemistryRuntime::indexed_atom_type;\n")
        f.write("using shell_t = LibChemist::BasisShell;\n")
        f.write("return_t default_bases(){\n")
        f.write("    auto sym2z = default_symbols();\n")
        f.write("    auto sym2l = default_am();\n")
        f.write("    return_t rv;\n")
        for bs_name, bs in bases.items():
            for z, atom in bs.items():
                for s in atom:
                    f.write("    rv[sym2z.at(\"{}\")]".format(z))
                    f.write(".bases[\"{}\"].push_back(\n".format(bs_name))
                    s.cxxify(f)
                    f.write(");\n")
        write_footer(f)


def main():
    basis_sets = [f for f in os.listdir("basis_sets") if os.path.isfile(os.path.join("basis_sets", f))]
    new_atom = re.compile("^\s*\D{1,2}\s*0\s*$")
    new_shell = re.compile("^\s*[a-zA-Z]+\s*\d+\s*1.00\s*$")
    same_shell = re.compile("^\s*(?:-?\d+.\d+(?:(E|e|D|d)(\+|-)\d\d)*\s*)+")
    my_dir = os.path.dirname(os.path.realpath(__file__))
    out_dir = os.path.join(os.path.dirname(my_dir), "SDE",
                           "Defaults")

    bases = {}
    for bs in basis_sets:
        bases[bs] = {}
        with open(os.path.join("basis_sets",bs),'r') as f:
            atom_z = 0
            for line in f:
                if re.search(new_atom, line):
                    atom_z = line.split()[0]
                    bases[bs][atom_z] = []
                elif re.search(new_shell, line):
                    bases[bs][atom_z].append(Shell(line.split()[0]))
                elif re.search(same_shell, line):
                    prim = line.split()
                    bases[bs][atom_z][-1].add_prim(prim[0], prim[1:])
    write_am(out_dir)
    write_bases(out_dir, bases)


if __name__ == "__main__":
    main()
