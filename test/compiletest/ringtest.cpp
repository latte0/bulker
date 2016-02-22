#include "../../ring_buffer.h"

int main(){
  bulker::ring_buffer<int,32,true> ring;

  for(int i = 0; i<34 ; i++){
    ring.push(i);
  }

  for(int i = 0; i< 32 ; i++){
  //  std::cout << *ring.pop() << std::endl;
    auto k = *ring.pop();
  }

  return 0;
}
