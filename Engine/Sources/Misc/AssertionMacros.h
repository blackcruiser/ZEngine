#pragma once

#include <cassert>
#include <stdexcept>


#define CHECK(expression) \
    assert(expression)

#define CHECK_MSG(expression, format, ...) \
    if (!(expression)) \
    { \
        throw std::runtime_error(format); \
    }