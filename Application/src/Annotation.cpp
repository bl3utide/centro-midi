#include "Common.hpp"
#include "Annotation.hpp"

namespace CentroMidi
{
namespace Annotation
{

// private
std::string _annotation;
Type _annotation_type;
const std::map<Message, std::string> MESSAGES {
    {Message::DisconnectedAlert, "Please set the MIDI connections"},
};

std::string& getAnnotation()
{
    return _annotation;
}

Type getAnnotationType()
{
    return _annotation_type;
}

//void setAnnotation(const std::string text, const Type type)
void setAnnotation(const Message message, const Type type)
{
    _annotation = MESSAGES.at(message);
    _annotation_type = type;
}

void clearAnnotation()
{
    _annotation.clear();
}

} // Annotation
} // CentroMidi