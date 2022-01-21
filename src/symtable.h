#pragma once

#include <string>
#include <vector>
#include <map>

namespace trove{

template<typename T>
class SymTable {
public:
	SymTable() {
		enter();
		index = 0;
	}
	void enter() {
		index++;
		values.push_back(std::map<std::string, T>());
	}
	void exit() {
		index--;
		values.pop_back();
	}
	void place(std::string key, T value) {
		values[index][key] = value;
	}
	u32 get_level() {
		return index;
	}
	u1 contains(std::string key) {
		for (s32 i = index; i >= 0; i--) {
			if (values[i].contains(key)) {
				return true;
			}
		}
		return false;
	}
	u32 scope_size() {
		return values[index].size();
	}
	std::optional<u32> lookup_level(std::string key) {
		for (s32 i = index; i >= 0; i--) {
			if (values[i].contains(key)) {
				return i;
			}
		}
		return std::optional<u32>();
	}
	std::optional<T> lookup(std::string key) {
		for (s32 i = index; i >= 0; i--) {
			if (values[i].contains(key)) {
				return values[i][key];
			}
		}
		return std::optional<T>();
	}
private:
	u32 index = 0;
	std::vector<std::map<std::string, T>> values;
};

}