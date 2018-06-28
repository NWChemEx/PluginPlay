#include <SDE/Parameters.hpp>
#include <pybind11/pybind11.h>
using namespace SDE;

PYBIND11_MODULE(DummyParameters, m) {
    Parameters params;
    params.insert("The number 3", Option{3,"some description",
                                        {GreaterThan<int>{0}},
                                        {OptionTraits::optional}});
    params.insert("Pi", Option{3.1416});
    params.insert("A vector", Option{std::vector<int>{1, 2, 3}});
    params.insert("Hello", Option{std::string{"Hello world"}});
    m.attr("params") = params;
}

