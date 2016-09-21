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
        float radicand = abs(pow(link_rest_length, 2) - pow(L.length(), 2) + pow(dotN, 2));

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

void Cell::set_values(float _link_rest_length, float _roi_squared, float _spring_factor, float _bulge_factor, float _planar_factor, float _repulsion_strength){
    link_rest_length = _link_rest_length;
    roi_squared = _roi_squared;
    spring_factor = _spring_factor;
    bulge_factor = _bulge_factor;
    repulsion_strength = _repulsion_strength;
    planar_factor = _planar_factor;
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

void Cell::print(void){
    cout << "Current Cell (Age) " + ofToString(age) << endl;
    cout << "Spring Delta: " + ofToString(position.distance(spring_target)) << endl;
    cout << "Planar Delta: " + ofToString(position.distance(planar_target)) << endl;
    cout << "Bulge Delta: " + ofToString(position.distance(bulge_target)) << endl;
    cout << "Collision Delta: " + ofToString(collision_offset.length()) + "\n" << endl;
}

void Cell::draw_springs(void){
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
    ofFill();
    ofDrawIcoSphere(position, radius);
}

ofPoint Cell::get_position(void){
    return position;
}

void Cell::set_position(ofPoint new_pos){
    position = new_pos;
}


//========================================================================
Grid::Grid(int _resolution, float _x_length, float _y_length, float _z_length){
    resolution = _resolution;
    x_length = _x_length;
    y_length = _y_length;
    z_length = _z_length;
    
    grid_cells.resize(pow(resolution, 3));
}

void Grid::add_cell(Cell* c){
    grid_cells[get_box(c->get_position())].push_back(c);
    cell_iter.push_back(c);
}

int Grid::size(void){
    return cell_iter.size();
}

vector<Cell*> Grid::iter(void){
    return cell_iter;
}

// wrong, fix this
bool Grid::in_bounds(int x, int y, int z){
    return ((x>0) and (y>0) and (z>0)
            and (x < grid_cells.size())
            and (y < grid_cells.size())
            and (z < grid_cells.size()));
}

int Grid::get_index(int x, int y, int z){
    if (not in_bounds(x,y,z)) return - 1;
    return (resolution*resolution*z) + (resolution*y) + x;
}

int Grid::get_box(ofPoint p){
    p += ofPoint(x_length/2.0, y_length/2.0, z_length/2.0);
    
    int x = (p.x * resolution) / x_length;
    int y = (p.y * resolution) / y_length;
    int z = (p.z * resolution) / z_length;
    
    return get_index(x,y,z);
}

void Grid::get_coords(int index, int * x, int * y, int * z){
    int scale = (x_length / resolution);
    (*x) = scale * (index % resolution);
    (*y) = scale * (((index-(*x)) % (resolution*resolution)) / resolution);
    (*z) = scale * ((index - (*x) - (*y)) / (resolution*resolution));
}

vector<Cell*> Grid::get_collisions(Cell* c){
    int x, y, z, index;
    index = get_box(c->get_position());
    get_coords(index, &x, &y, &z);
    float roi_squared = c->get_roi();
    
    vector<Cell*> neighbors;
    
    for (int i = -1; i <= 1; i++){
        for (int j = -1; j <= 1; j++){
            for (int k = -1; k <= 1; k++){
                for (Cell* other: grid_cells[get_index(x+i, y+j, z+k)]){
                    if ((not c->is_connected(other)) and (c != other)
                        and (c->get_position().squareDistance(other->get_position()) < roi_squared)){
                        neighbors.push_back(other);
                    }
                }
            }
        }
    }
    
    return neighbors;
}

void Grid::remove_element(int index, Cell* c){
    grid_cells[index].erase(remove(grid_cells[index].begin(), grid_cells[index].end(), c), grid_cells[index].end());
}

// can optimize, repeating looks
void Grid::update_positions(void){
    for (int i = 0; i < grid_cells.size(); i++){
        for (Cell* c: grid_cells[i]){
            int index =get_box(c->get_position());
            if (index != i){
                remove_element(i, c);
                grid_cells[index].push_back(c);
            }
        }
    }
}

// not working correctly...
void Grid::draw_boxes(void){
    int x, y, z;
    float offset = (float) (x_length / (2.0*resolution)) - (x_length/2.0);
    ofNoFill();
    ofSetLineWidth(.5);
    for (int i = 0; i < grid_cells.size(); i++){
        if (grid_cells[i].size()){
            get_coords(i, &x, &y, &z);
            ofDrawBox(x + offset, y + offset, z + offset, (float) x_length / resolution, (float) y_length / resolution,
                (float) z_length / resolution);
        }
    }
}


void Grid::draw_bounding_box(void){
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawBox(0, 0, 0, x_length, y_length, z_length);
}

//========================================================================
Simulation::Simulation(void){
    g = new Grid(100,100,100,100);
    
    vector<ofPoint> icos_vert = icosa_vertices();
    for (ofPoint p : icos_vert){
        Cell *c = new Cell(p*10);
        g->add_cell(c);
    }
    init_springs(15);
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
        
        c->add_food(.3);
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
    
    average /= g->size();
    
    for (Cell* c : g->iter()){
        c->set_position(c->get_position()-average);
    }
}

void Simulation::render(void){
    ofPushStyle();
    
    render_springs();
    //render_normals();
//    render_spheres(.5);
//    render_normals();
    g->draw_bounding_box();
//    g->draw_boxes();
    
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

int Simulation::get_population(void){
    return g->size();
}

void Simulation::set_values(float _link_rest_length, float _roi_squared,
                    float _spring_factor, float _bulge_factor, float _planar_factor,
                           float _repulsion_strength){
    for (Cell * c: g->iter()){
        c->set_values(_link_rest_length, _roi_squared, _spring_factor, _bulge_factor, _planar_factor, _repulsion_strength);
    }
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
