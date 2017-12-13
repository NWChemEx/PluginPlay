import os
import sys
import collections

""" Script for generating a list of physical constants in a C++ compilable form.

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

# Hardcoded constants and conversions (strings to preserve precision)
hard_code = {"pi" : ("3.14159265358979323846",
                     "Ratio of circle's circumference to it's diameter"),
             "J2cal" : ("4.184", "Ratio of a Joule to a calorie"),
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
         "na": "Avogadro's constant in per mole"
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
    with open("PhysicalConstants.hpp", 'w') as f:
        f.write("#pragma once\n\n")
        f.write("/** @file Fundamental physical constants for NWChemEx\n")
        f.write(" *\n")
        f.write(" *  Generally speaking, the idea is to define as few "
                "constants as\n")
        f.write(" *  possible and derive all others.  This helps to ")
        f.write("maintain consistency.\n\n")
        f.write(" *  @warning Data made by generate_constants.py.\n")
        f.write(" *           DO NOT EDIT!!!!!!!!\n")
        f.write(" */\n\n")
        f.write("namespace NWXRuntime {\n")
        f.write("namespace constant {\n")
        f.write("//Fundamental physical constants\n\n")
        for k,v in sorted(hard_code_.items()):
            f.write("///{:s}\n".format(v[1]))
            f.write("constexpr double {:s} = {:s};\n\n".format(k, v[0]))
        for k, v in sorted(cnsts_.items()):
            f.write("///{:s}\n".format(descs[k]))
            f.write("constexpr double {:s} = {:s};\n\n".format(k, v))
        f.write("//Derived constants\n\n")
        for k, v in derived_.items():
            f.write("///{:s}\n".format(v[2]))
            f.write("constexpr double {:s} = (".format(k))
            for i, num in enumerate(v[0]):  # Print numerator
                f.write("{:s}".format(str(num)))
                if not i+1 == len(v[0]):
                    f.write("*")
            f.write(")/(")
            for i, denom in enumerate(v[1]):  # Print denominator
                f.write("{:s}".format(str(denom)))
                if not i+1 == len(v[1]):
                    f.write("*")
            f.write(");\n\n")
        f.write("}}//End namespaces\n")


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


def write_tests(cnts_, hard_code_, derived_):
    # Print the tests
    with open("TestPhysicalConstants.cpp", 'w') as f:
        f.write("#include <NWChemExRuntime/PhysicalConstants.hpp>\n\n")
        f.write("#define CATCH_CONFIG_MAIN\n")
        f.write("#include <catch/catch.hpp>\n\n")
        f.write("/** @file Tests of fundamental physical constants to be used in\n")
        f.write(" *        NWChemEx\n")
        f.write(" *\n")
        f.write(" *  @warning This file made by generate_constants.py.\n")
        f.write(" *           DO NOT EDIT!!!!!!!!\n")
        f.write(" */\n\n")
        f.write("using namespace NWXRuntime::constant;\n")
        f.write("TEST_CASE(\"Fundamental Constants\")\n")
        f.write("{\n")
        for k, v in sorted(hard_code_.items()):
            val = float(v[0])
            f.write("    REQUIRE({:s} == Approx({:16.16e}));\n".format(k, val))
        for k, v in sorted(cnts_.items()):
            val = float(v)
            f.write("    REQUIRE({:s} == Approx({:16.16e}));\n".format(k, val))
        f.write("}\n")
        f.write("TEST_CASE(\"Derived Constants\")\n")
        f.write("{\n")
        for k, v in derived_.items():
            val = evaluate_variable(v, cnts_, hard_code_, derived_)
            f.write("    REQUIRE({:s} == Approx({:16.16e}));\n".format(k, val))
        f.write("}\n")


constants = extract_constants()
write_header(constants, hard_code, derived)
write_tests(constants, hard_code, derived)
