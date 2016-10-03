#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofEnableDepthTest();
    cam.setDistance(100);
    
    setup_gui();
    
    sim = new Simulation();
    setValues();
  
    time = getDate();
    
    light = new ofLight();
    light->setPointLight();
    light->setPosition(1000, 1000, 0);
    
    shader.load("shadersGL2/shader");
}

//--------------------------------------------------------------
void ofApp::update(){
    if (!pause){
        setValues();
        sim->set_split_threshold(split_threshold);
        sim->update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetWindowTitle("FPS: " + ofToString(ofGetFrameRate()) +
                     " Population: " + ofToString(sim->get_population()));
    

    
    ofBackgroundGradient(ofColor::white, ofColor::gray);
    
    ofPushStyle();
    cam.begin();
    
    if (lights) shader.begin();
    
    ofSetColor(color);
    
    if (render_springs) sim->render_springs();
    if (render_spheres) sim->render_spheres(sphere_size);
    if (render_boxes) sim->render_boxes();
    if (render_normals) sim->render_normals();
    
    sim->render_bounding_box();
    
    if (lights) shader.end();
    
    cam.end();
    ofPopStyle();
    
    ofDisableDepthTest();
    if(!bHide) gui.draw();
    ofEnableDepthTest();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key=='p') pause = !pause ;
    if (key==' ') sim = new Simulation();
    if (key=='u') sim->update();
    if (key == 'h') bHide = !bHide;
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

void ofApp::setup_gui(void){
    
    resetButton.addListener(this, &ofApp::resetButtonPressed);
    saveButton.addListener(this, &ofApp::saveButtonPressed);
    
    gui.setup(); // most of the time you don't need a name
    
    gui.add(roi_squared.setup("roi_squared", 0.25, 0.0, 1.0));
    gui.add(spring_factor.setup("spring_factor", 0.1, 0.0, 1.0));
    gui.add(bulge_factor.setup("bulge_factor", 0.0 , 0.0, 1.0));
    gui.add(planar_factor.setup("planar_factor", 0.0, 0.0, 1.0));
    gui.add(repulsion_strength.setup("repulsion_strength", 0.1, 0, 1.0));
    gui.add(split_threshold.setup("split_threshold", 100, 0, 500));
    gui.add(spring_decay_rate.setup("spring_decay_rate", 1.0, 0.0, 1.0));
    gui.add(link_rest_length.setup("link_rest_length", 5.0, 1.0, 10.0));
    
	gui.add(color.setup("color", ofColor(100, 100, 140), ofColor(0, 0), ofColor(255, 255)));
	gui.add(circleResolution.setup("circle res", 5, 3, 90));
    gui.add(render_springs.setup("render_springs", true));
    gui.add(render_spheres.setup("render_spheres", false));
    gui.add(render_boxes.setup("render_boxes", false));
    gui.add(render_normals.setup("render_normals", false));
    gui.add(sphere_size.setup("sphere_size", 1.0, 0.0, 10));
    
    gui.add(lights.setup("lights", false));
    gui.add(pause.setup("pause", true));
	gui.add(resetButton.setup("reset"));
    gui.add(saveButton.setup("save"));
	gui.add(screenSize.setup("screen size", ofToString(ofGetWidth())+"x"+ofToString(ofGetHeight())));

	bHide = false;
}

//--------------------------------------------------------------
void ofApp::resetButtonPressed(){
    sim->reset();
}

void ofApp::saveButtonPressed(){
    string s = *sim->point_list();
    
    string file_name = "saved/saved_output_";
    file_name.append(time + "_frame" + ofToString(ofGetFrameNum()));
    
    ofBuffer msg(s.c_str(), s.length());
    ofBufferToFile(file_name, msg);
}

string ofApp::getDate(void){
    return ofToString(ofGetMonth()) + "_" + ofToString(ofGetDay()) + "_" +
    ofToString(ofGetHours()) + "_" + ofToString(ofGetMinutes()) + "_" +
    ofToString(ofGetSeconds());
}

void ofApp::setValues(void){
    sim->set_values(roi_squared, spring_factor, bulge_factor, planar_factor, repulsion_strength, spring_decay_rate, link_rest_length);
}