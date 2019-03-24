#!/usr/bin/python
import CppHeaderParser
import os
import ntpath
import re
import sys

INDENT = "    "
INDENT2 = INDENT + INDENT
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


class FSeamerFile:

    # =====Public methods =====

    def __init__(self, pathFile):
        """
        :param pathFile: cpp header file that will be parsed at the "seamParse" call
        """
        self.mapClassMethods = dict()
        self.codeSeam = HEADER_INFO
        self.headerPath = os.path.normpath(pathFile)
        self.fileName = ntpath.basename(self.headerPath)
        self.functionSignatureMapping = {}
        self.fullClassNameMap = {}
        try:
            self._cppHeader = CppHeaderParser.CppHeader(self.headerPath)
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
        _classes = self._cppHeader.classes
        for c in _classes:
            _className = c
            self.fullClassNameMap[c] = _classes[c]["namespace"] + "::" + _className
            for encapsulationLevel in _classes[c]["methods"]:
                self.codeSeam += "\n// " + _className + " " + encapsulationLevel
                self.codeSeam += self._extractMethodsFromClass(_className, _classes[c]["methods"][encapsulationLevel])
            self.codeSeam = self.codeSeam.replace(CLASSNAME, _className)
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
        Generate a MockData.hpp file that contains:
        - DataModel structures used by FSeam in order to track the number of call made for each method,
                    the dupe made on those method, the arguments used when called, the return value and so on...
        - Helpers   some helper variable to be used by the client of FSeam in test, in order to not misspell methods
                    names
        - Internals Some internals helper used by FSeam in order to work properly (template specification to get naming
                    of the mocked class)

        If no content is provided as argument, a brand new file is generated, if a content is provided. The data for the
        each method re-made are overridden by the new one.

        :param content: content of the current MockData.hpp file (if any)
        :return: Content of the MockData.hpp file
        """
        if not content or len(content) < 10:
            content = HEADER_INFO.replace(FILENAME, "DataMock.hpp")
            content += LOCKING_HEAD.replace(CLASSNAME, "DATAMOCK")
            for incl in self._cppHeader.includes:
                content += BASE_HEADER_CODE + incl + "\n"
            content += "#include <MockVerifier.hpp>\n\n"
        if BASE_HEADER_CODE + "<" + self.fileName + ">\n" not in content:
            content += BASE_HEADER_CODE + "<" + self.fileName + ">\n"
        content += "namespace FSeam {\n"
        for className, methods in self.mapClassMethods.items():
            if methods or className in content:
                content = self._clearDataStructureData(content, className)
            _struct = "\nstruct " + className + "Data {\n"
            _helperMethod = "// Methods Helper\nnamespace " + className + "_FunName {\n"
            for methodName in methods:
                _struct += self._extractDataStructMethod(methodName)
                _helperMethod += INDENT + "static const std::string " + methodName.upper() + " = \"" + methodName + "\";\n"
            content += _struct + "};\n" + _helperMethod + "\n}\n"
            content += "// NameTypeTraits\ntemplate <> struct TypeParseTraits<" + self.fullClassNameMap[className] + \
                       "> {\n" + INDENT + "inline static const std::string ClassName = \"" + className + "\";\n};"
            content += " // End of DataStructure" + className + "\n\n\n"
        content += "}\n"
        content = re.sub("namespace FSeam {[\n ]+}\n", "", content)
        return content + LOCKING_FOOTER

    # =====Privates methods =====

    def _extractHeaders(self, ):
        _fseamerCodeHeaders = "// includes\n"
        for incl in self._cppHeader.includes:
            _fseamerCodeHeaders += BASE_HEADER_CODE + incl + "\n"
        _fseamerCodeHeaders += "#include <MockData.hpp>\n#include <MockVerifier.hpp>\n"
        _fseamerCodeHeaders += BASE_HEADER_CODE + "<" + self.fileName + ">\n"
        return _fseamerCodeHeaders

    def _extractDataStructMethod(self, methodName):
        _methodData = INDENT + "/**\n" + INDENT + " * method metadata : " + methodName + "\n" + INDENT + "**/\n"
        for param in self.functionSignatureMapping[methodName]["params"]:
            _paramType = param["type"]
            _paramName = param["name"]
            if "*" in _paramType or "shared_ptr" in _paramType or "unique_ptr" in _paramType:
                _methodData += INDENT + _paramType + " " + methodName + "_" + _paramName + "_ParamValue;\n"
            else:
                _methodData += INDENT + _paramType + " *" + methodName + "_" + _paramName + "_ParamValue;\n"
        _returnType = self.functionSignatureMapping[methodName]["rtnType"]
        if _returnType != "void":
            if "*" in _returnType or "shared_ptr" in _returnType or "unique_ptr" in _returnType:
                _methodData += INDENT + _returnType + " " + methodName + "_ReturnValue;\n\n"
            else:
                _methodData += INDENT + _returnType + " *" + methodName + "_ReturnValue;\n\n"
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

    @staticmethod
    def _generateMethodContent(returnType, className, methodName):
        _content = INDENT + "FSeam::" + className + "Data data;\n"
        _additional = ", &data"
        _returnStatement = INDENT + "return *data." + methodName + "_ReturnValue;"

        if 'void' == returnType:
            _returnStatement = ""
            _additional = ""
        _content += INDENT + "auto mockVerifier = (FSeam::MockVerifier::instance().isMockRegistered(this)) ?\n"
        _content += INDENT2 + "FSeam::MockVerifier::instance().getMock(this) :\n"
        _content += INDENT2 + "FSeam::MockVerifier::instance().getDefaultMock(\"" + className + "\");\n\n"
        _content += INDENT + "mockVerifier->invokeDupedMethod(__func__" + _additional + ");\n"
        _content += INDENT + "mockVerifier->methodCall(__func__, std::any(data));\n"
        _content += _returnStatement
        return _content

    def _clearDataStructureData(self, content, className):
        indexBegin = content.find("struct " + className + "Data")
        indexEnd = content.find("// End of DataStructure" + className) + len("// End of DataStructure" + className)
        if indexBegin > 0 and indexEnd > len("// End of DataStructure" + className) + 1:
            content = content[0: indexBegin] + content[indexEnd + 1:]
        return content


def generateFSeamFile(filePath, destinationFolder, forceGeneration=False):
    """
    Client exposed method, will create the FSeam mock file and fill them with the content provided by the FSeam parser

    :param filePath: path of the cpp header file to parse in order to generate the seam mock
    :param destinationFolder: folder in which the generated folder will be created
    :param forceGeneration: if there are no need to generate the FSeam mock (mock, apparently, up to date) this flag
                            make it able to bypass those check and to generate brand new mock anyway (the MockData.hpp
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

    _fileCreatedMockDataPath = os.path.normpath(destinationFolder + "/MockData.hpp")
    _fileCreatedMockDataContent = ""
    if os.path.exists(_fileCreatedMockDataPath):
        with open(_fileCreatedMockDataPath, "r") as _fileCreatedMockData:
            _fileCreatedMockDataContent = _fileCreatedMockData.read().replace(LOCKING_FOOTER, "")
    with open(_fileCreatedMockDataPath, "w") as _fileCreatedMockData:
        _fileCreatedMockData.write(_fSeamerFile.generateDataStructureContent(_fileCreatedMockDataContent))
    print("FSeam generated file MockData.hpp at " + os.path.abspath(destinationFolder))


if __name__ == '__main__':
    _args = sys.argv[1:]
    if len(_args) < 2:
        raise NameError("Error missing argument for generation")
    _forceGeneration = True
    if len(_args) > 2:
        _forceGeneration = _args[2]
    generateFSeamFile(_args[0], _args[1], _forceGeneration)
