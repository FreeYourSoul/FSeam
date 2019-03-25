//
// Created by FyS on 3/23/19.
//

#ifndef PROJECT_DEPEDENCYGETTABLE_HH
#define PROJECT_DEPEDENCYGETTABLE_HH

#include <string>

namespace source {
    class DependencyGettable {
    public:
        void checkCalled();

        void checkSimpleInputVariable(int simple, std::string easy);

        int checkSimpleReturnValue();

        /**
         * @brief check if this class has been used into its original form or not
         * @note each of the public testing method set to true the _hasOriginalBeenCalled boolean
         * @return true if a public method has been implemented with its original behavior,
         *         false othewise
         */
        bool hasOriginalServiceBeenCalled() { return _hasOriginalBeenCalled; }

    private:
        bool _hasOriginalBeenCalled = false;
    };

}


#endif //PROJECT_DEPEDENCYGETTABLE_HH
