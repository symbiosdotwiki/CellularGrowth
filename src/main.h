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
    
    
    bool is_connected(Cell* c);
    void add_spring(Cell * c);
    void add_food(float amount);
    void reset_food(void);
    ofPoint get_position(void);
    void set_position(ofPoint new_pos);
    float get_food_amount(void);
    float get_roi(void);
    vector<Cell*>* get_connections(void);
    
    vector<Cell*> connections;
    
    Cell* split(void);
    
    void set_values(float _link_rest_length, float _roi_squared,
                    float _spring_factor, float _bulge_factor,
                    float _planar_factor, float _repulsion_strength);
    void update(vector<Cell*> collision_list);
    void move(void);
    void print(void);
    
    bool dup = false;
    
    void draw_springs(void);
    void draw_spring_target(float radius);
    void draw_normal(void);
    void draw_cell(float radius);
    void draw_loop(void);
    
private:
    ofPoint position, original_pos;
    ofPoint next_position;
    ofPoint spring_target, planar_target, bulge_target;
    ofPoint collision_offset;
    ofPoint cell_normal;
    ofColor col;
    
    float food = 0;
    float age;
    
    float link_rest_length = .24;
    float bulge_distance;
    float roi_squared = .25;
    
    float spring_factor = 0.01;
    float planar_factor = -0.005;
    float bulge_factor = 0.01;
    float repulsion_strength = 0.1;
    
    
    Cell* find_next(Cell* current, Cell* previous);
    void remove_spring(Cell* to_remove);
    float get_average_link_len(void);
    Cell* compute_anchor(Cell* anchor1);
    void calculate_spring_target(void);
    void calculate_planar_target(void);
    void calculate_bulge_target(void);
    void calculate_collision_offset(vector<Cell*> collision_list);
    
    void calculate_normal(void);
    vector<Cell*> get_ordered_neighbors(void);
    
};


class Grid{
public:
    Grid(int resolution, int size);
    ~Grid(void);
    
    bool in_bounds(int x, int y, int z);
    int get_index(int x, int y, int z);
    int get_box(ofPoint p);
    vector<Cell*> get_collisions(Cell* c);
    void add_cell(Cell* c);
    void update_positions(void);
    void draw_bounding_box(void);
    vector<Cell*> iter(void);
    void draw_boxes(void);
    int get_size(void);
    
private:
    vector<vector<Cell*>> grid_cells;
    vector<Cell*> cell_iter;
    void remove_element(int index, Cell* c);
    void get_coords(int index, int *x, int *y, int *z);
    void get_grid_coords(int index, int* x, int* y, int* z);
    int size, resolution;
};

class Simulation{
public:
    
    Simulation(void);
    ~Simulation(void);
    
    void render(void);
    
    void render_spheres(float radius = 3);
    void render_springs(void);
    void render_normals(void);
    void render_bounding_box(void);
    void render_boxes(void);
    void update(void);
    
    int get_population(void);
    
    void set_values(float _link_rest_length, float _roi_squared,
                    float _spring_factor, float _bulge_factor,
                    float _planar_factor,float _repulsion_strength);
    void set_split_threshold(float _split_threshold);
    
private:
    void init_springs(float);

    void average_positions(void);
    vector<Cell*> find_collisions(Cell* c);
    float split_threshold = 10;
    
    Grid* g;
};


// sphere creation
vector<ofPoint> icosa_vertices(void);
void subdivide_iteration(deque<ofPoint>* vertices);
vector<ofPoint> remove_duplicates(deque<ofPoint>* dup_list);
vector<ofPoint> subdivided_icosahedron(int levels);
vector<ofPoint> init_sphere_points(float n, float r);

#endif /* main_h */
