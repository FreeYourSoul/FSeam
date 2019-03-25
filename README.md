# FSeamGenerator
Python script to generate cpp mock for FSeam using [CppHeaderParser](https://bitbucket.org/senex/cppheaderparser)

### Link Seam mock?
Beside the separate preprocessing step that occurs before compilation, we also have a post-compilation step called linking in C and C++ that is used to combine the results the compiler has emitted. The
linker gives us another kind of seam called link seam. We show three kinds of link seams here:

Shadowing functions through linking order (override functions in libraries with new definitions in object files)
Wrapping functions with GNU's linker option -wrap (GNU Linux only)
Run-time function interception with the preload functionality of the dynamic linker for shared libraries (GNU Linux and Mac OS X only)  
  
Source : http://www.mockator.com/projects/mockator/wiki/Link_Seams

#### In Other words
This python script is used to generate cpp files that contains implementation of class, this is a seam mocked implementation. This implementation is going to replace the actual implementation at Linking time (thanks to CMake, see example in this repository)

### CMake use
A Cmake module is present can be used in order to do an automatization of the generation of seam mocked files.  
It is going to list all headers (has to be .hh extension) files into your includes folders, and will generate the different seam mocked implementation files at the desired location.
