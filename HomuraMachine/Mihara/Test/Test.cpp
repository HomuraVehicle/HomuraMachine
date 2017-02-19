#include<vector>
#include<local_allocator.hpp>
#include<XC32/clock.hpp>
#include<XC32/interrupt.hpp>
struct cTest{
    int x;
};

int main(){
    std::vector<cTest,xc::local_allocator> vec;
    //std::vector<int,xc::local_allocator> vec;
	return 0;
}