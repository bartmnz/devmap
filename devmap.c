// slashies --outline
#include <limits.h>
#include "octTree.h"
#include "graph.h"
#include "queue.h"
#include "meditrik.h"

bool compare_device( void* first,  void* second);
struct llist *find_biggest_sub_network(graph* map, struct llist* devices);
struct llist* is_valid_graph(graph* map, struct llist* devices);

//#define CONNECT_DISTANCE = 5;

/* Function opens the file located at the specified path(s) TODO one file o multiple?
 *      and parses through looking for GPS packets. Returns a list of all valid GPS 
 *      data found in input.
 * @PARAM path -- string representing file path of input file.
 * @RETURN -- list of devices that were in input file 
 */
struct device* collect_GPS_data( char* path ){
    if ( ! path ){
        return NULL;
    }
    struct device* compile = NULL;
    return compile;
}

/* Function takes a list of devices and constructs a weighted graph based upon the max
 *      connected distance specified in the file. //possible flourish make distance dynamic
 * @PARAM list -- linked list of devices with their gps cordinates to make a graph of
 * @RETURN -- a graph structure detailing the interconnected-ness of the devices.
 */
struct graph* make_graph(struct device* list){
    if ( ! list ){
        return NULL;
    }
    struct graph* compile = NULL;
    return compile;
}

/* Function checks to see if two devices are close enough to be connected to each other. Only
 *      the first element in each list will be examined. Utilizes haversine function
 *      to determine distance between lat and long. Source implementation found:
 *      http://stackoverflow.com/questions/27928/calculate-distance-between-two-latitude-longitude-points-haversine-formula
 * @PARAM to_check -- first device to compare.
 * @PARAM against -- second device to compare.
 * @RETURN -- true if they are connected else false.
 */
bool is_connected(struct device* to_check, struct device* against){
    if ( ! to_check || ! against ){
        return false;
    }
    //TODO check case where at 180 
    double lat1 = to_check->latitude;
    double lat2 = against->latitude;
    double lon1 = to_check->longitude;
    double lon2 = against->longitude;
    
    double p = 0.017453292519943295;    // PI / 180
    double a = 0.5 - cos((lat2 - lat1) * p)/2 + 
          cos(lat1 * p) * cos(lat2 * p) * 
          (1 - cos((lon2 - lon1) * p))/2;
    
    double distance = 12742 * asin(sqrt(a)); // 2 * R; R = 6371 km
    
    double altitude = fabs(to_check->altitude - against->altitude);
    altitude = altitude * 0.3048; // feet to meters
    distance = distance * 1000; // km to meters
    // TODO check min connected value 1.25 m
    //printf ("altitude (%.2f) distance (%.2f)\n", altitude, distance);
   // printf("from (%d) to (%d)", to_check->device_id, against->device_id);
   // printf(" distance is : (%.2f)\n", sqrt(distance*distance + altitude*altitude));
    double result = sqrt(distance*distance + altitude*altitude);
    return  result < 5 && result > .381 ? true : false; // between 5 meters and 1.25 feet
}

/* Function determines if there are two disjointed paths between the specified devices
 * @PARAM points -- linked list of devices the head is the start node and the second 
 *      element is the end point.
 * @PARAM graph -- the graph to traverse looking for the paths.
 * @RETURN -- true if two seperate paths exist between the nodes else false.
 */
struct llist* dijkstra(graph* map, struct device* point1, struct device* point2){
    if ( ! point1 || ! point2 || ! map ){
        fprintf(stderr, "ERROR: need datas\n");
        return NULL;
    }
    //printf("dijkstra\n");
    struct llist* seen_before = ll_create(point2);
    struct llist* sb_head = seen_before;
    // make a list of nodes already visited
    queue* the_list = queue_create();
    struct llist* simple = ll_create(point2);
    queue_enqueue(the_list, simple);
    
