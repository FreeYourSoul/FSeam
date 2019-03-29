//
// Created by FyS on 3/23/19.
//

#ifndef PROJECT_DEPEDENCYGETTABLE_HH
#define PROJECT_DEPEDENCYGETTABLE_HH

#include <string>

namespace source {
    class DependencyGettable {

    struct StructTest {
        int testInt;
        short testShort; 
        std::string testStr;
    }

    struct NonCopiableTest {
        NonCopiableTest(const StructTest &) = delete;

        int testInt;
        short testShort; 
        std::string testStr;
    }

    public:
        void checkCalled() const;

        void checkSimpleInputVariable(int simple, std::string easy) const;

        int checkSimpleReturnValue();

        StructTest checkCustomStructReturnValue() const;

        void checkCustomStructInputVariableRef(const StructTest &testStr);
        void checkCustomStructInputVariableRValueRef(StructTest &&testStr);
        void checkCustomStructInputVariable(StructTest testStr);

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
