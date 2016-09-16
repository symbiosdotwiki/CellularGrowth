#include "ofMain.h"
#include "ofApp.h"
#include "main.h"

#include <math.h>
#include <vector>
#include <algorithm>
#include <stdexcept>

using namespace std;

//========================================================================
int main( ){
	ofSetupOpenGL(1920,1080,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}

//========================================================================
Cell::Cell(ofPoint _position){
    position = _position;
    original_pos = _position;
    age = 0;
//    col = ofColor::fromHsb(ofRandom(255), 255, 255, 255);
}

Cell::Cell(ofPoint _position, int _age, ofPoint _cell_normal){
    // use this constructor when spawning new cells
    position = _position;
    original_pos = _position;
    age = _age;
    cell_normal = _cell_normal;
    //col = ofColor::fromHsb(ofRandom(255), 255, 255, 255);
}

bool Cell::is_connected(Cell* c){
    return find(connections.begin(), connections.end(), c) != connections.end();
}

void Cell::add_spring(Cell * c){
    connections.push_back(c);
}

void Cell::add_food(float amount){
    food += amount;
}

void Cell::reset_food(void){
    food = 0;
}

float Cell::get_food_amount(void){
    return food;
}

float Cell::get_roi(void){
    return roi_squared;
}

vector<Cell*>* Cell::get_connections(void){
    return &connections;
}

void Cell::calculate_spring_target(void){
    /*
     The tendency for linked cells to maintain a constant distance from each other is implemented using a linear spring- like system. The target position for the springs is calculated by taking the average of the rest positions that each link would push the particle to if it were the only influence.
     
    */
    spring_target = ofPoint(0,0,0);
    for (Cell * c : connections){
        ofPoint delta =position - c->get_position();
        delta.normalize();
        delta*=link_rest_length;
        delta = delta + c->get_position() - position;
        spring_target += delta;
    }
    spring_target /= connections.size();
    spring_target += position;
    return spring_target;
}

void Cell::calculate_planar_target(void){
    /*
     The planar target position, which acts similar to a torsion spring, is simply calculated by taking the average of all the positions of directly linked particles. This is designed to have the effect of tending to reduce folds and bumps in the surface, restoring the surface to a local planar state.
    */
    
    bulge_target = ofPoint(0,0,0);
    for (Cell *c : connections){
        bulge_target += c->position;
    }
    bulge_target /= connections.size();
    bulge_target += position;
    return bulge_target;
}

void Cell::calculate_bulge_target(void){
    /*
     The bulge target position is determined by calculating the distance that each link would have to push the particle outwards along the direction of the surface normal in order to restore the link to its rest length. This is designed to have an effect of tending to bulge the surface outwards in the direction of the normal when links are in compression. The bulge distance due to each link is calculated by a simple application of the cosine formula for triangles, and the average taken for all the links to create the desired overall distance bulgeDist in the direction of the normal. The bulgeTarget vector is then taken by going bulgeDist in the direction of the surface normal from P.
     */
    bulge_distance = 0;
    for (Cell* c : connections){
        ofPoint L = c->get_position() - position;
        float dotN = (L - position).dot(cell_normal);
        float radicand = pow(link_rest_length, 2) - pow(L.length(), 2) + pow(dotN, 2);
        bulge_distance += sqrt(radicand) + dotN;
    }
    bulge_distance /= connections.size();
    bulge_target = position + (cell_normal * bulge_distance);
}


void Cell::calculate_collision_offset(vector<Cell*> collision_list){
    collision_offset = ofPoint(0,0,0);
    ofPoint temp;
    for (Cell* c : collision_list){
        temp = (position-c->get_position()).normalize();
        temp *= (roi_squared - (position - get_position()).lengthSquared())/roi_squared;
        collision_offset += temp;
    }
    collision_offset *= repulsion_strength;
}

void Cell::remove_spring(Cell* to_remove){
    connections.erase(remove(connections.begin(), connections.end(), to_remove), connections.end());
}

Cell* Cell::find_next(Cell* current, Cell* previous){
    /*
     Find the next cell that is connected to current and this but is not previous
     */
    for (Cell* d: *current->get_connections()){
        if ((d != this) and (d != previous) and
            (find (connections.begin(), connections.end(), d) != connections.end())){
            return d;
        }
    }
    throw 0;
    return NULL;
}

float Cell::get_average_link_len(void){
    float avg = 0;
    for (Cell* c : connections){
        avg += position.distance(c->get_position());
    }
    avg /= connections.size();
    cout << ofToString(avg) <<endl;
    return avg;
}

vector<Cell*> Cell::get_ordered_neighbors(void){
    //gotta start somewhere.. hb randomly
    vector<Cell*> ord_neigh;
    ord_neigh.push_back(connections[(int) ofRandom(connections.size())]);
    
    Cell* current;
    Cell* previous = this;
    
    // march through the loop
    do {
        current = find_next(ord_neigh.back(), previous);
        previous = ord_neigh.back();
        ord_neigh.push_back(current);
        
    } while (current != ord_neigh.front());
    
    
    return ord_neigh;
}

void Cell::calculate_normal(void){

    ofPoint temp = ofPoint(0,0,0);
    
    vector<Cell*> loop = get_ordered_neighbors();
    
    Cell * current = loop.front();
    Cell * previous = loop.back();
    
    
    for (int i = 0; i <loop.size(); i++){
        // add the normalized cross product
        ofPoint v1 = (current->get_position() - position);
        ofPoint v2 = (previous->get_position() - position);
        v1.cross(v2);
        
        v1.normalize();
        temp += v1;
        
        if (i + 1 < loop.size()){
            previous = loop[i];
            current = loop[i+1];
        }
    }
    temp.normalize();
    
    // on the first frame, point normal away from zero
    if (0 == age){
        if ((temp+position).lengthSquared() < ((-1*temp)+position).lengthSquared()){
            temp = -temp;
        }
    } else {
        if (cell_normal.dot(temp) < 0){
            temp *= -1;
        }
    }
                           
    cell_normal = temp;
}

Cell* Cell::compute_anchor(Cell* anchor1){
    Cell* anchor2;
    float max_dist = 0;
    for (Cell* c : connections){
        float cur_dist = c->get_position().distance(anchor1->get_position());
        if (cur_dist > max_dist){
            anchor2 = c;
            max_dist = cur_dist;
        }
    }
    return anchor2;
}

Cell* Cell::split(void){
    reset_food();
    vector<Cell*> ord_neigh = get_ordered_neighbors();
    
    Cell* bb = new Cell(position, age, cell_normal);
    
    Cell* anchor1 =ord_neigh[0];
    Cell* anchor2 =ord_neigh[ord_neigh.size()/2];
    
    
    // now traverse from one anchor to another
    Cell* current= find_next(anchor1, anchor1);
    Cell* previous = anchor1;
    
    vector<Cell*> loop;
    while (current != anchor2){
        loop.push_back(current);
        
        bb->add_spring(current);
        current->add_spring(bb);
        // now find the next point
        Cell* c_temp = find_next(current, previous);
        previous = current;
        current = c_temp;
    }
    
    for (Cell* c : *bb->get_connections()){
        if (c != this){
            this->remove_spring(c);
            c->remove_spring(this);
        }
    }
    
    bb->add_spring(this);
    this->add_spring(bb);
    
    bb->add_spring(anchor1);
    bb->add_spring(anchor2);
    anchor1->add_spring(bb);
    anchor2->add_spring(bb);
    
    // figure better way to set inital point positions...
    // How to determine a side?
    // Maybe average connections locations and move half way to that?
    
    ofPoint average1 = ofPoint(0,0,0);
    for (Cell* c : connections){
        average1 += c->get_position();
    }
    average1 /= connections.size();
    
    ofPoint average2 = ofPoint(0,0,0);
    for (Cell* c : *bb->get_connections()){
        average2 += c->get_position();
    }
    average2 /= bb->get_connections()->size();
    
    next_position= (position + average1)/2.0;
                 
    bb->next_position = ((bb->get_position() + average2) /2.0);
    
    return bb;
}

void Cell::update(vector<Cell*> collision_list){
    calculate_normal();
   
    calculate_spring_target();
    calculate_planar_target();
    calculate_bulge_target();
    calculate_collision_offset(collision_list);
    
    next_position = position;
    next_position += spring_factor * (spring_target - position);
    next_position += planar_factor * (planar_target - position);
    next_position += bulge_factor * (bulge_target - position);
    next_position += collision_offset;
    
    age++;
    
}

void Cell::move(void){
    position = next_position;
}

void Cell::draw_springs(void){
    ofSetColor(0);
    ofSetLineWidth(1);
    for (Cell * c : connections){
        ofDrawLine(position, c->position);
    }
}

void Cell::draw_loop(void){
    vector<Cell*> loop = get_ordered_neighbors();
    ofSetColor(255, 0, 0);
    ofSetLineWidth(3);
    for (int i = 0; i < loop.size(); i++){
        ofPoint p1 = loop[i]->get_position();
        ofPoint p2 = loop[(i+1)%loop.size()]->get_position();
        ofDrawLine(p1, p1+((p2-p1)/(i+1)));
    }
}

void Cell::draw_spring_target(float radius){
    ofSetColor(col);
    ofDrawBox(spring_target, radius);
}

void Cell::draw_normal(void){
    ofSetColor(ofColor::black);
    ofSetLineWidth(2);
    ofDrawLine(position, position + cell_normal);
}

void Cell::draw_cell(float radius){
    ofSetColor(col);
//    ofSetColor(ofMap(food, 0, 50, 0, 255));
    ofSetLineWidth(2);
    ofDrawIcoSphere(position, radius);
}

ofPoint Cell::get_position(void){
    return position;
}

void Cell::set_position(ofPoint new_pos){
    position = new_pos;
}

//========================================================================
Simulation::Simulation(void){
   // to do: generalize
    /*
    init_sphere_points(8, 10);
    init_springs(15);
    */
   
    /*
    init_sphere_points(32, 10);
    init_springs(9);
    */
    
    vector<ofPoint> icos_vert = icosa_vertices();
    for (ofPoint p : icos_vert){
        Cell *c = new Cell(p*10);
        cells.push_back(c);
    }
    init_springs(15);
}

void Simulation::init_sphere_points(float n, float r){
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
            Cell * c = new Cell(point);
            cells.push_back(c);
            n_count += 1;
        }
    }
}