    //printf("points->next is (%p)\n", (void*)points->next);
    // make a the_list to store current path
    // each path must be stored as a linked list
    // add a to queue
    while ( ! queue_is_empty(the_list)){
    //  if   ( ! queue_is_empty(the_list)){
        struct llist* cur_path = queue_dequeue(the_list);
        if( ! cur_path ) return NULL;
        struct llist* list_of_connections = graph_adjacent_to(map, cur_path->data, compare_device);
        struct llist* loc_head = list_of_connections;
        //if ( ! list_of_connections ) return NULL;
        //printf("list is at %p\n", (void*)list_of_connections);
        if ( list_of_connections ) {
            while( list_of_connections ->data){
            //    printf("checking ()\n");//, ((struct device*)list_of_connections->data)->device_id);
            // for each element in list_of_connections
                
                if(compare_device(list_of_connections->data, point1)){
                // if is b, return current_list
                    ll_add(&cur_path, list_of_connections->data );
                    ll_disassemble(sb_head);
                    ll_disassemble(loc_head);
                    seen_before = queue_dequeue(the_list);
                    while( seen_before ){
                        ll_disassemble(seen_before);
                        seen_before = queue_dequeue(the_list);
                    }
                    queue_disassemble(the_list);
                    //ll_disassemble(head);
                    // dequeue everything and check first element in list if it is not same
                    // as good path requeue it and reasign queue
                    // free all lists
                    //make sub grap
                    return cur_path;
                }
                else{
                    bool saw_it = false;
                    while ( seen_before ){
                        if ( compare_device(seen_before->data, list_of_connections->data)){
                            saw_it = true;
                        }
                        seen_before = seen_before->next;
                    }
                    seen_before = sb_head;
                    if(!saw_it){
                        ll_add(&seen_before, list_of_connections->data);
                        struct llist* newList = ll_clone(cur_path);
                        ll_add(&newList, list_of_connections->data);
                        queue_enqueue(the_list, newList);
                        sb_head = seen_before;
                        //queue
                    }
                }
                
                list_of_connections = list_of_connections->next;
                if( !list_of_connections) break;
            }
            ll_disassemble(cur_path);
            ll_disassemble(loc_head);
        }else{    
            ll_disassemble(simple);
        }
    }
    
    queue_disassemble(the_list);
    ll_disassemble(sb_head);
    
    return NULL;
}


bool two_paths(graph* map, struct device* point1,  struct device* point2){
    // run dijkstra's
    
    struct llist* path1 = dijkstra(map, point1, point2);
    struct llist* p1_head = path1;
    int hops = 0;
    while (path1){
       // printf("(%d) -> ", ((struct device*)path1->data)->device_id);
        hops ++;   
        path1 = path1->next;
    }
    //printf("\ntwopaths\n");
    // the nodes are adjacent to each other
    if (hops == 2){
        //printf("here1234\n");
        ll_disassemble(p1_head);
        return true;
    }
    path1 = p1_head;
    if ( ! path1 || !path1->data){
        printf("no path found!!\n");
        return false;
    }
    
    if ( ! path1->next || // found ourself ?? WTF
        compare_device(path1->next->data, point2 )){ // two directally connected elements
        ll_disassemble(path1);
        return true; //i guess???
    }
    
    if (compare_device(path1->data, path1->next->data) ){ // i'm not sure how this is happening
        return true;
    }
    // if result not null && bigger than two elements for each node that is not the end points,
    
    // copy graph
    graph* doppelganger = graph_clone(map);
   // printf("8\n");
    
   //print_edges(doppelganger);
   // printf("\n\nadded stuffs\n");
    struct llist* cur = path1->next;
    struct device* temp = NULL;
    struct device* prev = NULL;
    int count = 0; 
    struct llist* added_nodes = NULL;// = ll_create(NULL);
    do{
/*        printf("working on1 (%d)\n", ((struct device*)cur->data)->device_id );
        int a;
        scanf("%d", &a);
*/
        temp = malloc(sizeof(*temp));
        if ( ! temp ){
            fprintf(stderr, "ERROR: bad malloc\n");
            exit(0);
        }
        memset(temp, 0, sizeof(*temp));
        temp->device_id = INT_MAX-count;
        temp->altitude = EXTRA_NODE;
        graph_add_node(doppelganger, temp, compare_pointers);
        ll_add(&added_nodes, temp);
        struct llist* adj_list = graph_adjacent_to(doppelganger, cur->data, compare_device);
        if(! adj_list){
            printf("no list\n");
            break;
        }
        struct llist* head = adj_list;
        while ( adj_list && adj_list->data){
            graph_add_edge(doppelganger, temp, adj_list->data, 1, compare_device);
           // printf("removing edge from (%d) ", ((struct device*)cur->data)->device_id );
           // printf("to (%d)\n ", ((struct device*)adj_list->data)->device_id );
            graph_remove_edge(doppelganger, cur->data, adj_list->data, compare_device);
            adj_list = adj_list->next;
        }
        if( ! prev ){ // first time through
            
            graph_add_edge(doppelganger, cur->data, point1, 0, compare_device);
            
        } else {
            graph_add_edge(doppelganger, cur->data, prev, 0, compare_device);
        }
        graph_add_edge(doppelganger, temp, cur->data, 0, compare_device);
        
        count++;        // bookkeeping
        prev = temp;
        cur = cur->next;
        ll_disassemble(head);
//        printf("\n\n");
//        print_edges(doppelganger);
    }while (cur->next);
    graph_add_edge(doppelganger, cur->data, prev, 0, compare_device);
   // printf("\n");
    
   // print_edges(doppelganger);
   // printf("\n\nadded stuffs\n");
    struct llist* list2 = dijkstra(doppelganger, point1, point2);
    struct llist* an_head = added_nodes;
    while ( added_nodes && added_nodes->data){
        graph_remove_node(doppelganger, added_nodes->data, compare_device);
        free( added_nodes ->data);
        count--;
        added_nodes = added_nodes->next;
    }
    graph_disassemble(doppelganger);
    ll_disassemble(an_head);
    ll_disassemble(path1);
    bool rValue = false;
    //if( list2 && !list2->next){
    if ( list2 ){ // dijkstra returns a path from a to b if it exists // else NULL
        rValue = true;
    }
    ll_disassemble(list2);
    return  rValue;
    // need to make a new node a 
    
    // such that a points to node
    
    // add a node for each non-endpoint node in path -- add to a new list 
    // remove all outgoing edges from node in path and add to new node.
    // add edge to each new node from each old node in path
    
    // remove all edges from node and add them to a -- makes node have all incoming edges, and a have all outgoing
    // run dijkstra's again -- if path found have two disjoint paths.
}

