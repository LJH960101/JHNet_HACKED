#include<atomic>
class CSpinlock {
public:
	inline void Lock();
	inline bool TryLock();
	inline void Unlock();

private:
	std::atomic_flag lock = ATOMIC_FLAG_INIT;
};