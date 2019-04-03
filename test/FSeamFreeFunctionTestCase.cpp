//
// Created by FyS on 4/3/19.
//

#include <catch.hpp>
#include <MockData.hpp>
#include <TestingClass.hh>
#include <FreeFunctionClass.hh>

using namespace FSeam;

/**
 * Bonus test in this test case (out of the scope of FreeFunctions)
 *
 * As this test file is compiled in a separated target than other, the instance of TestingClass should be using original
 * implementation of the classes
 */
TEST_CASE("Test Non Mocked type") {

    source::TestingClass testingClass{};

    testingClass.execute();
    REQUIRE(888 == testingClass.getDepGettable().checkSimpleReturnValue());
    REQUIRE(888 == testingClass.getDepGettable().checkCustomStructReturnValue().testInt);
    REQUIRE(999 == testingClass.getDepGettable().checkCustomStructReturnValue().testShort);
    REQUIRE(std::string("tttt") == testingClass.getDepGettable().checkCustomStructReturnValue().testStr);
    REQUIRE(testingClass.getDepGettable().hasOriginalServiceBeenCalled());

} // End TestCase : Test Non Mocked type

TEST_CASE("Test FreeFunction") {
    auto mockFreeFunc = FSeam::getDefault<source::FreeFunctionClass>();

    SECTION("Test static method mock") {

        source::FreeFunctionClass::staticFunction();
        REQUIRE(mockFreeFunc->verify(FSeam::FreeFunctionClass::staticFunction::NAME, 1));

        SECTION("Test static method dupe") {
            bool hasBeenCalled = false;
            mockFreeFunc->dupeMethod(FSeam::FreeFunctionClass::staticFunction::NAME, [&hasBeenCalled](void *data) {
                hasBeenCalled = true;
            });

            source::FreeFunctionClass::staticFunction();
            REQUIRE(hasBeenCalled);
            REQUIRE(mockFreeFunc->verify(FSeam::FreeFunctionClass::staticFunction::NAME, 2));

        } // End section : Test static method dupe

    } // End section : Test static method mock

} // End TestCase : Test FreeFunction