#include <math.h>
#include <algorithm>

#include "ofApp.h"
#include "main.h"

//========================================================================
int main( ){
	ofSetupOpenGL(WIDTH,HEIGHT,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
}