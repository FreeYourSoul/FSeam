//
// Created by FyS on 3/27/19.
//

#include <catch2/catch.hpp>
#include <any>
#include <TestingClass.hh>
#include <FSeamMockData.hpp>

using namespace FSeam;

TEST_CASE("Test HelperMethods Simple UseCase") {
    source::TestingClass testClass{};
    auto fseamMock = FSeam::get(&testClass.getDepGettable());

    SECTION("Test DupeReturn") {
        fseamMock->dupeReturn<FSeam::DependencyGettable::checkSimpleReturnValue>(666);

        SECTION("Simple value") {
           REQUIRE(666 == testClass.getDepGettable().checkSimpleReturnValue());
        } // End section : Simple value

        SECTION("Test Composition override") {
            fseamMock->dupeReturn<FSeam::DependencyGettable::checkSimpleReturnValue>(668);
            fseamMock->dupeReturn<FSeam::DependencyGettable::checkSimpleReturnValue>(42);
            fseamMock->dupeReturn<FSeam::DependencyGettable::checkSimpleReturnValue>(1337);
            REQUIRE(1337 == testClass.getDepGettable().checkSimpleReturnValue());

        } // End section : Test Composition override

        SECTION("Custom struct/class") {
            source::StructTest testingStruct {42, 1337, "FyS"};
            fseamMock->dupeReturn<FSeam::DependencyGettable::checkCustomStructReturnValue>(testingStruct);
            REQUIRE(testingStruct.testInt == testClass.getDepGettable().checkCustomStructReturnValue().testInt);
            REQUIRE(testingStruct.testShort == testClass.getDepGettable().checkCustomStructReturnValue().testShort);
            REQUIRE(testingStruct.testStr == testClass.getDepGettable().checkCustomStructReturnValue().testStr);

        } // End section : Custom struct/class

    } // End section : Test DupeReturn

    SECTION("Clear expectations") {
        fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Any(), Any(), NeverCalled{});
        testClass.getDepGettable().checkSimpleInputVariable(41, "FyS");

        REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, false));
        fseamMock->clearExpectations();
        REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));
    } // End section : Clear expectations

    SECTION("Test ExpectArg") {

        SECTION("Any Comparator && Occurences") {
            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Any(), Any(), VerifyCompare{4});
            testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
            testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
            testClass.getDepGettable().checkSimpleInputVariable(33, "FyS");
            testClass.getDepGettable().checkSimpleInputVariable(41, "FyS");

            // overall check is only on 1 but 4 happened
            REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, 1, false));
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));
            fseamMock->clearExpectations();

            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Any(), Any(), AtLeast(5)); // false
            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Any(), Any(), AtLeast(2)); // true
            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Any(), Any(), AtMost(5));  // true
            testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
            testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
            testClass.getDepGettable().checkSimpleInputVariable(33, "FyS");
            testClass.getDepGettable().checkSimpleInputVariable(409, "FWdw");
            REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, false));
            testClass.getDepGettable().checkSimpleInputVariable(11, "FWdwwa");
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, 9)); // works as now 5 call are done
            testClass.getDepGettable().checkSimpleInputVariable(14, "psodw"); // fail as now more than 5
            REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, false));

        } // End section : Integral Comparator

        SECTION("Eq Comparator") {
            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Eq(42), Eq(std::string("4242")));
            REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, false));
            testClass.execute();
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));

            SECTION("Success Equal ") {
                fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Eq(29), Any(), VerifyCompare{2});
                testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
                testClass.getDepGettable().checkSimpleInputVariable(29, "dode");
                testClass.getDepGettable().checkSimpleInputVariable(33, "FyS");
                testClass.getDepGettable().checkSimpleInputVariable(41, "FyS");
                REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));
            } // End section : Success Equal

            SECTION("Failing Equal one arg false") {
                fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Eq(42), Eq(std::string("FALSE")));
                testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
                testClass.getDepGettable().checkSimpleInputVariable(29, "dodo");
                testClass.getDepGettable().checkSimpleInputVariable(33, "FyS");
                testClass.getDepGettable().checkSimpleInputVariable(41, "FyS");
                REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, false));

            } // End section : Failing Equal

            SECTION("Failing Equal all arg false") {
                fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Eq(1234), Eq(std::string("FALSE")));
                testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
                testClass.getDepGettable().checkSimpleInputVariable(29, "dodo");
                testClass.getDepGettable().checkSimpleInputVariable(33, "FyS");
                testClass.getDepGettable().checkSimpleInputVariable(41, "FyS");
                REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, false));

            } // End section : Failing Equal

            SECTION("Failing Expected occurence") {
                fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Eq(41), Eq(std::string("FyS")), NeverCalled{});
                testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
                testClass.getDepGettable().checkSimpleInputVariable(29, "dodo");
                testClass.getDepGettable().checkSimpleInputVariable(33, "FyS");
                testClass.getDepGettable().checkSimpleInputVariable(41, "FyS");
                REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, false));

            } // End section : Failing Equal

        } // End section : Integral Comparator

        SECTION("NotEq Comparator") {
            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(NotEq(42), NotEq(std::string("FyS")));
            testClass.getDepGettable().checkSimpleInputVariable(42, "dede"); // true , false    = false
            testClass.getDepGettable().checkSimpleInputVariable(42, "dodo"); // true, false     = false
            testClass.getDepGettable().checkSimpleInputVariable(46, "FyS");  // false, false    = false
            testClass.getDepGettable().checkSimpleInputVariable(33, "FyS");  // true, false     = false
            testClass.getDepGettable().checkSimpleInputVariable(41, "FyS");  // true, false     = false
            REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, 5, false));
            testClass.getDepGettable().checkSimpleInputVariable(456, "adad"); // true, true = true
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, 6));

        } // End section : Integral Comparator

        SECTION("Custom Comparator") {

            SECTION("Basic usage") {
                fseamMock->expectArg<FSeam::DependencyGettable::checkCustomStructInputVariable>(
                    CustomComparator<source::StructTest>([](auto test){
                        return test.testInt == 1 &&
                            test.testShort == 11 &&
                            test.testStr == "111";
                    }));
                testClass.getDepGettable().checkCustomStructInputVariable(source::StructTest{1, 11, "111"});
                REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCustomStructInputVariable::NAME, 1));

            } // End section : Basic usage

            SECTION("Failure on Custom Comparator") {
                fseamMock->expectArg<FSeam::DependencyGettable::checkCustomStructInputVariable>(
                    CustomComparator<source::StructTest>([](auto test){
                        return test.testInt == 1 &&
                            test.testShort == 42 && // short is not 42
                            test.testStr == "111";
                    }));
                testClass.getDepGettable().checkCustomStructInputVariable(source::StructTest{1, 11, "111"});
                REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkCustomStructInputVariable::NAME, 1, false));

            } // End section : Failure on Custom Comparator

            SECTION("Multiple Custom Comparator") {
                bool checkFirstLayer = false;
                bool checkSecondLayer = false;
                bool checkThirdLayer = false;
                fseamMock->expectArg<FSeam::DependencyGettable::checkCustomStructInputVariable>(
                    CustomComparator<source::StructTest>([&checkFirstLayer](auto test){
                        checkFirstLayer = true;
                        return test.testInt == 1;
                    }));
                fseamMock->expectArg<FSeam::DependencyGettable::checkCustomStructInputVariable>(
                    CustomComparator<source::StructTest>([&checkSecondLayer](auto test){
                        checkSecondLayer = true;
                        return test.testShort == 11;
                    }));
                fseamMock->expectArg<FSeam::DependencyGettable::checkCustomStructInputVariable>(
                    CustomComparator<source::StructTest>([&checkThirdLayer](auto test){
                        checkThirdLayer = true;
                        return test.testStr == "111";
                    }));

                testClass.getDepGettable().checkCustomStructInputVariable(source::StructTest{1, 11, "111"});
                REQUIRE(checkFirstLayer);
                REQUIRE(checkSecondLayer);
                REQUIRE(checkThirdLayer);
                REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCustomStructInputVariable::NAME, 1));

            } // End section : Multiple Custom Comparator+

        } // End section : Custom Comparator

        SECTION("Multiple expectations") {
            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Eq(29), Any(), VerifyCompare{2});
            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Eq(29), _, VerifyCompare{2}); // wildcard test
            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Eq(29), Eq(std::string("dede")), AtMost{1});
            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Any(),  Eq(std::string("FyS")), AtLeast{1});
            testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
            testClass.getDepGettable().checkSimpleInputVariable(29, "dode");
            testClass.getDepGettable().checkSimpleInputVariable(33, "FyS");
            testClass.getDepGettable().checkSimpleInputVariable(41, "FyS");
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, 4));
            testClass.getDepGettable().checkSimpleInputVariable(29, "dede"); // AtMost(1) not respected
            REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, 5, false));
            
        } // End section : Multiple expectation

    } // End section : Test ExpectArg

    FSeam::MockVerifier::cleanUp();
} // End TestCase : Test HelperMethods Simple Function

