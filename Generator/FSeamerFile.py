#!/usr/bin/python
import CppHeaderParser
import os
import ntpath
import re
import sys
from sty import fg

INDENT = "    "
INDENT2 = INDENT + INDENT
FILENAME = "__FILENAME__"
CLASSNAME = "__CLASSNAME__"
HEADER_INFO = "/**\n" \
              " * FSeam generated class for __FILENAME__\n" \
              " * Please do not modify\n" \
              " */\n\n"
LOCKING_HEAD = "#ifndef FREESOULS___CLASSNAME___HPP \n" \
               "#define FREESOULS___CLASSNAME___HPP\n\nusing std;\n\n"
LOCKING_FOOTER = "\n#endif\n"
BASE_HEADER_CODE = "#include "


class FSeamerFile:

    # =====Public methods =====

    def __init__(self, pathFile):
        self.mapClassMethods = dict()
        self.codeSeam = HEADER_INFO
        self.headerPath = os.path.normpath(pathFile)
        self.fileName = ntpath.basename(self.headerPath)
        self.functionSignatureMapping = {}
        try:
            self._cppHeader = CppHeaderParser.CppHeader(self.headerPath)
        except CppHeaderParser.CppParseError as e:
            print(e)
            sys.exit(1)

    def seamParse(self):
        self.codeSeam = self.codeSeam.replace(FILENAME, self.fileName)
        self.codeSeam += self._extractHeaders()
        _classes = self._cppHeader.classes
        for c in _classes:
            _className = c
            for encapsulationLevel in _classes[c]["methods"]:
                self.codeSeam += "\n// " + _className + " " + encapsulationLevel
                self.codeSeam += self._extractMethodsFromClass(_className, _classes[c]["methods"][encapsulationLevel])
            self.codeSeam = self.codeSeam.replace(CLASSNAME, _className)
        return self.codeSeam

    def isSeamFileUpToDate(self, fileFSeamPath):
        if not os.path.exists(fileFSeamPath):
            return False
        fileMockedTime = os.stat(fileFSeamPath).st_mtime
        fileToMockTime = os.stat(self.headerPath).st_mtime
        return fileMockedTime > fileToMockTime

    def getFSeamGeneratedFileName(self):
        return self.fileName.replace(".hh", ".fseam.cc")

    def getDataStructureContent(self, content):
        if not content or len(content) < 10:
            content = HEADER_INFO.replace(FILENAME, "DataMock.hpp")
            content += LOCKING_HEAD.replace(CLASSNAME, "DATAMOCK")
        content += "namespace FSeam {\n"
        for className, methods in self.mapClassMethods.items():
            if methods or className in content:
                content = self._clearDataStructureData(content, className)
            _struct = "\nstruct " + className + "Data {\n"
            _helperMethod = "// Methods Helper\nnamespace " + className + " {\n"
            for methodName in methods:
                _struct += self._extractDataStructMethod(methodName)
                _helperMethod += INDENT + "using " + methodName.upper() + " = \"" + methodName + "\";\n"
            content += _struct + "\n};\n" + _helperMethod + "\n}\n"
            content += "// NameTypeTraits\ntemplate <> struct TypeParseTraits {\n" + INDENT + "using ClassName = \"" + className + "\";\n}"
            content += " // End of DataStructure" + className + "\n\n\n"
        content += "}\n"
        content = re.sub("namespace FSeam {[\n ]+}\n", "", content)
        return content + LOCKING_FOOTER

    # =====Privates methods =====

    def _clearDataStructureData(self, content, className):
        indexBegin = content.find("\nstruct " + className + "Data {\n")
        indexEnd = content.find("} // End of DataStructure" + className) + len("\n// End of DataStructure" + className)
        if indexBegin > 0 and indexEnd > len("\n// End of DataStructure" + className) + 1:
            content = content[0: indexBegin] + content[indexEnd + 1:]
        return content

    def _extractHeaders(self):
        _fseamerCodeHeaders = "// includes\n"
        for incl in self._cppHeader.includes:
            _fseamerCodeHeaders += BASE_HEADER_CODE + incl + "\n"
        _fseamerCodeHeaders += "#include <MockData.hpp>\n#include <MockVerifier.hpp>\n"
        return _fseamerCodeHeaders

    def _extractDataStructMethod(self, methodName):
        _methodData = INDENT + "/**\n" + INDENT + " * method metadata : " + methodName + "\n" + INDENT + "**/\n"
        for param in self.functionSignatureMapping[methodName]["params"]:
            _paramType = param["type"]
            _paramName = param["name"]
            if "*" in _paramType or "shared_ptr" in _paramType or "unique_ptr" in _paramType:
                _methodData += INDENT + _paramType + " " + _paramName + "ParamValue;\n"
            else:
                _methodData += INDENT + _paramType + " *" + _paramName + "ParamValue;\n"
        _returnType = self.functionSignatureMapping[methodName]["rtnType"]
        if _returnType != "void":
            if "*" in _returnType or "shared_ptr" in _returnType or "unique_ptr" in _returnType:
                _methodData += INDENT + _returnType + " " + methodName + "ReturnValue;\n\n"
            else:
                _methodData += INDENT + _returnType + " *" + methodName + "ReturnValue;\n\n"
        return _methodData

    def _registerMethodIntoMethodSignatureMap(self, name, retType, params):
        self.functionSignatureMapping[name] = {}
        self.functionSignatureMapping[name]["rtnType"] = retType
        self.functionSignatureMapping[name]["params"] = params

    def _extractMethodsFromClass(self, className, methodsData):
        _methods = "\n// Methods Mocked Implementation for class " + className + "\n"
        _lstMethodName = list()

        if className in self.mapClassMethods:
            _lstMethodName = self.mapClassMethods[className]
        for methodData in methodsData:
            if not methodData["defined"]:
                _classFullName = methodData["path"]
                _returnType = methodData["rtnType"]
                _methodsName = methodData["name"]
                _lstMethodName.append(_methodsName)
                _signature = _returnType + " " + _classFullName + "::" + _methodsName + "("
                _parametersType = [t["type"] for t in methodData["parameters"]]
                _parametersName = [t["name"] for t in methodData["parameters"]]
                self._registerMethodIntoMethodSignatureMap(_methodsName, _returnType, methodData["parameters"])
                for i in range(len(_parametersType)):
                    _signature += _parametersType[i] + " " + _parametersName[i]
                    if i == 0 and len(_parametersType) > 1:
                        _signature += ", "
                _signature += ")"
                methodContent = self._generateMethodContent(_returnType, className, _methodsName)
                _methods += "\n" + _signature + " {\n" + methodContent + "\n}\n"

        self.mapClassMethods[className] = _lstMethodName
        return _methods

    def _generateMethodContent(self, returnType, className, methodName):
        _content = INDENT + "FSeam::" + className + "Data data;\n"
        _additional = ", &data"
        _returnStatement = INDENT + "return *data." + methodName + "ReturnValue;"

        if 'void' == returnType:
            _returnStatement = ""
            _additional = ""
            _content = ""
        _content += INDENT + "auto *mockVerifier = (FSeam::MockVerifier::instance().isMockRegistered(this)) ?\n"
        _content += INDENT2 + "FSeam::MockVerifier::instance().getMock(this) :\n"
        _content += INDENT2 + "FSeam::MockVerifier::instance().getDefaultMock(" + className + ");\n\n"
        _content += INDENT + "mockVerifier->invokeDupedMethod(__func__" + _additional + ");\n"
        _content += INDENT + "mockVerifier->methodCall(__func__, std::any(data));\n"
        _content += _returnStatement
        return _content


