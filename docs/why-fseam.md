<a id="top"></a>
# Why FSeam

## The others mocking framework

Before talking about FSeam, it is needed to talk about the other frameworks that exist ([GMock](https://github.com/google/googletest/blob/master/googlemock/README.md) being the most famous one by far).  
The base of those frameworks are kind of following the same principle as Mockito does in Java([ps:what is mockito](https://site.mockito.org/)).  
Basically depending on [dependency injection](https://en.wikipedia.org/wiki/Dependency_injection) where the dependency implementation injected is different in a testing binary than in production one.

### Dependency injection : Inheritance

The easiest way to manage a dependency injection, is to send an interface to a function/class. And changing the class that implement the interface at runtime (testing implementation when in a test, and normal implementation when in the production code).  
This implicitly implies that the code is going to use inheritance. In Java, it is a normal thing to do and it doesn't change anything in everyday Java code (as the usage of interface is recommended and very regular in Java).  
  
  But in C++, working inheritance implies virtual functions. And [virtual functions has a cost](https://stackoverflow.com/questions/667634/what-is-the-performance-cost-of-having-a-virtual-method-in-a-c-class), not incredible one if there are no jump to do (if the cost is only for the lookup) but still a cost. Which means that most of the time, you are going to pay for the price of a virtual just in order to enable testing into your code.  
  This is one of the problematic that **FSeam can resolve**.

  
Another thing important to note is that this kind of dependency injection force you to add arguments (and in case of class constructor, you will often also need to store those arguments) for each dependency you could have. Which can be quite a few in some cases.

### Dependency injection : Template

Another solution in C++, is to have the type dependencies of your class/function into templates.  
With this method you have more possibilities to inject your dependency, you can do like with inheritance and send them as parameters (template parameters resolved at compile time).  

Google Mock make it possible to create a standalone mocked object without using inheritance or virtuals methods. The google mock type can be passed as type [into a template](https://github.com/google/googlemock/blob/master/googlemock/docs/v1_6/CookBook.md#mocking-class-templates).  
 Or just instantiate the mock inside of the class/function, but it will force you to add a getter on this instantiated dependency if you want to change it's behavior through GMock via ON_CALL macro for example (we will see that **FSeam can answer to that problem**).

Another problem that can occur with this solution is that in some cases, a big load of template can be necessary in the code to cover all the dependencies (to be honest, this issue may be mitigated by having a class regrouping all/a part of the dependencies class, but it implies additional types to be created and may produce confusing and hard to read code)  
This is nonetheless the way to go in general, as it is easy to setup and using the commonly known mocking frameworks.

### The untestable code : legacy

But unfortunately, some dark code exist in this world. Everyone had to see the 1000 lines of code that contains several dependencies that you need to get rid of in order to unit test the feature. And I think everyone just dropped this function and said "Meh, it works since ages, no need to bother testing it".  
And I can't blame you as this thought is literally the one I had in front of such code. 

But what if you need to test it? Or let's just assume that you are masochist and want to unit test the feature anyway ?  
It would be hard/impossible with the above explained principles to make it works without impacting too much of the legacy code. And doing a refactoring of such code are often not recommended as you could/will have un-predicted impact on your business.  

**FSeam could answer this legacy code problem**.

## FSeam answer

In summary, FSeam will try to resolve multiple issues classic testing has : 
* Testing code without having to use virtuals
* Testing code without having to use a code bloating template code
* Testing legacy code without impacting the current code

### Untouched code
FSeam doesn't bother with code refactoring issues and so you don't need to have the entirety of your code thought in a testing way which could imply performance impact (virtual)

On top of that, FSeam makes it possible to mock static/free functions also without any code modifications. Which is a big plus compared to any other mocking framework available. Google mock makes it possible. But it is hardly doable and need specific syntax. 
With FSeam, the syntax to mock/verify a class or a free function is kept the same as you basically end up manipulating the same objects that represent a mocking handler of your class/function. 


### Legacy testing
With link seam, it is possible to test legacy features with little to no re-factoring by just seam mocking database access class for instance.  
It is possible to give a default behavior and monitor the default behaviors of FSeam mock. Which means (in opposition with the template dependency injection) you don't need to get the actual instance of the mock to alter it's behavior and verify its usage.

### The Trap
A code thought to be tested is going to be more readeable and provide more [pure functions/methods](https://en.wikipedia.org/wiki/Pure_function).
Splitting dependencies from the functionnality is usually making the classes less couple, and so more flexible and easier to modify.  

**Using FSeam in a way to not have to think about software architecture is the trap to not fall into**. FSeam is supposed to be used in order for the user to not have impact on the code performance, or building time/readibility, but should in no way be a reason to stop making the code testable by default as it going to make better code anyway.
