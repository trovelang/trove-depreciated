#pragma once

#include <iostream>
#include <string>
#include <types.h>

#define TO_STRING std::string to_string();

#define UNREACHABLE(msg) assert(false)

#define IF_VALUE(optional) if(optional.has_value())
#define IF_NO_VALUE(optional) if(!optional.has_value())

std::string load_file(const char* file);
u32 num_lines(std::string& source);
s32 compile(std::string source);
s32 output_tokens(std::string source);
s32 output_ast(std::string source);