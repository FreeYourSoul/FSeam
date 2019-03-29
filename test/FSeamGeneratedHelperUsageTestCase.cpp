//
// Created by FyS on 3/27/19.
//

//FSeam::DupeReturnMethod::DependencyGettable::checkSimpleReturn(42);
// fseam->dupeReturn<DependencyGettable::checkSimpleReturn>(42);

//FSeam::DupeMethod::DependencyGettable::checkInputVariable([](int i, std::string str){
//
//});
//FSeam::Verify::DependencyGettable::checkSimpleInputVariable(1, "dede");

#include <catch.hpp>
#include <any>
#include <TestingClass.hh>
#include <MockData.hpp>

using namespace FSeam;

TEST_CASE("Test HelperMethods Simple UseCase") {
    source::TestingClass testClass{};
    auto fseamMock = FSeam::get(&testClass.getDepGettable());

    SECTION("Test DupeReturn") {
        fseamMock->dupeReturn<FSeam::DependencyGettable::checkSimpleReturnValue>(666);
        CHECK(666 == testClass.getDepGettable().checkSimpleReturnValue());

    } // End section : Test DupeReturn

    SECTION("Clear expectations") {
        fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Any(), Any(), NeverCalled{});
        testClass.getDepGettable().checkSimpleInputVariable(41, "FyS");

        REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));
        fseamMock->clearExpectations();
        REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));
    }

    SECTION("Test ExpectArg") {

        SECTION("Any Comparator && Occurences") {
            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Any(), Any(), VerifyCompare{4});
            testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
            testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
            testClass.getDepGettable().checkSimpleInputVariable(33, "FyS");
            testClass.getDepGettable().checkSimpleInputVariable(41, "FyS");

            // overall check is only on 1 but 4 hapenned
            REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, 1));
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));
            fseamMock->clearExpectations();

            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Any(), Any(), AtLeast(5)); // false
            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Any(), Any(), AtLeast(2)); // true
            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Any(), Any(), AtMost(5));  // true
            testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
            testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
            testClass.getDepGettable().checkSimpleInputVariable(33, "FyS");
            testClass.getDepGettable().checkSimpleInputVariable(409, "FWdw");
            REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));
            testClass.getDepGettable().checkSimpleInputVariable(11, "FWdwwa");
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, 9)); // works as now 5 call are done
            testClass.getDepGettable().checkSimpleInputVariable(14, "psodw"); // fail as now more than 5
            REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));

        } // End section : Integral Comparator

        SECTION("Eq Comparator") {
            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Eq(42), Eq(std::string("4242")));
            REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));
            testClass.execute();
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME, 1));

            SECTION("Success Equal ") {
                fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Eq(29), Any(), VerifyCompare{2});
                testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
                testClass.getDepGettable().checkSimpleInputVariable(29, "dode");
                testClass.getDepGettable().checkSimpleInputVariable(33, "FyS");
                testClass.getDepGettable().checkSimpleInputVariable(41, "FyS");
                REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));
            }

            SECTION("Failing Equal one arg false") {
                fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Eq(42), Eq(std::string("FALSE")));
                testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
                testClass.getDepGettable().checkSimpleInputVariable(29, "dodo");
                testClass.getDepGettable().checkSimpleInputVariable(33, "FyS");
                testClass.getDepGettable().checkSimpleInputVariable(41, "FyS");
                REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));

            } // End section : Failing Equal

            SECTION("Failing Equal all arg false") {
                fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Eq(1234), Eq(std::string("FALSE")));
                testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
                testClass.getDepGettable().checkSimpleInputVariable(29, "dodo");
                testClass.getDepGettable().checkSimpleInputVariable(33, "FyS");
                testClass.getDepGettable().checkSimpleInputVariable(41, "FyS");
                REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));

            } // End section : Failing Equal

            SECTION("Failing Expected occurence") {
                fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Eq(41), Eq(std::string("FyS")), NeverCalled{});
                testClass.getDepGettable().checkSimpleInputVariable(29, "dede");
                testClass.getDepGettable().checkSimpleInputVariable(29, "dodo");
                testClass.getDepGettable().checkSimpleInputVariable(33, "FyS");
                testClass.getDepGettable().checkSimpleInputVariable(41, "FyS");
                REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));

            } // End section : Failing Equal

        } // End section : Integral Comparator

        SECTION("NotEq Comparator") {


        } // End section : Integral Comparator

        SECTION("Custom Comparator") {

        }

        SECTION("Multiple expectations") {


        } // End section : Multiple expectation

    } // End section : Test ExpectArg

    SECTION("Test Composition") {

    } // End section : Test Composition

    FSeam::MockVerifier::cleanUp();
} // End TestCase : Test HelperMethods Simple Function

TEST_CASE("Test HelperMethods CustomObject UseCase") {

    SECTION("Pointer manipulation") {

    } // End section : Test HelperMethods CustomObject UseCase

    SECTION("Non movable Object manipulation") {

    } // End section : Non movable Object manipulation

    SECTION("Non copiable Object manipulation") {

    } // End section : Non copiable Object manipulation

    FSeam::MockVerifier::cleanUp();
} // End TestCase : Test HelperMethods CustomObject UseCase

