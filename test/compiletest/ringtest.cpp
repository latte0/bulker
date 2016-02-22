#include "../../ring_buffer.h"

int main(){
  bulker::ring_buffer<int,34,true> ring;

  for(int i = 0; i<30 ; i++){
    ring.push(i);
  }

  ring.emergency_push(100);

  for(int i = 0; i< 30 ; i++){

    //std::cout << *ring.pop() << std::endl;
    auto k = ring.pop();
    if(k) std::cout << *k << std::endl;
  }

  return 0;
}
