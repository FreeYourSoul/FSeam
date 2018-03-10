#!/usr/bin/python
import CppHeaderParser
import os
import ntpath

CLASSNAME = "__CLASSNAME__"
HEADER_INFO = "/**\n" \
              " * FSeam generated class for __CLASSNAME__\n" \
              " * Please do not modify\n" \
              " */\n"
BASE_HEADER_CODE = "#include "


class FSeamerFile:

    def __init__(self, pathFile):
        self._codeSeam = HEADER_INFO
        self._headerPath = os.path.normpath(pathFile)
        self._fileName = ntpath.basename(self._headerPath)
        self._cppHeader = CppHeaderParser.CppHeader(self._headerPath);

    # =====Public methods =====

    def seam(self):
        self._codeSeam += self._codeSeam.replace(CLASSNAME, self._fileName)
        self._codeSeam += self.extractHeaders()
        classes = self._cppHeader.classes

        for c in classes:
            className = c["name"]
            for encapsulationLevel in c["methods"]:
                self._codeSeam += self.extractMethodsFromClass(className, c["method"][encapsulationLevel])

    # =====Privates methods =====

    def extractHeaders(self):
        fseamerCodeHeaders = "//includes\n#inlude <MockData.hpp>\n#include <MockVerifier.hpp>\n"
        for incl in self._cppHeader.includes:
            fseamerCodeHeaders += BASE_HEADER_CODE + incl + "\n"
        return fseamerCodeHeaders

    def extractMethodsFromClass(self, className, methodsData):
        methods = "\n// Methods Mocked Implementation\n"

        for methodData in methodsData:
            classFullName = methodData["path"]
            returnType = methodData["rtnType"]
            methodsName = methodData["name"]
            signature = returnType + " " + classFullName + methodsName + "("
            parametersType = [t["type"] for t in methodData["parameters"]]
            parametersName = [t["name"] for t in methodData["parameters"]]

            for i in range(len(parametersType)):
                signature += parametersType[i] + " " + parametersName[i]
                if i == 0 and len(parametersType) > 1:
                    signature += ", "

            signature += ")"
            print("signature method->\n%s" % signature)

            methodContent = self.generateMethodContent(returnType, className)

            methods += "\n" + signature + "{\n" + methodContent + "\n}\n\n"

        print("code in file->\n%s" % methods)
        return methods

    def generateMethodContent(self, returnType, className):
        additional = ", &data"
        content = "FSeam::GatewayData data;\n"
        returnStatement = "\n    return " + returnType + ";"

        if 'void' == returnType:
            returnStatement = ""
            additional = ""
            content = ""

        content += "    FSeam::MockVerifier::instance().getMock(this)->invokeDupedMethod(\"" + className + \
                   ", __FUNCTION__" + additional + ");\n"
        content += "    FSeam::MockVerifier::instance().getMock(this)->methodCall(\"" + className + "\", __FUNCTION__);"
        content += returnStatement
        return content
