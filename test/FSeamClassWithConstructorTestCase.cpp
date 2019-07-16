//
// Created by FyS on 3/24/19.
//

#include <catch2/catch.hpp>
#include <iostream>
#include <any>
#include <FSeam.hpp>
#include <TestingClass.hh>
#include <FSeamMockData.hpp>

TEST_CASE( "ClassWithConstructor" ) {
    source::TestingClass testingClass {};
    auto fseamMock = FSeam::getDefault<source::ClassWithConstructor>();

    fseamMock->dupeReturn<FSeam::ClassWithConstructor::mockedMethod>(42);

    SECTION("Check Mocked Constructor") {

        REQUIRE(42 == testingClass.instantiateClassWithConstructorAndRetFromMockedMethd());
        REQUIRE(fseamMock->verify(FSeam::ClassWithConstructor::ClassWithConstructor::NAME, 1));

    } // End section : RAW USAGE : set mock return value

    SECTION("Check Mocked Destructor") {

        REQUIRE(42 == testingClass.instantiateClassWithConstructorAndRetFromMockedMethd());
        REQUIRE(fseamMock->verify(FSeam::ClassWithConstructor::Destructor_ClassWithConstructor::NAME, 1));

    } // End section : RAW USAGE : set mock return value

    FSeam::MockVerifier::cleanUp();

} // End Test_Case : FSeamBasicTest
