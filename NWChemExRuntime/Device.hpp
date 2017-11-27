#pragma once

namespace NWChemExRuntime {

template<typename Derived>
class Device {
public:

    template<typename R,typename...Args>
    Future<R> add_task(std::function<R(Args...)> task) {
        return static_cast<Derived>(this)->add_task(task);
    }
};

}


