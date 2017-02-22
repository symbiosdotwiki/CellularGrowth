#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    ofEnableDepthTest();
    cam.setDistance(100);
    
    setup_gui();
    
    new_sim();
    
    time = getDate();

    shader.load("shadersGL2/shader");
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (!pause){
        sim->update();
        current_mesh = false;
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetWindowTitle("FPS: " + ofToString(ofGetFrameRate()) +
                     " Population: " + ofToString(sim->get_population()));
    
    ofBackgroundGradient(ofColor::black, ofColor::gray);
    
    //ofPushStyle();
    cam.begin();
    
    ofSetColor(ofColor::white);
    
    ofPushMatrix();
    ofRotate((float)ofGetFrameNum()*0.1, 0, 1, 0);
    render_simulation();

    /*
    ofPushStyle();
    ofFill();
    ofSetColor(255, 0, 0);
    
    for (Face f:sim->faces){
        Vec3f a = f.a->position;
        Vec3f b = f.b->position;
        Vec3f c = f.c->position;
        
        Vec3f cross = (a-b).cross(c-b);
        

        ofDrawTriangle(a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z);
    }
    ofPopStyle();
    */
    
    
    ofPopMatrix();

    
    cam.end();
    //ofPopStyle();
    
    ofDisableDepthTest();
    if(!bHide) gui.draw();
    ofEnableDepthTest();

    
}

void ofApp::new_sim(void){
    sim = new Simulation(   roi_squared,
                            spring_factor,
                            bulge_factor,
                            planar_factor,
                            repulsion_strength,
                            link_rest_length,
                            food_mode,
                            split_threshold);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key =='p') pause = !pause ;
    if (key ==' ') new_sim();
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
    
    gui.add(roi_squared.setup("roi_squared", ROI_SQUARED, 0.0, 27.0));
    gui.add(spring_factor.setup("spring_factor", SPRING_FACTOR, 0.0, 1.0));
    gui.add(bulge_factor.setup("bulge_factor", BULGE_FACTOR , 0.0, 1.0));
    gui.add(planar_factor.setup("planar_factor", PLANAR_FACTOR, 0.0, 1.0));
    gui.add(repulsion_strength.setup("repulsion_strength", REPULSION_STRENGTH, 0, 1.0));
    gui.add(split_threshold.setup("split_threshold", SPLIT_THRESHOLD, 1, 150));
    gui.add(link_rest_length.setup("link_rest_length", LINK_REST_LENGTH, 0.1, 10.0));
    gui.add(food_mode.setup("food_mode", FOOD_MODE, 0, 4));
    
	gui.add(color.setup("color", ofColor(100, 100, 140), ofColor(0, 0), ofColor(255, 255)));
    gui.add(render_springs.setup("render_springs", true));
    gui.add(render_spheres.setup("render_spheres", false));
    gui.add(render_boxes.setup("render_boxes", false));
    gui.add(render_normals.setup("render_normals", false));
    gui.add(render_mesh.setup("render_mesh", false));
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
    update_mesh();
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

void ofApp::render_simulation(void){
    for (Cell* c : *sim->get_cells()){
        if (render_spheres){
            ofSetColor(color);
            ofFill();
            Vec3f pos = c->get_position();
            ofDrawIcoSphere(pos.x, pos.y, pos.z, sphere_size);
        }
        /*
         // moved this to mesh wireframe
        if (render_springs){
            ofSetLineWidth(1);
            ofSetColor(color);
            for (Cell* n : *c->get_connections()){
                Vec3f p1 = c->get_position();
                Vec3f p2 = n->get_position();
                ofDrawLine(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
            }
        }
         */
        if (render_normals){
            ofSetColor(ofColor::red);
            Vec3f p1 = c->get_position();
            Vec3f p2 = c->cell_normal.getNormalized() + p1;
            ofDrawLine(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
        }
    }
    
    if ((render_mesh or render_springs) and not current_mesh){
        update_mesh();
    }
    
    if (render_mesh){
        ofPushStyle();
        ofSetColor(color);
        ofFill();
        m.draw();
        ofSetColor(ofColor::white);
        m.drawWireframe();
        ofPopStyle();
    }
    if (render_springs){
        ofPushStyle();
        ofSetColor(ofColor::white);
        m.drawWireframe();
        ofPopStyle();
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
    
    for (Face f : sim->faces){
        
        
        m.addVertex(ofPoint(f.a->position.x, f.a->position.y, f.a->position.z));
        
        m.addVertex(ofPoint(f.b->position.x, f.b->position.y, f.b->position.z));
        m.addVertex(ofPoint(f.c->position.x, f.c->position.y, f.c->position.z));
    }
    
    /*
    for (Cell* c: *sim->get_cells()){
        std::vector<Cell*> loop = c->ord_neigh;
        
        f = c->position;
        
        for (int i=0; i<loop.size(); i++){
            g = loop[i]->position;
            h = loop[(i+1)%loop.size()]->position;
            
            m.addVertex(ofPoint(f.x, f.y, f.z));
            m.addVertex(ofPoint(g.x, g.y, g.z));
            m.addVertex(ofPoint(h.x, h.y, h.z));
        }
    }
     */
}