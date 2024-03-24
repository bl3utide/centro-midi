#pragma once
#include "config/section.hpp"

namespace CentroMidi
{
namespace Config
{

/*
    Configuration Section-Key-Value Class

    compatible types(expression in .ini)
    - bool          ("1" or "0")
    - int           ("1", "-10")
    - std::string   (same)
*/
class Cv
{
public:
    enum class Type : int
    {
        String,
        Int,
        Bool
    };

    // constructor for std::string
    Cv(const Section section, const Key key, const std::string& v)
        : section_name(SECTION_NAMES[static_cast<int>(section)]), key_name(KEY_NAMES[static_cast<int>(key)]),
        _min(0), _max(0), _def(0), _type(Type::String)
    {
        _v = v;
    }

    // constructor for int
    Cv(const Section section, const Key key, const int v_min, const int v_max, const int v_def)
        : section_name(SECTION_NAMES[static_cast<int>(section)]), key_name(KEY_NAMES[static_cast<int>(key)]),
        _min(v_min), _max(v_max), _def(v_def), _type(Type::Int)
    {
        if (_min > _max) throw std::exception("Max-value is less than min-value");

        if (_def < _min) throw std::exception("Default-value is less than min-value");
        if (_def > _max) throw std::exception("Default-value is larger than max-value");

        _v = std::to_string(v_def);
    }

    // constructor for bool
    Cv(const Section section, const Key key, const bool v)
        : section_name(SECTION_NAMES[static_cast<int>(section)]), key_name(KEY_NAMES[static_cast<int>(key)]),
        _min(0), _max(0), _def(0), _type(Type::Bool)
    {
        _v = v ? "1" : "0";
    }

    Cv() = delete;

    void set(const std::string& v)
    {
        if (_type == Type::Int)
        {
            if (isWithinRange(std::atoi(v.c_str())))
            {
                _v = v;
            }
            else
            {
                throw std::exception(StringUtil::format("the internal config value is out of range (%s::%s)", section_name.c_str(), key_name.c_str()).c_str());
            }
        }
        else if (_type == Type::Bool)
        {
            _v = v == "1" ? v : "0";
        }
        else
        {   // Type::String
            _v = v;
        }
    }

    const std::string& cv() const noexcept
    {
        return _v;
    }

    const Type& type() const noexcept
    {
        return _type;
    }

    int max() const noexcept { return _max; }
    int min() const noexcept { return _min; }

    // works only if _type == Type::Int
    void setDefault() noexcept {
        if (_type == Type::Int)
        {
            _v = std::to_string(_def);
        }
    }

    std::string section_name;
    std::string key_name;

private:
    Type _type;
    std::string _v;
    int _min, _max, _def;   // use if _type == Type::Int

    bool isWithinRange(const int v) const noexcept
    {
        return _min <= v && v <= _max;
    };
};

} // Config
} // CentroMidi
