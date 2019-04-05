# Logging

By default, FSeam will log the error on the standard output. While it is the perfect way to go for some frameworks (like Google Test for instance), it is not obviously the case for all frameworks.  
For example Catch2 has its specific set of macro in order to log information into the correct scope of the test (in the correct section/test case). 
 
And It is perfectly possible that FSeam user would want to define it's own custom way of logging error.

## Framework specific logger

As of today, 2 specific loggers have been defined (GTest and Catch2), they are enabled by using the define **FSEAM_USE_< framework name >**

### Catch2 logger

Catch2 provides custom logger in order to be able to scope the log correctly (in the good section/use-case).
INFO(""), WARN("") are used.

You can specify the usage of Catch2 by defining **FSEAM_USE_CATCH2**  
```cpp
// TestingFile.cpp
#define FSEAM_USE_CATCH2 // define it before including FSeam header file
#include <FSeam.hpp>


TEST_CASE("test") {
    // your test will be logged using catch2
}
```

> FAIL and CHECK_FAIL are not used as they are making the test fail if encountered. And even though it could be a nice thing. In order to keep consistency and logic between logging behavior. We use only WARN in order to log warnings and errors. It is up to the user to check the verify correctly in order to make the test fail or not.

### GTest logger

GTest using the standard output, specifying GTest is just doing a little save of memory compared to the default behavior (as no default custom logger has been defined).

To do so it is needed to define **FSEAM_USE_GTEST**
```cpp
// TestingFile.cpp
#define FSEAM_USE_GTEST // define it before including FSeam header file
#include <FSeam.hpp>


TEST_CASE("test") {
    // your test will be logged using standard output
}
```

## Define your own logger

You can define your own logger. It is recommended to define it before any use of [verify](testing.md#verifications), Otherwise a default one will be set. When a logger is explicitly defined, it is set for the whole test execution.  
Set the custom logger into your setup fixture function or at the beginning of your test case.

### How to define it

You just need to call the static method custom inside of FSeam::Logging::Logger.  
This function takes a std::function<void (FSeam::Logging::Level, const std::string &)> as parameter, which is the logger that will be used by FSeam. A level is provided (INFO, WARNING, ERROR) and the actual message.

_Usage example:_
```cpp
FSeam::Logging::Logger::custom([](FSeam::Logging::Level level, const std::string &msg){
        if (level == FSeam::Logging::Level::ERROR)
            MY_CUSTOM_ERROR_LOGGER_IN_FILE(msg);
        else if (level == FSeam::Logging::Level::WARNING)
            MY_CUSTOM_WARNING_LOGGER_IN_FILE(msg);
        else
            MY_CUSTOM_LOGGER_IN_FILE(msg);
    });
```

## Logging priority
If you have no framework specific defined, you need to specify your custom logger before your first usage of verify. Otherwise the following logger will be specified by default (and used as custom one, which means will not be overridable):

```cpp
// logger used as custom logger if no custom logger user defined and
// if no framework defined by the define FSEAM_USE_CATCH2 or FSEAM_USE_GTEST
auto defaultCustomLogger = [](Level l, const std::string &msg) {
        if (l == Level::ERROR)
            std::cerr << msg << "\n";
        else
            std::cout << msg << "\n";
    }
```

 **In resume the priority is the following:**   
 1. User defined custom logger  
 2. If no custom logger defined : A default one is provided (see above lambda representing it) 
 3. Framework specific defined logger (Catch2, Gtest as of today)

> If a custom logger is defined, the framework specific one is ignored (Gtest or Catch2 enabled with the FSEAM_USE_CATCH2/FSEAM_USE_GTEST define).  
