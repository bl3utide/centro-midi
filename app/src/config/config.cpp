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
    // TODO delete old-style declarations
    //std::string in_dev_name;    // InputDevice
    //std::string out_dev_name;   // OutputDevice
    //int to_ch;                  // ToChannel
    //bool is_force_adj;          // ForceAdjustMidiCh

    Cv<std::string> in_dev_name{ "", "", "" };  // InputDevice
    Cv<std::string> out_dev_name{ "", "", "" }; // OuputDevice
    Cv<int> to_ch{ 1, 16, 1 };                  // ToChannel
    Cv<bool> is_force_adj{ true, true, true };  // ForceAdjustMidiCh

    // default
    InternalData()
    {}

    // load from ini
    InternalData(const std::string& input_device,
                 const std::string& output_device,
                 const int to_channel,
                 const bool is_force_adjust_midi_channel)
    {
        in_dev_name = input_device;
        out_dev_name = output_device;
        to_ch = to_channel;
        is_force_adj = is_force_adjust_midi_channel;
    }
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
InternalData _internal;
bool _use_pretty_print = true;

void initialize(const std::string& ini_file_name)
{
    mINI::INIFile file = mINI::INIFile(ini_file_name);

    if (file.read(_is))
    {
        // ini-file already exists

        // TODO load values from the ini file and set values to _internal
    }
    else
    {
        // ini-file doesn't exist
        _internal = InternalData();
    }
}

void finalize() noexcept
{

}

} // Config
} // CentroMidi
