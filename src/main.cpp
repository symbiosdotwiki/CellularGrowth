#include <math.h>
#include <algorithm>

#include "ofApp.h"
#include "main.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1920,1080,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
}


//===========================================================================
Simulation::Simulation(void){
    g = new Grid(50,100);
    
    vector<ofPoint> icos_vert =  subdivided_icosahedron(2);
    for (ofPoint p : icos_vert){
        p.normalize();
        Cell *c = new Cell(p*10.0);
        g->add_cell(c);
    }
    init_springs(4);
}

void Simulation::init_springs(float radius){
    for (Cell * c1 : g->iter()){
        for (Cell * c2 : g->iter()){
            if ((c1 != c2) and (c1->get_position().distance(c2->get_position())) < radius) {
                c1->add_spring(c2);
            }
        }
    }
}

vector<Cell*> Simulation::find_collisions(Cell * c){
    vector<Cell*> colliderz;
    float roi_squared = c->get_roi();
    for (Cell * other : g->iter()){
        if ((not c->is_connected(other)) and (c != other)
            and (c->get_position().squareDistance(other->get_position()) < roi_squared)){
            colliderz.push_back(other);
        }

    }
    return colliderz;
}

void Simulation::update(){
    bool split_this_update = false;
    
    for (Cell * c : g->iter()){
        vector<Cell*> collisions = g->get_collisions(c);
        c->update(collisions);
        
        c->add_food(ofRandom(1));
        if (c->get_food_amount() > split_threshold){
            split_this_update = true;
        }
    }
    
    if (split_this_update){
        vector<Cell *> new_cells;
    
        for (Cell * c : g->iter()){
            if (c->get_food_amount() > split_threshold){
                Cell * bb = c->split();
                new_cells.push_back(bb);
            }
        }
        for (Cell* c: new_cells){
            g->add_cell(c);
        }
    }
    
    for (Cell* c: g->iter()){
        c->move();
    }
    
    g->update_positions();
    
    
//    average_positions();
}

void Simulation::average_positions(void){
    ofPoint average = ofPoint(0,0,0);
    for (Cell* c: g->iter()){
        average += c->get_position();
    }
    
    average /= g->get_size();
    
    for (Cell* c : g->iter()){
        c->set_position(c->get_position()-average);
    }
}

void Simulation::render(void){
    ofPushStyle();
    
    render_springs();
    //render_normals();
//    render_spheres(.5);
    g->draw_boxes();
    
    ofPopStyle();
}

void Simulation::render_springs(void){
    for (Cell * c : g->iter()){
        c->draw_springs();
    }
}

void Simulation::render_spheres(float radius){
    for (Cell * c : g->iter()){
        c->draw_cell(radius);
    }
}

void Simulation::render_normals(void){
    for (Cell * c : g->iter()){
        c->draw_normal();
    }
}

void Simulation::render_bounding_box(void){
    g->draw_bounding_box();
}

void Simulation::render_boxes(void){
    g->draw_boxes();
}

int Simulation::get_population(void){
    return g->get_size();
}

void Simulation::set_values(float _link_rest_length, float _roi_squared,
                    float _spring_factor, float _bulge_factor, float _planar_factor,
                           float _repulsion_strength){
    for (Cell * c: g->iter()){
        c->set_values(_link_rest_length, _roi_squared, _spring_factor, _bulge_factor, _planar_factor, _repulsion_strength);
    }
}

void Simulation::set_split_threshold(float _split_threshold){
    split_threshold = _split_threshold;
}


//========================================================================
// Sphere creation function

vector<ofPoint> icosa_vertices(void){
    vector<ofPoint> vertices(12);
    
    double theta = 26.56505117707799 * PI / 180.0;
    
    double stheta = sin(theta);
    double ctheta = cos(theta);
    
    vertices[0] = ofPoint(0,0,-1);
    
    // the lower pentagon
    double phi = PI / 5.0;
    for (int i = 1; i < 6; ++i) {
        vertices[i] = ofPoint(ctheta * cos(phi), ctheta * sin(phi), -stheta);
        
        phi += 2.0 * PI / 5.0;
    }
    
    // the upper pentagon
    phi = 0.0;
    for (int i = 6; i < 11; ++i) {
        vertices[i] = ofPoint(ctheta * cos(phi), ctheta * sin(phi), stheta);
        
        phi += 2.0 * PI / 5.0;
    }
    
    vertices[11] =ofPoint(0,0,1); // the upper vertex
    
    return vertices;
}

void subdivide_iteration(deque<ofPoint>* vertices){
    int original_size = vertices->size();
    
    for (int i = 0; i < original_size; i += 3){
        ofPoint v1 = vertices->front();
        vertices->pop_front();
        ofPoint v2 = vertices->front();
        vertices->pop_front();
        ofPoint v3 = vertices->front();
        vertices->pop_front();
        
        ofPoint v4 = (v1 + v2) / 2.0;
        ofPoint v5 = (v2 + v3) / 2.0;
        ofPoint v6 = (v3 + v1) / 2.0;
        
        vertices->push_back(v1);
        vertices->push_back(v4);
        vertices->push_back(v6);
        
        vertices->push_back(v4);
        vertices->push_back(v2);
        vertices->push_back(v5);
        
        vertices->push_back(v6);
        vertices->push_back(v5);
        vertices->push_back(v3);
        
        vertices->push_back(v6);
        vertices->push_back(v4);
        vertices->push_back(v5);
    }
}


vector<ofPoint> remove_duplicates(deque<ofPoint>* dup_list){
    vector<ofPoint> unique_list;
    bool contains = false;
    while (not dup_list->empty()){
        ofPoint p = dup_list->front();
        dup_list->pop_front();
        unique_list.push_back(p);
        deque<ofPoint>::iterator it;
        
        for (it =dup_list->begin(); it !=dup_list->end();){
            if (it->match(p)){
                it = dup_list->erase(it);
            } else {
                ++it;
            }
        }
    }
    
    return unique_list;
}

vector<ofPoint> subdivided_icosahedron(int levels){
    vector<ofPoint> icosahedron = icosa_vertices();
    
    std::deque<ofPoint> vertices;
    
    int triangles[] = {0,2,1,0,3,2,0,4,3,0,5,4,0,1,5,1,2,7,2,3,8,3,4,9,4,5,10,5,1,6,1,
        7,6,2,8,7,3,9,8,4,10,9,5,6,10,6,7,11,7,8,11,8,9,11,9,10,11,10,6,11};
    
    for (int i : triangles){
        vertices.push_back(icosahedron[i]);
    }
    
    for (int i = 0; i < levels; i++){
        subdivide_iteration(&vertices);
    }
    
    vector<ofPoint> final_point_list = remove_duplicates(&vertices);
    
    return final_point_list;
}

vector<ofPoint> init_sphere_points(float n, float r){
    
    vector<ofPoint> return_me;
    // distribute n cells of a sphere of radius r
    int n_count = 0;
    
    float a = 4*PI/n;
    float d = sqrt(a);
    
    float mv = round(PI/d);
    
    
    float dv = PI / mv;
    float dp = a / dv;
    
    for (int m = 0; m < mv; m++){
        float v = PI*(m+0.5) / mv;
        float mp = round(2*PI*sin(v) / dp);
        for (int j = 0; j < mp; j++){
            float p = 2*PI*j / mp;
            ofPoint point = ofPoint(r*sin(v)*cos(p),
                                    r*sin(v)*sin(p),
                                    r*cos(v));
            return_me.push_back(point);
            n_count += 1;
        }
    }
    return return_me;
}
