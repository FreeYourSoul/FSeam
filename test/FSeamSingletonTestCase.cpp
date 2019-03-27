//
// Created by FyS on 3/24/19.
//

#include <catch.hpp>
#include <iostream>
#include <any>
#include <TestingClass.hh>
#include <MockData.hpp>

TEST_CASE("Test Singleton") {
    source::TestingClass testingClass {};
    REQUIRE_FALSE(FSeam::MockVerifier::instance().isMockRegistered(&testingClass.getDepGettable()));
    auto fseamMock = FSeam::get(&testingClass.getDepGettable());
    REQUIRE(FSeam::MockVerifier::instance().isMockRegistered(&testingClass.getDepGettable()));
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

    SECTION("Test multiple get") {
        testingClass.execute();
        testingClass.execute();
        fseamMock = FSeam::get(&testingClass.getDepGettable());
        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, 2));
        fseamMock = FSeam::get(&testingClass.getDepGettable());
        testingClass.execute();
        testingClass.execute();
        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, 4));
        fseamMock = FSeam::get(&testingClass.getDepGettable());
        testingClass.execute();
        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, 5));
        fseamMock = FSeam::get(&testingClass.getDepGettable());
        fseamMock = FSeam::get(&testingClass.getDepGettable());
        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, 5));
        CHECK(555 == valueChanging);

    } // End section : Test multiple get

    SECTION("FSeam::MockVerifier::cleanup") {
        FSeam::MockVerifier::cleanUp(); // Cleanup, remove all alternation of mocks : valueChanging won't change
        fseamMock = FSeam::get(&testingClass.getDepGettable()); // Need to reset the shared ptr as it has been cleaned up
        fseamMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKCALLED, [&valueChanging](void *dataStruct) {
                valueChanging += 1;
            }, true); // valueChanging = 1

        REQUIRE(0 == valueChanging);
        testingClass.execute();
        CHECK(fseamMock->verify(FSeam::DependencyGettable_FunName::CHECKCALLED, 1));
        CHECK_FALSE(111 == valueChanging);
        CHECK(1 == valueChanging);

    } // End section : FSeam::MockVerifier::cleanup

    SECTION("Test Override dupe") {
        REQUIRE(0 == valueChanging);
        testingClass.execute();
        CHECK(111 == valueChanging);

        fseamMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKCALLED, [&valueChanging](void *dataStruct) {
            valueChanging = 1337;
        });
        fseamMock->dupeMethod(FSeam::DependencyGettable_FunName::CHECKCALLED, [&valueChanging](void *dataStruct) {
            valueChanging = 42;
        }); // dupe is not composed (no last argument on dupeMethod) so the 1337 dupe is override by this 42 one

        REQUIRE(111 == valueChanging);
        testingClass.execute();
        CHECK(42 == valueChanging);

    } // End section : Test Override dupe

    FSeam::MockVerifier::cleanUp();
} // End TestCase : Test Singleton
