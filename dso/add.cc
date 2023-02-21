#include "interceptor.hh"


class Derived : public Base {
public:
    int get_value() override {
        return val;
    }
    int val = 3;
    ~Derived() override {
        std::cout << "Derived destructor called" << std::endl;    
    }
};

extern "C" int add(int x, int y) {
    return x + y;
} 

extern "C" Derived* gen() {
    auto pointer = new Derived;
    pointer->val = 5;
    return pointer;
}