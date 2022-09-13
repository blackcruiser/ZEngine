#pragma once

// Release / Debug
#define ZE_DEBUG

#ifndef ZE_DEBUG
#define ZE_RELEASE
#endif


// Platform
#if (defined _WIN64) || (defined _WIN32)
    #define ZE_PLATFORM_WINDOWS
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
        #define ZE_PLATFORM_MACOS
    #endif
#elif __ANDROID__
    #define ZE_PLATFORM_ANDROID
#elif __linux
    // linux
#elif __unix
    // Unix
#elif __posix
    // POSIX
#endif