/* Function determines if it is possible to remove nodes from a graph such that the result 
 * will have two disjoint paths between all nodes. //NOTE: there is no pretty way to do this.
 * @PARAM graph -- graph to check for possibilities 
 * @RETURN -- a list of nodes to remove to satisfy the condition, NULL if not possible
 */
struct llist* can_remove(struct graph* g){
    if ( ! g ){
        return NULL;
    }
    graph* map = graph_clone(g);
    size_t nodes = graph_node_count(map);
    size_t edges = graph_edge_count(map);
    
    if ( nodes == 1 ||(nodes == 2 && edges >= 1)){
        graph_disassemble(map);
        return NULL;
    }
    struct llist* devices = NULL;
    struct llist* removed = NULL;
    devices = graph_list_nodes(map);
    if (edges < nodes ){ // don't bother trying
        graph_disassemble(map);
        return devices;
    }
    // node count -- edge count check
    // create llist* removed nodes
    // create llist* remaining nodes
    struct llist* d_head = devices;
    bool done;
    do{
        done = true;
        if (nodes > 2 ){
            d_head = devices;
            struct llist* prev = NULL;
            int count = 0;
            while ( devices ){
                struct llist* adj_list = graph_adjacent_to(map, devices->data, compare_device);
                struct llist* al_head = adj_list;
                count = 0;
                while( adj_list ){
                    count ++;
                    adj_list = adj_list->next;
                }
                ll_disassemble(al_head);
                if ( count == 1 || count == 0 ){
                    graph_remove_node(map, devices->data, compare_device);
                    ll_add(&removed, devices->data);
                    ll_remove(&devices);
                    if( ! prev ){
                        // we removed the first element -- reset head
                        d_head = devices;
                        // no need to mess with prev -- devices has been incremented
                    }else{
                        prev->next = devices;
                    }
                    if(nodes == 3){
                        devices = d_head;
                        done = true;
                        goto DONE;
                    }
                    done = false; // if we have to remove anything we need to loop again;
                }
                else{
                    prev = devices;
                    devices = devices->next;
                    
                }
            }
            devices = d_head;
            nodes = graph_node_count(map);
        }
    DONE: // TODO remove below line
    done = true;
        
    }while(!done);
    
    devices = d_head;
    
    // if node > 3 check edge count for each node, 
        // if 1 || 0 add to remove list and remove from remaining nodes
    struct llist* best_graph = find_biggest_sub_network(map, devices);
    struct llist* bg_head = best_graph;
    printf("best_graph: \n");
    devices = d_head;
    while (best_graph){
        printf( "(%d)", ((struct device* )best_graph->data)->device_id);
        best_graph = best_graph->next;
    }
    best_graph = bg_head;
    devices = d_head;
    
    
    struct llist* all_gone = ll_diff(devices, best_graph, compare_device);
    struct llist* ag_head = all_gone;
    struct llist* r_head = removed;
    
