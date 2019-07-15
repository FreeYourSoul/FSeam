<a id="top"></a>
# Usage 

The following references shows how to use FSeam

## Install

### Dependencies

**CMake** required to use the cmake tooling (version minimum required is the 3.10.0)

**Usage of cpp 17,** it is needed to compile with gcc 7 or later.  
``` bash
# bash command line
g++ <sources> -std=c++17
```
```cmake
# cmake command
set_target_properties(target PROPERTIES CXX_STANDARD 17)
```

**Usage of Python** for the generator, if not installed yet :
```Bash
# fedora
sudo yum install python

# unbuntu
sudo apt-get install python
```
 
---

**Usage of python package ply** because of CppHeaderParser.py which has a dependency on python ply.lex
```bash
sudo apt-get install python3-pip
sudo pip install ply
```  
 
---
**Usage of catch2** into the test (v2.7.0 at this day 31/03/2019), if not installed yet you can do either :   
-- install the latest version from the repo directly
```Bash
git clone https://github.com/catchorg/Catch2.git
cd Catch2 && mkdir build && cd build
cmake .. -DBUILD_TESTING=OFF
sudo make install
```
   --  or download the file directly from the selected version (here v2.7.0):
```Bash
cd /path/to/your/fseam/clone/test/
curl -O https://raw.githubusercontent.com/catchorg/Catch2/v2.7.0/single_include/catch2/catch.hpp
```
  
### Installation of FSeam
Use the following commands in order to install FSeam
```Bash
  git clone https://github.com/FreeYourSoul/FSeam.git  
  cd FSeam  
  mkdir build  
  cd build  
  cmake ..  
  make && make test && sudo make install
```
This will install the **cmake helper** file, the **header only library**, and the **generator python script** on your machine.

## CMake with FSeam

### Add FSeam Test in project

After including FSeamModule.cmake, FSeam helper function become available, 

The addFSeamTests cmake function is a to call in order to generate a test executable from the generated FSeam mock and the provided test source

**Mandatory**
* arg **DESTINATION_TARGET**: target name of the test executable generated via this method  
* arg **TST_SRC**: catch2 test files containing the actual test to compile  
* arg **TO_MOCK**: files to mock for this specific given test  

**Either**  
* arg **TARGET_AS_SOURCE**: target of the library that contains the code to test  
**Or**
* arg **FILES_AS_SOURCE**: source file containing the code to test  
* arg **FOLDER_INCLUDES**: includes folder to attach to the test target DESTINATION_TARGET

**optional**
* arg **MAIN_FILE**: file containing the main (if any), this file will be removed from the compilation of the test  


function(addFSeamTests)
```CMake
include(FSeamModule.cmake)

# Create a executable/library target called binaryWithContentToTest with the code you want to test 

enable_testing()
addFSeamTests(
        DESTINATION_TARGET fseamTargetName
        TARGET_AS_SOURCE binaryWithContentToTest
        TST_SRC 
                ${CMAKE_CURRENT_SOURCE_DIR}/catch2TestFile.cpp          #Or any other framework
        TO_MOCK
                ${CMAKE_CURRENT_SOURCE_DIR}/src/DependencyToMock.hh     #Give header to mock
)
```
 

### Options
* If using Google Test, you can specify it as an option, by doing so the library will be linked by default to the test target and logging will be using standard output (iostream std::cout / std::cerr)
```bash
cmake -DFSEAM_USE_GTEST=ON
```
* If using Catch2, you can specify it as an option, by doing so the header will be linked (add_include_directory), and loggin will be using Catch2 logging (useful for debugging as it will be scoped in the section that produced the log).
```bash
cmake -DFSEAM_USE_CATCH2=ON
```

If both options are specified, Catch2 is prioritized (because I prefer catch2 NAaah :p !~)
