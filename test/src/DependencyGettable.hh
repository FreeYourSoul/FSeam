//
// Created by FyS on 3/23/19.
//

#ifndef PROJECT_DEPEDENCYGETTABLE_HH
#define PROJECT_DEPEDENCYGETTABLE_HH

#include <string>

namespace src {
    class DependencyGettable {
    public:
        void checkCalled();

        void checkSimpleInputVariable(int simple, std::string easy);

        int checkSimpleReturnValue();
    };
}


#endif //PROJECT_DEPEDENCYGETTABLE_HH