    // loop through both lists to ensure no duplicates.
    while (all_gone){
        while(removed){
            if ( compare_device(all_gone->data, removed->data) ){
                goto LOOP;
            }
            removed = removed->next;
        }
        ll_add(&r_head, all_gone->data);
    LOOP:
        removed = r_head;
        all_gone= all_gone->next;
    }
    
    ll_disassemble(d_head);
    if ( !(d_head == best_graph )){
        ll_disassemble(best_graph);
    }
    
    
    ll_disassemble(ag_head);
    graph_disassemble(map);

    return removed;
        // create a llist* for both 
            // compare a & b against all nodes in remaining nodes
            // if there are two paths from the a | b to the node add it to it's respective 
                // llist
            // compare the two lists to the list of remaining nodes -- if there are any nodes
            // missing from the two lists create additional lists to cover them
            // run is valid graph on each sub-list
            // IFF is valid_graph -- sub-network is valid and save min size 
                // if node count (sub_graph ) < min size return sub graph 
                // else check sub graphs
     // list is to messed up to try
    
    // run isgraph on list of remaining nodes 
        // if not a valid graph use the two returned disjointed nodes  a & b
}

struct llist *find_biggest_sub_network(graph* map, struct llist* devices){
    if ( ! map || ! devices ){
        return NULL;
    }
    struct llist* d_head = devices;
    
    struct llist* TN1 = ll_clone(devices);
    struct llist* notConnected = is_valid_graph(map, TN1);
    ll_disassemble(TN1);
        // returns single element if valid else null or two elements
    
   
    if( notConnected && notConnected->next ){ //graph is not connected, ... the two items in the list 
            // cannot be connected by two paths.
        
        struct llist* connected_to = NULL;
        struct llist* first = NULL;
        struct llist* second = NULL;
        ll_add(&first, notConnected->data);
        ll_add(&second, notConnected->next->data);
        // for both disjointed nodes identify their potential valid sub-networks.
        
        
        while ( devices ){
            
            if( ! compare_device(notConnected->data, devices->data) &&
                two_paths(map, notConnected->data, devices->data)){
                    ll_add(&first, devices->data);
            }
            if ( ! compare_device(notConnected->next->data, devices->data) &&
                two_paths(map, notConnected->next->data, devices->data)){
                    ll_add(&second, devices->data);
            }
            devices = devices->next;
        }
        devices = d_head;
        ll_add(&connected_to, first);
        ll_add(&connected_to, second);
        
        
        
        // check to see if there is anything else that wasn't picked up 
        struct llist* diff1 = ll_diff(devices, first, compare_device);
        struct llist* difference = ll_diff(diff1, second, compare_device);
        struct llist* dif_head = difference;
        ll_disassemble(diff1);
        /*printf("\ndifferent:\n");
        while( difference ){
            printf(" (%d)", ((struct device*)difference->data)->device_id);
            difference = difference->next;
            
        }*/
        struct llist* add = NULL;
        while ( difference ){
            while ( devices ){
                if ( ! compare_device(difference->data, devices->data) &&
                    two_paths(map, difference->data, devices->data)){
                        ll_add(&add, devices->data);
                }
                devices = devices->next;
            }
            devices = d_head;
            ll_add(&connected_to, add);
            // check to see if we are still missing things
            diff1 = ll_diff(difference, add, compare_device);
            difference = diff1;
            ll_disassemble(diff1);
            add = NULL;
        }
        ll_disassemble(dif_head);
        // now we need to compare the sub_networks 
        struct llist* best_graph = NULL;
        struct llist* ct_head = connected_to;
        int maxsize = 0;
        // iterate over the list of sub_networks
        while ( connected_to ){
            
            // if a sub_network has fewer nodes than the biggest .. it can't be bigger
            if( ll_length(connected_to->data) > maxsize ){
//                printf("recursion\n");
                struct llist* result = find_biggest_sub_network(map, connected_to->data);
                // we found a new biggest
                int sum = 0;
                if( (sum = ll_length(result)) > maxsize ){
                    maxsize = sum;
                    if ( best_graph ){
                        ll_disassemble(best_graph);
                    }
                    best_graph = result;
                    connected_to = connected_to->next;
                    continue;
                }
                ll_disassemble(result);
            }
            //clean up
            struct llist* to_kill = connected_to->data;
            connected_to = connected_to->next;
            ll_disassemble(to_kill);
        }
        
        //clean up
        ll_disassemble(ct_head);
        ll_disassemble(notConnected);
        return best_graph;
        
    }else if ( notConnected ){ // list is valid
        ll_disassemble(notConnected);
        
        return devices;
    } else return NULL;
}

