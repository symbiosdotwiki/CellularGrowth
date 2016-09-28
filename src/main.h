//
//  main.h
//  CellularGrowth
//
//  Created by Sage Jenson on 9/13/16.
//


#ifndef main_h
#define main_h

#include "ofMain.h"
#include "grid.h"
#include "cell.h"

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
    
    void set_values(float _roi_squared, float _spring_factor,
                    float _bulge_factor, float _planar_factor,
                    float _repulsion_strength);
    
    void set_split_threshold(float _split_threshold);
    
private:
    void init_springs(float);

    void average_positions(void);
    vector<Cell*> find_collisions(Cell* c);
    float split_threshold = 10.0;
    float roi_squared;
    Grid* g;
};


// sphere creation
vector<ofPoint> icosa_vertices(void);
void subdivide_iteration(deque<ofPoint>* vertices);
vector<ofPoint> remove_duplicates(deque<ofPoint>* dup_list);
vector<ofPoint> subdivided_icosahedron(int levels);
vector<ofPoint> init_sphere_points(float n, float r);

#endif /* main_h */
