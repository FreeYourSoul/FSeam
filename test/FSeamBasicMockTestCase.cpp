//
// Created by FyS on 3/24/19.
//

#include <catch2/catch.hpp>
#include <iostream>
#include <any>
#include <FSeam.hpp>
#include <TestingClass.hh>
#include <FSeamMockData.hpp>

TEST_CASE( "FSeamBasicTest" ) {
    source::TestingClass testingClass {};
    auto fseamMock = FSeam::get(&testingClass.getDepGettable());

    SECTION("Test hasOriginalServiceBeenCalled") { 
        testingClass.execute();
        // prove that we overrided the normal behavior (which has not been compiled at all)
        REQUIRE_FALSE(testingClass.getDepGettable().hasOriginalServiceBeenCalled());

    } // End section : Test hasOriginalServiceBeenCalled

    SECTION("FSeam_Verify") {
        REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, false));
        REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, FSeam::NeverCalled{}));
        testingClass.execute();
        CHECK(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME));
        CHECK(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, 1));
        testingClass.execute();
        testingClass.execute();
        testingClass.execute();
        testingClass.execute();

        // check at least 1 call has been done
        CHECK(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME));
        // check exactly 5 calls are done
        CHECK(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, 5));
        CHECK(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, FSeam::VerifyCompare{5}));

        CHECK(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, FSeam::IsNot{1}));
        CHECK_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, FSeam::IsNot{5}, false));

        CHECK(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, FSeam::AtLeast{4}));
        CHECK_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, FSeam::AtLeast{6}, false));

        CHECK(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, FSeam::AtMost{6}));
        CHECK_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, FSeam::AtMost{4}, false));

    } // End section : FSeam_Verify

    SECTION("FSeam_DupeMethod_simple") {
        bool isDupedImplCalled = false;
        fseamMock->dupeMethod(FSeam::DependencyGettable::checkCalled::NAME, [&isDupedImplCalled](void *dataStruct) {
            isDupedImplCalled = true;
        });
        CHECK(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, FSeam::NeverCalled{}));
        CHECK_FALSE(isDupedImplCalled);
        testingClass.execute();
        CHECK(isDupedImplCalled);
        CHECK(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME));

    } // End section : FSeam_DupeMethod_simple

    SECTION("FSeam_DupeMethod_composed") {
        int valueChanging = 0;
        fseamMock->dupeMethod(FSeam::DependencyGettable::checkCalled::NAME, [&valueChanging](void *dataStruct) {
                valueChanging += 1;
            }, true); // first layer  : valueChanging = 1
        fseamMock->dupeMethod(FSeam::DependencyGettable::checkCalled::NAME, [&valueChanging](void *dataStruct) {
                valueChanging += 10;
            }, true); // second layer : valueChanging = 11
        fseamMock->dupeMethod(FSeam::DependencyGettable::checkCalled::NAME, [&valueChanging](void *dataStruct) {
                valueChanging += 100;
            }, true); // third layer  : valueChanging = 111
        
        SECTION("Test Composition") {
            REQUIRE(0 == valueChanging);
            testingClass.execute();
            CHECK(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, 1));
            CHECK(111 == valueChanging);

        } // End section : Test Composition
        
    } // End section : Test On source::TestingClass::checkCalled

    SECTION ("Test FSeam::Mock Return Values") {

        SECTION("RAW USAGE : set mock return value") {
            fseamMock->dupeMethod(FSeam::DependencyGettable::checkSimpleReturnValue::NAME, [](void *methodCallData) {
                static_cast<FSeam::DependencyGettableData *>(methodCallData)->checkSimpleReturnValue_ReturnValue = 42;
            });
            CHECK(42 == testingClass.getDepGettable().checkSimpleReturnValue());
            fseamMock->dupeMethod(FSeam::DependencyGettable::checkSimpleReturnValue::NAME, [](void *methodCallData) {
                static_cast<FSeam::DependencyGettableData *>(methodCallData)->checkSimpleReturnValue_ReturnValue = 1337;
            });
            CHECK(1337 == testingClass.getDepGettable().checkSimpleReturnValue());
            fseamMock->dupeMethod(FSeam::DependencyGettable::checkSimpleReturnValue::NAME, [](void *methodCallData) {
                static_cast<FSeam::DependencyGettableData *>(methodCallData)->checkSimpleReturnValue_ReturnValue = -1;
            });
            CHECK(-1 == testingClass.getDepGettable().checkSimpleReturnValue());

        } // End section : RAW USAGE : set mock return value

    } // End section : Test FSeam::Verify on Return Values

    FSeam::MockVerifier::cleanUp();

} // End Test_Case : FSeamBasicTest
