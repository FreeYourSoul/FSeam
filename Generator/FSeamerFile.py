#!/usr/bin/python
import CppHeaderParser
import os
import ntpath
import time
import datetime
import sys
from sty import fg

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
        self.map = dict()
        self.codeSeam = HEADER_INFO
        self.headerPath = os.path.normpath(pathFile)
        self.fileName = ntpath.basename(self.headerPath)
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
                self.codeSeam += self._extractMethodsFromClass(_className,
                                                               _classes[c]["methods"][encapsulationLevel])
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
        if not content:
            content = HEADER_INFO
            content += LOCKING_HEAD
            content = content.replace(FILENAME, "DataMock.hpp")
            content = content.replace(CLASSNAME, "DATAMOCK")
        content += "namespace FSeam {\n"
        for className, methods in self.map.items():
            if methods and className not in content:
                _struct = "struct " + className + "Data {\n"
                for methodName in methods:
                    _struct += methodName + "ReturnValue;\n"
                content += _struct + "\n};\n\n"
        content += "}\n"
        content.replace("namespace FSeam {\n}\n", "")
        return content + LOCKING_FOOTER

    # =====Privates methods =====

    def _extractHeaders(self):
        _fseamerCodeHeaders = "//includes\n"
        for incl in self._cppHeader.includes:
            _fseamerCodeHeaders += BASE_HEADER_CODE + incl + "\n"
        _fseamerCodeHeaders += "#include <MockData.hpp>\n#include <MockVerifier.hpp>\n"
        return _fseamerCodeHeaders

    def _extractMethodsFromClass(self, className, methodsData):
        _methods = "\n// Methods Mocked Implementation for class " + className + "\n"
        _lstMethodName = list()

        if className in self.map:
            _lstMethodName = self.map[className]
        for methodData in methodsData:
            if not methodData["defined"]:
                _classFullName = methodData["path"]
                _returnType = methodData["rtnType"]
                _methodsName = methodData["name"]
                if _returnType != "void":
                    if "*" in _returnType or "share_ptr" in _returnType or "unique_ptr" in _returnType:
                        _lstMethodName.append(_returnType + " " + _methodsName)
                    else:
                        _lstMethodName.append(_returnType + " *" + _methodsName)
                _signature = _returnType + " " + _classFullName + "::" + _methodsName + "("
                _parametersType = [t["type"] for t in methodData["parameters"]]
                _parametersName = [t["name"] for t in methodData["parameters"]]

                for i in range(len(_parametersType)):
                    _signature += _parametersType[i] + " " + _parametersName[i]
                    if i == 0 and len(_parametersType) > 1:
                        _signature += ", "
                _signature += ")"

                methodContent = self._generateMethodContent(_returnType, className, _methodsName)
                _methods += "\n" + _signature + "{\n" + methodContent + "\n}\n"

        self.map[className] = _lstMethodName
        return _methods

    def _generateMethodContent(self, returnType, className, methodName):
        _additional = ", &data"
        _content = "    FSeam::" + className + "Data data;\n"
        _returnStatement = "\n    return *data." + methodName + "ReturnValue;"

        if 'void' == returnType:
            _returnStatement = ""
            _additional = ""
            _content = ""

        _content += "    FSeam::MockVerifier::instance().getMock(this)->invokeDupedMethod(\"" + className + \
                    "\", __func__" + _additional + ");\n"
        _content += "    FSeam::MockVerifier::instance().getMock(this)->methodCall(\"" + className + "\", __func__);"
        _content += _returnStatement
        return _content


# API function
def generateFSeamFiles(filePath, destinationFolder):
    if not str.endswith(filePath, ".hh") and not str.endswith(filePath, ".hpp"):
        raise NameError("Error file " + filePath + " is not a .hh file")

    _fSeamerFile = FSeamerFile(filePath)
    _fileContent = _fSeamerFile.seamParse()
    _fileName = _fSeamerFile.getFSeamGeneratedFileName()
    _fileFSeamPath = os.path.normpath(destinationFolder + "/" + _fileName)
    if _fSeamerFile.isSeamFileUpToDate(_fileFSeamPath):
        print (fg.yellow + "FSeam file is already generated at path " + _fileFSeamPath + fg.rs)
        return

    _fileCreated = open(_fileFSeamPath, "w")
    _fileCreated.write(_fileContent)
    _fileCreated.close()
    print(fg.cyan + "FSeam generated file " + _fileName + " at " + os.path.abspath(destinationFolder) + fg.rs)

    _fileCreatedMockDataPath = os.path.normpath(destinationFolder + "/MockData.hpp")
    _fileCreatedMockDataContent = ""
    if os.path.exists(_fileCreatedMockDataPath):
        _fileCreatedMockData = open(_fileCreatedMockDataPath, "r")
        _fileCreatedMockDataContent = _fileCreatedMockData.read().replace(LOCKING_FOOTER, "")
        _fileCreatedMockData.close()
    _fileCreatedMockData = open(_fileCreatedMockDataPath, "w")
    _fileCreatedMockData.write(_fSeamerFile.getDataStructureContent(_fileCreatedMockDataContent))
    _fileCreatedMockData.close()
    print(fg.cyan + "FSeam generated file MockData.hpp at " + os.path.abspath(destinationFolder) + fg.rs)
