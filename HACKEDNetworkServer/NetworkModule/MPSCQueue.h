#pragma once
#include <queue>
#include <atomic>
#include <mutex>

template<typename T>
class CMPSCQueue
{
public:
	CMPSCQueue()
	{
	}

	~CMPSCQueue()
	{
	}

	void Enqueue(const T& input)
	{
		std::lock_guard<std::recursive_mutex> locker(lock);
		_queue.push(input);
	}

	bool Dequeue(T& output)
	{
		std::lock_guard<std::recursive_mutex> locker(lock);
		if (_queue.empty()) {
			return false;
		}
		output = _queue.front();
		_queue.pop();
		return true;
	}


private:
	std::queue<T> _queue;
	std::recursive_mutex lock;
};
/*template<typename T>
class CMPSCQueue
{
public:
	CMPSCQueue() :
		_head(new FNode()),
		_tail(_head.load(std::memory_order_relaxed))
	{
		FNode* front = _head.load(std::memory_order_relaxed);
		front->next.store(NULL, std::memory_order_relaxed);
	}

	~CMPSCQueue()
	{
		T output;
		while (this->Dequeue(output)) {}
		FNode* front = _head.load(std::memory_order_relaxed);
		delete front;
	}

	void Enqueue(const T& input)
	{
		FNode* node = new FNode();
		node->node = input;
		node->next.store(NULL, std::memory_order_relaxed);

		FNode* prev_head = _head.exchange(node, std::memory_order_acq_rel);
		prev_head->next.store(node, std::memory_order_release);
	}

	bool Dequeue(T& output)
	{
		FNode* tail = _tail.load(std::memory_order_acquire);
		FNode* next = tail->next.load(std::memory_order_acquire);

		if (next == NULL) {
			return false;
		}

		output = next->node;
		_tail.store(next, std::memory_order_release);
		delete tail;

		return true;
	}


private:
	struct FNode
	{
		T node;
		std::atomic<FNode*> next;
	};

	std::atomic<FNode*> _head;
	std::atomic<FNode*> _tail;
};*/