TEST_CASE("Test HelperMethods Specific UseCase") {
    source::TestingClass testClass{};
    auto fseamMock = FSeam::get(&testClass.getDepGettable());

    SECTION("Reference manipulation") {
        source::StructTest structTest {1, 11, "111"};

        SECTION("Check args") {
            fseamMock->expectArg<FSeam::DependencyGettable::checkCustomStructInputVariableRef>(
                    FSeam::CustomComparator<const source::StructTest &>([](auto param) {
                        return param.testInt == 1 &&
                            param.testShort == 11 &&
                            param.testStr == "111";
                    }));
            testClass.getDepGettable().checkCustomStructInputVariableRef(structTest);
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCustomStructInputVariableRef::NAME));

            SECTION("Not Matching Arg check") {
                fseamMock->expectArg<FSeam::DependencyGettable::checkCustomStructInputVariableRef>(
                        FSeam::CustomComparator<const source::StructTest &>([](auto param) {
                            return param.testInt == 1 &&
                                   param.testShort == 11 &&
                                   param.testStr == "42";
                        }), FSeam::NeverCalled{});
                testClass.getDepGettable().checkCustomStructInputVariableRef(structTest);
                REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCustomStructInputVariableRef::NAME));

            } // End section : Not Matching Arg check

        } // End section : Check args

        SECTION("Check return") {
            fseamMock->dupeReturn<FSeam::DependencyGettable::checkCustomStructReturnValueRef>(structTest);
            REQUIRE(1 == testClass.getDepGettable().checkCustomStructReturnValueRef().testInt);
            REQUIRE(11 == testClass.getDepGettable().checkCustomStructReturnValueRef().testShort);
            REQUIRE("111" == testClass.getDepGettable().checkCustomStructReturnValueRef().testStr);
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCustomStructReturnValueRef::NAME, 3));

            source::StructTest structTest2 = {2, 22, "222"};
            fseamMock->dupeReturn<FSeam::DependencyGettable::checkCustomStructReturnValueRef>(structTest2);
            REQUIRE(2 == testClass.getDepGettable().checkCustomStructReturnValueRef().testInt);
            REQUIRE(22 == testClass.getDepGettable().checkCustomStructReturnValueRef().testShort);
            REQUIRE("222" == testClass.getDepGettable().checkCustomStructReturnValueRef().testStr);
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCustomStructReturnValueRef::NAME, 6));

            source::StructTest structTest3 = {3, 33, "333"};
            fseamMock->dupeReturn<FSeam::DependencyGettable::checkCustomStructReturnValueRef>(structTest3);
            REQUIRE(3 == testClass.getDepGettable().checkCustomStructReturnValueRef().testInt);
            REQUIRE(33 == testClass.getDepGettable().checkCustomStructReturnValueRef().testShort);
            REQUIRE("333" == testClass.getDepGettable().checkCustomStructReturnValueRef().testStr);
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCustomStructReturnValueRef::NAME, 9));

        } // End section : Check return

    } // End section : Reference manipulation

    SECTION("Pointer manipulation") {
        source::StructTest structTest {1, 11, "111"};

        SECTION("Check args") {
            fseamMock->expectArg<FSeam::DependencyGettable::checkCustomStructInputVariablePtr>(
                    FSeam::CustomComparator<source::StructTest *>([](auto param) {
                        return param->testInt == 1 &&
                               param->testShort == 11 &&
                               param->testStr == "111";
                    }));
            testClass.getDepGettable().checkCustomStructInputVariablePtr(&structTest);
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCustomStructInputVariablePtr::NAME, 1));

            SECTION("Not Matching Arg check") {
                fseamMock->expectArg<FSeam::DependencyGettable::checkCustomStructInputVariablePtr>(
                        FSeam::CustomComparator<source::StructTest *>([](auto param) {
                            return param->testInt == 1 &&
                                   param->testShort == 11 &&
                                   param->testStr == "42";
                        }), FSeam::NeverCalled{});
                testClass.getDepGettable().checkCustomStructInputVariablePtr(&structTest);
                REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCustomStructInputVariablePtr::NAME, 2));

            } // End section : Not Matching Arg check

        } // End section : Check args

        SECTION("Check return") {
            fseamMock->dupeReturn<FSeam::DependencyGettable::checkCustomStructReturnValuePtr>(&structTest);
            REQUIRE(1 == testClass.getDepGettable().checkCustomStructReturnValuePtr()->testInt);
            REQUIRE(11 == testClass.getDepGettable().checkCustomStructReturnValuePtr()->testShort);
            REQUIRE("111" == testClass.getDepGettable().checkCustomStructReturnValuePtr()->testStr);
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCustomStructReturnValuePtr::NAME, 3));

            source::StructTest structTest2 = {2, 22, "222"};
            fseamMock->dupeReturn<FSeam::DependencyGettable::checkCustomStructReturnValuePtr>(&structTest2);
            REQUIRE(2 == testClass.getDepGettable().checkCustomStructReturnValuePtr()->testInt);
            REQUIRE(22 == testClass.getDepGettable().checkCustomStructReturnValuePtr()->testShort);
            REQUIRE("222" == testClass.getDepGettable().checkCustomStructReturnValuePtr()->testStr);
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCustomStructReturnValuePtr::NAME, 6));

            source::StructTest structTest3 = {3, 33, "333"};
            fseamMock->dupeReturn<FSeam::DependencyGettable::checkCustomStructReturnValuePtr>(&structTest3);
            REQUIRE(3 == testClass.getDepGettable().checkCustomStructReturnValuePtr()->testInt);
            REQUIRE(33 == testClass.getDepGettable().checkCustomStructReturnValuePtr()->testShort);
            REQUIRE("333" == testClass.getDepGettable().checkCustomStructReturnValuePtr()->testStr);
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCustomStructReturnValuePtr::NAME, 9));

        } // End section : Check return

    } // End section : Test HelperMethods CustomObject UseCase

    SECTION("Non movable Object manipulation") {

    } // End section : Non movable Object manipulation

    SECTION("Non copiable Object manipulation") {

    } // End section : Non copiable Object manipulation

    FSeam::MockVerifier::cleanUp();
} // End TestCase : Test HelperMethods Specific UseCase

