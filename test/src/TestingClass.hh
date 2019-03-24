//
// Created by FyS on 3/23/19.
//

#ifndef PROJECT_TESTINGCLASS_HH
#define PROJECT_TESTINGCLASS_HH

#include <DependencyGettable.hh>
#include <DependencyNonGettable.hh>

namespace src {

    class TestingClass {
    public:
        TestingClass() {}

        void execute();

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
