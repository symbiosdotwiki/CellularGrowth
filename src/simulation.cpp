//
//  simulation.cpp
//  CellularGrowth
//
//  Created by Sage Jenson on 9/30/16.
//

#include "simulation.h"

//===========================================================================
Simulation::Simulation(void){
    initialize();
}

void Simulation::initialize(void){
    g = new Grid(120,500);
    
    std::vector<Vec3f> icos_vert =  subdivided_icosahedron(2);
    for (Vec3f p : icos_vert){
        p.normalize();
        Cell *c = new Cell(p);
        g->add_cell(c);
    }
    init_springs(.4);
    
    for (Cell* c : *g->iter()){
        c->set_position(c->get_position()*5.0);
    }
}

void Simulation::init_springs(float radius){
    for (Cell * c1 : *g->iter()){
        for (Cell * c2 : *g->iter()){
            if ((c1 != c2) and (c1->get_position().distance(c2->get_position())) < radius) {
                c1->add_spring(c2);
            }
        }
    }
}

std::vector<Cell*> Simulation::find_collisions(Cell * c){
    std::vector<Cell*> colliderz;
    float roi_squared = c->get_roi();
    for (Cell * other : *g->iter()){
        if ((not c->is_connected(other)) and (c != other)
            and (c->get_position().squareDistance(other->get_position()) < roi_squared)){
            colliderz.push_back(other);
        }
    }
    return colliderz;
}

void Simulation::spread_food(Cell* c, float amount, float decay){
    std::deque<Cell*> q;
    q.push_back(c);
    c->next_food_delta = amount;
    c->spread = true;
    
    while (not q.empty()){
        Cell* cur = q.front();
        q.pop_front();
        
        for (Cell* n : *cur->get_connections()){
            if (not (n->spread)){
                n->spread = true;
                n->next_food_delta = cur->next_food_delta * decay;
                q.push_back(n);
            }
        }
    }
}

void Simulation::add_food(float amount){
    for (Cell* c : *g->iter()){
        c->add_food(range_random(amount));
    }
}

void Simulation::calcification_food(float amount){
    for (Cell* c : *g->iter()){
        c->add_food(amount / (1.0 + 10.0*(float) c->collision_num));
    }
}

void Simulation::update(){
    bool split_this_update = false;
    
    // spread food throughout the system
    if (CONSTANT == mode){
		add_food(1.0);
	}
    else if (BREADTH == mode){
		spread_food(g->get_head(), 1.0, 0.99);
	}
    else if (DENSITY == mode){
        calcification_food(1.0);
    }
    
    for (Cell * c : *g->iter()){
        if (roi_squared > 0){
            g->set_collisions(c);
            c->update();
        } else {
            c->update_without_collisions();
        }
        
        if (c->get_food_amount() > split_threshold){
            split_this_update = true;
        }
    }
    
    if (split_this_update){
        std::vector<Cell *> new_cells;
    
        for (Cell * c : *g->iter()){
            if (c->get_food_amount() > split_threshold){
                Cell * bb = c->split();
                new_cells.push_back(bb);
            }
        }
        for (Cell* c: new_cells){
            g->add_cell(c);
        }
    }
    
    for (Cell* c: *g->iter()){
        c->tick();
    }
    
    g->update_positions();
    
    average_positions();
}

void Simulation::average_positions(void){
    Vec3f average = Vec3f(0,0,0);
    for (Cell* c: *g->iter()){
        average += c->get_position();
    }
    
    average /= g->get_size();
    
    for (Cell* c : *g->iter()){
        c->set_position(c->get_position()-average);
    }
}

std::vector<Cell*>* Simulation::get_cells(void){
    return g->iter();
}

int Simulation::get_population(void){
    return g->get_size();
}

void Simulation::set_values(float _roi_squared, float _spring_factor,
                    float _bulge_factor, float _planar_factor,
                    float _repulsion_strength,
                    float _link_rest_length, int _food_mode){
    for (Cell * c: *g->iter()){
        c->set_values(_roi_squared, _spring_factor, _bulge_factor, _planar_factor, _repulsion_strength, _link_rest_length);
    }
    roi_squared = _roi_squared;
    mode = (food_mode_enum) _food_mode;
}

