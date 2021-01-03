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

	// ��ü ť�� �̾��ش�.
	std::queue<T>* CreateQueue() {
		return new std::queue<T>();
	}

	// ť�� ���� �ִ´�.
	void Enqueue(const T& data) {
		locker mt(_mutex);
		_messageQueue->push(data);
	}

	// ť�� ��ü�Ѵ�.
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