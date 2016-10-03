//
//  simulation.h
//  CellularGrowth
//
//  Created by Sage Jenson on 9/30/16.
//
//

#ifndef simulation_h
#define simulation_h

#include "ofMain.h"
#include "cell.h"
#include "grid.h"

class Simulation{
public:
    
    Simulation(void);
    ~Simulation(void);
    
    void initialize(void);
    void render(void);
    void render_planar(void);
    void render_spheres(float radius = 3);
    void render_springs(void);
    void render_normals(void);
    void render_bounding_box(void);
    void render_boxes(void);
    void update(void);
    
    int get_population(void);
    
    void set_values(float _roi_squared, float _spring_factor,
                    float _bulge_factor, float _planar_factor,
                    float _repulsion_strength, float spring_decay_rate,
                    float _link_rest_length);
    
    void set_split_threshold(float _split_threshold);
    
    std::string* point_list(void);
    void reset(void);
    
private:
    void init_springs(float);
    void spread_food(Cell* c, float amount, float decay);
    void add_food(float amount);
    void average_positions(void);
    vector<Cell*> find_collisions(Cell* c);
    float split_threshold = 10.0;
    float roi_squared;
    Grid* g;
    
    // sphere creation
    vector<ofPoint> icosa_vertices(void);
    void subdivide_iteration(deque<ofPoint>* vertices);
    vector<ofPoint> remove_duplicates(deque<ofPoint>* dup_list);
    vector<ofPoint> subdivided_icosahedron(int levels);
    vector<ofPoint> init_sphere_points(float n, float r);
    string s;
};

#endif /* simulation_h */
