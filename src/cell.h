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
#include <math.h>
#include <vector>
#include <algorithm>

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
    void print(void);
    
    bool frozen;
    bool visited;
    bool spread;
    bool original;
    float next_food_delta;
    float spring_decay_rate;
    int collision_num;
    int split_begin;
    
    
    Vec3f get_spring_target(void);
    std::vector<Cell*> get_springs(void);
    void set_ordered_neighbors(void);
    Cell* find_next(Cell* current, Cell* previous);
    
    Vec3f position, original_pos, prev_delta;
    Vec3f next_position;
    Vec3f spring_target, planar_target, bulge_target;
    Vec3f collision_offset;
    Vec3f cell_normal;
    std::vector<Cell*> ord_neigh;
    
    float age;
    
    float link_rest_length, ititial_link_rest_length,
    bulge_distance, roi_squared, roi, spring_factor,
    planar_factor, bulge_factor, repulsion_strength;
    
    float prev_a, prev_b, a, b, next_a, next_b;
    
    void calculate_spring_target(void);
    void calculate_planar_target(void);
    void calculate_bulge_target(void);
    void calculate_collision_offset(void);
    void calculate_normal(void);
    
    void calculate_rd(float feed, float kill, float ra, float rb);
    void update_rd(void);
    
private:
    float food;
    int neighbors;
    void init_values(void);
    void remove_spring(Cell* to_remove);
    float get_average_link_len(void);
    Cell* compute_anchor(Cell* anchor1);

    
    
};

#endif /* cell_h */

