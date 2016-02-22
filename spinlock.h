#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <atomic>

class spinlock{
private:
  using LockState = enum {Locked, Unlocked};
  std::atomic<LockState> m_state;

public:
  spinlock() : m_state(Unlocked) {}

  void lock()
  {
    while(m_state.exchange(Locked, std::memory_order_acquire) == Locked){

    }
  }

  void unlcok()
  {
    m_state.store(Unlocked, std::memory_order_release);
  }
}

#endif
