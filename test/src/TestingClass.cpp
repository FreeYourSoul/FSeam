//
// Created by FyS on 3/23/19.
//

#include <iostream>
#include "TestingClass.hh"

namespace source {

    void TestingClass::execute() {
        this->executeDependency(_depNonGettable);
        this->executeDependency(_depGettable);
    }

}