/* Function opens the file specified by path and searches for pcap files which specify 
 * the device has a batery level <= the given threshold
 * @PARAM path -- string representing the path of the file to open.
 * @PARAM life -- battery level threshold 
 * @RETURN -- a list of devices such that (device battery level <= life)
 *
 */
struct device* check_bat_level( char* path, double life){
    if ( ! path || life < .0001 ){
        return NULL;
    } 
    struct device* compile = NULL;
    return compile;
}

bool compare_device( void* first,  void* second){
    if (! first || ! second ){
        return false;
    }
    if( EXTRA_NODE- fabs(((struct device*)first)->altitude)  < .1|| 
        EXTRA_NODE - fabs(((struct device*)second)->altitude) < .1){
            if( ((struct device*)first)->altitude - 
                ((struct device*)second)->altitude > .1)   {
                    return false;
                }
        }
        //printf("(%d) and (%d) \n", ((struct device*)first)->device_id, 
      //  ((struct device*)second)->device_id);
    return ((struct device*)first)->device_id == 
        ((struct device*)second)->device_id ? true : false;
}
/*
 *@RETURNS: NULL if can't even try -- single element if valid and two elements that are
 *      not connected by two paths if not valid
 */
struct llist* is_valid_graph(graph* map, struct llist* devices){
    if( ! map ){
        return NULL;
    }
    struct llist* rValue = NULL;
    size_t nodes = graph_node_count(map);
    size_t edges = graph_edge_count(map);
    if ( nodes == 1 ||(nodes == 2 && edges == 1)){
        ll_add(&rValue, devices->data);
        return rValue;
    }
    if (edges < nodes ){ // don't bother trying
        return NULL;
    }

    // while there are at least two things in the list 
    while ( devices && devices->next ){
        printf("(%d)\n", ((struct device*)devices->data)->device_id);
        // generate a list of all the things adjacent to the first elemnet
        struct llist* adj_list = NULL;
        adj_list = graph_adjacent_to(map, devices->data, compare_pointers);
        struct llist* al_head = adj_list;
        
        // get everything in the list of devices that is not adjacent to 
        // the first element
        adj_list = al_head;
        struct llist* to_check = ll_diff(devices->next, adj_list, compare_device); // yes it will check against itself --don't matter
        struct llist* tc_head = to_check;
        ll_disassemble(adj_list);
        
        // check to see if all elements are doubly connected
        while( to_check ){
//            printf("checking (%d) vs (%d)\n", ((struct device*)to_check->data)->device_id, ((struct device*)devices->data)->device_id);
            if ( ! two_paths(map, to_check->data, devices->data)){
//                printf("now data is (%d)", ((struct device*)devices->data)->device_id);
                
                //ll_disassemble(d_head);
 //               fprintf(stdout, "(%d) is not connected to (%d)\n", 
   //                         ((struct device*)to_check->data)->device_id, 
     //                       ((struct device*)devices->data)->device_id);
                ll_add(&rValue, to_check->data);
                ll_add(&rValue, devices->data);
                //ll_disassemble(tc_head);
                ll_disassemble(tc_head);
                return rValue;
            }
            to_check = to_check->next;
        }
        
        
        
        
        
        ll_disassemble(tc_head);
        
        //ll_disassemble(al_head);
        devices = devices->next;
    }
    
    ll_add(&rValue, devices->data);
    //ll_disassemble(d_head);
    return rValue;
   
}