void Simulation::set_split_threshold(float _split_threshold){
    split_threshold = _split_threshold;
}

/*

std::string* Simulation::point_list(void){
    s = "";
    for (Cell* c : *g->iter()){
        s.append(ofToString(c->get_position()) + "\n");
    }
    return &s;
}
 */

void Simulation::reset(void){
    initialize();
}


// Sphere creation functions
std::vector<Vec3f> Simulation::icosa_vertices(void){
    std::vector<Vec3f> vertices(12);
    
    double theta = 26.56505117707799 * M_PI / 180.0;
    
    double stheta = sin(theta);
    double ctheta = cos(theta);
    
    vertices[0] = Vec3f(0,0,-1);
    
    // the lower pentagon
    double phi = M_PI / 5.0;
    for (int i = 1; i < 6; ++i) {
        vertices[i] = Vec3f(ctheta * cos(phi), ctheta * sin(phi), -stheta);
        
        phi += 2.0 * M_PI / 5.0;
    }
    
    // the upper pentagon
    phi = 0.0;
    for (int i = 6; i < 11; ++i) {
        vertices[i] = Vec3f(ctheta * cos(phi), ctheta * sin(phi), stheta);
        
        phi += 2.0 * M_PI / 5.0;
    }
    
    vertices[11] =Vec3f(0,0,1); // the upper vertex
    
    return vertices;
}

void Simulation::subdivide_iteration(std::deque<Vec3f>* vertices){
    int original_size = vertices->size();
    
    for (int i = 0; i < original_size; i += 3){
        Vec3f v1 = vertices->front();
        vertices->pop_front();
        Vec3f v2 = vertices->front();
        vertices->pop_front();
        Vec3f v3 = vertices->front();
        vertices->pop_front();
        
        Vec3f v4 = (v1 + v2) / 2.0;
        Vec3f v5 = (v2 + v3) / 2.0;
        Vec3f v6 = (v3 + v1) / 2.0;
        
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


std::vector<Vec3f> Simulation::remove_duplicates(std::deque<Vec3f>* dup_list){
    std::vector<Vec3f> unique_list;
    bool contains = false;
    while (not dup_list->empty()){
        Vec3f p = dup_list->front();
        dup_list->pop_front();
        unique_list.push_back(p);
        std::deque<Vec3f>::iterator it;
        
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

std::vector<Vec3f> Simulation::subdivided_icosahedron(int levels){
    std::vector<Vec3f> icosahedron = icosa_vertices();
    
    std::deque<Vec3f> vertices;
    
    int triangles[] = {0,2,1,0,3,2,0,4,3,0,5,4,0,1,5,1,2,7,2,3,8,3,4,9,4,5,10,5,1,6,1,
        7,6,2,8,7,3,9,8,4,10,9,5,6,10,6,7,11,7,8,11,8,9,11,9,10,11,10,6,11};
    
    for (int i : triangles){
        vertices.push_back(icosahedron[i]);
    }
    
    for (int i = 0; i < levels; i++){
        subdivide_iteration(&vertices);
    }
    
    std::vector<Vec3f> final_point_list = remove_duplicates(&vertices);
    
    return final_point_list;
}

std::vector<Vec3f> Simulation::init_sphere_points(float n, float r){
    
    std::vector<Vec3f> return_me;
    // distribute n cells of a sphere of radius r
    int n_count = 0;
    
    float a = 4*M_PI/n;
    float d = sqrt(a);
    
    float mv = round(M_PI/d);
    
    
    float dv = M_PI / mv;
    float dp = a / dv;
    
    for (int m = 0; m < mv; m++){
        float v = M_PI*(m+0.5) / mv;
        float mp = round(2*M_PI*sin(v) / dp);
        for (int j = 0; j < mp; j++){
            float p = 2*M_PI*j / mp;
            Vec3f point = Vec3f(r*sin(v)*cos(p),
                                    r*sin(v)*sin(p),
                                    r*cos(v));
            return_me.push_back(point);
            n_count += 1;
        }
    }
    return return_me;
}

int Simulation::get_size(){
    return g->get_bounding_size();
}

