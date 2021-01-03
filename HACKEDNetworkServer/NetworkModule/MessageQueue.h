#pragma once
#include <queue>
#include <mutex>

typedef std::lock_guard<std::mutex> locker;

template<typename T>
class CMessageQueue {
public:
	typedef std::queue<T>* QueueType;

	CMessageQueue() {
		_messageQueue = new std::queue<T>();
	}
	~CMessageQueue() {
		delete _messageQueue;
	}

	// 교체 큐를 뽑아준다.
	std::queue<T>* CreateQueue() {
		return new std::queue<T>();
	}

	// 큐에 집어 넣는다.
	void Enqueue(const T& data) {
		locker mt(_mutex);
		_messageQueue->push(data);
	}

	// 큐를 교체한다.
	void UpdateQueue(std::queue<T>*& queue) {
		locker mt(_mutex);
		std::queue<T>* currentQueue = _messageQueue;
		_messageQueue = queue;
		queue = currentQueue;
	}

private:
	std::queue<T>* _messageQueue;
	std::mutex _mutex;
};