//
// Created by FyS on 4/3/19.
//

#include <iostream>
#include "ClassWithConstructor.hh"

namespace source {

    ClassWithConstructor::~ClassWithConstructor() {
        std::cout << "Original Destructor called\n";
    }

    ClassWithConstructor::ClassWithConstructor() {
        std::cout << "Original Constructor called\n";
    }

    int ClassWithConstructor::mockedMethod() const {
        std::cout << "Original mockedMethod called\n";
        return -1;
    }

}
