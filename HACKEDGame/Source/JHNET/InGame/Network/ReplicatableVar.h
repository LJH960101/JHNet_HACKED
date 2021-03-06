// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "JHNET.h"
#include "Component/NetworkBaseCP.h"

#define MAX_ERROR_IGNORE 3

/**
 * ReplicatableVar
 * If you change values, then every client can get new value.
 */

template<typename T>
class JHNET_API CReplicatableVar
{
public:
	CReplicatableVar() : _netBaseCP(nullptr){}
	~CReplicatableVar() {}

	void Init(UNetworkBaseCP** netBaseCP, const FString& variableName) {
		_netBaseCP = netBaseCP;
		if (_netBaseCP != nullptr) _BindHandle(variableName);
	}
	void SetReliable(const bool& bIsReliable) { _bReliable = bIsReliable; }

	// 네트워크 통신없이 강제로 바꾼다.
	void ForceChange(const T& rhs) {
		_value = rhs;
	}

	// Operators
	CReplicatableVar<T>& operator=(const CReplicatableVar<T>& rhs) {
		ChangeProcess(rhs._value);
		return *this;
	}
	CReplicatableVar<T>& operator=(const T& rhs) {
		ChangeProcess(rhs);
		return *this;
	}
	bool operator!() const {
		return !(this->_value);
	}
	T operator*() {
		return _value;
	}
	operator T() { return _value; }


	// Logical operators

	// Logical operator CReplicatableVar<T>
	bool operator==(const CReplicatableVar<T>& rhs) const {
		return this->_value == rhs._value;
	}
	bool operator!=(const CReplicatableVar<T>& rhs) const {
		return this->_value != rhs._value;
	}
	bool operator>=(const CReplicatableVar<T>& rhs) const {
		return this->_value >= rhs._value;
	}

	bool operator<=(const CReplicatableVar<T>& rhs) const {
		return this->_value <= rhs._value;
	}
	bool operator<(const CReplicatableVar<T>& rhs) const {
		return this->_value < rhs._value;
	}
	bool operator>(const CReplicatableVar<T>& rhs) const {
		return this->_value > rhs._value;
	}

	// Logical operator T
	bool operator==(const T& rhs) const {
		return this->_value == rhs;
	}
	bool operator!=(const T& rhs) const {
		return this->_value != rhs;
	}
	bool operator>=(const T& rhs) const {
		return this->_value >= rhs;
	}
	bool operator<=(const T& rhs) const {
		return this->_value <= rhs;
	}
	bool operator<(const T& rhs) const {
		return this->_value < rhs;
	}
	bool operator>(const T& rhs) const {
		return this->_value > rhs;
	}

	FString GetHandle() { return _handle; }

private:
	inline bool IsChanged(const T& newValue) {
		if (newValue != _value) return true;
		else return false;
	}
	void ChangeProcess(const T& newValue) {
		if (_netBaseCP == nullptr) {
			if (_errorCounter >= MAX_ERROR_IGNORE) {
				JHNET_LOG(Warning, "ReplicatableVar :: Dosen't have netbase. (_netBaseCP is null)");
			}
			else {
				++_errorCounter;
			}

			_value = newValue;
			return;
		}
		if (*_netBaseCP == nullptr) {
			if (_errorCounter >= MAX_ERROR_IGNORE) {
				JHNET_LOG(Warning, "ReplicatableVar :: Dosen't have netbase's point. (*_netBaseCP is null)");
			}
			else {
				++_errorCounter;
			}

			_value = newValue;
			return;
		}
		if (IsChanged(newValue)) {
			// Something is change.
			if (_handle.IsEmpty() || !(*_netBaseCP)->JHNET_CHECKAlreadyUseSyncVar(_handle)) {
				if (_errorCounter >= MAX_ERROR_IGNORE) {
					JHNET_LOG(Warning, "ReplicatableVar :: Not binding.");
				}
				else {
					++_errorCounter;
				}

				_value = newValue;
				return;
			}

			// SyncVar sync proc...
			if (!_handle.IsEmpty()) {
				(*_netBaseCP)->SetSyncVar(_handle, sizeof(newValue), (char*)& newValue, _bReliable);
			}
		}
	}
	bool _BindHandle(const FString& variableName) {
		_handle = variableName;
		if (!(
			(*_netBaseCP)->BindSyncVar(_handle, sizeof(_value), (void*)& _value))
			) {
			_handle.Empty();
			JHNET_LOG(Error, "ReplicatableVar :: Wrong handle... This change will not process.");
			return false;
		}
		return true;
	}
	
	// MAX_ERROR_IGNORE번이상 값을 강제로 바꾸려고 하면 로그 출력을 위한 카운터
	uint8 _errorCounter = 0;

	FString _handle;
	UNetworkBaseCP** _netBaseCP =  nullptr;
	T _value;
	bool _bReliable = true;
};

template class __declspec(dllexport) CReplicatableVar<int32>;
template class __declspec(dllexport) CReplicatableVar<float>;
template class __declspec(dllexport) CReplicatableVar<bool>;