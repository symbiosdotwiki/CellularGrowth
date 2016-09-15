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
    col = ofColor::fromHsb(ofRandom(255), 255, 255, 255);
}

Cell::Cell(ofPoint _position, int _age, ofPoint _cell_normal){
    // use this constructor when spawning new cells
    position = _position;
    original_pos = _position;
    age = _age;
    cell_normal = _cell_normal;
    col = ofColor::fromHsb(ofRandom(255), 255, 255, 255);
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
    cout << ofToString(bulge_distance) << endl;
    bulge_target = position + (cell_normal * bulge_distance);
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
    
    return NULL;
}

void Cell::calculate_normal(void){

    ofPoint temp = ofPoint(0,0,0);
    
    Cell * current = connections[0];
    Cell * previous;
    
    // initialize previous
    for (Cell* d : *connections[0]->get_connections()){
        if ((d != this) and (find(connections.begin(), connections.end(), d) != connections.end())){
            previous = d;
            break;
        }
    }
    
    for (int i = 0; i < connections.size(); i++){
        // add the normalized cross product
        ofPoint v1 = (current->get_position() - position);
        ofPoint v2 = (previous->get_position() - position);
        v1.cross(v2);
        
        v1.normalize();
        temp += v1;
        
        // now find the next point
        Cell* c_temp = find_next(current, previous);
        if (c_temp == NULL) break;
        
        previous = current;
        current = c_temp;
        
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

Cell* Cell::split(void){
    reset_food();
    Cell* bb = new Cell(position, age, cell_normal);
    
    // First calculate the anchors (farthest apart nodes)
    Cell* anchor1 = connections[0];
    Cell* anchor2 = connections[0];
    
    float max_dist = 0;
    for (int i = 0; i < connections.size(); i++){
        for (int j = i; j < connections.size(); j++){
            float cur_dist =connections[i]->get_position().distance(connections[j]->get_position());
            
            if (cur_dist > max_dist){
                anchor1 = connections[i];
                anchor2 = connections[j];
                max_dist = cur_dist;
            }
        }
    }
    
    
    // now traverse from one anchor to another, removing springs from this and adding to bb as we go
    Cell* current = find_next(anchor1, anchor2);
    Cell* previous = anchor1;
    
    while (current != anchor2){
        // throw error if try to connect to a node that was already visited
        if (find(bb->connections.begin(), bb->connections.end(), current) != bb->connections.end()) throw 0;
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
    return bb;
}

void Cell::update(void){
    calculate_normal();
    
    calculate_spring_target();
    calculate_planar_target();
    calculate_bulge_target();
    
    position += spring_factor * (spring_target - position);
    position += planar_factor * (planar_target - position);
    position += bulge_factor * (bulge_target - position);
    
    age++;
}

void Cell::draw_springs(void){
    ofSetColor(0);
    for (Cell * c : connections){
        ofDrawLine(position, c->position);
    }
}

void Cell::draw_spring_target(float radius){
    ofSetColor(col);
    ofDrawBox(spring_target, radius);
}

void Cell::draw_normal(void){
    ofSetColor(255,255,0);
    ofDrawLine(position, position + bulge_distance*cell_normal);
}

void Cell::draw_cell(float radius){
    ofSetColor(col);
    ofSetLineWidth(2);
    ofDrawIcoSphere(position, radius);
}

ofPoint Cell::get_position(void){
    return position;
}


//========================================================================
Simulation::Simulation(void){
   /*
    Cell * c1 = new Cell(ofPoint(0,0,0));
    Cell * c2 = new Cell(ofPoint(10,0,0));
    
    c1->add_spring(c2);
    c2->add_spring(c1);
    
    cells.push_back(c1);
    cells.push_back(c2);
    */
    
   // to do: generalize
    init_sphere_points(12, 10);
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

void Simulation::update(){
    bool split_this_update = false;
    for (Cell * c : cells){
//        c->add_food(ofRandom(1));
        c->update();
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

    
    // vector1.insert( vector1.end(), vector2.begin(), vector2.end() );
}

void Simulation::render(void){
    render_springs();
    render_normals();
    render_spheres(.3);
}

void Simulation::render_springs(void){
    for (Cell * c : cells){
        c->draw_springs();
    }
}

void Simulation::render_spheres(float radius){
    for (Cell * c : cells){
        c->draw_cell(radius);
        c->draw_spring_target(radius/3);
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