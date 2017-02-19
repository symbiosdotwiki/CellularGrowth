#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableLighting();
    
    areaLight.setup();
    areaLight.enable();
    areaLight.setAreaLight(120,400);
    //areaLight.setSpotlight(80,3);
    areaLight.setAmbientColor(ofFloatColor(0.4,0.4,0.4));
    areaLight.setAttenuation(1.0,0.0001,0.0001);
    areaLight.setDiffuseColor(ofFloatColor(1,1,1));
    areaLight.setSpecularColor(ofFloatColor(1,1,1));
    areaLight.rotate(-90,ofVec3f(1,0,0));
    areaLight.setPosition(0,-200,0);
    material.setAmbientColor(ofFloatColor(0.1,0.1,0.1,1.0));
    material.setDiffuseColor(ofFloatColor(0.8,0.8,0.8,1.0));
    material.setSpecularColor(ofFloatColor(0.8,0.8,0.8,1.0));
    //material.setShininess(10);
    
    ofSetFrameRate(60);
    ofEnableDepthTest();
    cam.setDistance(100);
    
    setup_gui();
    
    sim = new Simulation();
    setValues();
  
    time = getDate();

    shader.load("shadersGL2/shader");
}

//--------------------------------------------------------------
void ofApp::update(){
    if (!pause){
        setValues();
        sim->set_split_threshold(split_threshold);
        sim->update();
        
        update_mesh();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetWindowTitle("FPS: " + ofToString(ofGetFrameRate()) +
                     " Population: " + ofToString(sim->get_population()));
    
    ofBackgroundGradient(ofColor::black, ofColor::gray);
    
    //ofPushStyle();
    cam.begin();
    
    ofEnableLighting();
    areaLight.enable();
    //areaLight.draw();
    //shader.begin();
    //shader.setUniform2f("clipping", depth->x, depth->y);
    //material.begin();
    
    ofSetColor(ofColor::white);
    
    //render_simulation();
    
    
    m.draw();
    material.end();
    areaLight.disable();
    ofDisableLighting();
    ofSetColor(ofColor::white);
    m.drawWireframe();
    
    cam.end();
    //ofPopStyle();
    
    ofDisableDepthTest();
    if(!bHide) gui.draw();
    ofEnableDepthTest();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key =='p') pause = !pause ;
    if (key ==' ') sim = new Simulation();
    if (key =='u') sim->update();
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
    
    gui.add(roi_squared.setup("roi_squared", 0.25, 0.0, 27.0));
    gui.add(spring_factor.setup("spring_factor", 0.1, 0.0, 1.0));
    gui.add(bulge_factor.setup("bulge_factor", 0.0 , 0.0, 1.0));
    gui.add(planar_factor.setup("planar_factor", 0.0, 0.0, 1.0));
    gui.add(repulsion_strength.setup("repulsion_strength", 0.1, 0, 1.0));
    gui.add(split_threshold.setup("split_threshold", 100, 0, 500));
    gui.add(link_rest_length.setup("link_rest_length", 5.0, 0.1, 10.0));
    gui.add(food_mode.setup("food_mode", 1, 0, 2));
    
	gui.add(color.setup("color", ofColor(100, 100, 140), ofColor(0, 0), ofColor(255, 255)));
    gui.add(render_springs.setup("render_springs", true));
    gui.add(render_spheres.setup("render_spheres", false));
    gui.add(render_boxes.setup("render_boxes", false));
    gui.add(render_normals.setup("render_normals", false));
    gui.add(sphere_size.setup("sphere_size", 1.0, 0.0, 10));
    
    gui.add(lights.setup("lights", false));
    gui.add(depth.setup("depth", ofVec2f(0, 1000), ofVec2f(0, 0), ofVec2f(1000,1000)));
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
    m.save("meshes/mesh" +  ofToString(ofGetFrameNum()) + ".ply");
    /*
    string s = *sim->point_list();
    
    string file_name = "saved/saved_output_";
    file_name.append(time + "_frame" + ofToString(ofGetFrameNum()));
    
    ofBuffer msg(s.c_str(), s.length());
    ofBufferToFile(file_name, msg);
    */
}

string ofApp::getDate(void){
    return ofToString(ofGetMonth()) + "_" + ofToString(ofGetDay()) + "_" +
    ofToString(ofGetHours()) + "_" + ofToString(ofGetMinutes()) + "_" +
    ofToString(ofGetSeconds());
}

void ofApp::setValues(void){
    sim->set_values(roi_squared, spring_factor, bulge_factor, planar_factor, repulsion_strength, link_rest_length, food_mode);
}

void ofApp::render_simulation(void){
    for (Cell* c : *sim->get_cells()){
        if (render_spheres){
            ofSetColor(color);
            ofFill();
            Vec3f pos = c->get_position();
            ofDrawIcoSphere(pos.x, pos.y, pos.z, sphere_size);
        }
        if (render_springs){
            ofSetLineWidth(1);
            ofSetColor(color);
            for (Cell* n : *c->get_connections()){
                Vec3f p1 = c->get_position();
                Vec3f p2 = n->get_position();
                ofDrawLine(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
            }
        }
        if (render_normals){
            ofSetColor(ofColor::red);
            Vec3f p1 = c->get_position();
            Vec3f p2 = c->cell_normal + p1;
            ofDrawLine(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
        }
    }
    ofNoFill();
    ofDrawBox(0,0,0,sim->get_size());
    
    //    if (render_boxes) sim->render_boxes();
    //    if (render_normals) sim->render_normals();
    
}

void ofApp::update_mesh(void){
    m.clear();
    
    m.setMode(ofPrimitiveMode::OF_PRIMITIVE_TRIANGLES);
    Vec3f f, g, h;
    for (Cell* c: *sim->get_cells()){
        std::vector<Cell*> loop = c->get_ordered_neighbors();
        
        f = c->position;
        
        for (int i=0; i<loop.size(); i++){
            g = loop[i]->position;
            h = loop[(i+1)%loop.size()]->position;
            
            m.addVertex(ofPoint(f.x, f.y, f.z));
            m.addVertex(ofPoint(g.x, g.y, g.z));
            m.addVertex(ofPoint(h.x, h.y, h.z));
        }
    }
}