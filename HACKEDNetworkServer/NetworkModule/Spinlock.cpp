#include "Spinlock.h"

void CSpinlock::Lock()
{
	while (lock.test_and_set(std::memory_order_acquire)) {

	}
}

bool CSpinlock::TryLock()
{
	return !lock.test_and_set(std::memory_order_acquire);
}

void CSpinlock::Unlock()
{
	lock.clear(std::memory_order_release);
}
