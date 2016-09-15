#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofEnableDepthTest();
    
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
    ofSetWindowTitle("FPS: " + ofToString(ofGetFrameRate()));
    
    cout << "Frame Number: " + ofToString(ofGetFrameNum()) << endl;
    cout << ofToString(sim->get_population()) << endl;
    cam.begin();
    light->enable();
    sim->render();
    light->disable();
    cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    sim->update();

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
    sim->update();
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
