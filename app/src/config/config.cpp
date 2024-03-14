#include "common.hpp"
#include "config/config.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace CentroMidi
{
namespace Config
{

enum class Section : int
{
    Device,
    _COUNT_,
};

enum class DeviceKey : int
{
    InputDevice,
    OutputDevice,
    ToChannel,
    ForceAdjustMidiCh,
    _COUNT_,
};

// TODO move to config/cv.hpp
/*
    Configuration Value Class

    compatible types(expression in .ini)
    - bool          ("1" or "0")
    - int           ("1", "-10")
    - std::string   (same)
*/
template <class T>
class Cv
{
public:
    Cv(const T& v_min, const T& v_max, const T& v_def)
        : _min(v_min), _max(v_max), _def(v_def)
    {
        if constexpr (std::is_integral<T>::value)
        {
            if (_min > _max) throw std::exception("Max-value is less than min-value");

            if (_def < _min) throw std::exception("Default-value is less than min-value");
            if (_def > _max) throw std::exception("Default-value is larger than max-value");
        }

        _config_value = v_def;
    }

    Cv() = delete;

    Cv& operator=(const T& rhs)
    {
        if (isWithinRange(rhs))
        {
            _config_value = rhs;
        }
        else
        {
            throw std::exception("The internal value of Cv is out of range");
        }
        return *this;
    }

    void set(const T& v)
    {
        if (isWithinRange(v))
        {
            _config_value = v;
        }
        else
        {
            throw std::exception("The internal value of Cv is out of range");
        }
    }

    int cv() const noexcept
    {
        return _config_value;
    }

    int max() const noexcept { return _max; }
    int min() const noexcept { return _min; }
    void setDefault() noexcept { _config_value = _def; }

private:
    T _config_value;
    T _min, _max, _def;
    bool isWithinRange(const T& v) const noexcept
    {
        if constexpr (std::is_integral<T>::value)
        {
            return _min <= v && v <= _max;
        }
        else
        {
            return true;
        }
    };
};

// TODO move to config/model.hpp
struct InternalData
{
    Cv<std::string> in_dev_name { "", "", "" };         // InputDevice
    Cv<std::string> out_dev_name{ "", "", "" };         // OuputDevice
    Cv<int>         to_ch       { 1, 16, 1 };           // ToChannel
    Cv<bool>        is_force_adj{ true, true, true };   // ForceAdjustMidiCh

    InternalData()
    {}
};

// private
const char* _section_names[static_cast<int>(Section::_COUNT_)]
{
    "Device",
};
const char* _device_key_names[static_cast<int>(DeviceKey::_COUNT_)]
{
    "InputDevice",
    "OutputDevice",
    "ToChannel",
    "ForceAdjustMidiCh",
};
mINI::INIStructure _is;
std::string _file_name;
InternalData _internal;
bool _use_pretty_print = true;

// TODO change func name to more meaningful name
template<class T>
void setValue(const char* section, const char* key, Cv<T>* cv)
{
    if (_is.get(section).has(key))
    {
        const std::string& target = _is.get(section).get(key);

        if constexpr (std::is_same_v<T, std::string>)
        {
            cv->set(target);
        }
        else if constexpr (std::is_same_v<T, int>)
        {
            cv->set(std::stoi(target));
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
            if (target == "1")
            {
                cv->set(true);
            }
            else
            {
                cv->set(false);
            }
        }
        else
        {
            // unexpected type of value
            throw std::exception(format("Failed to load config from ini because [%s]%s type is unexpected"));
        }
#ifdef _DEBUG
        LOGD << "[" << section << "]" << key << " is set to value: " << cv->cv();
#endif
    }
    else
    {
        cv->setDefault();
#ifdef _DEBUG
        LOGD << "[" << section << "]" << key << " is set to default value: " << cv->cv();
#endif
    }
}

void initialize(const std::string& ini_file_name)
{
    _file_name = ini_file_name;
    mINI::INIFile file = mINI::INIFile(_file_name);

    _internal = InternalData();

    if (file.read(_is))
    {
#ifdef _DEBUG
        LOGD << "Load config from existing ini file";
#endif
        // ini-file already exists

        // [Device]
        setValue<std::string>(_section_names[static_cast<int>(Section::Device)], _device_key_names[static_cast<int>(DeviceKey::InputDevice)], &_internal.in_dev_name);
        setValue<std::string>(_section_names[static_cast<int>(Section::Device)], _device_key_names[static_cast<int>(DeviceKey::OutputDevice)], &_internal.out_dev_name);
        setValue<int>(_section_names[static_cast<int>(Section::Device)], _device_key_names[static_cast<int>(DeviceKey::ToChannel)], &_internal.to_ch);
        setValue<bool>(_section_names[static_cast<int>(Section::Device)], _device_key_names[static_cast<int>(DeviceKey::ForceAdjustMidiCh)], &_internal.is_force_adj);
    }
#ifdef _DEBUG
    else
    {
        LOGD << "Ini file does not exists";
    }
#endif
}

void finalize() noexcept
{
    mINI::INIFile file = mINI::INIFile(_file_name);

    // [Device]
    // TODO write config data to the ini file
}

} // Config
} // CentroMidi
