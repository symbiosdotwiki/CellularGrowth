//
//  simulation.h
//  CellularGrowth
//
//  Created by Sage Jenson on 9/30/16.
//

#ifndef simulation_h
#define simulation_h

#include <vector>
#include <deque>
#include <math.h>
#include <algorithm>
#include <fstream>
#include "utils.h"
#include "cell.h"
#include "grid.h"

enum food_mode_enum {CONSTANT, BREADTH, DENSITY, X_AXIS_DENSITY,
    PLANAR};

class Face {
public:
    Cell *a, *b, *c;
    Vec3f normal;
    
    Face(Cell* _a, Cell* _b, Cell* _c){
        a = _a;
        b = _b;
        c = _c;
        
        normal = (a->position-b->position).cross(c->position-b->position);
        normal.normalize();
    }
    
    bool is_equal(Face& other){
        return  ((a==other.a) and (b==other.b) and (c==other.c)) or
                ((b==other.a) and (c==other.b) and (a==other.c)) or
                ((c==other.a) and (a==other.b) and (b==other.c)) or
                ((a==other.a) and (c==other.b) and (b==other.c)) or
                ((b==other.a) and (a==other.b) and (c==other.c)) or
                ((c==other.a) and (b==other.b) and (a==other.c));
    }
};

class Simulation{
public:
    
    Simulation(float _roi_squared, float _spring_factor,
               float _bulge_factor, float _planar_factor,
               float _repulsion_strength, float _link_rest_length,
               int _food_mode, float _split_threshold);
    ~Simulation(void);
    
    void initialize(void);
    
    std::vector<Cell*>* get_cells(void);
    
    void update(void);
    int get_population(void);
    int get_frame_num(void);
    void set_split_threshold(float _split_threshold);
    int get_size(void);
    void point_list(void);
    void reset(void);
    void update_faces(void);
    std::vector<Face> faces;
    void face_helper(void);
    void add_food(void);
    Cell* farthest;
    
private:
    void init_springs(float);
    void spread_food(Cell* c, float amount, float decay);
    void add_food(float amount);
    void calcification_food(float amount);
    void x_axis_density(float amount);
    void planar(float amount);
    void average_positions(void);
    std::vector<Cell*> find_collisions(Cell* c);
    float split_threshold, roi_squared, spring_factor, bulge_factor,
    planar_factor, repulsion_strength, link_rest_length;
    int frame_num;
    bool equal_food;
    Grid* g;
    
    // sphere creation
    std::vector<Vec3f> icosa_vertices(void);
    void subdivide_iteration(std::deque<Vec3f>* vertices);
    std::vector<Vec3f> remove_duplicates(std::deque<Vec3f>* dup_list);
    std::vector<Vec3f> subdivided_icosahedron(int levels);
    std::vector<Vec3f> init_sphere_points(float n, float r);
    std::string s;
    food_mode_enum mode;
};

#endif /* simulation_h */

