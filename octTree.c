#include "octTree.h"

/* Function is used to insert a device into an Oct_tree structure. If the range of the 
 * tree is not large enough to encompase the device, a new tree will be MALLOC'D and 
 * the given tree will be stored within the new tree. INVALID FREE will occur if tree is
 * there is any attempt to call oct_tree_destroy on the new tree and @PARAM tree is not MALLOC'D
 * @PARAM tree -- tree to insert device into. MUST BE MALLOC'D memory.
 * @PARAM device -- device to insert into tree. MUST BE MALLOC'D memory. 
 */
bool insert (struct oct_tree** tree, struct device* device){
    if( ! (*tree) ){
        fprintf(stderr, "ERROR: initialize the tree\n");
        return false;
    }
    //printTree((*tree),0);
    // int a;
      // scanf("%d", &a);
    // first time through
    if ( (*tree)->status == 0){
        (*tree)->status = 1;
        (*tree)->altitude_max = device->altitude+.99;
        (*tree)->altitude_min = device->altitude-.99;
        (*tree)->latitude_max = device->latitude+LAT_INCREMENT;
        (*tree)->latitude_min = device->latitude-LAT_INCREMENT;
        (*tree)->longitude_max = device->longitude+LAT_INCREMENT;
        (*tree)->longitude_min = device->longitude-LAT_INCREMENT;
        (*tree)->device_ids = device;
    } else if( out_of_bounds((*tree), device)){ // tree isn't big enough
        //printf("grow\n");
        grow_tree(tree, device); // make it bigger and try again
        insert(tree, device);
    } else if( (*tree)->altitude_max - (*tree)->altitude_min < 2  ){ // at lowest possible level and within the bounds
        (*tree)->status = 1;
        struct device* holder = (*tree)->device_ids;
        while (holder){
            if (holder->device_id == device->device_id){
                return false;
            }
            holder = holder->next;
        }
        device->next = (*tree)->device_ids;
        (*tree)->device_ids = device;
    } else { // otherwise we have to insert into subtree  
            // see comments in tree structure true coresponds to a value such that
            // 1/2(max-min) < value < max;
        //printf("a (%.2f) b (%.2f) c(%.2f)", device->altitude, device->latitude, device->longitude );
        bool altiValue = device->altitude >= (*tree)->altitude_min + (fabs((*tree)->altitude_max - 
                                                    (*tree)->altitude_min)/2);
        bool latiValue = device->latitude >= (*tree)->latitude_min + (fabs((*tree)->latitude_max - 
                                                    (*tree)->latitude_min)/2);
        bool longValue = device->longitude >= (*tree)->longitude_min + (fabs((*tree)->longitude_max - 
                                                    (*tree)->longitude_min)/2);
        if( altiValue && latiValue && longValue ){
            //printf("1\n");
            initalize_sub_tree((*tree), 0);
            insert(&(*tree)->sub_tree[0], device);
        }else if( altiValue && latiValue && !longValue ){
            //printf("2\n");
            initalize_sub_tree((*tree), 1);
            insert(&(*tree)->sub_tree[1], device);//insert in q2
        }else if( altiValue && !latiValue && longValue ){
            //printf("3\n");
            initalize_sub_tree((*tree), 2);
            insert(&(*tree)->sub_tree[2], device);
        }else if( altiValue && !latiValue && !longValue ){
        //    int a;
        //    scanf("%d", &a);
           // printf("4\n");
            initalize_sub_tree((*tree), 3);
            insert(&(*tree)->sub_tree[3], device);//insert in q4
        }else if( !altiValue && latiValue && longValue ){
            //printf("5\n");
            initalize_sub_tree((*tree), 4);
            insert(&(*tree)->sub_tree[4], device);
        }else if( !altiValue && latiValue && !longValue ){
            //printf("6\n");
            initalize_sub_tree((*tree), 5);
            insert(&(*tree)->sub_tree[5], device);
        }else if( !altiValue && !latiValue && longValue ){
            //printf("7\n");
            initalize_sub_tree((*tree), 6);
            insert(&(*tree)->sub_tree[6], device);//insert in q7
        }else if( !altiValue && !latiValue && !longValue ){
            //printf("8\n");
            initalize_sub_tree((*tree), 7);
            insert(&(*tree)->sub_tree[7], device);//insert in q8
        }
    }
    return true;
}

