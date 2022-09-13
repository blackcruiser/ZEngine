#pragma once


// Release / Debug
#define TOYENGINE_DEBUG

#ifndef TOYENGINE_DEBUG
#define TOYENGINE_RELEASE
#endif

// Platform
#if (defined _WIN64) || (defined _WIN32)
    #define TOYENGINE_WINDOWS
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE && TARGET_OS_SIMULATOR
        // define something for simulator
        // (although, checking for TARGET_OS_IPHONE should not be required).
    #elif TARGET_OS_IPHONE && TARGET_OS_MACCATALYST
        // define something for Mac's Catalyst
    #elif TARGET_OS_IPHONE
        // define something for iphone  
    #else
        #define TOYENGINE_MACOS
    #endif
#elif __ANDROID__
    #define TOYENGINE_ANDROID
#elif __linux
    // linux
#elif __unix
    // Unix
#elif __posix
    // POSIX
#endif


#include <memory>
#include <vector>
#include <map>
#include <unordered_map>


namespace TE {

template<typename T>
using TPtr = std::shared_ptr<T>;

template<typename T>
using TWeakPtr = std::weak_ptr<T>;

template<typename T>
using TPtrArr = std::vector<std::shared_ptr<T>>;

template<typename Key, typename Value>
using TPtrMap = std::map<Key, std::shared_ptr<Value>>;

template<typename Key, typename Value>
using TPtrUnorderedMap = std::unordered_map<Key, std::shared_ptr<Value>>;

}