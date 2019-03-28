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

TEST_CASE("Test HelperMethods Simple UseCase") {
    source::TestingClass testClass{};
    auto fseamMock = FSeam::get(&testingClass.getDepGettable());

    SECTION("Test dupe") {
        bool dupeCalled = false;

        SECTION("Dupe simple") {
            fseamMock->dupe<FSeam::TestingClass::checkCalled>([&dupeCalled]() {
                dupeCalled = true;
            });
            REQUIRE_FALSE(dupeCalled);

        } // End section : Dupe simple

        SECTION ("Dupe with arguments") {
            fseamMock->dupe<FSeam::TestingClass::checkSimpleInputVariable>([&dupeCalled](int simple, std::string easy) {
                dupeCalled = true;
                CHECK(42 == simple);
                CHECK("4242" == easy);
            });
            REQUIRE_FALSE(dupeCalled);

        } // End section : Dupe with arguments

        SECTION("Dupe with return value") {
            fseamMock->dupe<FSeam::TestingClass::checkSimpleReturnValue>([&dupeCalled]() {
                dupeCalled = true;
                return 1337;
            });
            REQUIRE_FALSE(dupeCalled);  
            CHECK(1337 == testClass.checkSimpleReturnValue());

        } // End section : Dupe simple

        testClass.execute();
        REQUIRE(dupeCalled);

    } // End section : Test dupe


    SECTION("Test DupeReturn") {
        fseamMock->dupeReturn<FSeam::TestingClass::checkSimpleReturnValue>(666);
        REQUIRE_FALSE(dupeCalled);  
        CHECK(666 == testClass.checkSimpleReturnValue());
        REQUIRE_TRUE(dupeCalled);
    }

    SECTION("Test ExpectArg") {

    }


    SECTION("Test Overrides") {

        SECTION("ExpectArg and DupeReturn composition") {

        }

        SECTION("Dupe override") {

        }


    }



} // End TestCase : Test HelperMethods Simple Function

TEST_CASE("Test HelperMethods CustomObject UseCase") {

    SECTION("Pointer manipulation") {

    } // End section : Test HelperMethods CustomObject UseCase

    SECTION("Non movable Object manipulation") {

    } // End section : Non movable Object manipulation

    SECTION("Non copiable Object manipulation") {

    } // End section : Non copiable Object manipulation

} // End TestCase : Test HelperMethods CustomObject UseCase

