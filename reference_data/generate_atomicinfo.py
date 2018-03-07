# #!/usr/bin/env python3
import os
from helper_fxns import *

"""This script is used to create the experimental data look up tables for the
atom class.

Original author: Ben Pritchard

In order to run this script simply needs to know where you want the generated
files to live.

For readability and convenience we use a few abbreviations throughout this
script:

- Z the atomic number of an atom
- Sym the atomic symbol of an atom (e.g. H for hydrogen, He for helium)

"""

class IsotopeInfo:
    """ Class for storing the isotope info"""
    def __init__(self, iso_num, mass):
        self.isonum = iso_num
        self.mass = mass
        self.abundance = 0.0

    def cxxify(self, f_in):
        """Prints the class out using aggregate initialization"""
        f_in.write("IsotopeData{")  # Start class
        f_in.write("{},".format(self.isonum))
        f_in.write("{},".format(self.mass))
        f_in.write("{}".format(self.abundance))
        f_in.write("}")  # End class

class AtomicInfo:
    """ Class for storing the information relating to an atom

    """
    def __init__(self, z_in):
        self.Z = int(z_in)
        self.mult = 0
        self.mass = 0.0
        self.cov_radius = 0.0
        self.vdw_radius = 0.0
        self.isotopes = {}

    def cxxify(self, f_in):
        prop_add = "      {LibChemist::AtomProperty::"
        f_in.write("{}Z, {}}},\n".format(prop_add, self.Z))
        f_in.write("{}mass, {}}},\n".format(prop_add,self.mass))
        f_in.write("{}charge, 0.0}},\n".format(prop_add))
        f_in.write("{}nelectrons, {}}},\n".format(prop_add, self.Z))
        f_in.write("{}multiplicity, {}}},\n".format(prop_add, self.mult))
        f_in.write("{}cov_radius, {}}},\n".format(prop_add, self.cov_radius))
        f_in.write("{}vdw_radius, {}}}\n".format(prop_add, self.vdw_radius))


def write_symbols(out_dir, nmspace, name_file):
    # File format is: Z, Sym, full name
    with open(os.path.join(out_dir, "DefaultSymbols.cpp"),'w') as f:
        f.write("#include \"NWChemExRuntime/NWXDefaults.hpp\"\n\n")
        write_warning(__file__, f)
        f.write("\nnamespace {} {{\n".format(nmspace))
        f.write("using return_type = typename "
                "ChemistryRuntime::at_sym_lut_type;\n")
        f.write("return_type default_symbols("
                "){\n")
        f.write("    return_type rv;\n")
        for line in open(name_file).readlines():
            z, sym, name = line.strip().split()
            f.write("    rv[{:d}] = \"{:s}\";\n".format(int(z), sym))
        f.write("    return rv;\n}}\n}} // End namespace {}\n".format(nmspace))

def write_isotopes(out_dir, nmspace, iso_file, iso_ab_file):
    # Format for both files is:
    #  Z, Sym, isotope number, average mass, lower and upper bounds
    data = {}
    for file_name in [iso_file, iso_ab_file]:
        with open(file_name, 'r') as f:
            for line in f.readlines()[5:]:
                z, sym, isonum, mid = line.strip().split()[:4]
                if not z in data:
                    data[z] = {}
                if file_name == iso_file:
                    data[z][isonum] = IsotopeInfo(isonum, mid)
                else:
                    if isonum in data[z]:
                        data[z][isonum].abundance = mid
                    else:
                        print("Warning: {} isotope {}".format(sym,isonum))
    with open(os.path.join(out_dir, "DefaultIsotopes.cpp"), 'w') as f:
        f.write("#include \"NWChemExRuntime/NWXDefaults.hpp\"\n")
        write_warning(__file__,f)
        f.write("namespace {} {{\n".format(nmspace))
        f.write("using return_t = typename ChemistryRuntime::iso_lut_type;\n")
        f.write("return_t default_isotopes(){\n")
        f.write("    return_t rv;\n")
        for z, isotopes in data.items():
            f.write("    rv[{}] = IsotopeSet{{}};\n".format(z))

            for isonum, iso_data in isotopes.items():
                f.write("    rv[{}].push_back(".format(z))
                iso_data.cxxify(f)
                f.write(");\n")
        f.write("    return rv;\n}\n")
        f.write("}} // End namespace {}\n".format(nmspace))

