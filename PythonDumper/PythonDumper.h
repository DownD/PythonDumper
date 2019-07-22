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
	//No exclude modules
	static int getPythonModulesAndFunctions(PythonModuleMap* moduleMap);

	//Allows to specify some modules to be excluded from the dump
	static int getPythonModulesAndFunctions(PythonModuleMap* moduleMap,std::vector<std::string>* excludeModules);

private:
	typedef std::unordered_map<std::string, std::string> BuiltinModules;
	static bool getPythonFunctionMap(PyObject* module, FunctionMap* map);
	static bool getBuiltInModules(BuiltinModules* mod);
};