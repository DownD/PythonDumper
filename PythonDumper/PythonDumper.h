#pragma once
#include "Python.h"
#include <stdio.h>
#include <Windows.h>
#include <map>
#include <unordered_map>


typedef std::map<std::string, PyCFunction> FunctionMap;
typedef std::map<std::string, int> IntegerMap;

struct Methods {
	FunctionMap functions;
	IntegerMap integerVariables;
};

typedef std::map<std::string, Methods> PythonModuleMap;

class PythonDumper {

public:
	//No exclude modules
	static int getPythonModulesAndFunctions(PythonModuleMap* moduleMap);

	//Allows to specify some modules to be excluded from the dump
	static int getPythonModulesAndFunctions(PythonModuleMap* moduleMap,std::vector<std::string>* excludeModules);

private:
	typedef std::unordered_map<std::string, std::string> BuiltinModules;
	static bool getPythonModuleMap(PyObject* module, FunctionMap* map, IntegerMap* mapInt);
	static bool getBuiltInModules(BuiltinModules* mod);
};