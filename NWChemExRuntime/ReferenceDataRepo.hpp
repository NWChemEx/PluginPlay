#pragma once
#include "NWChemExRuntime/AtomicInfo.hpp"
#include "NWChemExRuntime/PhysicalConstants.hpp"
#include <LibChemist/Atom.hpp>

namespace NWXRuntime {

/**
 * @brief The ReferenceDataRepo class serves as a one-stop shop for obtaining
 * chemical and physical reference data (atomic masses, physical constants,...).
 *
 * The ReferenceDataRepo class provides the API for obtaining the physical data
 * in NWChemEx and decouples the act of getting the data from the actual
 * physical values.  This allows users to use NWChemEx with physical data from
 * another package or more up to date values without having to modify the
 * NWChemEx source code.
 *
 * Design philosophy:
 * The class itself ultimately aggregates three maps:
 *
 * 1. atomic symbol to Z
 * 2. enum of physical constant to value
 * 3. Z to atomic data
 *
 * Typically this would suggest that the class just be a struct with the
 * maps publicly accessible; however, I also want the data to be easily
 * obtained regardless of how one wants to look it up (e.g. symbol vs Z). In
 * order to avoid  storing the data multiple times we make the convention
 * internally that atomic data is mapped by Z and not sym.  Consequentially as
 * some look ups will involve some pre-processing that we don't want to expose
 * to the user of the class we have made the members private.
 *
 */
class ReferenceDataRepo {
public:
    ///Type of an index
    using size_type = std::size_t;

    ///The object type one should provide to override the default atom info
    using atomic_info_map_type = std::map<size_type, detail_::AtomicInfo>;

    ///The object type to provide to override the default physical constants
    using physical_constants_map_type = std::map<Constant, double>;

    /**
     * @brief Creates an instance of the ReferenceDataRepo class that is initialized
     *        with NWChemEx's default physical reference data.
     *
     * @throw std::bad_alloc if there is insufficient memory to make the
     * internal state of the class.  Strong throw guarantee.
     */
    ReferenceDataRepo();

    /**
     * @brief Makes a deep copy of the current instance.
     *
     * @param[in] rhs The instance to copy.
     *
     * @throw std::bad_alloc if there is insufficient memory to make the copy.
     * Strong throw guarantee.
     */
    ReferenceDataRepo(const ReferenceDataRepo&)=default;

    /**
     * @brief Takes ownership of another ReferenceDataRepo instance.
     * @param[in] rhs The instance to take ownership of.  After this call @p rhs
     * will be in a valid, but otherwise undefined state.
     * @throw None. No throw guarantee.
     */
    ReferenceDataRepo(ReferenceDataRepo&&)noexcept=default;

    /**
     * @brief Allows a ReferenceDataRepo instance to be created with non-default data.
     *
     * @param atoms A map between atomic number and AtomicInfo structs for that
     *        atomic number.
     * @throw std::bad_alloc if there is insufficient memory to copy the map
     *        into the class.  Strong throw guarantee.
     */
    ReferenceDataRepo(const atomic_info_map_type& atoms,
              const physical_constants_map_type& constants):
        atom_info_(atoms),
        constants_(constants)
    {}

    ///Cleans up the ReferenceDataRepo class
    ~ReferenceDataRepo()noexcept=default;

    /**
     * @brief Sets the state of the current instance to a deep copy of
     * another instance.
     *
     * @param[in] rhs The instance whose state is being copied.
     * @return The current instance containing a deep copy of @p rhs's
     * state.
     * @throw std::bad_alloc if there is insufficient memory to copy
     * @p rhs's state.  Strong throw guarantee.
     */
    ReferenceDataRepo& operator=(const ReferenceDataRepo& /*rhs*/)=default;

    /**
     * @brief Causes the current instance to take ownership of an already
     * existing instance.
     * @param[in] rhs The instance to take ownership of.  After this call @p rhs
     * will be in a valid, but otherwise undefined state.
     * @return The current instance containing @p rhs's state.
     * @throw None. No throw guarantee.
     */
    ReferenceDataRepo& operator=(ReferenceDataRepo&& /*rhs*/)noexcept=default;

    /**
     * @brief Creates an Atom instance initialized with the physical reference
     *        data stored within this instance.
     * @param sym The case-insensitive atomic symbol for the atom you want the
     *        default details of.
     * @param iso The isotope number (atomic number plus number of neutrons) for
     *        the requested atom.  A value of 0 results in the abundance
     *        weighted mass of the element.
     * @return An Atom instance situated at the origin with the default
     *         physical parameters for the element whose atomic symbol is @p sym
     * @throw std::out_of_range if @p sym is not a recognized atomic symbol or
     *        if @p iso is not a recognized isotope number.  Strong throw
     *        guarantee.
     */
    LibChemist::Atom atomic_info(const std::string& sym, size_type iso=0) const;


    /**
     * @brief Creates an Atom instance initialized with the physical reference
     *        data stored within this instance.
     * @param Z The atomic number for the atom.
     * @param iso The isotope number (atomic number plus number of neutrons) for
     *        the requested atom.  A value of 0 results in the abundance
     *        weighted mass of the element.
     * @return An Atom instance situated at the origin with the default
     *         physical parameters for the element whose atomic number is @p Z.
     * @throw std::out_of_range if @p Z is not a recognized atomic number or
     *        if @p iso is not a recognized isotope number.  Strong throw
     *        guarantee.
     */
    LibChemist::Atom atomic_info(size_type Z, size_type iso=0)const;

    /**
     * @brief Returns the value of a physical constant or unit conversion
     *
     * @param[in] c The constant whose value you want.  Choices are included in
     *            PhysicalConstants.hpp
     *
     * @throw std::out_of_range if the value of the requested constant was not
     * set.  Strong throw guarantee.
     */
    double physical_constant(Constant c)const
    {
        return constants_.at(c);
    }

private:
    ///Wrapper function for converting a string to lowercase
    size_type sym2Z(std::string sym)const;

    ///The known atomic information
    atomic_info_map_type atom_info_;

    ///The physical constants
    physical_constants_map_type constants_;
};

}
