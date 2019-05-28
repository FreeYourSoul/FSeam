//
// Created by FyS on 4/3/19.
//

#include <catch2/catch.hpp>
#include <FSeamMockData.hpp>
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

TEST_CASE("Test static method mock") {
    auto mockStaticFunc = FSeam::getFreeFunc();

    SECTION("simple call") {
        source::FreeFunctionClass::staticFunction();
        CHECK(mockStaticFunc->verify(FSeam::FreeFunction::staticFunction::NAME, 1));
    } // End section : simple call

    SECTION("Test static method dupe") {
        bool hasBeenCalled = false;
        mockStaticFunc->dupeMethod(FSeam::FreeFunction::staticFunction::NAME, [&hasBeenCalled](void *data) {
            hasBeenCalled = true;
            });

        source::FreeFunctionClass::staticFunction();
        REQUIRE(hasBeenCalled);
        REQUIRE(mockStaticFunc->verify(FSeam::FreeFunction::staticFunction::NAME, 1));

    } // End section : Test static method dupe

    SECTION("Multiple call") {
        source::FreeFunctionClass::staticFunction();
        REQUIRE(mockStaticFunc->verify(FSeam::FreeFunction::staticFunction::NAME, 1));
        source::FreeFunctionClass::staticFunction();
        REQUIRE(mockStaticFunc->verify(FSeam::FreeFunction::staticFunction::NAME, 2));
        source::FreeFunctionClass::staticFunction();
        REQUIRE(mockStaticFunc->verify(FSeam::FreeFunction::staticFunction::NAME, 3));
    } // End section : Multiple call

    FSeam::MockVerifier::cleanUp();
} // End TestCase : Test static method mock

TEST_CASE("Test FreeFunction") {
    auto mockFreeFunc = FSeam::getFreeFunc();

    SECTION("simple call") {
        source::freeFunctionSimple();
        CHECK(mockFreeFunc->verify(FSeam::FreeFunction::freeFunctionSimple::NAME, 1));
        source::freeFunctionSimple();
        source::freeFunctionSimple();
        source::freeFunctionSimple();
        source::freeFunctionSimple();
        CHECK(mockFreeFunc->verify(FSeam::FreeFunction::freeFunctionSimple::NAME, 5));

    } // End section : simple call

    SECTION("Test free function method dupe") {
        bool hasBeenCalled = false;
        mockFreeFunc->dupeMethod(FSeam::FreeFunction::freeFunctionSimple::NAME, [&hasBeenCalled](void *data) {
            hasBeenCalled = true;
        });

        source::freeFunctionSimple();
        REQUIRE(hasBeenCalled);
        REQUIRE(mockFreeFunc->verify(FSeam::FreeFunction::freeFunctionSimple::NAME, 1));

    } // End section : Test static method dupe

    SECTION("Dupe return value") {
        mockFreeFunc->dupeReturn<FSeam::FreeFunction::freeFunctionReturn>(4242);
        REQUIRE(4242 == source::freeFunctionReturn());
        REQUIRE(mockFreeFunc->verify(FSeam::FreeFunction::freeFunctionReturn::NAME, 1));

    } // End section : Dupe return value

    SECTION("Argument expectation") {
        using namespace FSeam;
        mockFreeFunc->expectArg<FSeam::FreeFunction::freeFunctionWithArguments>(Eq(42), Eq(1337), Eq('f'), VerifyCompare{2});
        mockFreeFunc->expectArg<FSeam::FreeFunction::freeFunctionWithArguments>(Eq(42), Any(), Any(), VerifyCompare{4});
        source::freeFunctionWithArguments(42, 1337, 'f');
        source::freeFunctionWithArguments(42, 1337, 'a');
        source::freeFunctionWithArguments(42, 1, 'f');
        source::freeFunctionWithArguments(1, 1337, 'f');
        source::freeFunctionWithArguments(42, 1337, 'f');
        REQUIRE(mockFreeFunc->verify(FSeam::FreeFunction::freeFunctionWithArguments::NAME, 5));

    } // End section : Argument expectation

    FSeam::MockVerifier::cleanUp();
} // End TestCase : Test FreeFunction