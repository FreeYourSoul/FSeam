#!/usr/bin/python
import CppHeaderParser
import os
import ntpath
import sys
from sty import fg

FILENAME = "__FILENAME__"
CLASSNAME = "__CLASSNAME__"
HEADER_INFO = "/**\n" \
              " * FSeam generated class for __FILENAME__\n" \
              " * Please do not modify\n" \
              " */\n\n"
LOCKING_HEAD = "#ifndef FREESOULS___CLASSNAME___HPP \n"\
               "#define FREESOULS___CLASSNAME___HPP\n\nusing std;\n\n"
LOCKING_FOOTER = "\n#endif\n"
BASE_HEADER_CODE = "#include "

class FSeamerFile:

    def __init__(self, pathFile):
        self._map = dict()
        self._codeSeam = HEADER_INFO
        self._headerPath = os.path.normpath(pathFile)
        self._fileName = ntpath.basename(self._headerPath)
        try:
            self._cppHeader = CppHeaderParser.CppHeader(self._headerPath)
        except CppHeaderParser.CppParseError as e:
            print(e)
            sys.exit(1)

    # =====Public methods =====

    def seamParse(self):

        self._codeSeam = self._codeSeam.replace(FILENAME, self._fileName)
        self._codeSeam += self.extractHeaders()
        classes = self._cppHeader.classes

        for c in classes:
            className = c
            for encapsulationLevel in classes[c]["methods"]:
                self._codeSeam += "\n// " + className + " " + encapsulationLevel
                self._codeSeam += self.extractMethodsFromClass(className, classes[c]["methods"][encapsulationLevel])
            self._codeSeam = self._codeSeam.replace(CLASSNAME, className)
        return self._codeSeam

    # =====Privates methods =====


    def extractHeaders(self):
        fseamerCodeHeaders = "//includes\n"
        for incl in self._cppHeader.includes:
            fseamerCodeHeaders += BASE_HEADER_CODE + incl + "\n"
        fseamerCodeHeaders += "#include <MockData.hpp>\n#include <MockVerifier.hpp>\n"
        return fseamerCodeHeaders

    def extractMethodsFromClass(self, className, methodsData):
        methods = "\n// Methods Mocked Implementation for class " + className + "\n"
        lstMethodName = list()

        if className in self._map:
            lstMethodName = self._map[className]
        for methodData in methodsData:
            classFullName = methodData["path"]
            returnType = methodData["rtnType"]
            methodsName = methodData["name"]
            if returnType != "void":
                if "*" in returnType or "share_ptr" in returnType or "unique_ptr" in returnType:
                    lstMethodName.append(returnType + " " + methodsName)
                else:
                    lstMethodName.append(returnType + " *" + methodsName)
            signature = returnType + " " + classFullName + "::" + methodsName + "("
            parametersType = [t["type"] for t in methodData["parameters"]]
            parametersName = [t["name"] for t in methodData["parameters"]]

            for i in range(len(parametersType)):
                signature += parametersType[i] + " " + parametersName[i]
                if i == 0 and len(parametersType) > 1:
                    signature += ", "
            signature += ")"

            methodContent = self.generateMethodContent(returnType, className, methodsName)
            methods += "\n" + signature + "{\n" + methodContent + "\n}\n"

        self._map[className] = lstMethodName
        return methods

    def generateMethodContent(self, returnType, className, methodName):
        additional = ", &data"
        content = "    FSeam::" + className + "Data data;\n"
        returnStatement = "\n    return *data." + methodName + "ReturnValue;"

        if 'void' == returnType:
            returnStatement = ""
            additional = ""
            content = ""

        content += "    FSeam::MockVerifier::instance().getMock(this)->invokeDupedMethod(\"" + className + \
                   "\", __FUNCTION__" + additional + ");\n"
        content += "    FSeam::MockVerifier::instance().getMock(this)->methodCall(\"" + className + "\", __FUNCTION__);"
        content += returnStatement
        return content

    def getFSeamGeneratedFileName(self):
        return self._fileName.replace(".hh", "Mock.cc")

    def getDataStructureContent(self, content):
        if not content:
            content = HEADER_INFO
            content += LOCKING_HEAD
            content = content.replace(FILENAME, "DataMock.hpp")
            content = content.replace(CLASSNAME, "DATAMOCK")
        content += "namespace FSeam {\n"
        for className, methods in self._map.items():
            if methods and className not in content:
                struct = "struct " + className + "Data {\n"
                for methodName in methods:
                    struct += methodName + "ReturnValue;\n"
                content += struct + "\n};\n\n"
        return content + "}\n" + LOCKING_FOOTER


def generateFSeamFiles(filePath, destinationFolder):
    if not str.endswith(filePath, ".hh"):
        raise NameError("Error file " + filePath + " is not a .hh file")

    fSeamerFile = FSeamerFile(filePath)
    fileContent = fSeamerFile.seamParse()
    fileName = fSeamerFile.getFSeamGeneratedFileName()
    fileCreated = open(os.path.normpath(destinationFolder + "/" + fileName), "w")
    fileCreated.write(fileContent)
    fileCreated.close()

    print(fg.cyan + "FSeam generated file " + fileName + " at " + os.path.abspath(destinationFolder) + fg.rs)

    fileCreatedMockDataPath = os.path.normpath(destinationFolder + "/MockData.hpp")
    fileCreatedMockDataContent = ""
    if os.path.exists(fileCreatedMockDataPath):
        fileCreatedMockData = open(fileCreatedMockDataPath, "r")
        fileCreatedMockDataContent = fileCreatedMockData.read().replace(LOCKING_FOOTER, "")
        fileCreatedMockData.close()
    fileCreatedMockData = open(fileCreatedMockDataPath, "w")
    fileCreatedMockData.write(fSeamerFile.getDataStructureContent(fileCreatedMockDataContent))
    fileCreatedMockData.close()
    print(fg.cyan + "FSeam generated file MockData.hpp at " + os.path.abspath(destinationFolder) + fg.rs)

