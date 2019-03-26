//
// Created by FyS on 3/23/19.
//

#ifndef PROJECT_DEPENDENCYNONGETTABLE_HH
#define PROJECT_DEPENDENCYNONGETTABLE_HH

#include <string>

namespace source {
    class DependencyNonGettable {
    public:
        void checkCalled();

        void checkSimpleInputVariable(int simple, std::string easy);

        int checkSimpleReturnValue();

        bool hasOriginalServiceBeenCalled() { return _hasOriginalBeenCalled; }

    private:
        bool _hasOriginalBeenCalled = false;
    };
}

#endif //PROJECT_DEPENDENCYNONGETTABLE_HH