graph* test_makeGraph(struct device* test, struct oct_tree** tree){
    struct device* head = test;
    struct device* close; 
    *tree = malloc( sizeof( * (*tree)));
    struct oct_tree range;// = { .altitude_max = , .altitude_min = 2, .latitude_max = 4, .latitude_min = 1,
                            //    .longitude_max = 4, .longitude_min = 2}; 
    graph* map = malloc( sizeof( graph));
    memset(map, 0, sizeof(*map));
    memset((*tree), 0, sizeof(*(*tree)));
   // printf("\n");
    while (test){
        head = head->next;
        test->next=NULL;
//        printf("inserting (%d) \n", test->device_id);
//        printf("(%.6f) alt, (%.8f) lati, (%.8f) long\n", test->altitude, test->latitude, test->longitude);
    //    printTree(tree, 0);
    //    int a;f
    //    scanf("%d", &a);
        if( ! (fabs(STATUS_PACKET- test->latitude) < 1)){
            if(insert(tree, test)){
                if (! graph_add_node(map, test, compare_device)){
                    // we already have this device_id in the graph
                    free(test);
                    goto LOOP;
                }
                //TODO change to full 5m not half 
                range.altitude_max = test->altitude + 17; 
                range.altitude_min = test->altitude - 17; 
                range.latitude_max = test->latitude + 17*LAT_INCREMENT;
                range.latitude_min = test->latitude - 17*LAT_INCREMENT;
                range.longitude_max = test->longitude + 17*LAT_INCREMENT;
                range.longitude_min = test->longitude - 17*LAT_INCREMENT;
                
                close = find_close((*tree), &range);
                
                struct device* holder;
                while ( close ){
                    //TODO check to ensure that device is infact within 5m and not 1.25 m
                    holder = close->next;
                    close->next = NULL;
                    if (! compare_device(close, test)){
                        //printf("different\n");
                        //printf("is connected? %s\n", is_connected(close, test)? "yes" : "no");
                        if( is_connected(close, test)){
                            graph_add_edge(map, test, close,1, compare_device);
                            graph_add_edge(map, close, test,1, compare_device);
                        }
                    }
                    while(close){
                        struct device* to_kill = close;
                        close = close->next;
                        free(to_kill);
                    }
                    close = holder;
                }
            }
            else{ // can't insert
            free(test);
        }
        }else{ // is a status packet
            free(test);
        }
       //TODO make work for longitude == 180 // wraparound
       
        //printf("here/n");
        //free(test);
        LOOP:
        test = head;
    }
    
    return map;
}


/* Main -- you know what this does. If not: please close this before you break something!
 * @AUTHOR -- BARTMNZ 
 * @VERSION -- beta
 */
int main(int argc, const char* argv[]){
    if ( argc < 2 || ! argv ){
        return 0;
    }
    struct oct_tree* tree = NULL;
    struct device* head = decoder(argc, argv);
    graph* map = test_makeGraph(head, &tree);
   // printf("\n");
   // printTree(tree, 0);
    
    struct llist* top = dijkstra(map, head, head->next);
    struct llist* t_head = top;
    /*while ( top ){
        printf(" (%d) -> ", ((struct device*)top->data)->device_id);
    //    graph_remove_node(map, top->data, compare_device);
        top = top->next;
    }
    */
   // printf("\n");
  /*  struct device* stuff = head->next;
    head->next = head->next->next;
    free( stuff);*/
    //printf( " (%d) -> (%d) ", head->device_id, head->next->device_id);
    struct llist* devices = graph_list_nodes(map);
    struct llist* check = is_valid_graph(map, devices);
    bool valid = false;
    if( check && !check->next){
        valid = true;
    }
   // print_edges(map);
    printf("valid graph ? (%s)\n", valid ? "true" : "false");
    if (!valid){
        
        struct llist* burn_with_fire = can_remove(map);
        struct llist* bwf_head = burn_with_fire;
        int changes = 0;
        while(burn_with_fire){
            changes ++;
            burn_with_fire = burn_with_fire->next;
        }
        printf("changes: (%d)\n", changes);
        burn_with_fire = bwf_head;
        int numNodes = 100;//graph_node_count(map); // 
        if ( changes <= (numNodes /2) ){
            fprintf(stdout, " remove :");
            while(burn_with_fire){
            fprintf(stdout, " (%d) ", ((struct device*)burn_with_fire->data)->device_id);
            burn_with_fire = burn_with_fire->next;
            }
        }else{
            fprintf(stdout, " program figured out the answer, " 
                    "but Liam said to just say that too many changes are needed\n" );
        }
        
        printf("\n");
        ll_disassemble(bwf_head);
    }
    
    ll_disassemble(devices);
    ll_disassemble(check);
    //ll_disassemble(diff);
    ll_disassemble(t_head);
    printf("graph has (%zu) nodes\n", graph_node_count(map));
    printf("graph has (%zu) edges\n", graph_edge_count(map));
   // print_edges(map);
    //graph_disassemble(map);
    
    oct_tree_disassemble(tree);
    graph_destroy(map);
    //kill_id_list(head);
    
    return 1;
}

