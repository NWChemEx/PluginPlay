#pragma once
#include "SDE/Module.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <Utilities/SmartEnum.hpp>

namespace SDE {

/**
 *  @brief Enumerations for the various meta data types associated with a
 *         module.
 *
 *  The MetaProperty enumeration is meant as a way to associate important meta
 *  data with a module.  At the moment, recognized meta data includes:
 *
 *  - name: A descriptive name for the module
 *  - version: Information that can be used to uniquely identify the state of
 *             the module's source code.
 *  - description: An informative discourse of what the module is capable of
 *  - authors: The people who wrote the module
 *  - citations: Things that should be cited if the module is used.
 *
 */
DECLARE_SmartEnum(MetaProperty, name, version, description, authors, citations);

/**
 * @brief Enumerations pertaining to the characteristics of the module.
 *
 * The moduletraits set of enumerations is envisioned as being one of the ways
 * the SDE, as well as other modules, can obtain information related to the
 * module, without knowing its contents.
 *
 * At the moment the recognized traits include:
 * - nondeterministic: Signals that runs of the same module with the same
 *   algorithmic parameters and the same input may yield different results
 *   (intentionally).  Consequentially memoization should not be performed.
 */
DECLARE_SmartEnum(ModuleTraits, nondeterministic);

/**
 *  @brief The class used by the ModuleManager to create a new instance of a
 *         module.  Also stores meta data that the module developer provided.
 *
 *  Once a module developer has written a module, the SDE needs to know how to
 *  create instances of that module.  The ModuleLoader ultimately fills that
 *  role.  In addition to literally being able to make that module, the module
 *  loader also tells us what the default parameters and submodules are for the
 *  module.
 *
 *  Usage of the ModuleLoader class is described in detail on the
 *  @ref using_module_loader "Using the ModuleLoader Class" page.
 */
struct ModuleLoader {
    /// The type of the pointer returned by this
    using module_pointer = std::unique_ptr<ModuleBase>;

    /// The type of the module keys
    using module_key_type = std::string;

    /// Ensures the class is polymorphic
    virtual ~ModuleLoader() = default;

    /// To be implemented by the developer to return an instance of their module
    virtual module_pointer get_module() const = 0;

    /// The meta data the developer set
    std::map<MetaProperty, std::string> meta_data;

    /// The traits the developer set
    std::set<ModuleTraits> traits;

    /// Default parameters for this module (implement when Parameters is ready)
    // Parameters params;

    /// A map from call-back points to the default submodule key to use
    Utilities::CaseInsensitiveMap<module_key_type> submodules;
}; // class ModuleLoader

} // namespace SDE

/**
 * @brief Convenience macro for declaring a derived class implementing a
 *        module's loader
 *
 * @param[in] loader_name The name of the resulting class.
 */
#define SDE_DECLARE_MODULE_LOADER(loader_name)      \
    class loader_name : public SDE::ModuleLoader {  \
    public:                                         \
        loader_name();                              \
        module_pointer get_module() const override; \
    }
