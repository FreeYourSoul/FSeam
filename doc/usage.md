<a id="top"></a>
# Usage 

The following references shows how to use FSeam

## Install

### Dependencies

**Usage of cpp 17,** it is needed to compile with gcc 7 or later.  
``` bash
g++ <sources> -std=c++17
```

**Usage of Python** for the generator, if not installed yet :
```Bash
// fedora
sudo yum install python

// unbuntu
sudo apt-get install python
```
 
---

**Usage of python package ply** because of CppHeaderParser.py which has a dependency on python ply.lex
```bash
sudo apt-get install python3-pip
sudo pip install ply
```  
 
---
**Usage of catch2** (v2.7.0 at this day 31/03/2019), if not installed yet you can do either :   
-- install the latest version from the repo directly
```Bash
git clone https://github.com/catchorg/Catch2.git
cd Catch2 && mkdir build && cd build
cmake .. -H. -DBUILD_TESTING=OFF
sudo make install
```
   --  or download the file directly from the selected version (here v2.7.0):
```Bash
cd /path/to/your/testing/directory
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

## Compile with FSeam