#pragma once
#ifndef __H_XUTIL_XSTATE_HPP__
#define __H_XUTIL_XSTATE_HPP__

#include "XUtil.hpp"
#include <bitset>
#include <initializer_list>

namespace XUtil {

template<size_t _Bits>
class XState
{
protected:
	std::bitset<_Bits> states_;
public:
	XState() {}

	inline void ResetAllState() {
		ResetInState();
	}
	inline void ResetState() { states_.reset(); }
	inline void SetState(size_t index, bool set = true) { states_[index] = set; }
	inline void SetState(std::initializer_list<size_t> list, bool set = true) { 
		for (auto it = list.begin(); it != list.end(); ++it) {
            states_[*it] = set; 
        }
	}
	inline bool IsStateful(size_t index) { return states_[index]; }
	inline bool IsStateless(size_t index) { return states_[index]; }
	inline bool IsAllStatefull() {
		return states_.all();
	}
	inline bool IsAnyStateful() {
		return states_.any();
	}
	inline bool IsAllStatefull(std::initializer_list<size_t> list) {
        for (auto it = list.begin(); it != list.end(); ++it) {
            if(!states_[*it]) {
				return false;
			}
        }
		return true;
    }
	inline bool IsAnyStateful(std::initializer_list<size_t> list) {
        for (auto it = list.begin(); it != list.end(); ++it) {
            if(states_[*it]) {
				return true;
			}
        }
		return false;
    }
};

template<size_t _Bits>
class XStateEx : public XState<_Bits>
{
	typedef XState<_Bits> Base;
protected:
	std::bitset<_Bits> instates_;
public:
	XStateEx() {}

	inline void ResetAllState() {
		Base::ResetState();
		ResetInState();
	}
	inline void SetState(size_t index, bool set = true) { 
		states_[index] = set; 
		if(set)  { 
			instates_[index] = false;
		}
	}
	inline void SetState(std::initializer_list<size_t> list, bool set = true) { 
		for (auto it = list.begin(); it != list.end(); ++it) {
			states_[*it] = set; 
			if(set) { 
				instates_[*it] = false;
			}
        }
	}

	inline void ResetInState() { instates_.reset(); }
	inline void SetInState(size_t index, bool set = true) { 
		instates_[index] = set; 
		if(set) {
			states_[index] = false;
		}
	}
	inline void SetInState(std::initializer_list<size_t> list, bool set = true) { 
		for (auto it = list.begin(); it != list.end(); ++it) {
            instates_[*it] = set; 
			if(set) {
				states_[*it] = false;
			}
        }
	}
	inline bool IsInStateful(size_t index) { return instates_[index]; }
	inline bool IsInStateless(size_t index) { return instates_[index]; }
	inline bool IsAllInStatefull() {
		return instates_.all();
	}
	inline bool IsAnyInStateful() {
		return instates_.any();
	}
	inline bool IsAllInStatefull(std::initializer_list<size_t> list) {
        for (auto it = list.begin(); it != list.end(); ++it) {
            if(!instates_[*it]) {
				return false;
			}
        }
		return true;
    }
	inline bool IsAnyInStateful(std::initializer_list<size_t> list) {
        for (auto it = list.begin(); it != list.end(); ++it) {
            if(instates_[*it]) {
				return true;
			}
        }
		return false;
    }
};

}

#endif //__H_XUTIL_XSTATE_HPP__
