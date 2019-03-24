//
// Created by FyS on 3/23/19.
//

#include <iostream>
#include "DependencyNonGettable.hh"

void source::DependencyNonGettable::checkCalled() {
    std::cout << "Original " << __func__ << " called\n";
}

void source::DependencyNonGettable::checkSimpleInputVariable(int simple, std::string easy) {
    std::cout << "Original " << __func__ << " called with args " << simple << " and " << easy << " \n";
}

int source::DependencyNonGettable::checkSimpleReturnValue() {
    std::cout << "Original " << __func__ << " called returning 42\n";
    return 42;
}