/* Function is used to initalize a sub_tree of the given tree with the appropiate range values
 * to maintain the properties of the oct_tree
 * @PARAM tree -- tree containing the sub_tree that is to be initalized.
 * @PARAM quad -- index of the sub_tree contained in tree. For reference please see the notes
 *      found oct_tree structure definition
 * @RETURN -- no return value. 
 */
void initalize_sub_tree(struct oct_tree* tree, int quad){
    if( ! tree->sub_tree[quad] ){
        tree->sub_tree[quad] = malloc(sizeof(*tree->sub_tree[quad]));
        if( ! tree->sub_tree[quad] ){
            fprintf(stderr, "ERROR: sub_tree malloc failed\n");
            return;
        }
        memset(tree->sub_tree[quad], 0, sizeof(*(tree->sub_tree[quad])));
    }
    //printf("%d\n", quad);
     //int a;
      //  scanf("%d", &a);
    if(tree->sub_tree[quad]->status == 0){ // have empty unitialized tree
        tree->sub_tree[quad]->status = -1;
        //set altitude 
        
        if( (quad+1)<5){
            tree->sub_tree[quad]->altitude_max = tree->altitude_max;
            tree->sub_tree[quad]->altitude_min = tree->altitude_min + (fabs(tree->altitude_max - 
                                            tree->altitude_min)/2);
            //printf("%.2f/n",((tree->altitude_max - 
            //                                tree->altitude_min))/2);
        } else{
            tree->sub_tree[quad]->altitude_max = tree->altitude_min + (fabs(tree->altitude_max - 
                                            tree->altitude_min)/2);
            tree->sub_tree[quad]->altitude_min = tree->altitude_min;
        }
        // set latitude
        if( (quad+1) % 4 == 1 || (quad+1) % 4 == 2){                              
            tree->sub_tree[quad]->latitude_max = tree->latitude_max;
            tree->sub_tree[quad]->latitude_min = tree->latitude_min + (fabs(tree->latitude_max - 
                                            tree->latitude_min)/2);
        } else {
            tree->sub_tree[quad]->latitude_max = tree->latitude_min + (fabs(tree->latitude_max - 
                                            tree->latitude_min)/2);
            tree->sub_tree[quad]->latitude_min = tree->latitude_min;
        }
        // set longitude
        if ( (quad+1) % 2 ){ 
            tree->sub_tree[quad]->longitude_max = tree->longitude_max;
            tree->sub_tree[quad]->longitude_min = tree->longitude_min + (fabs(tree->longitude_max - 
                                            tree->longitude_min)/2);
        }else{
            tree->sub_tree[quad]->longitude_max = tree->longitude_min + (fabs(tree->longitude_max - 
                                            tree->longitude_min)/2);
            tree->sub_tree[quad]->longitude_min = tree->longitude_min;
        }
    }
}

/* Function expands the specified tree by one level in the direction of the specifed device. 
 * The function does not guarantee that the device will fit into the expanded tree. Repeated calls
 * may be necessary to ensure the device will fit. One level is equivialnt to an 8-fold expansion
 * in the area covered by the tree. If specified device already fits into the tree the tree will be
 * expanded anyway, with all dimensions doubling in the positive. 
 * @PARAM tree -- tree to expand
 * @PARAM device -- device to dictate direction in which to expand. Only the first device in the 
 *      linked list will be examined.
 * @RETURN -- no return value. 
 */
