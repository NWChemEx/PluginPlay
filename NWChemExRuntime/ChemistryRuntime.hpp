#pragma once

namespace NWXRuntime {

/** @brief Collects constants, conversion factors, atomic data, basis sets, etc.
 *         into a one-stop-shop.
 *
 *  This class is meant primarily as an aggregation unit for simplifying
 *  signatures of functions.  All of its member classes have their own tests
 *  and this class adds no extra features over what they provide.
 */
struct ChemicalRuntime {

    ///Basis sets come from EMSL's basis set exchange
    BasisSetRepo bse;

    ///We all know wikipedia is where one goes to get reference data...
    ReferenceDataRepo wikipedia;

    ///Molecules come from pubchem
    MoleculeRepo pubchem;

};

}//End namespace
