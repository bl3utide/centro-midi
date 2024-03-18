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

const std::string SECTIONS[static_cast<unsigned long long>(Section::_COUNT_)]
{
    "Device"
};

// TODO move to config/cv.hpp
/*
    Configuration Section-Key-Value Class

    compatible types(expression in .ini)
    - bool          ("1" or "0")
    - int           ("1", "-10")
    - std::string   (same)
*/
template <typename T>
class Cv
{
public:
    Cv(const Section section, const std::string& key, const T& v_min, const T& v_max, const T& v_def)
        : section_name(SECTIONS[static_cast<unsigned long long>(section)]), key_name(key), _min(v_min), _max(v_max), _def(v_def)
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

    T cv() const noexcept
    {
        return _config_value;
    }

    int max() const noexcept { return _max; }
    int min() const noexcept { return _min; }
    void setDefault() noexcept { _config_value = _def; }

    std::string section_name;
    std::string key_name;

private:
    T _config_value;
    T _min, _max, _def;
    bool isWithinRange(const T& v) const noexcept
    {
        if constexpr (std::is_integral<T>::value && !std::is_same_v<T, bool>)
        {
            return _min <= v && v <= _max;
        }
        else
        {
            return true;
        }
    };
};

Cv<std::string> in_dev_name { Section::Device,  "input_device",         "",     "",     "" };
Cv<std::string> out_dev_name{ Section::Device,  "output_device",        "",     "",     "" };
Cv<int>         to_ch       { Section::Device,  "to_channel",           1,      16,     1 };
Cv<bool>        is_force_adj{ Section::Device,  "force_adjust_midi_ch", true,   true,   true };

// private
mINI::INIStructure _is;
std::string _file_name;
bool _use_pretty_print = true;

// TODO config/import.cpp
template<typename T>
void setValue(Cv<T>& cv, const std::string& val_str) {}

template <>
void setValue<std::string>(Cv<std::string>& cv, const std::string& val_str)
{
    cv.set(val_str);
}

template <>
void setValue<int>(Cv<int>& cv, const std::string& val_str)
{
    if (std::atoi(val_str.c_str()))
        cv.set(std::stoi(val_str));
    else
        cv.setDefault();
}

template <>
void setValue<bool>(Cv<bool>& cv, const std::string& val_str)
{
    if (val_str == "1")
        cv.set(true);
    else
        cv.set(false);
}

template<typename T>
void setToStructure(Cv<T>& cv)
{
    if (_is.get(cv.section_name).has(cv.key_name))
    {
        const std::string& src_val = _is.get(cv.section_name).get(cv.key_name);

        setValue<T>(cv, src_val);
#ifdef _DEBUG
        LOGD << "[" << cv.section_name << "]" << cv.key_name << " is set to value: " << cv.cv();
#endif
    }
    else
    {
        cv.setDefault();
#ifdef _DEBUG
        LOGD << "[" << cv.section_name << "]" << cv.key_name << " is set to default value: " << cv.cv();
#endif
    }
}
// config/import.cpp

void initialize(const std::string& ini_file_name)
{
    _file_name = ini_file_name;
    mINI::INIFile file = mINI::INIFile(_file_name);

    if (file.read(_is))
    {
#ifdef _DEBUG
        LOGD << "Load config from existing ini file";
#endif
        // ini-file already exists
        setToStructure<std::string>(in_dev_name);
        setToStructure<std::string>(out_dev_name);
        setToStructure<int>(to_ch);
        setToStructure<bool>(is_force_adj);
    }
#ifdef _DEBUG
    else
    {
        LOGD << "Ini file does not exists";
    }
#endif

    _is.clear();
}

void finalize() noexcept
{
    mINI::INIFile file = mINI::INIFile(_file_name);

    // TODO impl write function
    _is[in_dev_name.section_name][in_dev_name.key_name] = in_dev_name.cv();
    _is[out_dev_name.section_name][out_dev_name.key_name] = out_dev_name.cv();
    _is[to_ch.section_name][to_ch.key_name] = format("%d", to_ch.cv());
    _is[is_force_adj.section_name][is_force_adj.key_name] = is_force_adj.cv() ? "1" : "0";

    if (!file.write(_is, _use_pretty_print))
    {
#ifdef _DEBUG
        LOGD << "Failed to write config file";
#endif
    }
}

} // Config
} // CentroMidi
