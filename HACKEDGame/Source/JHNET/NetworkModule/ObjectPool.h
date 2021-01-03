#pragma once
#include <stack>
#include "NetworkModule/NetworkInfo.h"

template<typename T>
class CObjectPool
{
public:
	CObjectPool(int size = 100) {
		_maxSize = size;

		for (int i = 0; i < _maxSize; ++i) {
			T* newObject = new T();
			_objects.push(newObject);
		}
	}

	~CObjectPool()
	{
		while (!_objects.empty()) {
			T* object = _objects.top();
			_objects.pop();
			delete object;
		}

		_maxSize = 0;
	}

	T* PopObject()
	{
		if (_objects.empty()) {
			Expand();
		}

		T* retVal = _objects.top();
		_objects.pop();
		return retVal;
	}

	void Expand() {
		for (int i = 0; i < _maxSize; ++i)
		{
			T* newObject = new T();
			_objects.push(newObject);
		}

		_maxSize += _maxSize;
	}

	void ReturnObject(T* object)
	{
		_objects.push(object);
	}

private:
	std::stack<T*> _objects;
	int _maxSize;
};

template<>
class CObjectPool<char>
{
public:
	CObjectPool(int size = 100) {
		_maxSize = size;

		for (int i = 0; i < _maxSize; ++i) {
			char* newObject = new char[PACKET_BUFSIZE];
			_objects.push(newObject);
		}
	}

	~CObjectPool()
	{
		while (!_objects.empty()) {
			char* object = _objects.top();
			_objects.pop();
			delete object;
		}

		_maxSize = 0;
	}

	char* PopObject()
	{
		if (_objects.empty()) {
			Expand();
		}

		char* retVal = _objects.top();
		_objects.pop();
		return retVal;
	}

	void Expand() {
		for (int i = 0; i < _maxSize; ++i)
		{
			char* newObject = new char[PACKET_BUFSIZE];
			_objects.push(newObject);
		}

		_maxSize += _maxSize;
	}

	void ReturnObject(char* object)
	{
		_objects.push(object);
	}

private:
	std::stack<char*> _objects;
	int _maxSize;
};