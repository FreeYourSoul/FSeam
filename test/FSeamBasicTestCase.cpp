//
// Created by FyS on 3/24/19.
//

#include <catch.hpp>
#include <MockVerifier.hpp>
#include <TestingClass.hh>
#include <MockData.hpp>
#include <iostream>

TEST_CASE( "FSeamBasicTest", "[basic]" ) {
    source::TestingClass testingClass {};

    SECTION("Test hasOriginalServiceBeenCalled") {
        testingClass.execute();
        REQUIRE_FALSE(testingClass.getDepGettable().hasOriginalServiceBeenCalled());
    }

    SECTION("Test mocking on checkCalled") {
        auto *fseamMock = &testingClass.getDepGettable();
        auto ok = FSeam::get(fseamMock);

        ok->dupeMethod(FSeam::DependencyGettable_FunName::CHECKSIMPLEINPUTVARIABLE, [](void *data) {
            static_cast<FSeam::DependencyGettableData *>(data)->checkSimpleInputVariable_simple_ParamValue = nullptr;
        });
    }

}
