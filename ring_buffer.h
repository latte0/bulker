#define _RING_BUFFER_H_

#include <type_traits>
#include <algorithm>
#include <utility>
#include <deque>
#include <stdexcept>
#include <atomic>
#include <memory>
#include <exception>
#include <iostream>
#include <stdexcept>

#include "Optional/optional.hpp"



namespace bulker{

class Impl{};

template <class T, size_t Size, class Allocator = std::allocator<T>>
class ring_buffer_impl{

public:
  using This_type = ring_buffer_impl<T,Size,Allocator>;
  using allocator_type = Allocator;


  ring_buffer_impl(): m_capacity(0) ,
  push_error(std::make_exception_ptr(std::runtime_error("ring_buffer is filled!"))),
  pop_error(std::make_exception_ptr(std::runtime_error("ring_buffer cannot pop any more"))){}

  size_t size() const noexcept { return Size; }
  size_t& capacity() const noexcept { return m_capacity; }
  bool empty() const noexcept { return m_capacity == 0 ? true : false; }


  size_t next(size_t current){
    return (current + 1) % Size;
  }

  size_t back(size_t current){
    int tmp = current -1;
    if(tmp < 0 ) current = Size-1;
    return current;
  }

public:
  size_t m_capacity;

  std::exception_ptr push_error;
  std::exception_ptr pop_error;
};

template <class T, size_t Size, bool multi_thread, class Allocator = std::allocator<T>>
class ring_buffer;

template <class T, size_t Size, class Allocator>
class ring_buffer<T,Size,true, Allocator>
  : public ring_buffer_impl<T, Size, Allocator> {
public:
  ring_buffer() : m_head(0), m_tail(0), ring_buffer_impl<T, Size, Allocator>(){}
  ring_buffer(ring_buffer&&) = default;
  ring_buffer& operator=(ring_buffer&&) = default;
  ring_buffer(const ring_buffer&) = default;
  ring_buffer& operator=(const ring_buffer&) = default;
  ~ring_buffer() = default;

  void initialize() noexcept {
    m_tail = m_head;
  }

  bool emergency_push(const T & value){
    try{
      if (ring_buffer_impl<T, Size, Allocator>::m_capacity >= ring_buffer_impl<T, Size, Allocator>::size() -1 )
        std::rethrow_exception(ring_buffer_impl<T, Size, Allocator>::push_error);
      size_t tail = m_tail.load(std::memory_order_relaxed);
      size_t back_tail = ring_buffer_impl<T, Size, Allocator>::back(tail);
      if (m_head.load(std::memory_order_release) == back_tail)
        std::rethrow_exception(ring_buffer_impl<T, Size, Allocator>::push_error);
      m_ring[back_tail] = value;
      m_tail.store(back_tail, std::memory_order_release);
      ring_buffer_impl<T, Size, Allocator>::m_capacity++;
      return true;
    }catch (std::runtime_error e){
      std::cout << e.what() << std::endl;
      return false;
    }
  }

  bool push(const T & value)
  {
    try{
      size_t head = m_head.load(std::memory_order_relaxed);
      size_t next_head = ring_buffer_impl<T, Size, Allocator>::next(head);
      if (next_head == m_tail.load(std::memory_order_release))
        std::rethrow_exception(ring_buffer_impl<T, Size, Allocator>::push_error);
      m_ring[head] = value;
      m_head.store(next_head, std::memory_order_release);
      ring_buffer_impl<T, Size, Allocator>::m_capacity++;
      return true;
    }catch (std::runtime_error e){
      std::cout << e.what() << std::endl;
      return false;
    }

  }

  bool push(T&& value){
    try{
      size_t head = m_head.load(std::memory_order_relaxed);
      size_t next_head = ring_buffer_impl<T, Size, Allocator>::next(head);
      if (next_head == m_tail.load(std::memory_order_release))
        std::rethrow_exception(ring_buffer_impl<T, Size, Allocator>::push_error);
      m_ring[head] = std::forward<T>(value);
      m_head.store(next_head, std::memory_order_release);
      ring_buffer_impl<T, Size, Allocator>::m_capacity++;
      return true;
    }catch (std::runtime_error e){
      std::cout << e.what() << std::endl;
      return false;
    }
  }

  std::optional<T> pop()
  {
    try{
      size_t tail = m_tail.load(std::memory_order_relaxed);
      if (tail == m_head.load(std::memory_order_acquire))
        std::rethrow_exception(ring_buffer_impl<T, Size, Allocator>::pop_error);

      T value = std::forward<typename std::decay<T>::type>(m_ring[tail]);
      m_tail.store(ring_buffer_impl<T, Size, Allocator>::next(tail), std::memory_order_release);
      ring_buffer_impl<T, Size, Allocator>::m_capacity--;
      return std::optional<T>(value);
    }catch (std::runtime_error e){
      std::cout << e.what() << std::endl;
      return std::optional<T>{};
    }
  }


private:

  T m_ring[Size];
  std::atomic<size_t> m_head, m_tail;
};


template <class T, size_t Size,class Allocator>
class ring_buffer<T,Size,false,Allocator> {
public:
  ring_buffer() : m_head(0), m_tail(0) {}



private:

  std::array<T,Size> m_ring;
  size_t m_head, m_tail;
};

}