void grow_tree(struct oct_tree** tree, struct device* device){
    if ( ! *tree || ! device ){
        fprintf(stderr, "ERROR: nothing to work with\n");
        return;
    }
    struct oct_tree* bigger = malloc(sizeof(*bigger));
    if (! bigger ){
        fprintf(stderr, "ERROR: sub_tree malloc failed\n");
                    return;
    }
    memset(bigger, 0, sizeof(*bigger));
    bigger->status = -1;
    bool altiup = true;
    bool longup = true;
    bool latiup = true;
   //printTree((*tree), 0);
    // set size params to grow 4x size in direction of new point
    //printf("(%.4f) < (%.4f) %s\n", device->altitude, (*tree)->altitude_min, device->altitude < (*tree)->altitude_min?"true":"false");
    if ( device->altitude < (*tree)->altitude_min){
        int diff = fabs((*tree)->altitude_max - (*tree)->altitude_min);
        bigger->altitude_min = diff > 1 ? (*tree)->altitude_min - diff : (*tree)->altitude_min - fabs((*tree)->altitude_min);
        bigger->altitude_max = (*tree)->altitude_max;
        
    }else{
        bigger->altitude_min = (*tree)->altitude_min;
        int diff = fabs((*tree)->altitude_max - (*tree)->altitude_min);
        bigger->altitude_max = diff > 1 ? (*tree)->altitude_max + diff : (*tree)->altitude_max + fabs((*tree)->altitude_max) ;
            
         altiup = false;
    }
    //printf("(%.4f) < (%.4f) %s\n", device->latitude, (*tree)->latitude_min, device->latitude < (*tree)->latitude_min?"true":"false");
    if (device->latitude < (*tree)->latitude_min){
        int diff = fabs((*tree)->latitude_max - (*tree)->latitude_min);
        bigger->latitude_min = diff > 1 ? (*tree)->latitude_min - diff : (*tree)->latitude_min - fabs((*tree)->latitude_min);
            
        bigger->latitude_max = (*tree)->latitude_max;
        
    }else{
        int diff = fabs((*tree)->latitude_max - (*tree)->latitude_min);
        bigger->latitude_min = (*tree)->latitude_min;
        bigger->latitude_max = diff > 1 ? (*tree)->latitude_max + diff : (*tree)->latitude_max + fabs((*tree)->latitude_max);
            
        latiup = false;
    }
    if ( device->longitude < (*tree)->longitude_min){
        int diff = fabs((*tree)->longitude_max - (*tree)->longitude_min);
        bigger->longitude_min = diff > 1 ? (*tree)->longitude_min - diff : (*tree)->longitude_min - fabs((*tree)->longitude_min);
        bigger->longitude_max = (*tree)->longitude_max;
        
    }else{
        int diff =  fabs((*tree)->longitude_max - (*tree)->longitude_min);
        bigger->longitude_min = (*tree)->longitude_min;
        bigger->longitude_max = diff > 1 ? (*tree)->longitude_max + diff : (*tree)->longitude_max + fabs((*tree)->longitude_max);
           
        longup = false;
    }
    if( altiup && latiup && longup ){
        bigger->sub_tree[7] = (*tree);
    }else if( altiup && latiup && !longup ){
        bigger->sub_tree[6] = (*tree);
    }else if( altiup && !latiup && longup ){
        bigger->sub_tree[5] = (*tree);  
    }else if( altiup && !latiup && !longup ){
        bigger->sub_tree[4] = (*tree); 
    }else if( !altiup && latiup && longup ){
        bigger->sub_tree[3] = (*tree);
    }else if( !altiup && latiup && !longup ){
        bigger->sub_tree[2] = (*tree);
    }else if( !altiup && !latiup && longup ){
        bigger->sub_tree[1] = (*tree);
    }else if( !altiup && !latiup && !longup ){
        bigger->sub_tree[0] = (*tree);
    }
    
    
//     printTree(bigger, 0);
//    int a;
//    scanf("%d", &a);

    (*tree) = bigger;
}

/* Function will free an oct_tree and all data contained within it.
 * @PARAM tree -- oct_tree structure whihc is to be returned to the ether from whence it came.
 * @RETURN -- no return value. 
 */
void oct_tree_disassemble(struct oct_tree* tree){
    if ( ! tree ){
        return;
    }
//    if (tree->status ==1){
       // kill_id_list(tree->device_ids);
  //  }
    if (tree->status == -1){ // if you have initialized subtrees
        for (int i = 0; i < 8 ; i++){
            oct_tree_disassemble(tree->sub_tree[i]);
        }
    }
    free(tree);
}
/* Function is used to locate all unique nodes within a specified 3 dimensional range. It searches through tree
 *      and will identify nodes which are sored in sub_trees whose values fall within the bounds specified.
 * @PARAM tree -- oct_tree containing the data to be searched
 * @PARAM range -- oct_tree structure to specify the range to search 
 *      NOTE: structure is utilized out of lazyness only the top layer of the this tree will be examined.
 * @RETURN -- a newly MALLOC'D device structure (which is a linked list) containing a copy of
 *      all data points in tree whose GPS coordinates are within AND CLOSE to the specified range. (see note below)
 */
