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
    auto *fseamMock = FSeam::get(&testingClass.getDepGettable());

    SECTION("Test hasOriginalServiceBeenCalled") { 
        testingClass.execute();
        // prove that we overrided the normal behavior (which has not been compiled at all)
        REQUIRE_FALSE(testingClass.getDepGettable().hasOriginalServiceBeenCalled());

    } // End section : Test hasOriginalServiceBeenCalled

    SECTION("FSeam_Verify") {
        EXPECT_FALSE(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED));
        EXPECT(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::NeverCalled{}));
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

    } // End section : FSeam_Verify

    SECTION("FSeam_DupeMethod_simple") {
        bool isDupedImplCalled = false;
        fseamMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKCALLED, [&isDupedImplCalled](void *dataStruct) {
            isDupedImplCalled = true;
        });
        EXPECT(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED, FSeam::NeverCalled{}));
        EXPECT_FALSE(isDupedImplCalled);
        testingClass.execute();
        EXPECT(isDupedImplCalled);
        EXPECT(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED));

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
            EXPECT_FALSE(valueChanging);
            testingClass.execute();
            EXPECT(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED), 1);
            EXPECT(111 == valueChanging);

        } // End section : Test Composition

        SECTION("Test FSeam::MockVerifier::cleanup") {
            FSeam::MockVerifier::cleanUp(); // Cleanup, remove all alternation of mocks : valueChanging won't change

            fseamMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKCALLED, [&valueChanging](void *dataStruct) {
                    valueChanging += 1;
                }, true); // valueChanging = 1

            EXPECT_FALSE(valueChanging);
            testingClass.execute();
            EXPECT(fseamMock.verify(FSeam::DependencyGettable_FunName::CHECKCALLED), 1);
            EXPECT_FALSE(111 == valueChanging);
            EXPECT(1 == valueChanging);

        } // End section : test FSeam::MockVerifier::cleanup
        
    } // End section : Test On source::TestingClass::checkCalled
    
    SECTION("Test FSeam::Verify on Arguments") {
        EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, FSeam::NeverCalled{}));
        testingClass.execute(); // called once in total
        
        EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE));
        EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).simple_ArgValue == 42;
            }), 1);
        EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).simple_ArgValue != 42;
            }), FSeam::NeverCalled{});

        EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).easy_ArgValue != "4242";
            }), FSeam::NeverCalled{});

        EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).easy_ArgValue != "4242" ||
                        std::any_cast<FSeam::DependencyGettableData>(methodCallData).simple_ArgValue >= 16;
            }), 1});

        testingClass.execute();
        testingClass.execute();
        testingClass.execute();
        testingClass.execute(); // called 5 times in total

        SECTION ("RAW USAGE : Check one argument") {
            EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyGettableData>(methodCallData).simple_ArgValue == 42;
                }), FSeam::AtLeast{3});
            EXPECT_FALSE(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                    return std::any_cast<FSeam::DependencyGettableData>(methodCallData).simple_ArgValue == 42;
                }), FSeam::AtLeast{6});
            EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).simple_ArgValue == 42;
            }), FSeam::AtLeast{8});
            EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).simple_ArgValue == 42;
            }), FSeam::AtMost{3});
            EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).simple_ArgValue == 42;
            }), FSeam::VerifyCompare{5});

        } // End section : Check one argument

        SECTION ("RAW USAGE : Check all arguments") {
            EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).simple_ArgValue == 42 &&
                        std::any_cast<FSeam::DependencyGettableData>(methodCallData).easy_ArgValue == "4242";   
                }), FSeam::AtLeast{3});
            EXPECT_FALSE(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).simple_ArgValue == 42 &&
                        std::any_cast<FSeam::DependencyGettableData>(methodCallData).easy_ArgValue == "4242";   
                }), FSeam::AtLeast{6});
            EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).simple_ArgValue == 42 &&
                        std::any_cast<FSeam::DependencyGettableData>(methodCallData).easy_ArgValue == "4242";   
                }), FSeam::AtLeast{8});
            EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).simple_ArgValue == 42 &&
                        std::any_cast<FSeam::DependencyGettableData>(methodCallData).easy_ArgValue == "4242";   
                }), FSeam::AtMost{3});
            EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](std::any &methodCallData) {
                return std::any_cast<FSeam::DependencyGettableData>(methodCallData).simple_ArgValue == 42 &&
                        std::any_cast<FSeam::DependencyGettableData>(methodCallData).easy_ArgValue == "4242";   
                }), FSeam::VerifyCompare{5});

        } // End section : Check all argument

        /**
         * Use generated helper functions
         */
        SECTION ("RECOMMENDED USAGE : Check arguments") {
            // TODO

        } // End section : RECOMMENDED USAGE : Check arguments

    } // End section : Test FSeam::Verify on Arguments

    SECTION ("Test FSeam::Verify on Return Values") {
        EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLERETURNVALUE, FSeam::NeverCalled{}));
        testingClass.execute(); // called once in total
        EXPECT(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKSIMPLERETURNVALUE));


    } // End section : Test FSeam::Verify on Return Values

    FSeam::MockVerifier::cleanUp();
} // End Test_Case : FSeamBasicTest


