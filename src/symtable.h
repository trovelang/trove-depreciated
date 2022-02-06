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

	SymTable& merge(SymTable& other){
		// loop over the other values for each index and put them in ours
		for(u32 i=0;i<other.m_values.size();i++){
			if(!(m_values.size()>i)){
				m_values.push_back({});
			}
			m_values[i].insert(other.m_values[i].begin(), other.m_values[i].end());
		}
		return *this;
	}

	// merge 2 symbol tables
	SymTable& merge(SymTable& other, u32 scope){

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
	
	u1 has_key(std::string key) {
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

	std::optional<u32> lookup_level(std::string& key) {
		for (s32 i = m_index; i >= 0; i--) {
			if (m_values[i].contains(key)) {
				return i;
			}
		}
		return std::optional<u32>();
	}

	std::optional<T> lookup(std::string& key) {
		for (s32 i = m_index; i >= 0; i--) {
			if (m_values[i].contains(key)) {
				return m_values[i][key];
			}
		}
		return std::optional<T>();
	}

	std::optional<T> lookup_only_level(std::string& key, u32 level){
		if(m_values[level].contains(key)){
			return m_values[level][key];
		}
		return std::optional<T>();
	}

	u32& level(){
		return m_index;
	}

private:
	u32 m_index = 0;
	std::vector<std::map<std::string, T>> m_values;
};

}