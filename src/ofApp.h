#pragma once

//#include <libiomp/omp.h>
#include "main.h"
#include "ofxGui.h"

#define WIDTH 1920
#define HEIGHT 1080

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
        void setValues(void);
        string getDate(void);

        ofEasyCam cam;
        Simulation * sim;
        ofLight * light;
        ofShader shader;
    
    	bool bHide;

        // gui
        ofxFloatSlider roi_squared, spring_factor,bulge_factor, planar_factor, repulsion_strength, spring_decay_rate, link_rest_length;
        ofxFloatSlider split_threshold;
        ofxColorSlider color;
        ofxIntSlider circleResolution;
        ofxToggle render_springs, render_spheres, render_boxes, render_normals;
        ofxFloatSlider sphere_size;
    
        ofxToggle lights;
        ofxToggle pause;
        ofxButton resetButton, saveButton;
        ofxLabel screenSize;

        ofxPanel gui;
    
        string time;

};
