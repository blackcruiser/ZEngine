#include "StackTrace.h"

#include <iostream>


namespace ZE {

void DumpStackTrace()
{
    std::stacktrace current = std::stacktrace::current();
    std::cout << std::to_string(current) << std::endl;
}

}