void Simulation::init_springs(float radius){
    for (Cell * c1 : cells){
        for (Cell * c2 : cells){
            if ((c1 != c2) and (c1->get_position().distance(c2->get_position())) < radius) {
                c1->add_spring(c2);
            }
        }
    }
}

vector<Cell*> Simulation::find_collisions(Cell * c){
    vector<Cell*> colliderz;
    float roi_squared = c->get_roi();
    for (Cell * other : cells){
        if ((not c->is_connected(other)) and (c != other)
            and (c->get_position().squareDistance(other->get_position()) < roi_squared)){
            colliderz.push_back(other);
        }

    }
    return colliderz;
}

void Simulation::update(){
    bool split_this_update = false;
    for (Cell * c : cells){
        vector<Cell*> collisions = find_collisions(c);
        c->update(collisions);
        
        c->add_food(ofRandom(1));
        if (c->get_food_amount() > split_threshold){
            split_this_update = true;
        }
    }
    

    
    if (split_this_update){
        vector<Cell *> new_cells;
    
        for (Cell * c : cells){
            if (c->get_food_amount() > split_threshold){
                Cell * bb = c->split();
                new_cells.push_back(bb);
            }
        }
        cells.insert(cells.end(), new_cells.begin(), new_cells.end());
    }
    
    for (Cell* c: cells){
        c->move();
    }
    
//    average_positions();
}

void Simulation::average_positions(void){
    ofPoint average = ofPoint(0,0,0);
    for (Cell* c: cells){
        average += c->get_position();
    }
    
    average /= cells.size();
    
    for (Cell* c : cells){
        c->set_position(c->get_position()-average);
    }
}

void Simulation::render(void){
    render_springs();
    //render_normals();
    render_spheres(1);
//    render_normals();
}

void Simulation::render_springs(void){
    for (Cell * c : cells){
        c->draw_springs();
    }
}

void Simulation::render_spheres(float radius){
    for (Cell * c : cells){
        c->draw_cell(radius);
        
        //for testing
        //c->draw_spring_target(radius/3);
    }
}

void Simulation::render_normals(void){
    for (Cell * c : cells){
        c->draw_normal();
    }
}

int Simulation::get_population(void){
    return cells.size();
}



//========================================================================
// Helper functions to create a sphere
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