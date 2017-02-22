#pragma once

#include "main.h"
#include "ofxGui.h"
#include <thread>

#define WIDTH 1920
#define HEIGHT 1080

#define ROI_SQUARED 9
#define SPRING_FACTOR 0.14
#define BULGE_FACTOR 0.05
#define PLANAR_FACTOR 0.305
#define REPULSION_STRENGTH 0.24
#define LINK_REST_LENGTH 2.2
#define FOOD_MODE 2
#define SPLIT_THRESHOLD 35

class ofApp : public ofBaseApp{

	public:
    
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    
        void setup_gui(void);
    	void resetButtonPressed(void);
        void saveButtonPressed(void);
        string getDate(void);
    
        void new_sim(void);
        void render_simulation(void);
        void draw_springs(void);
        void draw_cells(void);
        void update_mesh(void);

        ofEasyCam cam;
        Simulation * sim;
        ofMesh m;
        ofMaterial material;
        ofShader shader;
    
        bool bHide, current_mesh;
        float rotation;

        // gui
        ofxFloatSlider roi_squared, spring_factor, bulge_factor, planar_factor, repulsion_strength, link_rest_length;
        ofxFloatSlider feed, kill, ra, rb;
        ofxFloatSlider split_threshold;
        ofxIntSlider food_mode;
        ofxColorSlider color;
        ofxToggle render_springs, render_spheres, render_boxes, render_normals, render_mesh;
        ofxFloatSlider sphere_size;
    
        ofxToggle lights, pause;
        ofxVec2Slider depth;
        ofxButton resetButton, saveButton;
        ofxLabel screenSize;

        ofxPanel gui;
    
        string time;

};
