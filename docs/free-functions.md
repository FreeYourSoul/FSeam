# Free Functions

In this documentation we call a free function is either a C-style function (standalone), or a static method of a class.

Those are usually hard to mock with classical mocking framework, making some code hard to test.

## How to mock free functions

In order to mock free function, you just need to get the FSeam mock handler. After, everything [works the same](testing.md#verifications) than for classic class method.

```cpp
#include <FSeam.hpp>

auto fseamFreeFunctionMock = FSeam::getFreeFunc();
```

Internally, this Handler will be linked with a class called FSeam::FreeFunction, which is the reason all the ClassMethodIdentifier (for [expectArg](testing.md#argument-expectation) or [dupeReturn](testing.md#dupe-return-values)) is in the FreeFunction namespace.

_Example:_
```cpp
// headerMocked.hh
int freeFunctionWithInReturned();

// test.cpp
#include <catch2/catch.hpp>
#include <FSeam.hpp>
#include <FSeamMockData.hpp> // contains ClassMethodIndentifiers 

TEST_CASE("test free function") {
    auto fseamFreeFunctionMock = FSeam::getFreeFunc();
    
    fseamFreeFunctionMock->dupeReturn<FSeam::FreeFunction::freeFunctionWithIntReturn>(42);
    
    REQUIRE(42 == freeFunctionWithIntReturned()); // Yeahhh works
}

```
