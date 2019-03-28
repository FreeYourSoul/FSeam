//
// Created by FyS on 3/24/19.
//

#include <catch.hpp>
#include <iostream>
#include <any>
#include <TestingClass.hh>
#include <MockData.hpp>

TEST_CASE( "FSeamDefaultMockTest" ) {
    source::TestingClass testingClass {};

    SECTION("Test hasOriginalServiceBeenCalled") { 
        testingClass.execute();
        /** 
         *  prove that we overrided the normal behavior (even without setting any mock)
         *  as no FSeam::get has been called yet (which is the difference with the test 
         *  in FSeamBasicMockTestCase.cpp "Test hasOriginalServiceBeenCalled")
         */
        REQUIRE_FALSE(testingClass.getDepGettable().hasOriginalServiceBeenCalled());

    } // End section : Test hasOriginalServiceBeenCalled

    SECTION("Test default full") {
        auto fseamDefaultMock = FSeam::getDefault<source::DependencyNonGettable>();

        SECTION("Test Normal default behaviors for NonGettable dependency") {
            int testingFlag = 0;
            fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable::checkCalled::NAME, [&testingFlag](void *dataStruct){
                testingFlag = 1;
            });

            REQUIRE(0 == testingClass.checkSimpleReturnValueNonGettable());
            fseamDefaultMock->dupeReturn<FSeam::DependencyNonGettable::checkSimpleReturnValue>(10);
            REQUIRE(0 == testingFlag);
            testingClass.execute();
            REQUIRE(1 == testingFlag);
            REQUIRE(10 == testingClass.checkSimpleReturnValueNonGettable());
        
        } // End section : Test Normal default behaviors for NonGettable dependency


        SECTION("FSeam_Verify") {
            REQUIRE_FALSE(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME));
            REQUIRE(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME, FSeam::NeverCalled{}));
            testingClass.execute();
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME));
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME, 1));
            testingClass.execute();
            testingClass.execute();
            testingClass.execute();
            testingClass.execute();

            // check at least 1 call has been done
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME));
            // check exactly 5 calls are done
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME, 5));
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME, FSeam::VerifyCompare{5}));

            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME, FSeam::IsNot{1}));
            CHECK_FALSE(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME, FSeam::IsNot{5}));

            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME, FSeam::AtLeast{4}));
            CHECK_FALSE(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME, FSeam::AtLeast{6}));

            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME, FSeam::AtMost{6}));
            CHECK_FALSE(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME, FSeam::AtMost{4}));

        } // End section : FSeam_Verify

        SECTION("FSeam_DupeMethod_simple") {
            bool isDupedImplCalled = false;
            fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable::checkCalled::NAME, [&isDupedImplCalled](void *dataStruct) {
                isDupedImplCalled = true;
            });
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME, FSeam::NeverCalled{}));
            CHECK_FALSE(isDupedImplCalled);
            testingClass.execute();
            CHECK(isDupedImplCalled);
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME));

        } // End section : FSeam_DupeMethod_simple

        SECTION("FSeam_DupeMethod_composed") {
            int valueChanging = 0;
            fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable::checkCalled::NAME, [&valueChanging](void *dataStruct) {
                valueChanging += 1;
            }, true); // first layer  : valueChanging = 1
            fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable::checkCalled::NAME, [&valueChanging](void *dataStruct) {
                valueChanging += 10;
            }, true); // second layer : valueChanging = 11
            fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable::checkCalled::NAME, [&valueChanging](void *dataStruct) {
                valueChanging += 100;
            }, true); // third layer  : valueChanging = 111

            SECTION("Test Composition") {
                REQUIRE(0 == valueChanging);
                testingClass.execute();
                CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable::checkCalled::NAME, 1));
                CHECK(111 == valueChanging);

            } // End section : Test Composition

        } // End section : Test On source::TestingClass::checkCalled
        

        SECTION ("Test FSeam::Mock Return Values") {

            SECTION("RAW USAGE : set mock return value") {
                fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable::checkSimpleReturnValue::NAME, [](void *methodCallData) {
                    static_cast<FSeam::DependencyNonGettableData *>(methodCallData)->checkSimpleReturnValue_ReturnValue = 42;
                });
                CHECK(42 == testingClass.checkSimpleReturnValueNonGettable());
                fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable::checkSimpleReturnValue::NAME, [](void *methodCallData) {
                    static_cast<FSeam::DependencyNonGettableData *>(methodCallData)->checkSimpleReturnValue_ReturnValue = 1337;
                });
                CHECK(1337 == testingClass.checkSimpleReturnValueNonGettable());
                fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable::checkSimpleReturnValue::NAME, [](void *methodCallData) {
                    static_cast<FSeam::DependencyNonGettableData *>(methodCallData)->checkSimpleReturnValue_ReturnValue = -1;
                });
                CHECK(-1 == testingClass.checkSimpleReturnValueNonGettable());

            } // End section : RAW USAGE : set mock return value

            SECTION("MACRO USAGE : set mock return value") {
                fseamDefaultMock->dupeReturn<FSeam::DependencyNonGettable::checkSimpleReturnValue>(1);
                CHECK(1 == testingClass.checkSimpleReturnValueNonGettable());
                fseamDefaultMock->dupeReturn<FSeam::DependencyNonGettable::checkSimpleReturnValue>(42);
                CHECK(42 == testingClass.checkSimpleReturnValueNonGettable());
                fseamDefaultMock->dupeReturn<FSeam::DependencyNonGettable::checkSimpleReturnValue>(177);
                CHECK(177 == testingClass.checkSimpleReturnValueNonGettable());

            } // End section : MACRO USAGE : set mock return value

        } // End section : Test FSeam::Verify on Return Values

        FSeam::MockVerifier::cleanUp();
    } // End section : Test default full

    SECTION("Test override") {
        auto fseamDefaultMock = FSeam::getDefault<source::DependencyGettable>();
        int testingFlag = 0;

        fseamDefaultMock->dupeMethod(FSeam::DependencyGettable::checkCalled::NAME, [&testingFlag](void *dataStruct){
            testingFlag = 1;
        });
        REQUIRE(0 == testingClass.getDepGettable().checkSimpleReturnValue());
        REQUIRE_FALSE(FSeam::MockVerifier::instance().isMockRegistered(&testingClass.getDepGettable()));
        fseamDefaultMock->dupeReturn<FSeam::DependencyGettable::checkSimpleReturnValue>(10);

        SECTION("Test Normal default behaviors for GettableDependency") {
            REQUIRE(0 == testingFlag);
            testingClass.execute();
            REQUIRE(1 == testingFlag);
            REQUIRE(10 == testingClass.getDepGettable().checkSimpleReturnValue());

        } // End section : Test Normal default behaviors for GettableDependency

        SECTION("Test Override default behaviors") {
            auto fseamMock = FSeam::get(&testingClass.getDepGettable());
            fseamMock->dupeMethod(FSeam::DependencyGettable::checkCalled::NAME, [&testingFlag](void *dataStruct){
                testingFlag = 42;
            });
            REQUIRE(FSeam::MockVerifier::instance().isMockRegistered(&testingClass.getDepGettable()));
            REQUIRE(0 == testingClass.getDepGettable().checkSimpleReturnValue());
            fseamMock->dupeReturn<FSeam::DependencyGettable::checkSimpleReturnValue>(1337);

            REQUIRE(0 == testingFlag);
            testingClass.execute();
            REQUIRE(1 != testingFlag);
            REQUIRE(42 == testingFlag);
            REQUIRE(10 != testingClass.getDepGettable().checkSimpleReturnValue());
            REQUIRE(1337 == testingClass.getDepGettable().checkSimpleReturnValue());

            SECTION("Test other instances still use default behaviors") {
                source::TestingClass otherTestingClass {};
                otherTestingClass.execute();
                REQUIRE(1 == testingFlag);
                REQUIRE(10 == otherTestingClass.getDepGettable().checkSimpleReturnValue());
            }

        } // End section : Test Override default behaviors
        
    } // End section : Test override

    FSeam::MockVerifier::cleanUp();
} // End Test_Case : FSeamDefaultMockTest


