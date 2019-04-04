# Future to be implemented

## TODO: 
* **Improve Documentation:**  
  * Detailed documentation on verify (into usage.md file)
  * Detailed documentation on dupe (into usage.md file)
  * ~~Detailed documentation on expectArg (into usage.md file)~~
    
* **Functional Improvements**
  * ~~**Add free/static function support**~~
  * **Add logging functionality** : Would be useful in order to have detailed explanation on failures (instead of just a return true/false on the verify)
    * Customization of the logging via registering method to do the logging
    * Pre-defined logging with Catch2 and GTest (at least)
  * **Provide a multi-threaded safety option**: Currently using singleton and static memory without having any synchronization mecanism. Any multithreading test would make the verify not being accurate.
  * **Remove Catch dependency from the FSeam module**:  
    * Replace it by an option (to keep it only if using Catch2 with a find_package not required)
    * Add an equivalent for Gtest
  * **Adding inner-class-struct/class**: The code generation is wrong and not compilable when mocking a header file with a class embedded in another one
* **Improve API:**
  * for static function (use the class name as base instead of the generic namespace "FreeFunction")
  * for verify (unify usage, use template in order to select the function you want to mock) --> Keep previous usage too ?
  
* **Bug Fix** (if any :p)
  * ?
