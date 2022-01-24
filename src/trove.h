// trove.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>

#define TO_STRING std::string to_string();

#define IF_VALUE(optional) if(optional.has_value())
#define IF_NO_VALUE(optional) if(!optional.has_value())