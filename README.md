[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/FreeYourSoul/FSeam/master/LICENSE)

# FSeam
![FSeam logo](https://github.com/FreeYourSoul/FSeam/blob/master/artwork/logo.png?raw=true)
  
  Python script to generate cpp mock for FSeam  
   
  Python and CMake (version minimum 3.10.0) are required to use FSeam

## What is a Link Seam mock?
A seam is a term introduced by Michael Feathers in his book [Working effectively with legacy code](https://www.abebooks.fr/Working-Effectively-Legacy-Code-Michael-Feathers/18824529190/bd?cm_mmc=gmc-_-new-_-PLA-_-v01&gclid=CjwKCAjwp_zkBRBBEiwAndwD9Ts6XzzhpZnVafPtxti_UMnsxTM8g4EMqE7aqr-IyJDlPRvFdlmCXxoCHbMQAvD_BwE). It basically describe a way to change the behavior of a function/functionality, which is very usefull for testing while ignoring external dependencies.

A lot of different seams exist and are described in [this accu article](https://accu.org/index.php/journals/1927) that is a must read.  
When talking about seam, for FSeam, it is assumed we are talking about link seam.
  
The goal of such seam is to change the behavior of a class by tweaking the ordering, number of the files compiled.  FSeam is a combination of **a code generator** (creating a mocking implementation of the given C++ class/functions) and **a header only library** that makes you able to change the behaviors of those mock at runtime. And **a CMake helper** in order to easily implements the generation of code and linking time tricks.  
[Why do we need yet another mocking framework?](docs/why-fseam.md#why-fseam)
 

### In Resume, what is FSeam?
FSeam is a python script that will generate cpp files that contains fake implementation of class/functions (those will not call the original implementation), this is a what we will call a seam mocked implementation. This implementation is going to replace the actual implementation at Linking time (thanks to CMake helpers functions) and is going to register information about how the functions has been called (arguments, number of time etc...). And thanks to the header only FSeam library, you will be able to manipulate those mocks (changing behavior, verify usage).  

### Advantage of FSeam

* Mocking without impacting production code
* Mocking default behavior (no need to access a particular instance of the mocked object to manipulate its behavior)
* Mocking of static / free functions as easily as any classes
* Easy to setup (all of those thing are installed and ready to use when following the installation step below):
  * Header only library to include in test file
  * A Python script
  * CMake file to include in order to easily do the compile tweaking and code generation

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

* [Complete documentation](docs/README.md#reference)  
* [Cheat sheet](docs/cheat-sheet.md#cheat-sheet)  

## Ambition

This project is extensively used to test FyS, it is a Proof of Concept. The idea in itself is worth exploring and improving, and I intend to do that through this project. If this project can be of any interest for your personal and/or professional projects, any help to make it more stable and usable is welcomed.

## Dependencies  

Using extensively CppHeaderParser, originally developed by Jashua Cloutier([original repo](https://bitbucket.org/senex/cppheaderparser))   
The project is currently handled by robotpy ([current repo](https://github.com/robotpy/robotpy-cppheaderparser)) 

