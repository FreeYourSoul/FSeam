<a id="top"></a>
# Why FSeam

## The others mocking framework

< Testing framework following jUnit principles which requires you to inject the dependencies into the tested class/functions>

### Dependency injection : Inheritance

< Inheritance is required which mean virtual methods, most of the time impact performance just for testing purpose >  
  
< Force you to inject the dependency in the constructor, in big project, could be a lot of them >

### Dependency injection : Template

< Make your code mockable/testable in classical ways (google mock make it possible to create standalone mock without inheritance) but in some case, force a blutter of template in the code (may be mitigated by having a class regrouping all the mockable class. But imply that sometimes) >

This is the best choice in general and

### The untestable code : legacy

< Here explain about the code blutter (big functions) and that is not re-factorable easily. And thus cannot be tested>

## FSeam answer

### Untouched code
< Here explain that FSeam doesn't bother with code refactoring issues and so you don't need to have the entirety of your code thought in a testing way which could imply performance impact (virtual) >

### Legacy testing
< Here explain that thanks to link seam, it is possible to test legacy features with little to no re-factoring by just seam mocking database access class for instance >

### The Trap
A code thought to be tested is going to be more readeable and provide more [pure functions/methods](https://en.wikipedia.org/wiki/Pure_function).
Splitting dependencies from the functionnality is usually making the classes less couple, and so more flexible and easier to modify.  

**Using FSeam in a way to not have to think about software architecture is the trap to not fall into**. FSeam is supposed to be used in order for the user to not have impact on the code performance, or building time/readibility, but should in no way be a reason to stop making the code testable by default as it going to make better code anyway.
