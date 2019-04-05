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
    std::cout << "Original " << __func__ << " called returning 888\n";
    _hasOriginalBeenCalled = true;
    return 888;
}

void source::DependencyGettable::checkCustomStructInputVariablePtr(source::StructTest *testStr) {
    std::cout << "Original " << __func__ << " called with " << testStr->testInt << " " << testStr->testShort << " " << testStr->testStr << " \n";
    _hasOriginalBeenCalled = true;
}

source::StructTest *source::DependencyGettable::checkCustomStructReturnValuePtr() {
    std::cout << "Original " << __func__ << " called returning nullptr\n";
    _hasOriginalBeenCalled = true;
    return nullptr;
}

source::StructTest source::DependencyGettable::checkCustomStructReturnValue()  {
    std::cout << "Original " << __func__ << " called returning {888,000,'ttt'}\n";
    _hasOriginalBeenCalled = true;
    return source::StructTest {888, 999, "tttt"};
}

void source::DependencyGettable::checkCustomStructInputVariableRef(const source::StructTest &testStr) {
    std::cout << "Original " << __func__ << " called\n";
    _hasOriginalBeenCalled = true;
}

void source::DependencyGettable::checkCustomStructInputVariable(source::StructTest testStr) {
    std::cout << "Original " << __func__ << " called with " << testStr.testInt << " " << testStr.testShort << " " << testStr.testStr << " \n";
    _hasOriginalBeenCalled = true;
}