def write_atoms(out_dir, nmspace, mass_file, cov_file, vdw_file,
                mult_file, pm2bohr):
    atomicinfo = {}
    # File format is: Z, Sym, average mass, lower and upper bounds to mass
    for l in open(mass_file).readlines()[5:]:
        z, sym, mid = l.strip().split()[:3]
        if not z in atomicinfo:
            atomicinfo[z] = AtomicInfo(z)
        atomicinfo[z].mass = mid

    # File format is: Z, radius, unit
    for l in open(cov_file).readlines()[1:]:
        z, r = l.strip().split()[:2]
        atomicinfo[z].cov_radius = float(r)/pm2bohr

    # File format is: Z, radius, unit
    for l in open(vdw_file).readlines()[1:]:
        z, r = l.strip().split()[:2]
        atomicinfo[z].vdw_radius = float(r)/pm2bohr

    # File format: Z, configuration, multiplicity, term symbol
    for l in open(mult_file).readlines()[5:]:
        z, occ, mult = l.strip().split()[:3]
        atomicinfo[z].mult = mult

    src_file = os.path.join(out_dir, "DefaultAtoms.cpp")
    with open(src_file, 'w') as f:
        f.write("#include \"NWChemExRuntime/NWXDefaults.hpp\"\n")
        write_warning(__file__, f)
        f.write("namespace {} {{\n".format(nmspace))
        f.write("using return_t = typename "
                "ChemistryRuntime::indexed_atom_type;\n")
        f.write("return_t default_atoms() {\n")
        f.write("    return_t rv;\n")
        for k, v in sorted(atomicinfo.items()):
            f.write("    rv[{}] ".format(k))
            f.write("= LibChemist::Atom({0.0, 0.0, 0.0}, {\n")
            v.cxxify(f)
            f.write("    });\n")
        f.write("    return rv;\n")
        f.write("}\n} // End namespace NWXRuntime\n")

def main():
    # Knobs that you may want to tweak to control settings
    pm2bohr = 52.917721067  # Conversion from picometers to Bohr
    dest_dir = os.path.join("NWChemExRuntime", "Defaults") #Directory 4 output
    nmspace = "NWXRuntime"  # Namespace for all C++ classes

    # Get and set some paths
    my_dir =os.path.dirname(os.path.realpath(__file__))
    data_dir = os.path.join(my_dir, "physical_data")
    out_dir = os.path.join(os.path.dirname(my_dir), dest_dir)
    name_file = os.path.join(data_dir, "ElementNames.txt")
    iso_file = os.path.join(data_dir, "CIAAW-ISOTOPEMASSES.formatted.txt")
    iso_ab_file = os.path.join(data_dir, "CIAAW-ABUNDANCE.formatted.txt")
    mass_file = os.path.join(data_dir, "CIAAW-MASSES.formatted.txt")
    cov_file = os.path.join(data_dir, "CovRadii.txt")
    vdw_file = os.path.join(data_dir, "VanDerWaalRadius.txt")
    mult_file = os.path.join(data_dir, "NIST-ATOMICION.formatted.txt")

    write_symbols(out_dir, nmspace, name_file)
    write_isotopes(out_dir, nmspace, iso_file, iso_ab_file)
    write_atoms(out_dir, nmspace, mass_file, cov_file, vdw_file, mult_file,
                pm2bohr)

if __name__ == '__main__' :
    main()

#

#
#

#
#
# atomicinfo = {}
#

