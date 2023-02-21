#include <iostream>

class Base {
public:
    virtual int get_value() {
        return 1;
    }
    virtual ~Base() {
        std::cout << "Base destructor called" << std::endl;
    }
};