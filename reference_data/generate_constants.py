import os
import sys
import collections

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

# Check usage of script before doing anything
if len(sys.argv) != 2:
    print("Usage: generate_constants.py <output_directory>")
    quit(1)

# Get some paths for input and output of files
outbase = sys.argv[1]
mypath = os.path.dirname(os.path.realpath(__file__))
datadir = os.path.join(mypath, "physical_data")

# Hardcoded constants (strings to preserve precision)
hard_code = {"pi" : ("3.14159265358979323846",
                     "Ratio of circle's circumference to it's diameter"),
             "Cal2J" : ("4.184", "Ratio of a Joule to a calorie"),
             }

# list of constants we want from file (names match file) and our symbol for them
cs_we_want = [("electron mass", "me"),
              ("elementary charge", "qe"),
              ("Planck constant", "h"),
              ("fine-structure constant", "alpha"),
              ("speed of light in vacuum", "c"),
              ("Boltzmann constant", "kb"),
              ("Avogadro constant", "na")
              ]

# Mapping from fundamental constants we extract to our descriptions
descs = {"me": "Rest mass of electron in kilograms",
         "qe": "Charge of electron in Coulombs",
         "h": "Plank's constant in Joule seconds",
         "alpha": "Fine-structure constant (dimensionless)",
         "c": "Speed of light in vacuum in meters per second",
         "kb": "Boltzmann's constant in Joules per Kelvin",
         "na": "Avogadro's constant in things per mole"
         }

# Derived units symbol to (numerator tuple, denominator tuple description)
derived = collections.OrderedDict()
derived["hbar"] = (('h',), (2.0, "pi"),"Reduced Plank constant Joule seconds")
derived["e0"] = (("qe", "qe"), (2.0, "alpha", 'h', 'c'),
                 "Vacuum permittivity Coulomb^2/(Joule Meter)")
derived["kc"] = ((1.0,), (4.0, "pi", "e0"), "Coulomb's constant Joule meter/ "
                                            "Coulomb^2")
derived["Eh"] = (("alpha", "alpha", "me", "c", "c"), (1.0,),
                 "Hartree constant in Joules")
derived["a0"] = (("hbar",), ("me", "alpha", 'c'),"Bohr radius in meters")
derived["R"] = (("kb","na"), (1.0,), "Ideal gas constant Joules per Kelvin "
                                     "per mole")
derived["Hz2J"] = (('h'), (1.0,), "Conversion from Hertz to Joules")
derived["inv_m2J"] = (('h', 'c'), (1.0,), "Conversion from m^-1 to Joules")


def extract_constants():
    constants_ = {}
    constant_file = os.path.join(datadir, "NIST-CODATA-2014.txt")
    # File format is: name, value, uncertainty, unit
    for l in open(constant_file).readlines()[10:]:
        line = list(filter(None, l.split("  ")))
        for cs in cs_we_want:
            if line[0] == cs[0]:
                constants_[cs[1]] = "".join(line[1].split())
    return constants_


def write_header(cnsts_, hard_code_, derived_):
    # Print the header file
    with open("../NWChemExRuntime/PhysicalConstants.hpp", 'w') as f:
        f.write("#pragma once\n\n")
        f.write("/**\n")
        f.write(" *  @file PhysicalConstants.hpp\n")
        f.write(" *\n")
        f.write(" * Contains enumerations for the recognized physical "
                "constants\n")
        f.write(" */\n\n")
        f.write("#include <map>\n")
        f.write("namespace NWXRuntime {\n")
        f.write("enum class Constant{\n")
        for k,v in sorted(hard_code_.items()):
            f.write("    {:s}, ///< {:s}\n".format(k, v[1]))
        for k, v in sorted(cnsts_.items()):
            f.write("    {:s}, ///< {:s}\n".format(k, descs[k]))
        for k, v in derived_.items():
            f.write("    {:s}, ///< {:s}\n".format(k, v[2]))
        f.write("};\n\n")
        f.write("namespace detail_ {\n")
        f.write("extern const std::map<Constant, double> constants;\n")
        f.write("}\n")
        f.write("}\n")

def evaluate_variable(formula, cts_, hard_code_, derived_):
    frac = [1.0, 1.0]
    for i_ in range(2):
        for val in formula[i_]:
            if val in cts_:
                frac[i_] *= float(cts_[val])
            elif val in hard_code_:
                frac[i_] *= float(hard_code_[val][0])
            elif val in derived_:
                lbl = derived_[val]
                frac[i_] *= evaluate_variable(lbl, cts_, hard_code_, derived_)
            else:
                frac[i_] *= val

    return frac[0]/frac[1]

def write_source(cnsts_, hard_code_, derived_):
    with open("../NWChemExRuntime/PhysicalConstants.cpp", 'w') as f:
        f.write("#include \"NWChemExRuntime/PhysicalConstants.hpp\"\n\n")
        f.write("namespace NWXRuntime {\n")
        f.write("namespace detail_ {\n")
        f.write("extern const std::map<Constant, double> constants([]()\n{\n")
        f.write("    std::map<Constant, double> temp;\n")
        for k,v in sorted(hard_code_.items()):
            f.write("    temp[Constant::{:s}] = {:s};\n".format(k,v[0]))
        for k,v in sorted(cnsts_.items()):
            f.write("    temp[Constant::{:s}] = {:s};\n".format(k,v))
        for k,v in derived_.items():
            f.write("    temp[Constant::{:s}] = ".format(k))
            f.write("{:16.16e};\n".format(evaluate_variable(v, cnsts_,
                                                            hard_code_,
                                                            derived_)))
        f.write("    return temp;\n}());\n")
        f.write("}}//End namespaces\n")

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


constants = extract_constants()
write_header(constants, hard_code, derived)
write_source(constants, hard_code, derived)
write_tests(constants, hard_code, derived)
