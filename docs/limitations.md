# Known Limitations

## Usage of CMake

It is undeniable that the usage of the CMake utility function is strongly recommenced. It is still possible to manually generate the code yourself using the python script, but it would be bothersome and certainly not worth the effort.

## Code structure

What is usually a good practice, for one header file, one cpp file to compile. Is now an obligation as FSeam is going to provide another implementation of the class/functions defined in the header. If the implementation of the header file is scatered in different source file.   
Compilation time issue could occurs as FSeam is going to create a second implementation of the same function/class.

## Argument expectation on Non copyable object

When using argument expectation with FSeam::Eq or FSeam::NotEq FSeam internally does copy of object into a temporary std::any in order to make the comparison in the verify.
This imply that it is impossible to use with non-copyable object :  
  
For instance the following code is illegal  
```cpp
using namespace FSeam;
source::TestingClass testClass{};
auto fseamMock = FSeam::get(&testClass.getDepGettable());
NonCopiableObject obj(1);

// illegal to use non-copiable object with Eq or NonEq
fseamMock->expectArg<FSeam::DependencyGettable::functionWithIntegerAndNonCopiableInput>(Eq(1), Eq(obj)); 
```
If only the copyable argument are to be checked you can use FSeam::Any in order to mitigate this issue

```cpp
using namespace FSeam;
source::TestingClass testClass{};
auto fseamMock = FSeam::get(&testClass.getDepGettable());

// Legal as Any is used instead of Eq or NotEq
fseamMock->expectArg<FSeam::DependencyGettable::functionWithIntegerAndNonCopiableInput>(Eq(1), Any()); 
```

If you absolutely need to do a comparison with a non-copyable object. It is assumed that it is going to be sent to the function as reference or pointer. 
So you can use the RAW dupeMethod version in order to do the check.
The Raw method is closely related to the internal way FSeam works, and should be avoided as the interface is not user friendly. But it bypass this issue.
It is important to note that if you need to know how many time the method has been called, you will need to implement such a mechanism

```cpp
source::TestingClass testClass{};
auto fseamMock = FSeam::get(&testClass.getDepGettable());
bool isOk = false;
// Legal as Any is used instead of Eq or NotEq
fseamMock->dupeMethod(FSeam::DependencyGettable::functionWithIntegerAndNonCopiableInput::NAME, [&isOk](void *dataStructure){
        auto *data = static_cast<FSeam::DependencyGettableData *> (dataStructure);
        isOk = data->functionWithIntegerAndNonCopiableInput_integerInput_ParamValue == 42 &&
               data->functionWithIntegerAndNonCopiableInput_nonCopiableInput_ParamValue == NonCopiableObject(1);
    }); 
REQUIRE(isOk);
```

### Todo
Check out the [todo list](future.md#future-to-be-implemented) in the "Functional Improvement" section in order to find out other potential known limitation.
