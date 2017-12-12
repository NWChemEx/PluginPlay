#!/usr/bin/env python3
import os
import sys

"""This script is used to create the experimental data look up tables for the
atom class.

Original author: Ben Pritchard

In order to run this script simply needs to know where you want the generated
files to live.

For readability and convenience we use a few abbreviations throughout this
script:

- Z the atomic number of an atom
- Sym the atomic symbol of an atom (e.g. H for hydrogen, He for helium)


usage:
    python3 generate_atomicinfo.py <output_directory>

"""

# Knobs that you may want to tweak to control settings
pm2bohr = 52.917721067  # Conversion from picometers to Bohr
output_name = "AtomicInfo"
nmspace = "NWXRuntime"  # Namespace for all C++ classes

if len(sys.argv) != 2:
    print("Usage: generate_atomicinfo.py <output_directory>")
    quit(1)

# Get some paths for input and output of files
outbase = sys.argv[1]
mypath = os.path.dirname(os.path.realpath(__file__))
datadir = os.path.join(mypath, "physical_data")


class IsotopeInfo:
    """ Class for storing the isotope info

    """
    def __init__(self, iso_num, mass):
        self.isonum = int(iso_num)
        self.mass = float(mass)
        self.abundance = 0.0

    def __repr__(self):
        rv = "Isotope number   : {0:d}\n".format(self.isonum)
        rv += "Mass    (Daltons): {0:16.16f}\n".format(self.mass)
        rv += "Abundance (ratio): {0:16.16f}\n".format(self.abundance)
        return rv

    def cxxify(self, f_in):
        """Prints the class out using aggregate initialization"""
        f_in.write("{")  # Start class
        f_in.write("{0:d},".format(self.isonum))
        f_in.write("{0:16.16f},".format(self.mass))
        f_in.write("{0:16.16f}".format(self.abundance))
        f_in.write("}")  # End class


class AtomicInfo:
    """ Class for storing the information relating to an atom

    """
    def __init__(self, z_in, atom_sym, atom_name):
        self.Z = int(z_in)
        self.sym = atom_sym.lower()
        self.name = atom_name.lower()
        self.mult = 0
        self.mass = 0.0
        self.cov_radius = 0.0
        self.vdw_radius = 0.0
        self.isotopes = {}

    def __repr__(self):
        """ Prints the class out

        :return: the string representation of the instance
        """
        rv = "Atom {0:s} ({1:s}):\n".format(self.name, self.sym)
        rv += "  Atomic number          : {0:d}\n".format(self.Z)
        rv += "  Mass           (Dalton): {0:16.16f}\n".format(self.mass)
        rv += "  Multiplicity           : {0:d}\n\n".format(self.mult)
        rv += "            Radii         \n"
        rv += "  ------------------------\n"
        rv += "  Covalent         (a.u.): {0:16.16f}\n".format(self.cov_radius)
        rv += "  Noncovalent:     (a.u.): {0:16.16f}\n".format(self.vdw_radius)
        rv += "           Isotopes       \n"
        rv += "  ------------------------\n"
        for x in self.isotopes.items():
            rv += str(x[1])
        return rv

    def cxxify(self, f_in):
        f_in.write("{//Start AtomicInfo instance\n")
        f_in.write("    {0:d},//Z\n".format(self.Z))
        f_in.write("    \"{0:s}\",//Atomic symbol\n".format(self.sym))
        f_in.write("    \"{0:s}\",//Full name\n".format(self.name))
        f_in.write("    {0:d},//Multiplicity\n".format(self.mult))
        f_in.write("    {0:16.16f},//Mass\n".format(self.mass))
        f_in.write("    {0:16.16f},//Covalent radius\n".format(self.cov_radius))
        f_in.write("    {0:16.16f},//VDW radius\n".format(self.vdw_radius))
        f_in.write("    {//Start map<int,IsotopeData>\n")
        for ki, vi in sorted(self.isotopes.items()):
            f.write("        {//Start pair<int,IsotopeData\n")
            f.write("            {0:d},".format(ki))
            vi.cxxify(f)
            f.write("\n        },//End pair<int,IsotopeData\n")
        f.write("    }//End map<int,IsotopeData>\n")
        f.write("}//End AtomicInfo")


atomicinfo = {}

# Read in names and make initial entries
name_file = os.path.join(datadir, "ElementNames.txt")
# File format is: Z, Sym, full name
for l in open(name_file).readlines():
    z, sym, name = l.strip().split()
    atomicinfo[int(z)] = AtomicInfo(z, sym, name)

