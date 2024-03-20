#pragma once
#include "config/section.hpp"

namespace CentroMidi
{
namespace Config
{

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
    Cv(const Section section, const Key key, const T& v_min, const T& v_max, const T& v_def)
        : section_name(SECTIONS[static_cast<int>(section)]), key_name(KEYS[static_cast<int>(key)]), _min(v_min), _max(v_max), _def(v_def)
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
            throw std::exception(format("the internal config value is out of range (%s::%s)", section_name.c_str(), key_name.c_str()).c_str());
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
            throw std::exception(format("the internal config value is out of range (%s::%s)", section_name.c_str(), key_name.c_str()).c_str());
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

} // Config
} // CentroMidi
