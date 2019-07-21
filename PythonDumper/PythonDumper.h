#pragma once
#include "Python.h"
#include <stdio.h>
#include <Windows.h>
#include <map>
#include <unordered_map>

typedef std::map<std::string, PyCFunction> FunctionMap;
typedef std::map<std::string, FunctionMap> PythonModuleMap;


class PythonDumper {

public:
	static int getPythonModulesAndFunctions(PythonModuleMap* moduleMap);
	static int getPythonModulesAndFunctions(PythonModuleMap* moduleMap,std::vector<std::string>* excludeModules);

private:
	typedef std::unordered_map<std::string, std::string> BuiltinModules;
	static bool getPythonFunctionMap(PyObject* module, FunctionMap* map);
	static bool getBuiltInModules(BuiltinModules* mod);
};