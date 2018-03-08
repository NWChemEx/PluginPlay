import os
import sys
import collections
from helper_fxns import *

""" Script for generating a list of physical constants in a C++ compilable form.

    For unit conversion-like constants conversions are given as a means from X
    to the SI base unit.  Thus an arbitrary conversion from X to Y should be 
    possible by converting from X to the base, and then dividing by the 
    conversion from Y to the base.

    usage: python3 generate_constants.py <output_directory>
    
    args:
        output_directory (path) : the path to the directory you want the file to
                                  go into
                                  
    Constants will be read out of the NIST-CODATA-2014.txt file 

"""

class Constant:
    def __init__(self, value, sym, units, desc):
        self.value = value
        self.sym = sym
        self.units = units
        self.desc = desc
    def cxxify(self, f):
        f.write("{}, \"{}\", \"{}\"".format(self.value, self.units, self.desc))

def hard_coded():
    return [
    Constant("3.14159265358979323846", "pi", "",
                       "Ratio of circle's circumference to it's diameter"),
    Constant("4.184", "Cal2J", "Joules per calorie",
                       "Ratio of a Joule to a calorie")
    ]

def extract_constants(data_dir):
    cs_we_want = [("electron mass", "me", "kilograms", "Rest mass of electron"),
                  ("elementary charge", "qe", "Coulombs", "Charge of electron"),
                  ("Planck constant", "h", "Joule seconds", "Planck constant"),
                  ("fine-structure constant", "alpha", "", "Fine-structure "
                                                           "constant"),
                  ("speed of light in vacuum", "c", "meters per second",
                   "speed of light in vacuum"),
                  ("Boltzmann constant", "kb", "Joules per Kelvin",
                   "Boltzmann's constant"),
                  ("Avogadro constant", "na", "things per mole", "Avogadro's "
                                                                 "constant")
                  ]

    constants_ = []
    constant_file = os.path.join(data_dir, "NIST-CODATA-2014.txt")
    # File format is: name, value, uncertainty, unit
    for l in open(constant_file).readlines()[10:]:
        line = list(filter(None, l.split("  ")))
        for cs in cs_we_want:
            if line[0] == cs[0]:
                c = Constant("".join(line[1].split()), cs[1], cs[2], cs[3])
                constants_.append(c)
    return constants_

def evaluate_variable(formula, cts_):
    frac = [1.0, 1.0]
    for i_ in range(2):
        for val in formula[i_]:
            found = False
            for c in cts_:
                if c.sym == val :
                    frac[i_] = float(frac[i_])*float(c.value)
                    found = True
                    break
            if not found:
                frac[i_] = float(frac[i_])*val
    return frac[0]/frac[1]

def get_derived(constants):
    # Derived units symbol to (numerator tuple, denominator tuple description)
    derived = collections.OrderedDict()
    derived["hbar"] = (('h',), (2.0, "pi"),"Joule seconds",
                       "Reduced Plank constant")
    derived["e0"] = (("qe", "qe"), (2.0, "alpha", 'h', 'c'),
                     "Coulomb^2/(Joule Meter)", "Vacuum permittivity")
    derived["kc"] = ((1.0,), (4.0, "pi", "e0"), "meter/Coulomb^2",
                     "Coulomb's constant")
    derived["Eh"] = (("alpha", "alpha", "me", "c", "c"), (1.0,),
                     "Joules", "Hartree constant")
    derived["a0"] = (("hbar",), ("me", "alpha", 'c'), "meters",
                     "Bohr radius")
    derived["R"] = (("kb","na"), (1.0,), "Joules per Kelvin per mole",
                    "Ideal gas constant")
    derived["Hz2J"] = (('h', ), (1.0,), "Joules per Hertz",
                       "Conversion from Hertz to Joules")
    derived["inv_m2J"] = (('h', 'c'), (1.0,), "Joule meters",
                          "Conversion from m^-1 to Joules")
    derived["eV2J"] = ((1.0,), ("qe",), "Joules per eV",
                       "Conversion from eV to Joules")
    for k,v in derived.items():
        c = Constant(evaluate_variable((v[0], v[1]), constants), k, v[2], v[3])
        constants.append(c)
    return constants

def write_source(my_path, cnsts_):
    file = os.path.join(os.path.dirname(my_path), "NWChemExRuntime","Defaults")
    with open(os.path.join(file,"PhysicalConstants.cpp"), 'w') as f:
        write_header(__file__, f)
        f.write("using const_t = typename ChemistryRuntime::constant_type;\n")
        f.write("using return_t = typename "
                "ChemistryRuntime::constant_lut_type;\n")
        f.write("return_t default_constants(){\n")
        f.write("    return_t rv;\n")
        for c in cnsts_:
            f.write("    rv[\"{}\"] = const_t{{".format(c.sym))
            c.cxxify(f)
            f.write("};\n")
        write_footer(f)

def main():
    my_path = os.path.dirname(os.path.realpath(__file__))
    data_dir = os.path.join(my_path, "physical_data")

    constants = hard_coded()
    constants += extract_constants(data_dir)
    constants = get_derived(constants)
    write_source(my_path, constants)

def write_tests(cnts_, hard_code_, derived_):
    # Print the tests
    with open("../NWChemExRuntime_Test/TestPhysicalConstants.cpp", 'w') as f:
        f.write("#include <NWChemExRuntime/PhysicalConstants.hpp>\n\n")
        f.write("#include <catch/catch.hpp>\n\n")
        f.write("/** @file Tests of fundamental physical constants to be used in\n")
        f.write(" *        NWChemEx\n")
        f.write(" *\n")
        f.write(" *  @warning This file made by generate_constants.py.\n")
        f.write(" *           DO NOT EDIT!!!!!!!!\n")
        f.write(" */\n\n")
        f.write("using namespace NWXRuntime;\n")
        f.write("using namespace NWXRuntime::detail_;\n")
        f.write("TEST_CASE(\"Fundamental Constants\")\n")
        f.write("{\n")
        prefix = "    REQUIRE(constants.at(Constant::"
        for k, v in sorted(hard_code_.items()):
            val = float(v[0])
            f.write(prefix + "{:s}) == Approx({:16.16e}));\n".format(k, val))
        for k, v in sorted(cnts_.items()):
            val = float(v)
            f.write(prefix + "{:s}) == Approx({:16.16e}));\n".format(k, val))
        f.write("}\n")
        f.write("TEST_CASE(\"Derived Constants\")\n")
        f.write("{\n")
        for k, v in derived_.items():
            val = evaluate_variable(v, cnts_, hard_code_, derived_)
            f.write(prefix + "{:s}) == Approx({:16.16e}));\n".format(k, val))
        f.write("}\n")


if __name__ == "__main__":
    main()
