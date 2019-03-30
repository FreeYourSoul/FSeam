//
// Created by FyS on 3/30/19.
//

#ifndef FSEAM_ARGSSTRUCT_HH
#define FSEAM_ARGSSTRUCT_HH

#include <string>

namespace source {
    struct StructTest {
        int testInt;
        short testShort;
        std::string testStr;
    };

    struct NonCopiableTest {
        NonCopiableTest(const NonCopiableTest &other) = delete;

        int testInt;
        short testShort;
        std::string testStr;
    };
}

#endif //FSEAM_ARGSSTRUCT_HH
