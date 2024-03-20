#include "common.hpp"
#include "config/import.hpp"

namespace CentroMidi
{
namespace Config
{
namespace Import
{

template <>
void setToCv<std::string>(Cv<std::string>& cv, const std::string& val_str)
{
    cv.set(val_str);
}

template <>
void setToCv<int>(Cv<int>& cv, const std::string& val_str)
{
    if (std::regex_match(val_str, std::regex("[(-|+)|][0-9]*")))
        cv.set(std::stoi(val_str));
    else
        cv.setDefault();
}

template <>
void setToCv<bool>(Cv<bool>& cv, const std::string& val_str)
{
    if (val_str == "1")
        cv.set(true);
    else
        cv.set(false);
}

} // Import
} // Config
} // CentroMidi
