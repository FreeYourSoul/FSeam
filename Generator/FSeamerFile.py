#! /usr/bin/env python
# MIT License
#
# Copyright (c) 2019 Quentin Balland
# Project : https://github.com/FreeYourSoul/FSeam
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import ntpath
import os
import re
import sys

# !/usr/bin/python
import CppHeaderParser

INDENT = "    "
INDENT2 = INDENT + INDENT
INDENT3 = INDENT + INDENT + INDENT
FILENAME = "__FILENAME__"
CLASSNAME = "__CLASSNAME__"
HEADER_INFO = "/**\n" \
              " * FSeam generated class for __FILENAME__\n" \
              " * Please do not modify\n" \
              " */\n\n"
LOCKING_HEAD = "#ifndef FREESOULS___CLASSNAME___HPP \n" \
               "#define FREESOULS___CLASSNAME___HPP\n\n"
LOCKING_FOOTER = "\n#endif\n"
BASE_HEADER_CODE = "#include "
PARAM_SUFFIX = "_ParamValue"
FREE_FUNC_FAKE_CLASS = "FreeFunction"
RETURN_SUFFIX = "_ReturnValue"


class FSeamerFile:

    # =====Public methods =====

    def __init__(self, pathFile):
        """
        :param pathFile: cpp header file that will be parsed at the "seamParse" call
        """
        self.mapClassMethods = {}
        self.codeSeam = HEADER_INFO
        self.headerPath = os.path.normpath(pathFile)
        self.fileName = ntpath.basename(self.headerPath)
        self.specContent = ""
        self.functionSignatureMapping = {}
        self.fullClassNameMap = {}
        self.staticFunction = list()
        self.freeFunctionClassMethodId = None
        self.freeFunctionDataStructContent = None
        self.freeFunctionTemplateSpecContent = ""
        try:
            self.cppHeader = CppHeaderParser.CppHeader(self.headerPath)
        except CppHeaderParser.CppParseError as e:
            print(e)
            sys.exit(1)

    def seamParse(self):
        """
        Parse the header file and return the cpp file corresponding to the FSeam mock implementation of the given
        header file
        :return: FSeam cpp content to be filed into a file
        """
        self.codeSeam = self.codeSeam.replace(FILENAME, self.fileName)
        self.codeSeam += self._extractHeaders()
        _classes = self.cppHeader.classes
        for c in _classes:
            _className = c
            self.fullClassNameMap[c] = _className
            if len(_classes[c]["namespace"]) > 0:
                self.fullClassNameMap[c] = _classes[c]["namespace"] + "::" + _className
            for encapsulationLevel in _classes[c]["methods"]:
                self.codeSeam += "\n// " + _className + " " + encapsulationLevel
                self.codeSeam += self._extractMethodsFromClass(_className, _classes[c]["methods"][encapsulationLevel])
            self.codeSeam = self.codeSeam.replace(CLASSNAME, _className)
        self.cppHeader.functions.extend(self.staticFunction)
        if len(self.cppHeader.functions) > 0:
            _listFunc = list()
            if FREE_FUNC_FAKE_CLASS in self.mapClassMethods:
                _listFunc = self.mapClassMethods[FREE_FUNC_FAKE_CLASS]
            self.codeSeam += "\n// Free functions (put into fake class " + FREE_FUNC_FAKE_CLASS + ")\n"
            for functionData in self.cppHeader.functions:
                _listFunc.append(functionData["name"])
                self.codeSeam += self._extractFreeFunctions(functionData)
            self.mapClassMethods[FREE_FUNC_FAKE_CLASS] = _listFunc
            self.fullClassNameMap[FREE_FUNC_FAKE_CLASS] = FREE_FUNC_FAKE_CLASS
        return self.codeSeam

    def isSeamFileUpToDate(self, fileFSeamPath):
        """
        Check if the newly created file (the FSeam mock file) has been updated sooner than the header it is originated
        from
        :param fileFSeamPath: path of the FSeam file
        :return: True if the file given as parameter is more up to date than the initial header file used for its
                 generation
        """
        if not os.path.exists(fileFSeamPath):
            return False
        fileMockedTime = os.stat(fileFSeamPath).st_mtime
        fileToMockTime = os.stat(self.headerPath).st_mtime
        return fileMockedTime > fileToMockTime

    def getFSeamGeneratedFileName(self):
        """
        :return: name of the file to generate: <headerFileNameWithoutExtension>.fseam.cc
        """
        return self.fileName.replace(".hh", ".fseam.cc").replace("hpp", "fseam.cc")

    def generateDataStructureContent(self, content):
        """
        Generate a FSeamMockData.hpp file that contains:
        - DataModel structures used by FSeam in order to track the number of call made for each method,
                    the dupe made on those method, the arguments used when called, the return value and so on...
        - Helpers   some helper variable to be used by the client of FSeam in test, in order to not misspell methods
                    names
        - Internals Some internals helper used by FSeam in order to work properly (template specification to get naming
                    of the mocked class, or to dupe method/return value via helper methods)

        If no content is provided as argument, a brand new file is generated, if a content is provided. The data for the
        each method re-made are overridden by the new one.

        :param content: content of the current FSeamMockData.hpp file (if any)
        :return: Content of the FSeamMockData.hpp file
        """
        if not content or len(content) < 10:
            content = HEADER_INFO.replace(FILENAME, "DataMock.hpp")
            content += LOCKING_HEAD.replace(CLASSNAME, "DATAMOCK")
            for incl in self.cppHeader.includes:
                content += BASE_HEADER_CODE + incl + "\n"
            content += "#include <type_traits>\n"
            content += "#include <optional>\n"
            content += "#include <FSeam/FSeam.hpp>\n\n"
        if BASE_HEADER_CODE + "<" + self.fileName + ">\n" not in content:
            content += BASE_HEADER_CODE + "<" + self.fileName + ">\n"
        content += "namespace FSeam {\n"
        for className, methods in self.mapClassMethods.items():
            content += "//Beginning of " + className
            if FREE_FUNC_FAKE_CLASS is className:
                self.freeFunctionDataStructContent = self._getCurrentFreeFunctionDataContent(content)
                self.freeFunctionClassMethodId = self._getCurrentFreeFunctionClassMethodIdContent(content)
            if methods or className in content:
                content = self._clearDataStructureData(content, className)
            _struct = "\nstruct " + className + "Data {\n"
            if FREE_FUNC_FAKE_CLASS is className and self.freeFunctionDataStructContent is not None:
                _struct += self.freeFunctionDataStructContent
            for methodName in methods:
                if FREE_FUNC_FAKE_CLASS is className:
                    _struct += self._extractDataStructMethod(className, methodName, self.freeFunctionDataStructContent)
                else:
                    _struct += self._extractDataStructMethod(className, methodName)
            content += _struct + "};\n\n"
            if className is not FREE_FUNC_FAKE_CLASS:
                content += "// NameTypeTraits\ntemplate <> struct TypeParseTraits<" + self.fullClassNameMap[className] + \
                           "> {\n" + INDENT + "inline static const std::string ClassName = \"" + className + "\";\n};\n"
            if className in self.functionSignatureMapping:
                content += self._generateDupeVerifyTemplateSpecialization(className)
            content += "// End of DataStructure" + className + "\n\n\n"
        content += "}\n"
        content = re.sub("namespace FSeam {[\n ]+}\n", "", content)
        content = re.sub("struct [a-zA-Z0-9_]+ {[\n ]+};\n", "", content)
        return content + LOCKING_FOOTER

    def getSpecializationContent(self, content):
        """
        Fil the FSeamSpecialization.cpp file with template specialization for the dupeReturn / expectArg methods
        :param content: string representing the current content of FSeamSpecialization.cpp
        :return: updated content (final file)
        """
        if not content or len(content) < 10:
            content = HEADER_INFO.replace(FILENAME, "FSeamSpecialization.hpp")
            content += "#include <FSeamMockData.hpp>\n\n"
        for className, methods in self.mapClassMethods.items():
            if FREE_FUNC_FAKE_CLASS is className:
                for method in methods:
                    if method in self.freeFunctionTemplateSpecContent:
                        content = self._clearSpecializationFreeFunction(content, method)
            else:
                content = self._clearSpecialization(content, className)
        return content + self.specContent + self.freeFunctionTemplateSpecContent

    # =====Privates methods =====

    def _extractHeaders(self, ):
        _fseamerCodeHeaders = "// includes\n"
        for incl in self.cppHeader.includes:
            _fseamerCodeHeaders += BASE_HEADER_CODE + incl + "\n"
        _fseamerCodeHeaders += "#include <functional>\n"
        _fseamerCodeHeaders += "#include <FSeamMockData.hpp>\n#include <FSeam/FSeam.hpp>\n"
        _fseamerCodeHeaders += BASE_HEADER_CODE + "<" + self.fileName + ">\n"
        return _fseamerCodeHeaders

    def _extractDataStructMethod(self, className, methodName, excluderStruct=None):
        _methodData = ""
        if methodName in self.functionSignatureMapping[className].keys():
            if self.functionSignatureMapping[className][methodName]["isConstructorOrDestructor"] is True:
                pass
            elif excluderStruct is None or methodName not in excluderStruct:
                _methodData = INDENT + "/**\n" + INDENT + " * method metadata : " + className + "::" + methodName + "\n" + INDENT + "**/\n"
                for param in self.functionSignatureMapping[className][methodName]["params"]:
                    _paramType = param["type"]
                    _paramName = param["name"]
                    if _paramName not in ["&", "", None, "*", "&&"]:
                        typeStr = _paramType
                        if "&" in typeStr:
                            typeStr = "std::reference_wrapper<" + typeStr.replace("&", "") + "> "
                        _methodData += INDENT + "std::optional<" + typeStr + "> " + methodName + "_" + _paramName + PARAM_SUFFIX + ";\n"
                _returnType = self.functionSignatureMapping[className][methodName]["rtnType"].replace("&", "").replace("static ", "")
                if _returnType != "void":
                    _methodData += INDENT + _returnType + " " + methodName + RETURN_SUFFIX + ";\n\n"
        return _methodData

    def _registerMethodIntoMethodSignatureMap(self, className, methodName, retType, params, isConstructor = False, isDestructor = False):
        if className not in self.functionSignatureMapping.keys():
            self.functionSignatureMapping[className] = {}
        self.functionSignatureMapping[className][methodName] = {}
        self.functionSignatureMapping[className][methodName]["isConstructorOrDestructor"] = isConstructor or isDestructor
        if self.functionSignatureMapping[className][methodName]["isConstructorOrDestructor"] is True:
            retType = "void"
        self.functionSignatureMapping[className][methodName]["rtnType"] = retType.replace("static ", "").replace("~", "Destructor_")
        self.functionSignatureMapping[className][methodName]["params"] = params

    def _extractFreeFunctions(self, freeFunctionData):
        _functionFakeClassMethod = ""
        _functionName = freeFunctionData["name"]
        _returnType = freeFunctionData["rtnType"].replace("static ", "")
        _signature = _returnType.replace("static ", "") + " " + freeFunctionData["namespace"] + _functionName + "("
        _parametersType = [t["type"] for t in freeFunctionData["parameters"]]
        _parametersName = [t["name"] for t in freeFunctionData["parameters"]]
        self._registerMethodIntoMethodSignatureMap(FREE_FUNC_FAKE_CLASS, _functionName, _returnType,
                                                   freeFunctionData["parameters"])
        for i in range(len(_parametersType)):
            _signature += _parametersType[i] + " " + _parametersName[i]
            if (i + 1) < len(_parametersType):
                _signature += ", "
        _signature += ")"
        _functionFakeClassMethod += "\n" + _signature + " {\n"
        _functionFakeClassMethod += self._generateMethodContent(_returnType, FREE_FUNC_FAKE_CLASS, _functionName, True)
        return _functionFakeClassMethod + "\n}\n"

    def _extractMethodsFromClass(self, className, methodsData):
        _methods = "\n// Methods Mocked Implementation for class " + className + "\n"
        _lstMethodName = list()

        if className in self.mapClassMethods:
            _lstMethodName = self.mapClassMethods[className]
        for methodData in methodsData:
            if methodData["static"]:
                methodData["namespace"] += className + "::"
                self.staticFunction.append(methodData)
            elif not methodData["defined"]:
                _classFullName = methodData["path"]
                _returnType = ""
                if methodData["constructor"] is False and methodData["destructor"] is False:
                    _returnType = methodData["rtnType"].replace("static ", "")
                _methodsName = ""
                if methodData["destructor"] is True:
                    _methodsName += "~"
                _methodsName += methodData["name"]
                _lstMethodName.append(_methodsName)
                _signature = _returnType + " " + _classFullName + "::" + _methodsName + "("
                _parametersType = [t["type"] for t in methodData["parameters"]]
                _parametersName = [t["name"] for t in methodData["parameters"]]
                self._registerMethodIntoMethodSignatureMap(className, _methodsName, _returnType,
                                                           methodData["parameters"],
                                                           methodData["constructor"], methodData["destructor"])
                for i in range(len(_parametersType)):
                    _signature += _parametersType[i] + " " + _parametersName[i]
                    if (i + 1) < len(_parametersType):
                        _signature += ", "
                _signature += ")"
                if methodData["const"] is True:
                    _signature += " const"
                methodContent = self._generateMethodContent(_returnType, className, _methodsName)
                _methods += "\n" + _signature + " {\n" + methodContent + "\n}\n"

        self.mapClassMethods[className] = _lstMethodName
        return _methods

    def _generateDupeVerifyTemplateSpecialization(self, className): 
        ### TODO: extract _genSpecial for the MethodIdentifier in another method
        _genSpecial = "// ClassMethodIdentifiers\n"
        _genSpecial += "namespace " + className + " {\n"
        if self.freeFunctionClassMethodId is not None:
            _genSpecial += self.freeFunctionClassMethodId
        for methodName, methodsMapping in self.functionSignatureMapping[className].items():
            if self.freeFunctionClassMethodId is None or methodName not in self.freeFunctionClassMethodId:
                mn = methodName
                if methodName.startswith("~"):
                    mn = methodName.replace("~", "Destructor_")
                _genSpecial += INDENT + "struct " + mn + " { inline static const std::string NAME = \"" + methodName + "\";};\n"
        _genSpecial += "}\n"

        _specContent = ""
        if FREE_FUNC_FAKE_CLASS is not className:
            _specContent = "\n\n// Duping/Expectations specializations for " + className + "\n"
        for methodName, methodMapping in self.functionSignatureMapping[className].items():
            if methodName.startswith("Destructor_"):
                methodName.replace("Destructor_", "~")
            if (FREE_FUNC_FAKE_CLASS is className):
                _specContent += "// Generated duping for method " + className + "::" + methodName + " begin\n"
            # Specialization for dupeReturn
            if methodMapping["rtnType"].replace("static ", "") != "void":
                _rtnType = "std::decay_t<" + methodMapping["rtnType"].replace("static ", "") + ">"
                _specContent += "template <> void FSeam::MockClassVerifier::dupeReturn<FSeam::" + className + "::" + methodName + ", " + _rtnType + "> (" + _rtnType + " returnValue) {\n"
                _specContent += INDENT + "this->dupeMethod(\"" + methodName + "\", [=](void *methodCallData) { \n"
                _specContent += INDENT2 + "static_cast<FSeam::" + className + "Data *>(methodCallData)->" + methodName + RETURN_SUFFIX + " = returnValue;\n"
                _specContent += INDENT + "}, true);\n}\n"

            # Specialization for verifyArg
            if len(methodMapping["params"]) > 0:
                _specContent += "// Expectation specializations for " + className + "::" + methodName + "\n"
                for comparator in [None, "FSeam::IsNot", "FSeam::AtMost", "FSeam::AtLeast", "FSeam::NeverCalled", "FSeam::VerifyCompare"]:
                    _specContent += self._generateSpecializationVerifyArg(className, methodName, methodMapping, comparator)
            if (FREE_FUNC_FAKE_CLASS is className):
                _specContent += "// Generated duping for method " + className + "::" + methodName + " end\n"
        # cleanup loops last separator tokens
        _specContent = _specContent.replace(", >", ">").replace(", )", ")").replace(", \n);", ");").replace("(\n)", "()")
        if FREE_FUNC_FAKE_CLASS is not className:
            _specContent += "// End of Specialization for " + className + "\n\n"
        if FREE_FUNC_FAKE_CLASS is className:
            self.freeFunctionTemplateSpecContent = _specContent
        else:
            self.specContent += _specContent
        return _genSpecial

    def _getCurrentFreeFunctionDataContent(self, content):
        indexBegin = content.find("struct FreeFunctionData {\n") + len("struct FreeFunctionData {\n")
        indexEnd = content.find("};\n", indexBegin)
        if indexBegin > len("struct FreeFunctionData {\n") and \
                indexEnd > (len("struct FreeFunctionData {\n") + len("};\n")):
            return content[indexBegin:indexEnd]
        return ""

    def _getCurrentFreeFunctionClassMethodIdContent(self, content):
        indexBegin = content.find("namespace FreeFunction {\n") + len("namespace FreeFunction {\n")
        indexEnd = content.find("}\n", indexBegin)
        if indexBegin > len("namespace FreeFunction {\\n") and \
                indexEnd > (len("namespace FreeFunction {\\n") + len("}\n")):
            return content[indexBegin:indexEnd]
        return ""

    def _generateMethodContent(self, returnType, className, methodName, isFreeFunction=False):
        if isFreeFunction:
            _content = INDENT + "auto mockVerifier = FSeam::MockVerifier::instance().getDefaultMock(\"" + className + "\");\n"
        else:
            _content = INDENT + "auto mockVerifier = (FSeam::MockVerifier::instance().isMockRegistered(this)) ?\n"
            _content += INDENT2 + "FSeam::MockVerifier::instance().getMock(this, \"" + className + "\") :\n"
            _content += INDENT2 + "FSeam::MockVerifier::instance().getDefaultMock(\"" + className + "\");\n"
        if "&" in returnType:
            _content += INDENT + "static std::vector<FSeam::" + className + "Data> datas;\n"
            _content += INDENT + "datas.emplace_back();\n"
            _content += INDENT + "FSeam::" + className + "Data &data = datas.back();\n\n"
        else:
            _content += INDENT + "FSeam::" + className + "Data data {};\n\n"
        for p in self.functionSignatureMapping[className][methodName]["params"]:
            _content += INDENT + "if (std::is_copy_constructible<std::decay<" + p["type"] + ">>())\n"
            _content += INDENT2 + "data." + methodName + "_" + p["name"] + PARAM_SUFFIX + " = " + p["name"] + ";\n"
        _content += INDENT + "mockVerifier->invokeDupedMethod(__func__, &data);\n"
        _content += INDENT + "mockVerifier->methodCall(__func__, &data);\n"
        if 'void' != returnType and self.functionSignatureMapping[className][methodName]["isConstructorOrDestructor"] is False:
            _content += INDENT + "return data." + methodName + "_ReturnValue;"
        return _content

    @staticmethod
    def _generateSpecializationVerifyArg(className, methodName, methodMapping, comparator=None):
        _gen = "template <> void FSeam::MockClassVerifier::expectArg<FSeam::" + className + "::" + methodName + ", "
        for _ in methodMapping["params"]:
            _gen += "FSeam::ArgComp, "
        if comparator is not None:
            _gen += comparator
        _gen += "> "
        _gen += "("
        for param in methodMapping["params"]:
            _gen += "FSeam::ArgComp " + param["name"] + ", "
        if comparator is not None:
            _gen += comparator + " comp"
        _gen += ") {\n"
        _gen += INDENT + "auto expectationChecker = [=](void *methodCallData) { \n"
        _gen += INDENT2 + "bool argCheck = true;\n"
        for param in methodMapping["params"]:
            _gen += INDENT2 + "argCheck &= " + param["name"] + ".compare<" + param[
                "type"] + ">(*static_cast<FSeam::" + className + "Data *>(methodCallData)->" + methodName + "_" + param[
                        "name"] + PARAM_SUFFIX + ");\n"
        _gen += INDENT2 + "return argCheck;\n"
        _gen += INDENT + "};\n"
        _gen += INDENT + "this->registerExpectation(\"" + methodName + "\", MethodCallVerifier::Expectation{ expectationChecker"
        if comparator is not None:
            _gen += ", comp"
        else:
            _gen += ", FSeam::AtLeast{1}"
        _gen += " });\n}\n"
        return _gen

    @staticmethod
    def _clearDataStructureData(content, className):
        indexBegin = content.find("//Beginning of " + className)
        indexEnd = content.find("// End of DataStructure" + className) + len("// End of DataStructure" + className)
        if indexBegin > 0 and indexEnd > len("// End of DataStructure" + className) + 1:
            content = content[0: indexBegin] + content[indexEnd + 1:]
        return content

    @staticmethod
    def _clearSpecialization(content, className):
        indexBegin = content.find("\n\n// Duping/Expectations specializations for " + className + "\n")
        indexEnd = content.find("// End of Specialization for " + className + "\n\n") + len(
            "// End of Specialization for " + className + "\n\n")
        if indexBegin > 0 and indexEnd > len("// End of Specialization for " + className + "\n\n"):
            content = content[0: indexBegin] + content[indexEnd:]
        return content

    @staticmethod
    def _clearSpecializationFreeFunction(content, freeFunctionName):
        indexBegin = content.find("// Generated duping for method " + FREE_FUNC_FAKE_CLASS + "::" + freeFunctionName + " begin\n")
        strEnd = "// Generated duping for method " + FREE_FUNC_FAKE_CLASS + "::" + freeFunctionName + " end\n"
        indexEnd = content.find(strEnd) + len(strEnd)
        if indexBegin > 0 and indexEnd > len(strEnd):
            content = content[0: indexBegin] + content[indexEnd:]
        return content


