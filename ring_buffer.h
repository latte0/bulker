#define _RING_BUFFER_H_

#include <type_traits>
#include <algorithm>
#include <utility>
#include <deque>
#include <stdexcept>

namespace bulker{
template <class T, class Allocator = std::allocator<T>>
class ring_buffer {
  using This_type = ring_buffer<T,Allocator>;
  using allocator_type = Allocator;
};
}
