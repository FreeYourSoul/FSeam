//
// Created by FyS on 3/24/19.
//

#include <catch.hpp>
#include <iostream>
#include <any>
#include <MockVerifier.hpp>
#include <TestingClass.hh>
#include <MockData.hpp>

TEST_CASE( "FSeamDefaultMockTest", "[default_mock]" ) {
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
            fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable_FunName::CHECKCALLED, [&testingFlag](void *dataStruct){
                testingFlag = 1;
            });

            REQUIRE(0 == testingClass.checkSimpleReturnValueNonGettable());
            fseamDefaultMock->dupeMethod(mock_return_value(checkSimpleReturnValue, FSeam::DependencyNonGettableData, 10));
            REQUIRE(0 == testingFlag);
            testingClass.execute();
            REQUIRE(1 == testingFlag);
            REQUIRE(10 == testingClass.checkSimpleReturnValueNonGettable());
        
        } // End section : Test Normal default behaviors for NonGettable dependency


        SECTION("FSeam_Verify") {
            REQUIRE_FALSE(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED));
            REQUIRE(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED, FSeam::NeverCalled{}));
            testingClass.execute();
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED));
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED, 1));
            testingClass.execute();
            testingClass.execute();
            testingClass.execute();
            testingClass.execute();

            // check at least 1 call has been done
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED));
            // check exactly 5 calls are done
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED, 5));
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED, FSeam::VerifyCompare{5}));

            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED, FSeam::IsNot{1}));
            CHECK_FALSE(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED, FSeam::IsNot{5}));

            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED, FSeam::AtLeast{4}));
            CHECK_FALSE(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED, FSeam::AtLeast{6}));

            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED, FSeam::AtMost{6}));
            CHECK_FALSE(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED, FSeam::AtMost{4}));

        } // End section : FSeam_Verify

        SECTION("FSeam_DupeMethod_simple") {
            bool isDupedImplCalled = false;
            fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable_FunName::CHECKCALLED, [&isDupedImplCalled](void *dataStruct) {
                isDupedImplCalled = true;
            });
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED, FSeam::NeverCalled{}));
            CHECK_FALSE(isDupedImplCalled);
            testingClass.execute();
            CHECK(isDupedImplCalled);
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED));

        } // End section : FSeam_DupeMethod_simple

        SECTION("FSeam_DupeMethod_composed") {
            int valueChanging = 0;
            fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable_FunName::CHECKCALLED, [&valueChanging](void *dataStruct) {
                valueChanging += 1;
            }, true); // first layer  : valueChanging = 1
            fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable_FunName::CHECKCALLED, [&valueChanging](void *dataStruct) {
                valueChanging += 10;
            }, true); // second layer : valueChanging = 11
            fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable_FunName::CHECKCALLED, [&valueChanging](void *dataStruct) {
                valueChanging += 100;
            }, true); // third layer  : valueChanging = 111

            SECTION("Test Composition") {
                REQUIRE(0 == valueChanging);
                testingClass.execute();
                CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKCALLED, 1));
                CHECK(111 == valueChanging);

            } // End section : Test Composition

        } // End section : Test On source::TestingClass::checkCalled

        SECTION("Test FSeam::Verify on Arguments") {
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE, FSeam::NeverCalled{}));
            testingClass.execute(); // called once in total

            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE));
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42;
            }));
            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue != 42;
            }, FSeam::NeverCalled{}));

            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_easy_ParamValue != "4242";
            }, FSeam::NeverCalled{}));

            CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_easy_ParamValue != "4242" ||
                       std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue >= 16;
            }));

            testingClass.execute();
            testingClass.execute();
            testingClass.execute();
            testingClass.execute(); // called 5 times in total

            SECTION ("RAW USAGE : Check one argument") {
                CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42;
                }, FSeam::AtLeast{3}));
                CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42;
                }, FSeam::AtMost{6}));
                CHECK_FALSE(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42;
                }, FSeam::AtLeast{8}));
                CHECK_FALSE(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42;
                }, FSeam::AtMost{3}));
                CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42;
                }, FSeam::VerifyCompare{5}));

            } // End section : Check one argument

            SECTION ("RAW USAGE : Check all arguments") {
                CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42 &&
                           std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_easy_ParamValue == "4242";
                }, FSeam::AtLeast{3}));
                CHECK_FALSE(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42 &&
                           std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_easy_ParamValue == "4242";
                }, FSeam::AtLeast{6}));
                CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42 &&
                           std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_easy_ParamValue == "4242";
                }, FSeam::AtMost{8}));
                CHECK_FALSE(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42 &&
                           std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_easy_ParamValue == "4242";
                }, FSeam::AtMost{3}));
                CHECK(fseamDefaultMock->verify(FSeam::DependencyNonGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_simple_ParamValue == 42 &&
                           std::any_cast<FSeam::DependencyNonGettableData>(methodCallData).checkSimpleInputVariable_easy_ParamValue == "4242";
                }, FSeam::VerifyCompare{5}));

            } // End section : Check all argument

            SECTION ("MACRO USAGE : Check argument") {
                CHECK(fseamDefaultMock->verify(check_arg_value(checkSimpleInputVariable, FSeam::DependencyNonGettableData, simple, 42), FSeam::AtLeast{3}));
                CHECK(fseamDefaultMock->verify(check_arg_value(checkSimpleInputVariable, FSeam::DependencyNonGettableData, easy, "4242"), FSeam::AtLeast{3}));

            } // End section : MACRO USAGE : Check arguments

        } // End section : Test FSeam::Verify on Arguments

        SECTION ("Test FSeam::Mock Return Values") {

            SECTION("RAW USAGE : set mock return value") {
                fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable_FunName::CHECKSIMPLERETURNVALUE, [](void *methodCallData) {
                    static_cast<FSeam::DependencyNonGettableData *>(methodCallData)->checkSimpleReturnValue_ReturnValue = 42;
                });
                CHECK(42 == testingClass.checkSimpleReturnValueNonGettable());
                fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable_FunName::CHECKSIMPLERETURNVALUE, [](void *methodCallData) {
                    static_cast<FSeam::DependencyNonGettableData *>(methodCallData)->checkSimpleReturnValue_ReturnValue = 1337;
                });
                CHECK(1337 == testingClass.checkSimpleReturnValueNonGettable());
                fseamDefaultMock->dupeMethod(FSeam::DependencyNonGettable_FunName::CHECKSIMPLERETURNVALUE, [](void *methodCallData) {
                    static_cast<FSeam::DependencyNonGettableData *>(methodCallData)->checkSimpleReturnValue_ReturnValue = -1;
                });
                CHECK(-1 == testingClass.checkSimpleReturnValueNonGettable());

            } // End section : RAW USAGE : set mock return value

            SECTION("MACRO USAGE : set mock return value") {
                fseamDefaultMock->dupeMethod(mock_return_value(checkSimpleReturnValue, FSeam::DependencyNonGettableData, 1));
                CHECK(1 == testingClass.checkSimpleReturnValueNonGettable());
                fseamDefaultMock->dupeMethod(mock_return_value(checkSimpleReturnValue, FSeam::DependencyNonGettableData, 42));
                CHECK(42 == testingClass.checkSimpleReturnValueNonGettable());
                fseamDefaultMock->dupeMethod(mock_return_value(checkSimpleReturnValue, FSeam::DependencyNonGettableData, 177));
                CHECK(177 == testingClass.checkSimpleReturnValueNonGettable());

            } // End section : MACRO USAGE : set mock return value

        } // End section : Test FSeam::Verify on Return Values

        FSeam::MockVerifier::cleanUp();
    } // End section : Test default full

    SECTION("Test override") {
        auto fseamDefaultMock = FSeam::getDefault<source::DependencyGettable>();
        int testingFlag = 0;

        fseamDefaultMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKCALLED, [&testingFlag](void *dataStruct){
            testingFlag = 1;
        });
        REQUIRE(0 == testingClass.getDepGettable().checkSimpleReturnValue());
        EXPECT_FALSE(FSeam::MockVerifier::isMockRegistered(&testingClass.getDepGettable()));
        fseamDefaultMock->dupeMethod(mock_return_value(checkSimpleReturnValue, FSeam::DependencyNonGettableData, 10));

        SECTION("Test Normal default behaviors for GettableDependency") {
            REQUIRE(0 == testingFlag);
            testingClass.execute();
            REQUIRE(1 == testingFlag);
            REQUIRE(10 == testingClass.getDepGettable().checkSimpleReturnValue());

        } // End section : Test Normal default behaviors for GettableDependency

        SECTION("Test Override default behaviors") {
            auto fseamMock = FSeam::get(&testingClass.getDepGettable());
            fseamMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKCALLED, [&testingFlag](void *dataStruct){
                testingFlag = 42;
            });
            EXPECT(FSeam::MockVerifier::isMockRegistered(&testingClass.getDepGettable()));
            REQUIRE(0 == testingClass.getDepGettable().checkSimpleReturnValue());
            fseamMock->dupeMethod(mock_return_value(checkSimpleReturnValue, FSeam::DependencyGettableData, 1337));

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


