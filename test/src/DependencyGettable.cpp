//
// Created by FyS on 3/23/19.
//

#include <iostream>
#include "DependencyGettable.hh"

namespace src {

    void DependencyGettable::checkCalled() {
        std::cout << "Original " << __func__ << " called\n";
    }

    void DependencyGettable::checkSimpleInputVariable(int simple, std::string easy) {
        std::cout << "Original " << __func__ << " called with args " << simple << " and " << easy << " \n";
    }

    int DependencyGettable::checkSimpleReturnValue() {
        std::cout << "Original " << __func__ << " called returning 42\n";
        return 42;
    }

}
