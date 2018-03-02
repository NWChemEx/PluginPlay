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

    write_symbols(out_dir, nmspace, name_file)
    write_isotopes(out_dir, nmspace, iso_file, iso_ab_file)

if __name__ == '__main__' :
    main()

#

#
#
# class AtomicInfo:
#     """ Class for storing the information relating to an atom
#
#     """
#     def __init__(self, z_in, atom_sym, atom_name):
#         self.Z = int(z_in)
#         self.sym = atom_sym.lower()
#         self.name = atom_name.lower()
#         self.mult = 0
#         self.mass = 0.0
#         self.cov_radius = 0.0
#         self.vdw_radius = 0.0
#         self.isotopes = {}
#
#     def cxxify(self, f_in):
#         f_in.write("rv[{:d}] = LibChemist::Atom{{}};\n".format(self.Z))
#         prop_add = "rv[{:d}].property[LibChemist::AtomProperty::".format(self.Z)
#         f_in.write(prop_add + "Z] = {:d};\n".format(self.Z))
#         f_in.write(prop_add + "mass] = {:16.16f};\n".format(self.mass))
#         f_in.write(prop_add + "charge] = 0.0;\n")
#         f_in.write(prop_add + "nelectrons] = {:16.16f};\n".format(self.Z))
#         f_in.write(prop_add + "multiplicity] = {:d};\n".format(self.mult))
#         f_in.write(prop_add + "cov_radius] = {:16.16f};\n".format(
#             self.cov_radius))
#         f_in.write(prop_add + "vdw_radius] = {:16.16f};\n".format(
#             self.vdw_radius))
#         isotopes = "tmp{:d}.isotopes[".format(self.Z)
#         iso_mass = 0.0
#         iso_abun = 0.0
#         for ki, vi in sorted(self.isotopes.items()):
#             if vi.abundance > iso_abun:
#                 iso_abun = vi.abundance
#                 iso_mass = vi.mass
#         f_in.write(prop_add + "isotope_mass] = {:16.16f};\n".format(iso_mass))
#
#
# atomicinfo = {}
#
# # Read in names and make initial entries
#
#
#
# # Read in experimental masses
# mass_file = os.path.join(data_dir, "CIAAW-MASSES.formatted.txt")
# # File format is: Z, Sym, average mass, lower and upper bounds to mass
# for l in open(mass_file).readlines()[5:]:
#     z, sym, mid = l.strip().split()[:3]
#     atomicinfo[int(z)].mass = float(mid)
#
# # Read in Covalent Radii
# cov_file = os.path.join(data_dir, "CovRadii.txt")
# # File format is: Z, radius, unit
# for l in open(cov_file).readlines()[1:]:
#     z, r = l.strip().split()[:2]
#     atomicinfo[int(z)].cov_radius = float(r)/pm2bohr
#
# # Read in van der waal Radii
# vdw_file = os.path.join(data_dir, "VanDerWaalRadius.txt")
# # File format is: Z, radius, unit
# for l in open(vdw_file).readlines()[1:]:
#     z, r = l.strip().split()[:2]
#     atomicinfo[int(z)].vdw_radius = float(r)/pm2bohr
#
# # Read in multiplicities
# mult_file = os.path.join(data_dir, "NIST-ATOMICION.formatted.txt")
# # File format: Z, configuration, multiplicity, term symbol
# for l in open(mult_file).readlines()[5:]:
#     z, occ, mult = l.strip().split()[:3]
#     atomicinfo[int(z)].mult = int(mult)
#
# src_file = os.path.join(outbase, output_name+".cpp")
# with open(src_file, 'w') as f:
#     atom_info_type = "std::map<std::size_t, AtomicInfo>"
#     f.write("#include \"NWChemExRuntime/NWXDefaults.hpp\"\n")
#     f.write("#include <algorithm>\n\n")
#     f.write("namespace {0:s} {{\n".format(nmspace))
#     f.write("namespace detail_ {\n")
#
#     # Atomic symbol to Z
#     f.write("extern const std::map<std::string, std::size_t> sym2Z_{"
#             "\n")
#     for k, v in sorted(atomicinfo.items()):
#         f.write("  {{ \"{}\" , {} }},\n".format(v.sym, k))
#     f.write("}; // close sym2Z_\n\n\n")
#
#     # Next, full atomic data
#     f.write("extern const {} atomic_data_(\n".format(atom_info_type))
#     f.write("    [](){"+" {} temp;\n".format(atom_info_type))
#     for k, v in sorted(atomicinfo.items()):
#         f.write("AtomicInfo tmp{:d};\n".format(k))
#         v.cxxify(f)
#         f.write("temp[{0:d}] = tmp{0:d};\n".format(k))
#     f.write("return temp;}());\n")
#     f.write("}}//End namespaces\n")
