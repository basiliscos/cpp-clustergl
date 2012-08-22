/*******************************************************************************
 libcglinput 
*******************************************************************************/
#include "main.h"

//Globals
IInject *mInject = NULL;
Config *mConfig = NULL;

/*******************************************************************************
 Entry point 
*******************************************************************************/
bool run(string method){
	LOG("run(%s)\n", method.c_str());

	mConfig = new Config("libcglinput.cfg");

	if(!mInject){
		LOG("Injection method wasn't initialised!");
		return false;
	}

	if(!begin_web()){
		LOG("Failed to start web server\n");
		return false;
	}

	if(!begin_spacenav()){
		LOG("Failed to start spacenav listener");
	}

	return true;
}

