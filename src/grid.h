//
//  grid.hpp
//  CellularGrowth
//
//  Created by Sage Jenson on 9/26/16.
//
//

#ifndef grid_h
#define grid_h

#include "ofMain.h"
#include "cell.h"

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

#endif /* grid_h */
