//
//  grid.hpp
//  CellularGrowth
//
//  Created by Sage Jenson on 9/26/16.
//

#ifndef grid_h
#define grid_h

#include "utils.h"
#include "cell.h"

class Grid{
public:
    Grid(int resolution, int size);
    ~Grid(void);
    
    bool in_bounds(int b);
    int get_index(int x, int y, int z);
    int get_box(Vec3f p);
    void set_collisions(Cell* c);
    void add_cell(Cell* c);
    void update_positions(void);
    std::vector<Cell*>* iter(void);
    
    int get_size(void);
    int get_bounding_size(void);
    Cell* get_head(void);
    
//    void draw_boxes(void);
//    void draw_bounding_box(void);
    
private:
    std::vector<std::vector<Cell*>> grid_cells;
    std::vector<Cell*> cell_iter;
    void remove_element(int index, Cell* c);
    void get_coords(int index, int *x, int *y, int *z);
    void get_grid_coords(int index, int* x, int* y, int* z);
    int size, resolution;
};

#endif /* grid_h */
