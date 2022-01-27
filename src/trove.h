#pragma once

#include <iostream>

#define TO_STRING std::string to_string();

#define UNREACHABLE(msg) assert(false)

#define IF_VALUE(optional) if(optional.has_value())
#define IF_NO_VALUE(optional) if(!optional.has_value())

u32 num_lines(std::string & source);