//
// Created by FyS on 3/24/19.
//

#include <catch.hpp>
#include <iostream>
#include <any>
#include <MockVerifier.hpp>
#include <TestingClass.hh>
#include <MockData.hpp>

TEST_CASE( "FSeamDefaultMockTest", "[default]" ) {
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
            fseamDefaultMock->dupeMethod(Seam::DependencyNonGettable_FunName::CHECKCALLED, [&testingFlag](void *dataStruct){
                testingFlag = 1;
            });
            fseamDefaultMock->dupeMethod(mock_return_value(checkSimpleReturnValue, FSeam::DependencyNonGettableData, 10));
            
            REQUIRE(0 == testingFlag);
            REQUIRE(0 == testingClass.checkSimpleReturnValue());
            testingClass.execute();
            REQUIRE(1 == testingFlag);
            REQUIRE(10 == testingClass.checkSimpleReturnValue());
        
        } // End section : Test Normal default behaviors for NonGettable dependency

        /**
         *  ! Other test are made in FSeamBasicMockTestCase via the use of a GENERATOR making the whole test set working for 
         *  ! Gettable and NonGettable dependencies.
         */

    } // End section : Test default full

    SECTION("Test override") {
        auto fseamDefaultMock = FSeam::getDefault<source::DependencyGettable>();
        int testingFlag = 0;

        fseamDefaultMock->dupeMethod(Seam::DependencyGettable_FunName::CHECKCALLED, [&testingFlag](void *dataStruct){
            testingFlag = 1;
        });
        fseamDefaultMock->dupeMethod(mock_return_value(checkSimpleReturnValue, FSeam::DependencyNonGettableData, 10));

        SECTION("Test Normal default behaviors for GettableDependency") {
            REQUIRE(0 == testingFlag);
            REQUIRE(0 == testingClass.checkSimpleReturnValue());
            testingClass.execute();
            REQUIRE(1 == testingFlag);
            REQUIRE(10 == testingClass.checkSimpleReturnValue());

        } // End section : Test Normal default behaviors for GettableDependency

        SECTION("Test Overrided default behaviors") {
            auto fseamMock = FSeam::getDefault<source::DependencyGettable>();
            fseamMock->dupeMethod(Seam::DependencyGettable_FunName::CHECKCALLED, [&testingFlag](void *dataStruct){
                testingFlag = 42;
            });
            fseamMock->dupeMethod(mock_return_value(checkSimpleReturnValue, FSeam::DependencyNonGettableData, 1337));

            REQUIRE(0 == testingFlag);
            REQUIRE(0 == testingFlag.checkSimpleReturnValue());
            testingClass.execute();
            REQUIRE(1 != testingFlag);
            REQUIRE(42 == testingFlag);
            REQUIRE(10 != testingFlag.checkSimpleReturnValue());
            REQUIRE(1337 == testingFlag.checkSimpleReturnValue());

            SECTION("Test other instances still use default behaviors") {
                source::TestingClass otherTestingClass {};
                REQUIRE(0 == testingFlag);
                REQUIRE(0 == otherTestingClass.checkSimpleReturnValue());
                otherTestingClass.execute();
                REQUIRE(1 == testingFlag);
                REQUIRE(10 == otherTestingClass.checkSimpleReturnValue());
            }

        } // End section : Test Overrided default behaviors
        
    } // End section : Test override

    FSeam::MockVerifier::cleanUp();
} // End Test_Case : FSeamDefaultMockTest


