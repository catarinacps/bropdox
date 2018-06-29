#pragma once

#include "util/FileData.hpp"
#include "util/Messages.hpp"

#include <memory>

/******************************************************************************
 * Helper converter functions
 */

namespace bdu {

template <typename T>
std::unique_ptr<T> convert_bytes(byte_t const* data)
{
    auto convert = std::make_unique<T>();
    std::memcpy(convert.get(), data, sizeof(T));
    return convert;
}
}