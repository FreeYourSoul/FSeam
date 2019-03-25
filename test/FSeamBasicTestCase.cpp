//
// Created by FyS on 3/24/19.
//

#include <catch.hpp>
#include <MockVerifier.hpp>
#include <TestingClass.hh>
#include <MockData.hpp>
#include <iostream>

TEST_CASE( "FSeamBasicTest", "[basic]" ) {
    source::TestingClass testingClass {};

    SECTION("Test hasOriginalServiceBeenCalled") {
        testingClass.execute();
        // prove that we overrided the normal behavior (which has not been compiled at all)
        REQUIRE_FALSE(testingClass.getDepGettable().hasOriginalServiceBeenCalled());
    }

    /**
     * Test on source::TestingClass::checkCalled
     * method without argument and without returned value: type "void (void)"
     */
    SECTION("Test On source::TestingClass::checkCalled") {
        auto *fseamMock = FSeam::get(&testingClass.getDepGettable());

        SECTION("FSeam_Verify") {
            testingClass.execute();
            EXPECT(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED));
            EXPECT(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED, 1));
            testingClass.execute();
            testingClass.execute();
            testingClass.execute();
            testingClass.execute();
            
            // check at least 1 call has been done
            EXPECT(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED));
            // check exactly 5 calls are done
            EXPECT(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED, 5));
            EXPECT(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::VerifyCompare{5});

            EXPECT(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::IsNot{1}));
            EXPECT_FALSE(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::IsNot{5}));

            EXPECT(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::AtLeast{4}));
            EXPECT_FALSE(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::AtLeast{6}));

            EXPECT(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::AtMost{6}));
            EXPECT_FALSE(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::AtMost{4}));
        }

        SECTION("FSeam_DupeMethod") {
            bool isDupedImplCalled = false;
            fseamMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKCALLED, [&isDupedImplCalled](void *data) {
                isDupedImplCalled = true;
            });
            testingClass.execute();
            EXPECT(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED));
        }
    }

}
