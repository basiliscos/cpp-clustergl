/*******************************************************************************
	ClusterGL - main.cpp
*******************************************************************************/
#include "main.h"


/*******************************************************************************
	Globals
*******************************************************************************/
static bool bHasInit = false;

Config *gConfig = NULL;

/*******************************************************************************
	Entry if invoked as a renderer output
*******************************************************************************/
int App::run(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr,"usage: %s <window id>\n", argv[0]);
		exit(0);
	}
	
	if(bHasInit) {
		return 1;
	}
	
	init(false, argv[1]);
	
	//Set up the module chain	
	mModules.push_back(new NetSrvModule());
	//mModules.push_back(new InsertModule());
	mModules.push_back(new ExecModule());

	while( tick() ){ 
	    //run tick() until we decide to bail
	}
	
	return 0;
}


/*******************************************************************************
	Entry if invoked as capture (shared library)
*******************************************************************************/
int App::run_shared()
{
	//This is needed to ensure that multiple calls to SDL_Init() don't cause
	//us to do the wrong thing.
	if(bHasInit) {
		return 1;
	}
	
	init(true, "capture");
	
	for(int i=0;i<gConfig->numOutputs;i++){
		LOG("Found output: %s:%d\n", 
				gConfig->outputAddresses[i].c_str(), 
				gConfig->outputPorts[i]);
	}
			
	//Set up the module chain
	mModules.push_back(new AppModule(""));
	//mModules.push_back(new TextModule()); 	
	mModules.push_back(new NetClientModule());

	//Return control to the parent process.

	return 0;
}

/*******************************************************************************
	Load the config file, init various internal variables
*******************************************************************************/
void App::init(bool shared, const char *id)
{
    printf("**********************************************\n");
	printf(" ClusterGL(%s - %s)\n", bIsIntercept ? "intercept" : "renderer", id);
	printf("**********************************************\n");	

    bIsIntercept = shared;

	gConfig = new Config("cgl.conf", string(id ? id : "null"));

	bHasInit = true;
}


/*******************************************************************************
	Main loop
*******************************************************************************/
bool App::tick()
{
	//LOG("tick()\n");
	
	thisFrame = new list<Instruction>();
	
	if(gConfig->enableStats){
	    stats_begin();
	}
/*
	//Make sure we have a real frame
	if (!thisFrame) {
		thisFrame = &oneFrame;
		for(int i=0;i<(int)mModules.size();i++)
			mModules[i]->prevFrame = &twoFrame;
	}
*/

	//Go through each module and process the frame
	for(int i=0;i<(int)mModules.size();i++) {
		Module *m = mModules[i];
		if( !m->process(*thisFrame) ) {
			LOG("Failed to process frame (in %d), bailing out\n", i);
			return false;
		}
	}
/*
	//return appropriate frames
	for(std::list<Instruction>::iterator iter = thisFrame->begin();
	    iter != (*thisFrame).end(); iter++) {
		for(int i=0;i<3;i++) {
			//A bit dodgy. This is how we determine if it was created on this
			//end of the network
			if(iter->buffers[i].needReply && iter->buffers[i].needClear) {
				mModules[0]->reply(&(*iter), i);
			}
		}
	}
*/
/*
	//Sync frames if necessary
	if(gConfig->syncRate > 0) {
		if (totalFrames % syncRate == 0 && totalFrames > 0) {
			for(int i=0;i<(int)mModules.size();i++) {
				Module *m = mModules[i];
				if( !m->sync() ) {
					LOG("Failed to sync frame (in %d), bailing out\n", i);
					return false;
				}
			}
		}
	}
*/

	if(gConfig->enableStats){
	    stats_end();
	}

/*	
	//Swap frames
	for(int i=0;i<(int)mModules.size();i++){
		mModules[i]->prevFrame = thisFrame;

        
	if(thisFrame == &oneFrame){
		thisFrame = &twoFrame;
	}else{
		thisFrame = &oneFrame;
	}

	//clear previous frames
	for(std::list<Instruction>::iterator iter = thisFrame->begin();
	iter != (*thisFrame).end(); iter++) {
		iter->clear();
	}
	thisFrame->clear();    
*/	
	delete thisFrame;

	return true;
}

/*******************************************************************************
	Begin stats run
*******************************************************************************/
void App::stats_begin(){

}

/*******************************************************************************
	End stats run
*******************************************************************************/
void App::stats_end(){
 
}


/*******************************************************************************
	main()
*******************************************************************************/
App *theApp = NULL;

int main( int argc, char **argv )
{
	theApp = new App();
	int ret = theApp->run(argc, argv);
	delete theApp;
	return ret;
}

//The shared object entry is now in mod_app
