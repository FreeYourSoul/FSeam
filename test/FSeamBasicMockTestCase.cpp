//
// Created by FyS on 3/24/19.
//

#include <catch.hpp>
#include <iostream>
#include <any>
#include <MockVerifier.hpp>
#include <TestingClass.hh>
#include <MockData.hpp>

TEST_CASE( "FSeamBasicTest", "[basic]" ) {
    source::TestingClass testingClass {};
    auto fseamMock = FSeam::get(&testingClass.getDepGettable());

    SECTION("Test hasOriginalServiceBeenCalled") { 
        testingClass.execute();
        // prove that we overrided the normal behavior (which has not been compiled at all)
        REQUIRE_FALSE(testingClass.getDepGettable().hasOriginalServiceBeenCalled());

    } // End section : Test hasOriginalServiceBeenCalled

    SECTION("FSeam_Verify") {
        REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED));
        REQUIRE(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::NeverCalled{}));
        testingClass.execute();
        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED));
        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, 1));
        testingClass.execute();
        testingClass.execute();
        testingClass.execute();
        testingClass.execute();
        
        // check at least 1 call has been done
        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED));
        // check exactly 5 calls are done
        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, 5));
        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::VerifyCompare{5}));

        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::IsNot{1}));
        CHECK_FALSE(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::IsNot{5}));

        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::AtLeast{4}));
        CHECK_FALSE(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::AtLeast{6}));

        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::AtMost{6}));
        CHECK_FALSE(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::AtMost{4}));

    } // End section : FSeam_Verify

    SECTION("FSeam_DupeMethod_simple") {
        bool isDupedImplCalled = false;
        fseamMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKCALLED, [&isDupedImplCalled](void *dataStruct) {
            isDupedImplCalled = true;
        });
        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::NeverCalled{}));
        CHECK_FALSE(isDupedImplCalled);
        testingClass.execute();
        CHECK(isDupedImplCalled);
        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED));

    } // End section : FSeam_DupeMethod_simple

    SECTION("FSeam_DupeMethod_composed") {
        int valueChanging = 0;
        fseamMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKCALLED, [&valueChanging](void *dataStruct) {
                valueChanging += 1;
            }, true); // first layer  : valueChanging = 1
        fseamMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKCALLED, [&valueChanging](void *dataStruct) {
                valueChanging += 10;
            }, true); // second layer : valueChanging = 11
        fseamMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKCALLED, [&valueChanging](void *dataStruct) {
                valueChanging += 100;
            }, true); // third layer  : valueChanging = 111
        
        SECTION("Test Composition") {
            REQUIRE(0 == valueChanging);
            testingClass.execute();
            CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, 1));
            CHECK(111 == valueChanging);

        } // End section : Test Composition

        SECTION("Test FSeam::MockVerifier::cleanup") {
            FSeam::MockVerifier::cleanUp(); // Cleanup, remove all alternation of mocks : valueChanging won't change
            fseamMock = FSeam::get(&testingClass.getDepGettable()); // Need to reset the shared ptr as it has been cleaned up
            fseamMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKCALLED, [&valueChanging](void *dataStruct) {
                    valueChanging += 1;
                }, true); // valueChanging = 1

            REQUIRE(0 == valueChanging);
            testingClass.execute();
            CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, 1));
            CHECK_FALSE(111 == valueChanging);
            CHECK(1 == valueChanging);

        } // End section : test FSeam::MockVerifier::cleanup
        
    } // End section : Test On source::TestingClass::checkCalled

    SECTION("Test FSeam::Verify on Arguments") {
        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, FSeam::NeverCalled{}));
        testingClass.execute(); // called once in total

        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE));
        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42;
            }));
        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue != 42;
            }, FSeam::NeverCalled{}));

        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_easy_ParamValue != "4242";
            }, FSeam::NeverCalled{}));

        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_easy_ParamValue != "4242" ||
                       std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue >= 16;
            }));

        testingClass.execute();
        testingClass.execute();
        testingClass.execute();
        testingClass.execute(); // called 5 times in total

        SECTION ("RAW USAGE : Check one argument") {
            CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42;
                }, FSeam::AtLeast{3}));
            CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42;
                }, FSeam::AtMost{6}));
            CHECK_FALSE(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42;
                }, FSeam::AtLeast{8}));
            CHECK_FALSE(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42;
                }, FSeam::AtMost{3}));
            CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42;
                }, FSeam::VerifyCompare{5}));

        } // End section : Check one argument

        SECTION ("RAW USAGE : Check all arguments") {
            CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42 &&
                       std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_easy_ParamValue == "4242";
                }, FSeam::AtLeast{3}));
            CHECK_FALSE(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42 &&
                       std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_easy_ParamValue == "4242";
                }, FSeam::AtLeast{6}));
            CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42 &&
                       std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_easy_ParamValue == "4242";
                }, FSeam::AtMost{8}));
            CHECK_FALSE(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42 &&
                       std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_easy_ParamValue == "4242";
                }, FSeam::AtMost{3}));
            CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42 &&
                       std::any_cast<FSeam::DependencyGettableData>(methodCallData).checkSimpleInputVariable_easy_ParamValue == "4242";
                }, FSeam::VerifyCompare{5}));

        } // End section : Check all argument

        /**
         * Use generated helper functions
         */
        SECTION ("RECOMMENDED USAGE : Check arguments") {
            // TODO

        } // End section : RECOMMENDED USAGE : Check arguments

    } // End section : Test FSeam::Verify on Arguments

    SECTION ("Test FSeam::Mock Return Values") {

        SECTION("RAW USAGE : set mock return value") {
            fseamMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKSIMPLERETURNVALUE, [](void *methodCallData) {
                static_cast<FSeam::DependencyGettableData *>(methodCallData)->checkSimpleReturnValue_ReturnValue = 42;
            });
            CHECK(42 == testingClass.getDepGettable().checkSimpleReturnValue());
            fseamMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKSIMPLERETURNVALUE, [](void *methodCallData) {
                static_cast<FSeam::DependencyGettableData *>(methodCallData)->checkSimpleReturnValue_ReturnValue = 1337;
            });
            CHECK(1337 == testingClass.getDepGettable().checkSimpleReturnValue());
            fseamMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKSIMPLERETURNVALUE, [](void *methodCallData) {
                static_cast<FSeam::DependencyGettableData *>(methodCallData)->checkSimpleReturnValue_ReturnValue = -1;
            });
            CHECK(-1 == testingClass.getDepGettable().checkSimpleReturnValue());

        } // End section : RAW USAGE : set mock return value

        SECTION("MACRO USAGE : set mock return value") {
            fseamMock->dupeMethod(mock_return_value(checkSimpleReturnValue, FSeam::DependencyGettableData, 1));
            CHECK(1 == testingClass.getDepGettable().checkSimpleReturnValue());
            fseamMock->dupeMethod(mock_return_value(checkSimpleReturnValue, FSeam::DependencyGettableData, 42));
            CHECK(42 == testingClass.getDepGettable().checkSimpleReturnValue());
            fseamMock->dupeMethod(mock_return_value(checkSimpleReturnValue, FSeam::DependencyGettableData, 177));
            CHECK(177 == testingClass.getDepGettable().checkSimpleReturnValue());

        } // End section : MACRO USAGE : set mock return value

    } // End section : Test FSeam::Verify on Return Values

    FSeam::MockVerifier::cleanUp();

} // End Test_Case : FSeamBasicTest