# Read in experimental masses
mass_file = os.path.join(datadir, "CIAAW-MASSES.formatted.txt")
# File format is: Z, Sym, average mass, lower and upper bounds to mass
for l in open(mass_file).readlines()[5:]:
    z, sym, mid = l.strip().split()[:3]
    atomicinfo[int(z)].mass = float(mid)

# Read in Covalent Radii
cov_file = os.path.join(datadir, "CovRadii.txt")
# File format is: Z, radius, unit
for l in open(cov_file).readlines()[1:]:
    z, r = l.strip().split()[:2]
    atomicinfo[int(z)].cov_radius = float(r)/pm2bohr

# Read in van der waal Radii
vdw_file = os.path.join(datadir, "VanDerWaalRadius.txt")
# File format is: Z, radius, unit
for l in open(vdw_file).readlines()[1:]:
    z, r = l.strip().split()[:2]
    atomicinfo[int(z)].vdw_radius = float(r)/pm2bohr

# Read in isotope masses
iso_file = os.path.join(datadir, "CIAAW-ISOTOPEMASSES.formatted.txt")
# Format is Z, Sym, isotope number, average mass, lower and upper bounds
for l in open(iso_file).readlines()[5:]:
    z, sym, isonum, mid = l.strip().split()[:4]
    atomicinfo[int(z)].isotopes[int(isonum)] = IsotopeInfo(isonum, mid)

# Read in isotope abundances
iso_ab_file = os.path.join(datadir, "CIAAW-ABUNDANCE.formatted.txt")
# Format is Z, Sym, isotope number, average abundance, lower and upper bounds
for l in open(iso_ab_file).readlines()[5:]:
    z, sym, isonum, mid = l.strip().split()[:4]
    if int(isonum) in atomicinfo[int(z)].isotopes:
        atomicinfo[int(z)].isotopes[int(isonum)].abundance = float(mid)

# Read in multiplicities
mult_file = os.path.join(datadir, "NIST-ATOMICION.formatted.txt")
# File format: Z, configuration, multiplicity, term symbol
for l in open(mult_file).readlines()[5:]:
    z, occ, mult = l.strip().split()[:3]
    atomicinfo[int(z)].mult = int(mult)

comment = """
/** @file Declares structures containing basic experimental data.
 *
 *  @warning This file is automatically generated via generate_atomicinfo.py.
 *    DO NOT EDIT!!!
 */
"""


def cxx_isotopedata_declaration(f):
    """Prints the C++ declaration of the IsotopeInfo class

       f (file object): file to print to
    """
    f.write("/** @brief Struct to hold reference data related to a "
            "particular isotope.\n *\n *  Data for this class was filled "
            "using generate_atomicinfo.py by parsing a\n *  collection of NIST "
            "and IUPAC data files located in the directory \n"
            " *  NWChemExRuntime/reference_data/physical_data.\n"
            " */\n")
    f.write("struct IsotopeData {\n")
    f.write("    ///Isotope number (Z + # neutrons)\n")
    f.write("    std::size_t isonum;\n\n")
    f.write("    ///Mass of the isotope in Daltons\n")
    f.write("    double mass;\n\n")
    f.write("    ///Natural abundance of isotope (out of 1)\n")
    f.write("    double abundance;\n")
    f.write("};//End IsotopeData\n\n")


def cxx_atominfo_declaration(f):
        """Prints out the C++ declaration of the AtomicInfo class
        """
        f.write("/** @brief Struct to hold reference data related to a "
                "particular atom.\n *\n *  Data for this class was filled "
                "using "
                "generate_atomicinfo.py by parsing a\n *  collection of NIST "
                "and IUPAC data files located in the directory \n"
                " *  NWChemExRuntime/reference_data/physical_data.\n"
                " */\n")
        f.write("struct AtomicInfo {\n")
        f.write("    ///Atomic number (# of protons)\n")
        f.write("    std::size_t Z;\n\n")
        f.write("    ///Atomic symbol of the element (lowercase)\n")
        f.write("    std::string sym;\n\n")
        f.write("    ///Full name of the element (lowercase)\n")
        f.write("    std::string name;\n\n")
        f.write("    ///Ground-state multiplicity of the atom\n")
        f.write("    std::size_t multiplicity;\n\n")
        f.write("    ///Isotope weighted mass in Daltons\n")
        f.write("    double mass;\n\n")
        f.write("    ///Covalent radius in a.u.\n")
        f.write("    double covradius;\n\n")
        f.write("    ///Noncovalent (van der Waals) radius in a.u.\n")
        f.write("    double vdwradius;\n\n")
        f.write("    ///Isotope data for this element\n")
        f.write("    std::unordered_map<std::size_t,IsotopeData> isotopes;\n")
        f.write("};//End AtomicInfo class\n\n")


