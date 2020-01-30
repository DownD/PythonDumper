#include "PythonDumper.h"
#include "shlwapi.h"

bool setupPython()
{
	HMODULE mod = GetModuleHandle(L"python27");
	if (!mod) {
		DEBUG_INFO("Error finding Python27 dll for hooking %d", GetLastError()); return NULL;
	}

	PyRunSimpleFileEx = (tPyRunSimpleFileEx)GetProcAddress(mod, "PyRun_SimpleFileEx");
	if (!PyRunSimpleFileEx) { DEBUG_INFO("Error finding PyRunSimpleFileEx function for Hooking Error Code: %d", GetLastError()); return NULL; }
	PyFileFromString = (tPyFileFromString)GetProcAddress(mod, "PyFile_FromString");
	if (!PyFileFromString) { DEBUG_INFO("Error finding PyFileFromString function for Hooking Error Code: %d", GetLastError()); return NULL; }
	return true;
}

bool pythonExecuteFile(char* filePath) {
	//char* arr = new char[strlen(filePath)];
	//strcpy(arr, filePath);
	//PathStripPathA(arr);
	int result = -1;
	//__try {
		PyObject* PyFileObject = PyFileFromString(filePath, (char*)"r");
		printf("1\n");
		//FILE* f = fopen(arr, "r");
		printf("2\n");
		if (PyFileObject == NULL) {
			printf("Error Not a File\n");
			//delete[] arr;
			return 0;
		}
		result = PyRunSimpleFileEx(PyFile_AsFile(PyFileObject), "somethig", 1);
		printf("3\n");
	//}__finally{
		//printf("Exception\n");
	//}
	//delete[] arr;
	printf("Returning\n");
	if (result == -1)
		return false;
	else
		return true;
}

bool PythonDumper::getPythonModuleMap(PyObject* module, FunctionMap* mapFunc, IntegerMap* mapInt) {
	PyObject *func_dict = PyModule_GetDict(module);
	if (!PyDict_Check(func_dict)) {
		return false;
	}
	PyObject *func_key, *func_value = NULL;
	Py_ssize_t pos = 0;

	while (PyDict_Next(func_dict, &pos, &func_key, &func_value)) {
		if (PyCFunction_Check(func_value)) {
			std::string func_name(PyString_AsString(func_key));
			PyCFunction func = PyCFunction_GET_FUNCTION(func_value);
			mapFunc->insert({ func_name,func });
		}
		else if (PyInt_Check(func_value)) {
			std::string func_name(PyString_AsString(func_key));
			int value = PyInt_AsLong(func_value);
			mapInt->insert({ func_name,value });
		}
	}
}

bool PythonDumper::getBuiltInModules(BuiltinModules* mod) {
	PyObject* const sys_module = PyImport_ImportModule("sys");
	PyObject* const modules_touple = PyObject_GetAttrString(sys_module, "builtin_module_names");

	int tuple_size = PyTuple_GET_SIZE(modules_touple);
	for (int i = 0; i < tuple_size; i++) {
		PyObject* name = PyTuple_GetItem(modules_touple, i);
		std::string nameStr(PyString_AsString(name));
		mod->insert({ nameStr,nameStr });
	}
	return true;
}
int PythonDumper::getPythonModulesAndFunctions(PythonModuleMap * moduleMap)
{
	return getPythonModulesAndFunctions(moduleMap, &std::vector<std::string>());
}
int PythonDumper::getPythonModulesAndFunctions(PythonModuleMap* moduleMap, std::vector<std::string>* excludeModules)
{
	if (!Py_IsInitialized())
		return NULL;	//Python not initilized
	BuiltinModules builtModulesMap;
	getBuiltInModules(&builtModulesMap);

	for (std::string mod : *excludeModules) {
		builtModulesMap.insert({ mod,mod });
	}

	PyObject* const sys_module = PyImport_ImportModule("sys");
	PyObject* const modules_dict = PyObject_GetAttrString(sys_module, "modules");
	PyObject *module_key, *module_value = NULL;
	Py_ssize_t module_pos = 0;


	while (PyDict_Next(modules_dict, &module_pos, &module_key, &module_value)) {
		if (!module_value || !module_key) {
			continue;
		}
		std::string moduleName(PyString_AsString(module_key));
		if (!PyModule_Check(module_value)) {
			continue;
		}
		if (builtModulesMap.find(moduleName) != builtModulesMap.end())
			continue;

		Methods* methods = &(moduleMap->insert({ moduleName,Methods() }).first->second);
		getPythonModuleMap(module_value, &methods->functions,&methods->integerVariables);
	}
	return 1;
}
