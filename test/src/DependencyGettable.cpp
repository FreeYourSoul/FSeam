//
// Created by FyS on 3/23/19.
//

#include <iostream>
#include "DependencyGettable.hh"

void source::DependencyGettable::checkCalled() {
    std::cout << "Original " << __func__ << " called\n";
    _hasOriginalBeenCalled = true;
}

void source::DependencyGettable::checkSimpleInputVariable(int simple, std::string easy) {
    std::cout << "Original " << __func__ << " called with args " << simple << " and " << easy << " \n";
    _hasOriginalBeenCalled = true;
}

int source::DependencyGettable::checkSimpleReturnValue() {
    std::cout << "Original " << __func__ << " called returning 42\n";
    _hasOriginalBeenCalled = true;
    return 888;
}

StructTest checkCustomStructReturnValue() const {
    std::cout << "Original " << __func__ << " called returning 42\n";
    _hasOriginalBeenCalled = true;
    return StructTest {888, 999, "tttt"};
}

void checkCustomStructInputVariableRef(const StructTest &testStr) {
    std::cout << "Original " << __func__ << " called returning 42\n";
    _hasOriginalBeenCalled = true;
}

void checkCustomStructInputVariableRValueRef(StructTest &&testStr) {
    std::cout << "Original " << __func__ << " called returning 42\n";
    _hasOriginalBeenCalled = true;
}

void checkCustomStructInputVariable(StructTest testStr) {
    std::cout << "Original " << __func__ << " called returning 42\n";
    _hasOriginalBeenCalled = true;
}
