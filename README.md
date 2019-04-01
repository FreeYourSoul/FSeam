[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/FreeYourSoul/FSeam/master/LICENSE)

# FSeam
![FSeam logo](https://github.com/FreeYourSoul/FSeam/blob/master/artwork/logo.png?raw=true)
  
  Python script to generate cpp mock for FSeam,  

## What is a Link Seam mock?
A seam is a term introduced by Michael Feathers in his book [Working effectively with legacy code](https://www.abebooks.fr/Working-Effectively-Legacy-Code-Michael-Feathers/18824529190/bd?cm_mmc=gmc-_-new-_-PLA-_-v01&gclid=CjwKCAjwp_zkBRBBEiwAndwD9Ts6XzzhpZnVafPtxti_UMnsxTM8g4EMqE7aqr-IyJDlPRvFdlmCXxoCHbMQAvD_BwE). It basically describe a way to change the behavior of a function/functionality, which is very usefull for testing while ignoring external dependencies.

A lot of different seams exist and are described in [this accu article](https://accu.org/index.php/journals/1927) that is a must read.  
When talking about seam, for FSeam, it is assumed we are talking about link seam.
  
The goal of such seam is to change the behavior of a class by tweaking the ordering, number of the files compiled.  FSeam is a combination of **a code generator** (creating a mocking implementation of the given C++ class/functions) and **a header only library** that makes you able to change the behaviors of those mock at runtime. And **a CMake helper** in order to easily implements the generation of code and linking time tricks.  
[Why do we need yet another mocking framework?](docs/why-fseam.md#why-fseam)
 

### In Other words
FSeam python script is used to generate cpp files that contains implementation of class, this is a what we will call a seam mocked implementation. This implementation is going to replace the actual implementation at Linking time (thanks to CMake helpers functions). And thanks to the header only FSeam library, you will be able to manipulate those mocks (changing behavior, verify usage).

## How to install
```Bash
  git clone https://github.com/FreeYourSoul/FSeam.git  
  cd FSeam  
  mkdir build  
  cd build  
  cmake ..  
  make && make test && sudo make install
```
In case of dependencies issue : [follow this link](docs/usage.md#dependencies)

## Documentation
* Complete documentation : [documentation link](docs/README.md#reference)  
* Cheat sheet doc : [documentation link](docs/cheat-sheet.md#cheat-sheet)  


## Dependencies  

Using extensively CppHeaderParser, originally developed by https://bitbucket.org/senex/cppheaderparser Jashua Cloutier([original repo](https://bitbucket.org/senex/cppheaderparser))   
The project is currently handled by robotpy ([current repo](https://github.com/robotpy/robotpy-cppheaderparser)) 

