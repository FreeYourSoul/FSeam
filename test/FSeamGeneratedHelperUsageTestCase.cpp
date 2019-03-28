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

    SECTION("Test ExpectArg") {

        SECTION("Eq Comparator") {
            fseamMock->expectArg<FSeam::DependencyGettable::checkSimpleInputVariable>(Eq(42), Eq(std::string("4242")));
            REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));
            testClass.execute();
            REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkSimpleInputVariable::NAME));
        } // End section : Integral Comparator

        SECTION("NotEq Comparator") {


        } // End section : Integral Comparator

        SECTION("Any Comparator") {


        } // End section : Integral Comparator

        SECTION("Custom Comparator") {

        }

        SECTION("Multiple expectation") {


        } // End section : Multiple expectation

    } // End section : Test ExpectArg

    SECTION("Test Composition") {

    } // End section : Test Composition


} // End TestCase : Test HelperMethods Simple Function

TEST_CASE("Test HelperMethods CustomObject UseCase") {

    SECTION("Pointer manipulation") {

    } // End section : Test HelperMethods CustomObject UseCase

    SECTION("Non movable Object manipulation") {

    } // End section : Non movable Object manipulation

    SECTION("Non copiable Object manipulation") {

    } // End section : Non copiable Object manipulation

} // End TestCase : Test HelperMethods CustomObject UseCase

