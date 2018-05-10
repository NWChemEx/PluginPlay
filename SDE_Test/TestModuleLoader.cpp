#include <SDE/ModuleLoader.hpp>
#include <catch/catch.hpp>

using namespace SDE;

SDE_NEW_MODULE_TYPE(FakeModuleType, bool, int);

class FakeModule : public FakeModuleType {
    bool run_(int) { return true; }
};

SDE_DECLARE_MODULE_LOADER(TestLoader);

std::unique_ptr<ModuleBase> TestLoader::get_module() const {
    return std::make_unique<FakeModule>();
}

TestLoader::TestLoader() {
    meta_data[MetaProperty::name]    = "My Fake GR Module";
    meta_data[MetaProperty::version] = "1.2.3";
    meta_data[MetaProperty::authors] = "Albert Einstein";
    meta_data[MetaProperty::description] =
      "Computes the Einstein field equations.";
    meta_data[MetaProperty::citations] =
      "Einstein, Albert (1916). The Foundation of the General Theory of "
      "Relativity. *Annalen der Physik*. 354 (7): 769";

    // params["cosmological constant"] = Option{0.0, "Lambda in the EFE"};

    submodules["stress-energy tensor"] = "";
}

TEST_CASE("Typedefs") {
    using module_pointer = typename TestLoader::module_pointer;
    using module_key     = typename TestLoader::module_key_type;
    REQUIRE(std::is_same<std::unique_ptr<ModuleBase>, module_pointer>::value);
    REQUIRE(std::is_same<std::string, module_key>::value);
}

TEST_CASE("ModuleLoader") {
    TestLoader l;

    SECTION("get_module") {
        auto mod = l.get_module();
        REQUIRE(mod->type() == typeid(FakeModuleType));
    }

    SECTION("Data") { // Just make sure it's there
        REQUIRE(l.meta_data.count(MetaProperty::name));
        REQUIRE(l.submodules.count("stress-energy tensor"));
    }
}