struct device* find_close(struct oct_tree* tree, struct oct_tree* range){
    if ( ! tree || ! range ){
        fprintf(stderr, "ERROR: invalid pointers \n" );
        return NULL;
    }
    if ( tree->status == 0){ // there is nothing in the tree 
        return NULL;
    }
    // if any of the minimum values in the search range are greater tahn the max value
    // in tree there are no valid datapoints in tree
    bool tooHigh = range->altitude_min > tree->altitude_max ||
                range->latitude_min > tree->latitude_max ||
                range->longitude_min > tree->longitude_max;
    
    bool tooLow = range->altitude_max < tree->altitude_min || 
                range->latitude_max < tree->latitude_min ||
                range->longitude_max < tree->longitude_min;
                
    if ( tooHigh || tooLow){ // search area outside of our scope
       // printf("%.2f alt %.2f too %s\n", tree->altitude_min, tree->altitude_max, tooHigh ? "high" : "low");
        return NULL;
    }
    struct device* rValue = NULL;
    if( tree->status == 1 ){ // at end of element list 
        // NOTE: TODO check each device coordinates to see if they are valid 
        // Not done because in this implementation it will be done later in code anyway. 
        rValue = malloc( sizeof(*rValue));
        if( ! rValue ){
            fprintf(stdout, "ERROR: malloc failed\n");
            return NULL;
        }
        memset(rValue, 0, sizeof(*rValue));
        clone_list(rValue, tree->device_ids); 
    }
    for (int i = 0; i < 8; i++){
        if( tree->sub_tree[i] ){
            append_list(&rValue, find_close(tree->sub_tree[i], range));   
        }
    }
    return rValue;
}

/* Function creates a copy of the data found in list2 and stores it in list1
 * if list2 is longer than a single element data will be newly malloc'd
 * @PARAM list1 -- pointer to list where copy is to be stored
 * @PARAM list2 -- pointer to list to copy
 * @RETURN -- no return value.
 */
void clone_list(struct device* list1, struct device* list2){
    //TODO check this --- IDK if it's right should be if( !list1 || !list2) return
    if( ! list1 ){
        if ( ! list2 ){
            return;
        }
        list1 = list2;
        return;
    }
    list1->altitude = list2->altitude;
    list1->latitude = list2->latitude;
    list1->longitude = list2->longitude;
    list1->device_id = list2->device_id;
    struct device* temp = NULL;
    if ( list2->next) {
        temp = malloc( sizeof(*temp));
        if( ! temp ){
            fprintf(stderr, "ERROR: malloc failed\n");
            return;
        }
        memset(temp, 0, sizeof(*temp));
        clone_list(temp, list2->next);
    }
    list1->next = temp;
    
}

/* Function appends the second list to the end of the first. if the first is NULL
 *      at function call it will become a pointer to the second list. 
 * @PARAM list1 -- list to append to
 * @PARAM list2 -- list to be appened
 * @RETURN -- no return value.
 */
void append_list(struct device** list1, struct device* list2){
    if( ! (*list1) ){
        if ( ! list2 ){
            return;
        }
        (*list1) = list2;
        return;
    }
    struct device* temp = *list1;
        while( temp->next ){
            temp = temp->next;
        }
    temp->next = list2;
}


/* Function is used to free memory associated with device linked list structure
 * @PARAM list -- struct device* (linked list) to destroy 
 * @RETURN -- no return value.
 */
void kill_id_list(struct device* list){
    if( list ){
        struct device* temp = list->next;
        free( list );
        kill_id_list(temp);
    }
}

/* Function to determine if a device is within the bounds of a given tree
 * @PARAM tree -- tree to check against the device
 * @PARAM device -- device to examine
 * @RETURN -- true if the GPS coordinates of the device fall wtihin the bounds 
 *      of the tree else false. 
 *
 */
