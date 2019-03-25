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
   
    SECTION("Test behavior of default for NonGettable Mock") {
        auto fseamMock = FSeam::getDefault<source::DependencyNonGettable>();


    } // End section : Test behavior of default for NonGettable Mock

    SECTION("Test behavior of default for Gettable Mock") {
        auto fseamMock = FSeam::getDefault<source::DependencyGettable>();

    } // End section : Test behavior of default for Gettable Mock

    FSeam::MockVerifier::cleanUp();
} // End Test_Case : FSeamDefaultMockTest