header_file = os.path.join(outbase, output_name+".hpp")
with open(header_file, 'w') as f:

    f.write("#pragma once\n")
    f.write("#include <string>\n")
    f.write("#include <unordered_map>\n")
    f.write("#include <vector>\n")
    f.write(comment+'\n')
    f.write("namespace {0:s} {{\n".format(nmspace))
    f.write("namespace detail_ {\n\n")
    cxx_isotopedata_declaration(f)
    cxx_atominfo_declaration(f)
    f.write("///Convenience converter from atomic symbol to atomic number\n")
    f.write("extern const std::unordered_map<std::string, std::size_t> "
            "sym2Z_;\n\n")
    f.write("///Map from atomic number to known data for element\n")
    f.write("extern const std::unordered_map<std::size_t, AtomicInfo> "
            "atomic_data_;\n\n")
    f.write("}}//End namespaces\n")

src_file = os.path.join(outbase, output_name+".cpp")
with open(src_file, 'w') as f:
    f.write("#include \"NWChemExRuntime/AtomicInfo.hpp\"\n")
    f.write("#include <algorithm>\n\n")
    f.write("namespace {0:s} {{\n".format(nmspace))
    f.write("namespace detail_ {\n")

    # Atomic symbol to Z
    f.write("extern const std::unordered_map<std::string, std::size_t> sym2Z_{"
            "\n")
    for k, v in sorted(atomicinfo.items()):
        f.write("  {{ \"{}\" , {} }},\n".format(v.sym, k))
    f.write("}; // close sym2Z_\n\n\n")

    # Next, full atomic data
    f.write("extern const std::unordered_map<std::size_t, AtomicInfo> "
            "atomic_data_{\n")
    for k, v in sorted(atomicinfo.items()):
        f.write("  {{ {:d},//Start pair<int,AtomicInfo>\n".format(k))
        v.cxxify(f)
        f.write("\n  },//End pair<int,AtomicInfo>\n")
    f.write("};\n")
    f.write("}}//End namespaces\n")

with open("TestAtomicInfo.cpp", 'w') as f:
    f.write("#include <NWChemExRuntime/AtomicInfo.hpp>\n")
    f.write("#define CATCH_CONFIG_MAIN\n")
    f.write("#include <catch/catch.hpp>\n\n")
    f.write("using namespace {}::detail_;\n".format(nmspace))
    f.write("TEST_CASE(\"Symbol to Z conversion\")\n")
    f.write("{\n")
    for k, v in sorted(atomicinfo.items()):
        f.write("    REQUIRE(sym2Z_.at(\"{:s}\") == {:d});\n".format(v.sym, k))
    f.write("}\n\n")
    f.write("TEST_CASE(\"Atomic Data\")\n")
    f.write("{\n")
    for k, v in sorted(atomicinfo.items()):
        prefix = "        REQUIRE(atomic_data_.at({:d})".format(k)
        f.write("    SECTION(\"Atom # {:d}\")\n".format(k))
        f.write("    {\n")
        f.write("{:s}.Z == {:d});\n".format(prefix, k))
        f.write("{:s}.sym == \"{:s}\");\n".format(prefix, v.sym))
        f.write("{:s}.name == \"{:s}\");\n".format(prefix, v.name))
        f.write("{:s}.multiplicity == {:d});\n".format(prefix, v.mult))
        f.write("{:s}.mass == Approx({:16.16f}));\n".format(prefix, v.mass))
        comp_val = "Approx({:16.16f})".format(v.cov_radius)
        f.write("{:s}.covradius == {:s});\n".format(prefix,comp_val))
        comp_val = "Approx({:16.16f})".format(v.vdw_radius)
        f.write("{:s}.vdwradius == {:s});\n".format(prefix,comp_val))
        for ki, vi in sorted(v.isotopes.items()):
            iso_prefix = "{:s}.isotopes.at({:d})".format(prefix, ki)
            f.write("{:s}.isonum == {:d});\n".format(iso_prefix, ki))
            comp_val = "Approx({:16.16f})".format(vi.mass)
            f.write("{:s}.mass == {:s});\n".format(iso_prefix, comp_val))
            comp_val = "Approx({:16.16f})".format(vi.abundance)
            f.write("{:s}.abundance == {:s});\n".format(iso_prefix, comp_val))
        f.write("    }\n")
    f.write("}\n")
