# Known Limitations

## Argument expecation on Non copiable object

When using argument expectation with FSeam::Eq or FSeam::NotEq FSeam internally does copy of object into a tempory std::any in order to make the comparaison in the verify.
This imply that it is impossible to use with non-copiable object :  
  
For instance the following code is illegal  
```cpp
using namespace FSeam;
source::TestingClass testClass{};
auto fseamMock = FSeam::get(&testClass.getDepGettable());
NonCopiableObject obj(1);

// illegal to use non-copiable object with Eq or NonEq
fseamMock->expectArg<FSeam::DependencyGettable::functionWithIntegerAndNonCopiableInput>(Eq(1), Eq(obj)); 
```
If only the copiable argument are to be checked you can use FSeam::Any in order to mitigate this issue

```cpp
using namespace FSeam;
source::TestingClass testClass{};
auto fseamMock = FSeam::get(&testClass.getDepGettable());

// Legal as Any is used instead of Eq or NotEq
fseamMock->expectArg<FSeam::DependencyGettable::functionWithIntegerAndNonCopiableInput>(Eq(1), Any()); 
```

If you absolutely need to do a comparison with a non-copiable object. It is assumed that it is going to be sent to the function as reference or pointer. 
So you can use the RAW dupeMethod version in order to do the check.
The Raw method is closely related to the internal way FSeam works, and should be avoided as the interface is not user friendly. But it bypass this issue.
It is important to note that if you need to know how many time the method has been called, you will need to implement such a mecanism

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
