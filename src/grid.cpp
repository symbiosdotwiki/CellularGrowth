//
//  grid.cpp
//  CellularGrowth
//
//  Created by Sage Jenson on 9/26/16.
//
//

#include "grid.h"

Grid::Grid(int _resolution, int _size){
    resolution = _resolution;
    size = _size;
    
    grid_cells.resize(pow(resolution, 3));
}

void Grid::add_cell(Cell* c){
    grid_cells[get_box(c->get_position())].push_back(c);
    cell_iter.push_back(c);
}

vector<Cell*> Grid::iter(void){
    return cell_iter;
}

bool Grid::in_bounds(int x, int y, int z){
    //todo
    return true;
    /*
    return ((abs(x) < (x_length/2.0))
            and (abs(y) < (y_length/2.0))
            and (abs(z) < (z_length/2.0)));
     */
}

int Grid::get_index(int x, int y, int z){
    if (not in_bounds(x,y,z)) return - 1;
    return (resolution*resolution*z) + (resolution*y) + x;
}

int Grid::get_box(ofPoint p){
    p += ofPoint(size/2, size/2, size/2);
    
    int x = (p.x * resolution) / size;
    int y = (p.y * resolution) / size;
    int z = (p.z * resolution) / size;
    
    return get_index(x,y,z);
}

void Grid::get_grid_coords(int index, int* x, int* y, int*z){
    int scale = size / resolution;
    (*x) = (index % resolution);
    (*y) = (((index-(*x)) % (resolution*resolution)) / resolution);
    (*z) = ((index - (*x) - (*y)) / (resolution*resolution));
}

int Grid::get_size(void){
    return cell_iter.size();
}



vector<Cell*> Grid::get_collisions(Cell* c){
    int x, y, z, index;
    index = get_box(c->get_position());
    get_grid_coords(index, &x, &y, &z);
    float roi_squared = c->get_roi();
    
    vector<Cell*> neighbors;
    
    for (int i = -1; i <= 1; i++){
        for (int j = -1; j <= 1; j++){
            for (int k = -1; k <= 1; k++){
                for (Cell* other : grid_cells[get_index(x+i, y+j, z+k)]){
                    if ((not c->is_connected(other)) and (c != other)
                        and (c->get_position().squareDistance(other->get_position()) < roi_squared)){
                        neighbors.push_back(other);
                    }
                }
            }
        }
    }
    
    return neighbors;
}

void Grid::remove_element(int index, Cell* c){
    grid_cells[index].erase(remove(grid_cells[index].begin(), grid_cells[index].end(), c), grid_cells[index].end());
}

// can optimize, repeating looks
void Grid::update_positions(void){
    for (int i = 0; i < grid_cells.size(); i++){
        for (Cell* c: grid_cells[i]){
            int index =get_box(c->get_position());
            if (index != i){
                remove_element(i, c);
                grid_cells[index].push_back(c);
            }
        }
    }
}

void Grid::get_coords(int index, int * x, int * y, int * z){
    int scale = size / resolution;
    (*x) = (index % resolution);
    (*y) = (((index-(*x)) % (resolution*resolution)) / resolution);
    (*z) = ((index - (*x) - (*y)) / (resolution*resolution));
    
    (*x) *= scale;
    (*y) *= scale;
    (*z) *= scale;
}

// not working correctly...
void Grid::draw_boxes(void){
    int x, y, z;
    float offset = (float) (size / (2*resolution)) - (size/2.0);
    ofNoFill();
    ofSetLineWidth(.5);
    for (int i = 0; i < grid_cells.size(); i++){
        if (grid_cells[i].size()){
            get_coords(i, &x, &y, &z);
            ofDrawBox(x + offset, y + offset, z + offset, (float) size / resolution);
        }
    }
}

void Grid::draw_bounding_box(void){
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawBox(0, 0, 0, size);
}