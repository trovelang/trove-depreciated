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
		m_index = 0;
	}
	void enter() {
		m_index++;
		m_values.push_back(std::map<std::string, T>());
	}
	void exit() {
		m_index--;
		m_values.pop_back();
	}
	void place(std::string key, T value) {
		m_values[m_index][key] = value;
	}
	u32 get_level() {
		return m_index;
	}
	u1 contains(std::string key) {
		for (s32 i = m_index; i >= 0; i--) {
			if (m_values[i].contains(key)) {
				return true;
			}
		}
		return false;
	}
	u32 scope_size() {
		return m_values[m_index].size();
	}
	std::optional<u32> lookup_level(std::string key) {
		for (s32 i = m_index; i >= 0; i--) {
			if (m_values[i].contains(key)) {
				return i;
			}
		}
		return std::optional<u32>();
	}
	std::optional<T> lookup(std::string key) {
		for (s32 i = m_index; i >= 0; i--) {
			if (m_values[i].contains(key)) {
				return m_values[i][key];
			}
		}
		return std::optional<T>();
	}
private:
	u32 m_index = 0;
	std::vector<std::map<std::string, T>> m_values;
};

}