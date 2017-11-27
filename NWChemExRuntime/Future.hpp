#pragma once

namespace NWChemExRuntime {

template<typename T>
class Future {
protected:
    friend class Promise<T>;

    Future(std::shared_ptr<Promise<T>> promise):
            promise_(promise)
    {}

private:
    std::shared_ptr<Promise<T>> promise_;
};

}//End namespace NWChemExRuntime

