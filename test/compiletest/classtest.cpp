#include <iostream>

class A{
public:
	int a = 3;
	auto getA(){
std::cout << "hello A" << std::endl;
return a;}
};

class B:public A{};

class C: public B{
public:
	int a = 2;
	auto getA(){
std::cout << "hello C" << std::endl;
return A::getA(); }
};

int main(void){

	C c;
	std::cout << c.a << std::endl;
	std::cout << c.getA() << std::endl;

	return 0;
}
