//
//  main.h
//  CellularGrowth
//
//  Created by Sage Jenson on 9/13/16.
//


#ifndef main_h
#define main_h

#include "ofMain.h"
#include <math.h>
#include <vector>
using namespace std;

class Cell {
public:
    Cell(ofPoint _position);
    Cell(ofPoint _position, int _age, ofPoint _cell_normal);
    ~Cell(void);
    
    void add_spring(Cell * c);
    void add_food(float amount);
    void reset_food(void);
    ofPoint get_position(void);
    float get_food_amount(void);
    vector<Cell*>* get_connections(void);
    
    vector<Cell*> connections;
    Cell* split(void);
    void update(void);
    
    void draw_springs(void);
    void draw_spring_target(float radius);
    void draw_normal(void);
    void draw_cell(float radius);
    
private:
    ofPoint position, original_pos;
    ofPoint spring_target, planar_target, bulge_target;
    ofPoint cell_normal;
    ofColor col;
    
    float food = 0;
    float age;
    
    float link_rest_length = 8;
    float bulge_distance;
    
    float spring_factor = 0.1;
    float planar_factor = 0.1;
    float bulge_factor = 0.1;
    
    
    Cell* find_next(Cell* current, Cell* previous);
    void remove_spring(Cell* to_remove);
    void calculate_spring_target(void);
    void calculate_planar_target(void);
    void calculate_bulge_target(void);
    
    void calculate_normal(void);
    
};

class Simulation{
public:
    
    Simulation(void);
    ~Simulation(void);
    
    void render(void);
    void update(void);
    
    int get_population(void);
    
private:
    vector<Cell*> cells;
    
    void init_sphere_points(float n, float r);
    void init_springs(float);
    void render_spheres(float radius = 3);
    void render_springs(void);
    void render_normals(void);
    
    float split_threshold = 10;
};

#endif /* main_h */
