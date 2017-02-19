//
//  cell.h
//  CellularGrowth
//
//  Created by Sage Jenson on 9/26/16.
//
//

#ifndef cell_h
#define cell_h

#include "utils.h"
#include <vector>

class Cell {
public:
    Cell(Vec3f _position);
    Cell(Vec3f _position, int _age, Vec3f _cell_normal);
    ~Cell(void);
    
    
    bool is_connected(Cell* c);
    void add_spring(Cell * c);
    void add_food(float amount);
    void reset_food(void);
    Vec3f get_position(void);
    void set_position(Vec3f new_pos);
    float get_food_amount(void);
    float get_roi(void);
    std::vector<Cell*>* get_connections(void);
    std::vector<Cell*> connections;
    std::vector<float> rest_lengths;
    std::vector<Cell*> collisions;
    
    Cell* split(void);
    
    void set_values(float _roi_squared,
                    float _spring_factor, float _bulge_factor,
                    float _planar_factor, float _repulsion_strength,
                    float _link_rest_length);
    void set_rest_length(float _rest_length);
    void update(void);
    void update_without_collisions(void);
    void tick(void);
    void tick(bool move);
    void print(void);
    
    bool dup = false;
    bool spread = false;
    float next_food_delta = 0;
    float spring_decay_rate;
    int collision_num = 0;
    
    
    Vec3f get_spring_target(void);
    std::vector<Cell*> get_springs(void);
    std::vector<Cell*> get_ordered_neighbors(void);
    
/*
    void draw_springs(void);
    void draw_spring_target(float radius);
    void draw_planar_target(void);
    void draw_normal(void);
    void draw_cell(float radius);
    void draw_loop(void);
*/
    Vec3f position, original_pos;
    Vec3f next_position;
    Vec3f spring_target, planar_target, bulge_target;
    Vec3f collision_offset;
    Vec3f cell_normal;
    
    
private:

    float   food, age, link_rest_length, initial_link_rest_length,
            bulge_distance, roi_squared, roi, spring_factor,
            planar_factor, bulge_factor, repulsion_strength;
    
    
    /*
    float food = 0;
    float age;
    
    float link_rest_length = 1.0;
    float ititial_link_rest_length = 1.0;
    float bulge_distance;
    float roi_squared = .25;
    float roi = .5;
    
    float spring_factor = 0.01;
    float planar_factor = 0.01;
    float bulge_factor = 0.01;
    float repulsion_strength = 0.1;
     */
    
    int neighbors = 0;
    
    Cell* find_next(Cell* current, Cell* previous);
    void remove_spring(Cell* to_remove);
    float get_average_link_len(void);
    Cell* compute_anchor(Cell* anchor1);
    void calculate_spring_target(void);
    void calculate_planar_target(void);
    void calculate_bulge_target(void);
    void calculate_collision_offset(void);
    
    void calculate_normal(void);
    
    
};

#endif /* cell_h */
