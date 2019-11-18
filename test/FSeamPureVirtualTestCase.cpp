//
// Created by FyS on 3/24/19.
//

#include <catch2/catch.hpp>
#include <iostream>
#include <any>
#include <FSeam.hpp>
#include <TestingClass.hh>
#include <FSeamMockData.hpp>

class TestingClass : public AbstractClass {

public:
    bool pureVirtualMethod() override {
        return nonPureVirtualMethod();
    }
};

TEST_CASE( "FSeam PureVirtual" ) {
    auto fseamMock = FSeam::getDefault<AbstractClass>();
    std::unique_ptr<AbstractClass> c = std::make_unique<TestingClass>();

    SECTION("Test nonPureVirtualMethod called") {
        fseamMock->dupeReturn<FSeam::AbstractClass::nonPureVirtualMethod>(true);
        REQUIRE(c->nonPureVirtualMethod());

    } // End section : Test nonPureVirtualMethod called

    FSeam::MockVerifier::cleanUp();

} // End Test_Case : FSeam PureVirtual
