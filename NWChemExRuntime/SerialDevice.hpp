#pragma once

#include "NWChemExRuntime/Device.hpp"

namespace NWChemExRuntime {

///Serial implementation of the Device API
class SerialDevice: public Device<SerialDevice> {
private:
    ///Allow the base class to call the implementation, but not the user
    friend class Device<SerialDevice>;

    template<typename ReturnType, typename FxnType, typename...Args>
    void add_task_(Promise<ReturnType>&& p, FxnType task, Args&&...args)
    {
        p.set_value(task(std::forward<Args>(args)...));
    }

    template<typename ReturnType, typename ItrType, typename FxnType>
    void foreach_(Promise<std::vector<ReturnType>>&&p,
                  ItrType begin, ItrType end, FxnType task)
    {
        std::vector<ReturnType> rv;
        while(begin!=end)
        {
            rv.push_back(task(*begin));
            ++begin;
        }
        p.set_value(std::move(rv));
    };

    template<typename ReturnType, typename ItrType, typename FxnType,
             typename ReduceFxn>
    void reduce_(Promise<ReturnType>&&p, ItrType begin, ItrType end,
                    FxnType task, ReduceFxn red)
    {
        //Watch for case where there's no iteration range and we can't get an
        //initial value.
        if(begin==end)
        {
            p.set_value(ReturnType());
            return;
        }

        //First value
        ReturnType total=std::move(task(*begin));
        ++begin;
        while(begin!=end)
        {
            total=red(total,task(*begin));
            ++begin;
        }
        p.set_value(std::move(total));
    };

};

}


