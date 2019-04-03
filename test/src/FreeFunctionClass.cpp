//
// Created by FyS on 4/3/19.
//

#include "FreeFunctionClass.hh"

int source::freeFunctionReturn() {
    std::cout << "Original " << __func__ << " called return -1 \n";
    return -1;
}

void source::freeFunctionSimple() {
    std::cout << "Original " << __func__ << " called return\n";
}

void source::freeFunctionWithArguments(int arg1, int arg2, char arg3) {
    std::cout << "Original " << __func__ << " called with arg " << arg1 << " " << arg2 << " " << arg3 << "\n";
}

int source::FreeFunctionClass::staticFunction() {
    std::cout << "Original " << __func__ << " called return -1 \n";
    return -1;
}

