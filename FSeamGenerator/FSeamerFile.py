#!/usr/bin/python
import CppHeaderParser
import os
import ntpath
import sys
from sty import fg

CLASSNAME = "__CLASSNAME__"
HEADER_INFO = "/**\n" \
              " * FSeam generated class for __CLASSNAME__\n" \
              " * Please do not modify\n" \
              " */\n\n"
BASE_HEADER_CODE = "#include "

class FSeamerFile:

    def __init__(self, pathFile):
        self._codeSeam = HEADER_INFO
        self._headerPath = os.path.normpath(pathFile)
        self._fileName = ntpath.basename(self._headerPath)
        try:
            self._cppHeader = CppHeaderParser.CppHeader(self._headerPath)
        except CppHeaderParser.CppParseError as e:
            print(e)
            sys.exit(1)

    # =====Public methods =====

    def seam(self):
        self._codeSeam = self._codeSeam.replace(CLASSNAME, self._fileName)
        self._codeSeam += self.extractHeaders()
        classes = self._cppHeader.classes

        for c in classes:
            className = c
            for encapsulationLevel in classes[c]["methods"]:
                self._codeSeam += "\n// " + className + " " + encapsulationLevel
                self._codeSeam += self.extractMethodsFromClass(className, classes[c]["methods"][encapsulationLevel])
        return self._codeSeam

    # =====Privates methods =====

    def extractHeaders(self):
        fseamerCodeHeaders = "//includes\n"
        for incl in self._cppHeader.includes:
            fseamerCodeHeaders += BASE_HEADER_CODE + incl + "\n"
        fseamerCodeHeaders += "#include <MockData.hpp>\n#include <MockVerifier.hpp>\n"
        return fseamerCodeHeaders

    def extractMethodsFromClass(self, className, methodsData):
        methods = "\n// Methods Mocked Implementation class \n"

        for methodData in methodsData:
            classFullName = methodData["path"]
            returnType = methodData["rtnType"]
            methodsName = methodData["name"]
            signature = returnType + " " + classFullName + "::" + methodsName + "("
            parametersType = [t["type"] for t in methodData["parameters"]]
            parametersName = [t["name"] for t in methodData["parameters"]]

            for i in range(len(parametersType)):
                signature += parametersType[i] + " " + parametersName[i]
                if i == 0 and len(parametersType) > 1:
                    signature += ", "

            signature += ")"
            print("signature method->\n%s" % signature)


            methodContent = self.generateMethodContent(returnType, className, methodsName)

            methods += "\n" + signature + "{\n" + methodContent + "\n}\n"

        return methods

    def generateMethodContent(self, returnType, className, methodName):
        additional = ", &data"
        content = "    FSeam::" + className + "Data data;\n"
        returnStatement = "\n    return " + methodName + "ReturnValue;"

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


def generateFSeamFile(filePath, destinationFolder):
    if not str.endswith(filePath, ".hh"):
        raise NameError("Error file " + filePath + " is not a .hh file")

    fSeamerFile = FSeamerFile(filePath)
    fileContent = fSeamerFile.seam()
    fileName = fSeamerFile.getFSeamGeneratedFileName()
    fileCreated = open(os.path.normpath(destinationFolder + "/" + fileName), "w")
    fileCreated.write(fileContent)
    fileCreated.close()
    print(fg.cyan + "FSeam generated file " + fileName + " from " + destinationFolder+ fg.rs)
