#pragma once
#include <string>
#include <map>


namespace NWChemExRuntime {

class ParallelRuntime {
public:
    Device get_device(const std::string& device_name)
    {
        return devices_.at(device_name);
    }
private:
    std::map<std::string,Device> devices_;
};

}//End namespace
