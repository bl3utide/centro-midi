#pragma once

namespace CentroMidi
{
namespace Image
{

enum class Texture : int
{
    Reload,
    _COUNT_,
};

void initialize();
void finalize() noexcept;
GLuint getTextureId(Texture tex) noexcept;

} // Image
} // CentroMidi
