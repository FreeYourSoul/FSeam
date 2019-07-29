<a id="top"></a>
# How to use FSeam

This page is going to try to give the most detailed way possible of how you can use FSeam in your project.

The test below are using the asserting macro of the Catch2 testing framework.    
If you need any more usage example than this, please request them by [opening a new issue](https://github.com/FreeYourSoul/FSeam/issues) or just look at [the existing tests](https://github.com/FreeYourSoul/FSeam/tree/master/test) which are quite complete and good samples.

> To use the functionality below, you need to include FSeam.hpp which contains the mocking tools.  
>  For low level usage (dupe), including FSeamMockData.hpp could be required

## Get a FSeam mock handler

In order to dupe/verify behavior of a mocked class/function you need to get a handler to manipulate.

**Two handler type exist:**
* Instance mock handlers: it is a handler on a specific instance of the mocked class. Make you able to have multiple instances of the same mocked class have different dupped behaviors.
```cpp
TestClass testingClass;
auto fseamMock = FSeam::get(&testingClass); // get the specific instance mock
```
* Default mock handlers: it is a default behavior for a given mocked class type.
```cpp
auto fseamMock = FSeam::getDefault<TestClass>(); // get the default behavior for the mocked TestClass
```

> Static method and free functions are, internally, using the Default mock handler mechanism on a class called FSeam::FreeFunction [more explanation](free-functions.md#free-functions)

## Verifications

Verification is the basic of FSeam, a verify function is implemented at the level of any mock handler. It is used to check how many times a method has been called and also does additional check if any [argument expectations](testing.md#argument-expectation) has been set.  
  
Here are the verify function signatures:

```cpp
/**
 * @brief Verify if the given method has been called at least one time
 * 
 * @param methodName Name of the method to check on the mock (Use the helpers constant to ensure no typo)
 * @param verbose flag if a debug string is required in case of false response (set to true by default)
 * @return true if the method encounter the provided comparator conditions, false otherwise
 */
bool verify(const std::string &methodName, bool verbose = true) constconst {
    return verify(methodName, AtLeast(1), verbose);
}

/**
 * @brief Verify if a method has been called under certain conditions (number of times)
 * 
 * @tparam Comparator  comparator class used, can be also an Integer, those comparator are defined under the namespace FSeam,
 *          VerifyCompare/Integral value : Check if the method has been called exactly the number provided
 *          NeverCalled : Check if the method has never been called
 *          AtLeast : Check if the method has been called at least the number provided
 *          AtMost  : Check if the method has been called at most the number provided
 *          IsNot   : Check if the method is not the number provided
 *          
 * @param methodName Name of the method to check on the mock (Use the helpers constant to ensure no typo)
 * @param comp comparator instance on which the number of times the mock method is called on a provided value
 *         is checked against
 * @param verbose flag if a debug string is required in case of false response (set to true by default)
 * @return true if the method encounter the provided comparator conditions, false otherwise
 */
template <typename Comparator>
bool verify(std::string methodName, Comparator &&comp, bool verbose = true) const;
```
The code above is directly taken from the header as it is quite self explanatory, the first method is the "light one", it basically just an override that calls the real verify function (the second one) with a [calling comparator](testing.md#called-comparator) AtLeast{1} (to check that the function has been called at least once).  
A verbose argument can be provided (set to true by default), when set to true, error are logged (and so visible in the test output). If this flag is set to false, no output are generated from the verify call.

## Argument Expectation

The mock object used into test has a ```expectArg``` method that makes you able to check with what arguments the function has been called. This function has the following signature:  
```cpp
template <typename ClassMethodIdentifier, typename ...Verifiers>
void expectArg(Verifiers ... verifiers);
```
**ClassMethodIdentifier** type being a fseam generated empty structure (under the namespace FSeam::<ClassMockedName>::) present in the include ```#include<FSeamMockData.hpp>``` whose is representing the method you want to mock. For destructor, as it is impossible to create a type with a '~' character, the name is Destructor_<ClassName>.

**Verifiers** variadic template being [arguments comparator](testing.md#argument-comparator), used to compare the argument of the mocked function.  
A [calling comparator](testing.md#calling-comparator) argument can be added (after the arguments comparator still part of the variadic template). Thanks to that argument it is possible to check how many times the function met this requirement.

More information on how to use argument expectations (with example) with [arguments comparators](testing.md#argument-comparator)
  

## Comparators

Do not be confused about the comparators, there is just two types of them:
* **The calling comparator**: used to do comparison on how many time the match has been called [detailed here](testing.md#calling-comparator) 
* **The argument comparator**: used to do comparison of arguments in argument expectation [detailed here](testing.md#argument-comparator)

### Calling comparator

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

### Argument Comparator

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
/**
 * Signature of the method mocked on which we will expect args
 */
 void TestingClass::functionCalled(int, int, std::string);
```

```cpp
#include <FSeam.hpp> // Contains the FSeam mocking tools
#include<FSeamMockData.hpp> // Contains the ClassMethodIdentifiers
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
This also mean that the expectations are to be set before any call to the testing function (the expectation are checked when the functions are called, any late declared expectation are not taken into account into calls).
  
_Example of wrong and good usage of argument expectations:_

```cpp
#include <FSeam.hpp> // Contains the FSeam mocking tools
#include<FSeamMockData.hpp> // Contains the ClassMethodIdentifiers
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

## Dupe return values

It is possible to dupe return value of a method/function mocked by fseam thanks to the fseam function.
```cpp
// Signature of the FSeam mock method to dupe the return value
template<typename ClassMethodIdentifier, typename RtnType> 
void dupeReturn<ClassMethodIdentifier>(RtnType)
```

**ClassMethodIdentifier** type being a fseam generated empty structure present in the include ```#include<FSeamMockData.hpp>``` whose is representing the method you want to mock.

**RtnType** Type of the value returned (can be deduced thanks to the argument).

_Example:_

```cpp
fseamMock->dupeReturn<FSeam::TestinClass::returnIntMethod>(42);
fseamMock->dupeReturn<FSeam::TestinClass::returnStringMethod>(std::string("okok")); // need to help the type deduction a little

struct S {
    int ok;
    int ko;
    int oo
};
fseamMock->dupeReturn<FSeam::TestinClass::returnStructMethod>(S{});
```


## Dupe

This is the most low level feature we have. Unfortunately, if you need to use arguments of the called mock into your dupped implementation you will have to understand a little bit the inner implementation of FSeam (not too hard to get).  
But usually, a mock doesn't need anything more than [checking argument](testing.md#argument-expectation), [duping return values](testing.md#dupe-return-values), and [verify if called](testing.md#verifications). But if any edge cases appears. You can play with this low level feature.

< Explain the feature here >

```cpp
#include <FSeam.hpp> // Contains the FSeam mocking tools
#include <FSeamMockData.hpp> // Contains the inner data class fseam use
```
