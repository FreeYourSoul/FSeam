//
// Created by FyS on 3/23/19.
//

#include <iostream>
#include <assert.h>
#include "TestingClass.hh"

namespace src {

    void TestingClass::execute() {
        this->executeDependency(_depNonGettable);
        this->executeDependency(_depGettable);
    }

}