# API function
def generateFSeamFile(filePath, destinationFolder, forceGeneration=False):
    if not str.endswith(filePath, ".hh") and not str.endswith(filePath, ".hpp"):
        raise NameError("Error file " + filePath + " is not a .hh file")

    _fSeamerFile = FSeamerFile(filePath)
    _fileName = _fSeamerFile.getFSeamGeneratedFileName()
    _fileFSeamPath = os.path.normpath(destinationFolder + "/" + _fileName)
    if not forceGeneration and _fSeamerFile.isSeamFileUpToDate(_fileFSeamPath):
        print(fg.yellow + "FSeam file is already generated at path " + _fileFSeamPath + fg.rs)
        return

    with open(_fileFSeamPath, "w") as _fileCreated:
        _fileCreated.write(_fSeamerFile.seamParse())
    print(fg.cyan + "FSeam generated file " + _fileName + " at " + os.path.abspath(destinationFolder) + fg.rs)

    _fileCreatedMockDataPath = os.path.normpath(destinationFolder + "/MockData.hpp")
    _fileCreatedMockDataContent = ""
    if os.path.exists(_fileCreatedMockDataPath):
        with open(_fileCreatedMockDataPath, "r") as _fileCreatedMockData:
            _fileCreatedMockDataContent = _fileCreatedMockData.read().replace(LOCKING_FOOTER, "")
    with open(_fileCreatedMockDataPath, "w") as _fileCreatedMockData:
        _fileCreatedMockData.write(_fSeamerFile.getDataStructureContent(_fileCreatedMockDataContent))
    print(fg.cyan + "FSeam generated file MockData.hpp at " + os.path.abspath(destinationFolder) + fg.rs)


if __name__ == '__main__':
    _args = sys.argv[1:]
    if len(_args) < 2:
        print(fg.red + "Error missing argument for generation" + fg.rs)
        exit(1)
    _forceGeneration = False
    if len(_args) > 2:
        _forceGeneration = _args[2]
    generateFSeamFile(_args[0], _args[1], _forceGeneration)
