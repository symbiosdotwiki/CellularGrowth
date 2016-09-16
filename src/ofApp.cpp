#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofEnableDepthTest();
    ofBackgroundGradient(ofColor::white, ofColor::lightGray);
    cam.setDistance(100);
    
    sim = new Simulation();
    
    light = new ofLight();
    light->setPointLight();
    light->setPosition(1000, 1000, 0);
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    sim->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetWindowTitle("FPS: " + ofToString(ofGetFrameRate()) +
                     " Population: " + ofToString(sim->get_population()));
    
    
//    cout << "Frame Number: " + ofToString(ofGetFrameNum()) << endl;
//    cout << ofToString(sim->get_population()) << endl;
    cam.begin();
    light->enable();
    sim->render();
    light->disable();
    cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key=='u') sim->update();
    if (key==' ') sim = new Simulation();

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
