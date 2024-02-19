#pragma once

namespace CentroMidi
{
namespace Annotation
{

enum class Type : int
{
    General,
    Alert,
    _COUNT_,
};

enum class Message : int
{
    DisconnectedAlert,
    _COUNT_,
};

std::string& getAnnotation();
Type getAnnotationType();
void setAnnotation(const Message message, const Type type);
void clearAnnotation();

} // Annotation
} // CentroMidi
