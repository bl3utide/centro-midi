﻿#pragma once

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

std::string& getText() noexcept;
Type getType() noexcept;
void setText(const Message message, const Type type);
void clearText() noexcept;

} // Annotation
} // CentroMidi
