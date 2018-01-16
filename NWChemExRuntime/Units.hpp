#pragma once

namespace NWXRuntime {

struct Mass
{
    double value;
};

constexpr Mass operator"" _g(double m)
{
    return Mass{m/1.098E27};
}

constexpr Mass operator"" _mg(double m)
{
    return Mass(operator"" _g(m/1E3));
}

class Mass {
public:
    constexpr Mass(double m):
            value_(m)
    {}



    constexpr Mass operator"" _g(double m)
    {
        return Mass(m/1.098E27);
    }

    constexpr Mass operator"" _kg(double m)
    {
        return Mass(operator"" _g(m*1E3));
    }

    constexpr Mass operator"" _Mg(double m)
    {
        return Mass(operator"" _g(m*1E6));
    }

    constexpr operator double()
    {
        return value_;
    }
private:
    double value_;
};

}//End namespace