def generateFSeamFile(filePath, destinationFolder, forceGeneration=False):
    """
    Client exposed method, will create the FSeam mock file and fill them with the content provided by the FSeam parser

    :param filePath: path of the cpp header file to parse in order to generate the seam mock
    :param destinationFolder: folder in which the generated folder will be created
    :param forceGeneration: if there are no need to generate the FSeam mock (mock, apparently, up to date) this flag
                            make it able to bypass those check and to generate brand new mock anyway (the FSeamMockData.hpp
                            won't be deleted, the usual process of removing only the part re-generated will stays as is)
                            by default, this flag is set to False
    :return: no return
    """
    if not str.endswith(filePath, ".hh") and not str.endswith(filePath, ".hpp"):
        raise NameError("Error file " + filePath + " is not a .hh file")

    _fSeamerFile = FSeamerFile(filePath)
    _fileName = _fSeamerFile.getFSeamGeneratedFileName()
    _fileFSeamPath = os.path.normpath(destinationFolder + "/" + _fileName)
    if not forceGeneration and _fSeamerFile.isSeamFileUpToDate(_fileFSeamPath):
        print("FSeam file is already generated at path " + _fileFSeamPath)
        return

    with open(_fileFSeamPath, "w") as _fileCreated:
        _fileCreated.write(_fSeamerFile.seamParse())
    print("FSeam generated file " + _fileName + " at " + os.path.abspath(destinationFolder))

    _fileCreatedMockDataPath = os.path.normpath(destinationFolder + "/FSeamMockData.hpp")
    _fileCreatedMockDataContent = ""
    if os.path.exists(_fileCreatedMockDataPath):
        with open(_fileCreatedMockDataPath, "r") as _fileCreatedMockData:
            _fileCreatedMockDataContent = _fileCreatedMockData.read().replace(LOCKING_FOOTER, "")
    with open(_fileCreatedMockDataPath, "w") as _fileCreatedMockData:
        _fileCreatedMockData.write(_fSeamerFile.generateDataStructureContent(_fileCreatedMockDataContent))
    print("FSeam generated file FSeamMockData.hpp at " + os.path.abspath(destinationFolder))
    _fileCreatedSpecializationPath = os.path.normpath(destinationFolder + "/FSeamSpecialization.cpp")
    _fileCreatedSpecializationContent = ""
    if os.path.exists(_fileCreatedSpecializationPath):
        with open(_fileCreatedSpecializationPath, "r") as _fileCreatedSpecData:
            _fileCreatedSpecializationContent = _fileCreatedSpecData.read()
    with open(_fileCreatedSpecializationPath, "w") as _fileCreatedSpecData:
        _fileCreatedSpecData.write(_fSeamerFile.getSpecializationContent(_fileCreatedSpecializationContent))
    print("FSeam generated file FSeamSpecialization.cpp at " + os.path.abspath(destinationFolder))


if __name__ == '__main__':
    _args = sys.argv[1:]
    if len(_args) < 2:
        raise NameError("Error missing argument for generation")
    _forceGeneration = True
    if len(_args) > 2:
        _forceGeneration = _args[2]
    generateFSeamFile(_args[0], _args[1], _forceGeneration)
