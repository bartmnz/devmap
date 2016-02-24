#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct device{
    double altitude;
    double latitude;
    double longitude;
    int device_id;
    struct device* next;
};

struct oct_tree{
    struct oct_tree* sub_tree[8];
            // Provided for reference if needed. corresponds to indexes of sub_trees. 
            // q0 altitude, 1/2 < max, 	latitude 1/2 < max, 		longitude 1/2 < max
            // q1 altitude, 1/2 < max, 	latitude 1/2 < max, 		longitude min >= 1/2
            // q2 altitude, 1/2 < max, 	latitude min >= 1/2,  		longitude 1/2 < max
            // q3 altitude, 1/2 < max, 	latitude min >= 1/2, 		longitude min >= 1/2
            // q4 altitude, min >= 1/2, 	latitude 1/2 < max, 		longitude 1/2 < max
            // q5 altitude, min >= 1/2, 	latitude 1/2 < max, 		longitude min >= 1/2
            // q6 altitude, min >= 1/2, 	latitude min >= 1/2, 		longitude 1/2 < max
            // q7 altitude, min >= 1/2, 	latitude min >= 1/2, 		longitude min >= 1/2
    int status; // negative is partial 0 is empty and positive is full
    struct device* device_ids; 
    
    double altitude_max;
    double altitude_min;
    
    double latitude_max;
    double latitude_min;
    
    double longitude_max;
    double longitude_min;
};

void insert (struct oct_tree** tree, struct device* device);
void initalize_sub_tree(struct oct_tree* tree, int quad);
void grow_tree(struct oct_tree** tree, struct device* device);
void oct_tree_destroy(struct oct_tree* tree);
struct device* find_close(struct oct_tree* tree, struct oct_tree* range);
void clone_list(struct device* list1, struct device* list2);
void append_list(struct device** list1, struct device* list2);
void kill_id_list(struct device* list);
bool out_of_bounds(struct oct_tree* tree, struct device* device);
void printTree(struct oct_tree* tree, int count);