//
// Created by FyS on 3/23/19.
//

#include <iostream>
#include "DependencyGettable.hh"

void source::DependencyGettable::checkCalled() {
    std::cout << "Original " << __func__ << " called\n";
}

void source::DependencyGettable::checkSimpleInputVariable(int simple, std::string easy) {
    std::cout << "Original " << __func__ << " called with args " << simple << " and " << easy << " \n";
}

int source::DependencyGettable::checkSimpleReturnValue() {
    std::cout << "Original " << __func__ << " called returning 42\n";
    return 42;
}

