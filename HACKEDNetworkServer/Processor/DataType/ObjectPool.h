#pragma once
#include <stack>
#include <mutex>
#include <concurrent_queue.h>
#include "JHNETGameServer.h"

#define DEBUG_AVAILABLE 0

#if !DEBUG_AVAILABLE
template<typename T>
class CObjectPool
{
public:
	// 사이즈만큼 오브젝트를 만든다.
	CObjectPool(int size = 100) {
		_maxSize = size;

		for (int i = 0; i < _maxSize; ++i) {
			T* newObject = new T();
			_queue.push(newObject);
		}
	}

	// 오브젝트를 비운다.
	~CObjectPool()
	{
		T* obj;
		while (_queue.try_pop(obj)) {
			delete obj;
		}
	}

	// 오브젝트를 꺼낸다.
	T* PopObject()
	{
		T* obj = nullptr;
		bool isSuccess = _queue.try_pop(obj);
		if (isSuccess) return obj;
		else {
			if (_queue.empty()) {
				Expand();
				return PopObject();
			}
			else return PopObject();
		}
	}

	// 현재 크기만큼 새로운 오브젝트를 넣어 확장한다.
	void Expand() {
		int maxSize = _maxSize;
		// 늘린 크기만큼 새로운 오브젝트를 넣어준다.
		for (int i = 0; i < maxSize; ++i)
		{
			T* newObject = new T();
			if(newObject) _queue.push(newObject);
		}

		// 최대 크기를 늘린다.
		_maxSize += maxSize;
	}

	// 오브젝트를 수거한다.
	void ReturnObject(T* object)
	{
		_queue.push(object);
	}

	void Print() {
		printf("maxSize : %d\n", _maxSize);
	}

	void SetWithMutex(bool isOn) {

	}

private:
	Concurrency::concurrent_queue<T*> _queue;
	int _maxSize = 0;
};

template<>
class CObjectPool<char>
{
public:
	// 사이즈만큼 오브젝트를 만든다.
	CObjectPool(int size = 10000) {
		_maxSize = size;

		for (int i = 0; i < _maxSize; ++i) {
			char* newObject = new char[PACKET_BUFSIZE];
			if(newObject) _queue.push(newObject);
		}
	}

	// 오브젝트를 비운다.
	~CObjectPool()
	{
		char* obj;
		while (_queue.try_pop(obj)) {
			delete[] obj;
		}
	}

	// 오브젝트를 꺼낸다.
	char* PopObject()
	{
		char* obj = nullptr;
		bool isSuccess = _queue.try_pop(obj);
		if (isSuccess) return obj;
		else {
			if (_queue.empty()) {
				Expand();
				return PopObject();
			}
			else return PopObject();
		}
	}

	// 현재 크기만큼 새로운 오브젝트를 넣어 확장한다.
	void Expand() {
		// 늘린 크기만큼 새로운 오브젝트를 넣어준다.
		for (int i = 0; i < _maxSize; ++i)
		{
			char* newObject = new char[PACKET_BUFSIZE];
			_queue.push(newObject);
		}

		// 최대 크기를 늘린다.
		_maxSize += _maxSize;
	}

	// 오브젝트를 수거한다.
	void ReturnObject(char* object)
	{
		_queue.push(object);
	}

	void Print() {
		printf("maxSize : %d\n", _maxSize);
	}

	void SetWithMutex(bool isOn) {

	}

private:
	Concurrency::concurrent_queue<char*> _queue;
	int _maxSize = 0;
};
#else
template<typename T>
class CObjectPool
{
public:
	// 사이즈만큼 오브젝트를 만든다.
	CObjectPool(int size = 100) {
		_maxSize = size;

		for (int i = 0; i < _maxSize; ++i) {
			T* newObject = new T();
			_objects.push(newObject);
		}
	}

	// 오브젝트를 비운다.
	~CObjectPool()
	{
		if (_withMutex) _mt.lock();

		while (!_objects.empty()) {
			T* object = _objects.top();
			_objects.pop();
			delete object;
		}

		_maxSize = 0;

		if (_withMutex) _mt.unlock();
	}

	// 오브젝트를 꺼낸다.
	T* PopObject()
	{
		if (_withMutex) _mt.lock();

		// 오브젝트가 없다면 확장한다.
		if (_objects.empty()) {
			Expand();
		}
			
		T* retVal = _objects.top();
		_objects.pop();

		if (_withMutex) _mt.unlock();
		return retVal;
	}

	// 현재 크기만큼 새로운 오브젝트를 넣어 확장한다.
	void Expand() {
		if (_withMutex) _mt.lock();

		// 늘린 크기만큼 새로운 오브젝트를 넣어준다.
		for (int i = 0; i < _maxSize; ++i)
		{
			T* newObject = new T();
			_objects.push(newObject);
		}

		// 최대 크기를 늘린다.
		_maxSize += _maxSize;

		if (_withMutex) _mt.unlock();
	}

	// 오브젝트를 수거한다.
	void ReturnObject(T* object)
	{
		if (_withMutex) _mt.lock();

		_objects.push(object);

		if (_withMutex) _mt.unlock();
	}

	void Print() {
		printf("%zd(%d)\n", _objects.size(), _maxSize);
	}

	void SetWithMutex(bool isOn) {
		_withMutex = isOn;
	}

private:
	std::stack<T*> _objects;
	int _maxSize; // 최대 배열 크기
	bool _withMutex = false;
	std::recursive_mutex _mt;
};

template<>
class CObjectPool<char>
{
public:
	// 사이즈만큼 오브젝트를 만든다.
	CObjectPool(int size = 100) {
		_maxSize = size;

		for (int i = 0; i < _maxSize; ++i) {
			char* newObject = new char[PACKET_BUFSIZE];
			_objects.push(newObject);
		}
	}

	// 오브젝트를 비운다.
	~CObjectPool()
	{
		if (_withMutex) _mt.lock();

		while (!_objects.empty()) {
			char* object = _objects.top();
			_objects.pop();
			delete[] object;
		}

		_maxSize = 0;

		if (_withMutex) _mt.unlock();
	}

	// 오브젝트를 꺼낸다.
	char* PopObject()
	{
		if (_withMutex) _mt.lock();

		// 오브젝트가 없다면 확장한다.
		if (_objects.empty()) {
			Expand();
		}

		char* retVal = _objects.top();
		_objects.pop();

		if (_withMutex) _mt.unlock();
		return retVal;
	}

	// 현재 크기만큼 새로운 오브젝트를 넣어 확장한다.
	void Expand() {
		if (_withMutex) _mt.lock();

		// 늘린 크기만큼 새로운 오브젝트를 넣어준다.
		for (int i = 0; i < _maxSize; ++i)
		{
			char* newObject = new char[PACKET_BUFSIZE];
			_objects.push(newObject);
		}

		// 최대 크기를 늘린다.
		_maxSize += _maxSize;

		if (_withMutex) _mt.unlock();
	}

	// 오브젝트를 수거한다.
	void ReturnObject(char* object)
	{
		if (_withMutex) _mt.lock();

		_objects.push(object);

		if (_withMutex) _mt.unlock();
	}

	void Print() {
		printf("%zd(%d)\n", _objects.size(), _maxSize);
	}

	void SetWithMutex(bool isOn) {
		_withMutex = isOn;
	}

private:
	std::stack<char*> _objects;
	int _maxSize; // 최대 배열 크기
	bool _withMutex = false;
	std::recursive_mutex _mt;
};
#endif