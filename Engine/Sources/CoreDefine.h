#pragma once

#define TOYENGINE_DEBUG 1

#ifndef TOYENGINE_DEBUG
#define TOYENGINE_RELEASE
#endif

#include <memory>
#include <vector>
#include <map>


namespace TE {

template<typename T>
using TPtr = std::shared_ptr<T>;

template<typename T>
using TWeakPtr = std::weak_ptr<T>;

template<typename T>
using TPtrArr = std::vector<std::shared_ptr<T>>;

}