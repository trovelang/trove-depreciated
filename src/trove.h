#pragma once

#include <iostream>
#include <string>
#include <types.h>
#include <optional>

#define TO_STRING std::string to_string();

#define UNREACHABLE(msg) assert(false)

#define IF_VALUE(optional) if(optional.has_value())
#define IF_NO_VALUE(optional) if(!optional.has_value())

inline std::string working_dir_from_entry(std::string entry_point);
inline std::string time_to_str(long long microseconds);
inline std::string load_file(std::string& file);
std::optional<std::string> load_file_with_working_dir(std::string working_dir, std::string& file);
inline u32 num_lines(std::string& source);
inline s32 compile(std::string& source_name);
inline s32 output_tokens(std::string& source_name);
inline s32 output_ast(std::string& source_name);
inline void help();
inline s32 args_parser(int argc, char** argv);