bool out_of_bounds(struct oct_tree* tree, struct device* device){
    if ( ! tree || ! device ){
        return false;
    }
    /*
    printf("alti %s", device->altitude <= tree->altitude_max && device->altitude >= tree->altitude_min?
                        "true" : "false");
    printf(" lati %s", device->latitude <= tree->latitude_max && device->latitude >= tree->latitude_min?
                        "true" : "false");
    printf(" long %s\n", device->longitude <= tree->longitude_max && device->longitude >= tree->longitude_min?
                        "true" : "false");
    printTree(tree, 0);               
    */
    bool rValue = device->altitude <= tree->altitude_max && device->altitude >= tree->altitude_min &&
            device->latitude <= tree->latitude_max && device->latitude >= tree->latitude_min &&
            device->longitude <= tree->longitude_max && device->longitude >= tree->longitude_min;
    return !rValue;
}

/* Function used to print the Oct-Tree structure Prints min and max altitude
 *      latitude and longitude follows by status and device id if aplicable. 
 *      sub_trees are indented to indicate parentage. 
 * @PARAM tree -- the oct_tree structure to be printed
 * @PARAM count -- should be zero utilized for formatting because I'm too lazy to make
 *      make public and private methods.
 */
void printTree(struct oct_tree* tree, int count){
    if( ! tree ){
        return;
    }
//    if( tree->status == 0 ){
//        return;
//    }
    printf("%*c", count, ' ');
    printf( "%.2f <alt< %.2f   ", tree->altitude_min, tree->altitude_max);
    printf( "%.2f <lat< %.2f   ", tree->latitude_min, tree->latitude_max);
    printf( "%.2f <long< %.2f ", tree->longitude_min, tree->longitude_max);
    printf( "status (%d)", tree->status);
    if( tree->status == 1 ){
        printf(" id (%d) ", tree->device_ids->device_id);
        struct device* temp = NULL;
        temp = tree->device_ids->next;
        while ( temp ){
            printf(" (%d) ", temp->device_id);
            temp= temp->next;
        }
    }
    printf("\n");
    for (int i =0; i < 8; i++){
        if(tree->sub_tree[i]){
            printf("  ");
            //printf(" %*c", count, ' ');
            printTree(tree->sub_tree[i], count+1);
        }
    }
    
}

/* Main is only here to test to make sure i didn't do too much stupid stuff and that the linker is happy
 *
 */
/*
int main(void){
    //here for linker happiness
    struct device test = { .altitude = 1, .latitude = 1, .longitude = 1, .device_id = 46, .next = NULL};
    struct device test5 = { .altitude = 2, .latitude = 2, .longitude = 2, .device_id = 22, .next = NULL};
    struct device test6 = { .altitude = 4, .latitude = 6, .longitude = 6, .device_id = 11, .next = NULL};
    struct device test7 = { .altitude = 6, .latitude = 4, .longitude = 4, .device_id = 57, .next = NULL};
    struct device test2 = { .altitude = 3, .latitude = 3, .longitude = 3, .device_id = 88, .next = NULL};
    struct device test3 = { .altitude = -700, .latitude = 7, .longitude = 7, .device_id = 77, .next = NULL};
    struct device test4 = { .altitude = 400, .latitude = 1, .longitude = 3, .device_id = 43, .next = NULL};
    struct oct_tree* tree = malloc( sizeof( * tree));
    memset(tree, 0, sizeof(*tree));
    insert(&tree, &test);
    insert(&tree, &test3);
    insert(&tree, &test2);
    insert(&tree, &test4);
    insert(&tree, &test5);
    insert(&tree, &test6);
    insert(&tree, &test7);
    printTree(tree, 0);
    struct oct_tree range = { .altitude_max = 4, .altitude_min = 2, .latitude_max = 4, .latitude_min = 1,
                                .longitude_max = 4, .longitude_min = 2}; 
    struct device* find = find_close(tree, &range);
    struct device* temp = find;
    while( find ) {
        printf("%d, ", find->device_id);
        find = find->next;
    }
    printf("\n");
    kill_id_list(temp);
    oct_tree_destroy(tree);
}
*/