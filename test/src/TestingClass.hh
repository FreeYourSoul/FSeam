//
// Created by FyS on 3/23/19.
//

#ifndef PROJECT_TESTINGCLASS_HH
#define PROJECT_TESTINGCLASS_HH

#include <ClassWithConstructor.hh>
#include <DependencyGettable.hh>
#include <DependencyNonGettable.hh>

namespace source {

    class TestingClass {
    public:
        TestingClass() {}

        void execute();

        int instantiateClassWithConstructorAndRetFromMockedMethd() const {
            ClassWithConstructor c;
            return c.mockedMethod();
        }

        int checkSimpleReturnValueNonGettable() {
            return _depNonGettable.checkSimpleReturnValue();
        }

        DependencyGettable &getDepGettable() { return _depGettable; }

    private:
        template <typename Dependency>
        void executeDependency(Dependency &dependency) {
            dependency.checkCalled();
            dependency.checkSimpleInputVariable(42, "4242");
            dependency.checkSimpleReturnValue();
        }

    private:
        DependencyGettable _depGettable;
        DependencyNonGettable _depNonGettable;
    };

}

#endif //PROJECT_TESTINGCLASS_HH
