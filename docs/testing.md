<a id="top"></a>
# How to use FSeam

This page is going to try to give the most detailed way possible of how you can use FSeam in your project.

The test below are using the asserting macro of the Catch2 testing framework.    
If you need any more usage example than this, please request them by [opening a new issue](https://github.com/FreeYourSoul/FSeam/issues) or just look at [the existing tests](https://github.com/FreeYourSoul/FSeam/tree/master/test) which are quite complete and good samples.

> To use the functionality below, you need to include FSeam.hpp which contains the mocking tools.  
>  For low level usage (dupe), including FSeamMockData.hpp could be required

## Verifications

< todo >  
[you can use the any of the called comparator](testing.md#called-comparator)

## Argument Expectation

The mock object used into test has a ```expectArg``` method that makes you able to check with what arguments the function has been called.
It is also possible to check how many times the function met this requirement, for that [you can use the any of the called comparator](testing.md#called-comparator).  
There are 4 argument comparator available in FSeam available by calling the following functions: 
* FSeam::Eq comparator : signature ```template<typename T> FSeam::Eq(T)```
Takes any object as argument, if the object type cannot be deduced, you need to add the template parameter explicitly.  
This comparator is checking that the given parameter is equal to the argument  
* FSeam::NotEq comparator :  signature ```template<typename T> FSeam::NotEq(T)```  
Takes any object as argument, if the object type cannot be deduced, you need to add the template parameter explicitly.   
This comparator is checking that the given parameter is not equal to the argument
* FSeam::Any comparator : signature ```FSeam::Any()```  
Doesn't take argument, doesn't apply check on this argument   
* FSeam::CustomComparator : [Described here](testing.md#custom-argument-comparator)  

```cpp
#include <FSeam.hpp> // Contains the FSeam mocking tools
using namespace FSeam; // used in order to be able to use comparator without specifying FSeam

/**
 * This expectArg call is registing a check that the method TestingClass::functionCalled is called at least 2 times with
 *  the first integer argument equal to 42,
 *  the second argument could be anything
 *  the third string argument equal to "okok" 
 */
mock->expectArg<FSeam::TestingClass::functionCalled>(Eq(42), Any(), NotEq(std::string("okok"), AtLeast{2}));
```

>It is important to note that calling expectArg is going to register your expectations but not verifying them.

In order to have your expectations checked you need to make a call to the [verify function](testing.md#verifications).  
This also mean that the expectations are to be set before any call to the testing function (the expectation are checked when the functions, any late declared expectation are not taken into account into calls).
  
_Example of wrong and good usage of argument expectations:_

```cpp
#include <FSeam.hpp> // Contains the FSeam mocking tools
using namespace FSeam; // used in order to be able to use comparator without specifying FSeam

// BAD
TEST_CASE("Expectation is never checked") {
    TestingClass testClass;
    auto mock = FSeam::get(&testClass);

    mock->expectArg<FSeam::TestingClass::functionCalled>(Eq(42), Eq(1337)); // add expectation before call
    testClass.functionCalled(42, 1111); // 42 is ok, 1111 != 1337 so the expectation is not met
    // Test succeed, but the expectation requirement are not met!!
    // The expectation has never been checked (need to call verify function).
}

// GOOD
TEST_CASE("Expectation checked") {
    auto mock = FSeam::getDefault<TestingClass>();

    mock->expectArg<FSeam::TestingClass::functionCalled>(Eq(42), Eq(1337)); // add expectation before call
    testClass.functionCalled(42, 1111); // 42 is ok, 1111 != 1337 so the expectation is not met

    // verify expectations, also verify the function as been called only once overall
    REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, 1)); 
    // Test fails because the function has been called once, but not with the good arguments 
    // (expectation has been checked)
}

// BAD
TEST_CASE("Expectation is done too late") {
    auto mock = FSeam::getDefault<TestingClass>();

    testClass.functionCalled(42, 1111); // 42 is ok, 1111 != 1337 so the expectation is not met
    mock->expectArg<FSeam::TestingClass::functionCalled>(Eq(42), Eq(1337)); // add expectation after call (too late)

    // verify expectations, also verify the function as been called only once overall
    REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, 1)); 
    // Test succeed because the function has been called once, and expectation not checked has not 
    // registered in due time (expectation has been checked)
}
```
### Custom Argument comparator

This custom comparator can be very useful if you are checking argument on a class, and you need to check some member variable on it, or if the object doesn't provide a == or != operator overload.  
This comparator takes into argument a function that return a boolean and takes as parameters the parameters that should have been sent to the function (which are forwarded to your provided comparator).

Here is the signature of this the custom comparator : ```template<typename T...> FSeam::CustomComparator(std::function<bool(T)>)```

_Example being more explicit than words:_

```cpp

TestingClass testingClass {};
auto fseamMock = FSeam::get(&testingClass);

fseamMock->expectArg<FSeam::TestingClass::checkCustomStructInputVariable>(
    CustomComparator<StructTest>([](auto test){
        return test.testInt == 1 &&
            test.testShort == 11 &&
            test.testStr == "111";
    }));

testingClass.checkCustomStructInputVariable(StructTest{1, 11, "111"});

REQUIRE(fseamMock->verify(FSeam::TestingClass::checkCustomStructInputVariable::NAME, 1));
// Expectation met correctly, test passing
```

It is unfortunately needed to specify the types of each argument into the Custom comparator. But the usage of C++17 templated lambda (auto as argument in the lambda) avoid us to do any repetition, so this is not that bad.

## Called comparator

When using verifications (or expectations) you can specify how many times you the functions has been called. There are multiple way to do so.
* You can check if the function has been called **exactly a given number of times** by using FSeam::VerifyCompare  
For the case of verify, it is possible to just send an integral value which is going to be automatically taken into a VerifyCompare comparator object:
```cpp
// Check the function DependencyGettable::checkCalled has been called exactly 5 times
REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, FSeam::VerifyCompare{5}));
```

* You can check if the function has been called **at least a given number of times** by using FSeam::AtLeast:
```cpp
// Check the function DependencyGettable::checkCalled has been called at least 5 times
REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, FSeam::AtLeast{5})); 
```

* You can check if the function has been called **at most a given times** by using FSeam::AtMost:
```cpp
// Check the function DependencyGettable::checkCalled has been called at most 5 times
REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, FSeam::AtMost{5})); 
```

* You can check if the function **has never been called** by using FSeam::NeverCalled:
```cpp
// Check the function DependencyGettable::checkCalled has been never been called
REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, FSeam::NeverCalled{})); 
```

### Usage advice on never called

If you want to check that a method has never been called it is better to use the FSeam::NeverCalled comparator instead of doing a false assertion.    
_For example:_ 
```cpp
// Bad practice
REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, false));

// Good practice
REQUIRE(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, FSeam::NeverCaled{});
```
It is important to note that verify is going to return false in case of any error in the verifying process. Which means it could return false if an argument didn't feel the requirements it should have (because of a call to [expectArg](testing.md#argument-expectation)).  
By doing so, you are taking the assumption that the verify returned false because the function has never been called. While it could be because of another reason. If you use the FSeam::NeverCalled{}, you assert for sure that the function has never been called.  

But if you want to check a false expectation anyway (for whatever reason) don't forget it's possible to disable the logging in the verification by adding the verbose argument (last argument of verify) in order to not pollute your test log with expected error log.

```cpp
REQUIRE_FALSE(fseamMock->verify(FSeam::DependencyGettable::checkCalled::NAME, AtLeast{1}, false));
```

## Dupe return values

< todo >

## Dupe

This is the most low level feature we have. Unfortunately, if you need to use arguments of the called mock into your dupped implementation you will have to understand a little bit the inner implementation of FSeam (not too hard to get).  
But usually, a mock doesn't need anything more than [checking argument](testing.md#argument-expectation), [duping return values](testing.md#dupe-return-values), and [verify if called](testing.md#verifications). But if any edge cases appears. You can play with this low level feature.

< Explain the feature here >

```cpp
#include <FSeam.hpp> // Contains the FSeam mocking tools
#include <FSeamMockData.hpp> // Contains the inner data class fseam use
```
