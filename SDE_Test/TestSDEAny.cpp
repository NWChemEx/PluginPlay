#include <SDE/SDEAny.hpp>
#include <SDE/Serialization.hpp>
#include <catch/catch.hpp>
#include <fstream>
#include <map>
#include <typeindex>
#include <vector>

using namespace SDE;
using namespace detail_;

/* Note at the moment Pybind11 seems to have a bug where we can't check the
 * pythonize member function purely from C++.*/

// The correct hashes based on RTTI (assumes all instances of the same type have
// the same value)
static std::map<std::type_index, std::string> corr_hvs{
  {typeid(nullptr), "7b018a8f67b01a9f30105059dc7721af"},
  {typeid(double), "377cf5b742574a826e0824b9a59bfed0"},
  {typeid(std::vector<int>), "d73e9ea9861ab9d828d0df4f19735f5a"}};

template<typename T>
void check_state(SDEAny& da_any, std::initializer_list<T> contents) {
    // Const reference for checking functions on const instances
    const SDEAny& const_da_any = da_any;

    REQUIRE(da_any.has_value() == contents.size());
    REQUIRE(const_da_any.has_value() == contents.size());
    REQUIRE(da_any.type() == typeid(T));
    REQUIRE(const_da_any.type() == typeid(T));

    Hasher h(bphash::HashType::Hash128);
    h(da_any);
    auto hv = h.finalize();
    REQUIRE(corr_hvs.at(typeid(T)) == bphash::hash_to_string(hv));

    // Check the wrapped type (for non-empty SDEAnys)
    if(contents.size()) {
        auto right_val     = *contents.begin();
        T& val             = SDEAnyCast<T>(da_any);
        const T& const_val = SDEAnyCast<T>(da_any);
        REQUIRE(val == right_val);
        REQUIRE(const_val == right_val);

#ifndef USING_pybind11
        REQUIRE_THROWS_AS(da_any.pythonize(), std::runtime_error);
        REQUIRE_THROWS_AS(const_da_any.pythonize(), std::runtime_error);
        REQUIRE_THROWS_AS(da_any.insert_python(), std::runtime_error);
#endif

        REQUIRE_THROWS_AS(SDEAnyCast<std::string>(da_any), std::bad_cast);
        REQUIRE_THROWS_AS(SDEAnyCast<std::string>(const_da_any), std::bad_cast);
    }
}

TEST_CASE("SDEAny Class") {
    SECTION("Basic SDEAny Usage") {
        // The "contents" of an empty SDEAny
        std::initializer_list<decltype(nullptr)> empty;
        SDEAny defaulted; // An empty instance

        double pi{3.14};
        SDEAny wrap_double(pi); // An instance holding a double

        SECTION("Default Ctor") { check_state(defaulted, empty); }

        SECTION("By value CTor") { check_state(wrap_double, {pi}); }

        SECTION("Copy CTor w/ defaulted instance") {
            SDEAny copy_default(defaulted);
            check_state(copy_default, empty);
        }

        SECTION("Move CTor w/ defaulted instance") {
            SDEAny move_default(std::move(defaulted));
            check_state(move_default, empty);
        }

        SECTION("Copy CTor w/ non-defaulted instance") {
            SDEAny copy_double(wrap_double);
            check_state(copy_double, {pi});
        }

        SECTION("Copy Assignment w/ defaulted instance") {
            wrap_double = defaulted;
            check_state(wrap_double, empty);
        }

        SECTION("Copy Assignment w/ non-defaulted instace") {
            defaulted = wrap_double;
            check_state(wrap_double, {pi});
        }

        SECTION("Move CTor w/ non-defaulted instance") {
            // Get address of wrapped instance in original
            const double* pval = &(SDEAnyCast<double>(wrap_double));
            SDEAny move_double(std::move(wrap_double));
            check_state(move_double, {pi});
            // Get address of wrapped instance in "new"
            const double* pnew_val = &(SDEAnyCast<double>(move_double));
            REQUIRE(pval == pnew_val); // Make sure they're the same
        }

        SECTION("Move Assignment w/ defaulted instance") {
            wrap_double = std::move(defaulted);
            check_state(wrap_double, empty);
        }

        SECTION("Move Assignment w/ non-defaulted instance") {
            // Address before move
            const double* pval = &(SDEAnyCast<double>(wrap_double));
            defaulted          = std::move(wrap_double);
            check_state(defaulted, {pi});
            // Address after move
            const double* pnew_val = &(SDEAnyCast<double>(defaulted));
            REQUIRE(pval == pnew_val); // Literally same instance check
        }

        SECTION("Reset") {
            wrap_double.reset();
            check_state(wrap_double, empty);
        }

        SECTION("Swap default and non-default") {
            // Address before move
            const double* pval = &(SDEAnyCast<double>(wrap_double));
            defaulted.swap(wrap_double);
            check_state(defaulted, {pi});
            check_state(wrap_double, empty);
            // Address after move
            const double* pnew_val = &(SDEAnyCast<double>(defaulted));
            REQUIRE(pval == pnew_val); // Literally same instance check
        }

        SECTION("Swap non-default and default") {
            wrap_double.swap(defaulted);
            check_state(defaulted, {pi});
            check_state(wrap_double, empty);
        }

        SECTION("Emplace") {
            double& new_pi = defaulted.emplace<double>(pi);
            check_state(defaulted, {pi});
            double& new_pi2 = SDEAnyCast<double>(defaulted);
            REQUIRE(&new_pi ==
                    &new_pi2); // Ensure emplace returns same instance
        }

#ifdef USING_pybind11
        SECTION("Pythonize/insert_python") {
            auto pyobject = wrap_double.pythonize();
            double pyval  = pyobject.cast<double>();
            REQUIRE(pyval == pi);

            pybind11::object double_obj = pybind11::cast(3.12);
            wrap_double.insert_python(double_obj);
            REQUIRE(SDEAnyCast<double>(wrap_double) == 3.12);
        }
#endif
    }

    SECTION("Non-POD Wrapped Types") {
        std::vector<int> value(1, 6);

        SECTION("make_SDEAny") {
            auto wrapped_vector = make_SDEAny<std::vector<int>>(1, 6);
            check_state(wrapped_vector, {value});
        }

        SECTION("Move value ctor") {
            int* ptr = value.data();
            std::vector<int> copy_value(value);
            SDEAny wrapped_value(std::move(value));
            check_state(wrapped_value, {copy_value});
            int* new_ptr = SDEAnyCast<std::vector<int>>(wrapped_value).data();
            REQUIRE(ptr == new_ptr); // Make sure it's the same instance
        }
    }

    SECTION("Serialization") {
        int ival(123);
	std::vector<double> dval(1.1, 2.2, 3.3);
	
	SECTION("Cereal XML Archive") {
	  { std::ofstream file("testCereal.log");
	  cereal::XMLOutputArchive xoa(file);
	  SDEAny ivalAny(ival);
	  SDEAny dvalAny(dval);
	  ivalAny.save(xoa, "ival");
	  dvalAny.save(xoa, "dval"); }
	  
	std::ifstream file("testCereal.log");
	cereal::XMLInputArchive xia(file);
	SDEAny _ivalAny;
	SDEAny _dvalAny;
	//Out-of-order loading
	_dvalAny.load(xia, "dval");
	_ivalAny.load(xia, "ival");
	auto _dval = SDEAnyCast<std::vector<double> >(_dvalAny);
	auto _ival = SDEAnyCast<int>(_ivalAny);
	REQUIRE(_ival == ival);
	REQUIRE(_dval.at(1) == Approx(dval.at(1)));
        }
    }    

}
