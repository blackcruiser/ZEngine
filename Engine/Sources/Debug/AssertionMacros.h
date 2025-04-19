#pragma once

#include <cassert>
#include <stdexcept>


#define ZE_CHECK(expression) \
    assert(expression)

#define ZE_CHECK_MSG(expression, format, ...) \
    if (!(expression)) \
    { \
        throw std::runtime_error(format); \
    }