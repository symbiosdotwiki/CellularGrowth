//
//  cell.cpp
//  CellularGrowth
//
//  Created by Sage Jenson on 9/26/16.
//

#include "cell.h"

Cell::Cell(Vec3f _position){
    position = _position;
    original_pos = _position;
    age = 0;
}

Cell::Cell(Vec3f _position, int _age, Vec3f _cell_normal){
    // use this constructor when spawning new cells
    position = _position;
    original_pos = _position;
    age = _age;
    cell_normal = _cell_normal;
}

bool Cell::is_connected(Cell* c){
    return find(connections.begin(), connections.end(), c) != connections.end();
}

void Cell::add_spring(Cell * c){
    connections.push_back(c);
}

void Cell::add_food(float amount){
    next_food_delta += amount;
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

std::vector<Cell*>* Cell::get_connections(void){
    return &connections;
}

void Cell::calculate_spring_target(void){
    spring_target = Vec3f(0,0,0);
    for (Cell * c : connections){
        Vec3f delta =position - c->get_position();
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
    planar_target = Vec3f(0,0,0);
    for (Cell *c : connections){
        planar_target+= c->position;
    }
    planar_target /= connections.size();
}

void Cell::calculate_bulge_target(void){
    bulge_distance = 0;
    for (Cell* c : connections){
        Vec3f L = c->get_position() - position;
        // decided to use absolute val
        float tmp = (L - position).dot(cell_normal);
        float dotN = (tmp > 0) ? tmp : 0 ;
        float radicand = pow(link_rest_length, 2) - pow(L.length(), 2) + pow(dotN, 2);
        if (radicand < 0.0) radicand = 0;

        bulge_distance += sqrt(radicand) + dotN;
    }
    bulge_distance /= connections.size();
    bulge_target = position + (cell_normal * bulge_distance);
}


void Cell::calculate_collision_offset(void){
    neighbors = collisions.size();
    collision_offset = Vec3f(0,0,0);
    Vec3f temp;
    float len;
    for (Cell* c : collisions){
        temp = position - c->get_position();
        len = temp.length();
        temp.normalize();
        len = (roi- len)/roi;
        
        // triweight!
        len = (35.0/32.0)*(pow(1.0-pow(len, 2), 3));
        temp *= len;
        collision_offset += temp;
    }
    
    collision_offset /= collisions.size();
    
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

std::vector<Cell*> Cell::get_ordered_neighbors(void){
    //gotta start somewhere.. hb randomly
    std::vector<Cell*> ord_neigh;
    ord_neigh.push_back(connections[(int) range_random(connections.size())]);
    
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

    Vec3f temp = Vec3f(0,0,0);
    
    std::vector<Cell*> loop = get_ordered_neighbors();
    
    Cell * current = loop.front();
    Cell * previous = loop.back();
    
    
    for (int i = 0; i < loop.size(); i++){
        // add the normalized cross product
        Vec3f v1 = (current->get_position() - position);
        Vec3f v2 = (previous->get_position() - position);
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
        if ((temp+position).lengthSquared() < ((-1.0*temp)+position).lengthSquared()){
            temp = -temp;
        }
    } else {
        if (cell_normal.dot(temp) < 0){
            temp *= -1.0;
        }
    }
    
    
    if (temp.dot(planar_target) < 0.0){
        temp *= 0.3; //= -temp;
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
    std::vector<Cell*> ord_neigh = get_ordered_neighbors();
    
    Cell* bb = new Cell(position, age, cell_normal);
    
    Cell* anchor1 =ord_neigh[0];
    Cell* anchor2 =ord_neigh[ord_neigh.size()/2];
    
    // now traverse from one anchor to another
    Cell* current= find_next(anchor1, anchor1);
    Cell* previous = anchor1;
    
    std::vector<Cell*> loop;
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
    
    // figure better way to set initial point positions...
    // How to determine a side?
    // Maybe average connections locations and move half way to that?
    
    Vec3f average1 = Vec3f(0,0,0);
    for (Cell* c : connections){
        average1 += c->get_position();
    }
    average1 /= connections.size();
    
    Vec3f average2 = Vec3f(0,0,0);
    for (Cell* c : *bb->get_connections()){
        average2 += c->get_position();
    }
    
    average2 /= bb->get_connections()->size();
    
    next_position= (position + average1)/2.0;
                 
    bb->next_position = ((bb->get_position() + average2) /2.0);
    
    bb->link_rest_length = link_rest_length;
    
    bb->cell_normal = cell_normal;
    return bb;
}

void Cell::set_values(float _roi_squared, float _spring_factor, float _bulge_factor, float _planar_factor, float _repulsion_strength, float _link_rest_length){
    link_rest_length = _link_rest_length;
    roi_squared = _roi_squared;
    roi = sqrt(roi_squared);
    spring_factor = _spring_factor;
    bulge_factor = _bulge_factor;
    repulsion_strength = _repulsion_strength;
    planar_factor = _planar_factor;
}

void Cell::update(void){
    collision_num = collisions.size();
    calculate_normal();
   
    calculate_spring_target();
    calculate_planar_target();
    calculate_bulge_target();
    calculate_collision_offset();
    
    next_position = Vec3f(0,0,0);
    next_position += spring_factor * (spring_target - position);
    next_position += planar_factor * (planar_target - position);
    next_position += bulge_factor * (bulge_target - position);
    next_position += collision_offset;
    
    next_position += position;
    
    age++;
}

void Cell::update_without_collisions(void){
    calculate_normal();
   
    calculate_spring_target();
    calculate_planar_target();
    calculate_bulge_target();
    
    next_position = position;
    next_position += spring_factor * (spring_target - position);
    next_position += planar_factor * (planar_target - position);
    next_position += bulge_factor * (bulge_target - position);
    
    age++;
}

void Cell::tick(void){
    tick(true);
}

void Cell::tick(bool move){
    if (move){
        position = next_position;
    }
    food += next_food_delta;
    next_food_delta = 0;
    spread = false;
}
/*

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
    std::vector<Cell*> loop = get_ordered_neighbors();
    ofSetColor(255, 0, 0);
    ofSetLineWidth(3);
    for (int i = 0; i < loop.size(); i++){
        Vec3f p1 = loop[i]->get_position();
        Vec3f p2 = loop[(i+1)%loop.size()]->get_position();
        ofDrawLine(p1, p1+((p2-p1)/(i+1)));
    }
}



void Cell::draw_spring_target(float radius){
    ofSetColor(col);
    ofDrawBox(spring_target, radius);
}

void Cell::draw_planar_target(void){
    ofSetColor(col);
    ofDrawBox(planar_target, 1);
}

void Cell::draw_normal(void){
    ofPushStyle();
    if (position.dot(cell_normal) > 0) ofSetColor(ofColor::black);
    else (ofSetColor(ofColor::red));
    ofSetLineWidth(2);
    ofDrawLine(position, position + cell_normal);
    ofPopStyle();
}
 
void Cell::draw_cell(float radius){
    ofFill();
    ofDrawIcoSphere(position, radius);
}
 
*/

Vec3f Cell::get_spring_target(void){
    return spring_target;
}

std::vector<Cell*> Cell::get_springs(void){
    return connections;
}


Vec3f Cell::get_position(void){
    return position;
}

void Cell::set_position(Vec3f new_pos){
    position = new_pos;
}