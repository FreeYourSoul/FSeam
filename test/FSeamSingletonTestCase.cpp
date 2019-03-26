//
// Created by FyS on 3/24/19.
//

#include <catch.hpp>
#include <iostream>
#include <any>
#include <MockVerifier.hpp>
#include <TestingClass.hh>
#include <MockData.hpp>

TEST_CASE("Test Singleton", "[base]") {
    source::TestingClass testingClass {};
    auto fseamMock = FSeam::get(&testingClass.getDepGettable());
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
        testingClass.execute();
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

} // End TestCase : Test Singleton
