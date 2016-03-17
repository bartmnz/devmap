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
     //   fprintf(stderr, "ERROR: need datas\n");
        return NULL;
    }
    //printf("dijkstra\n");
    struct llist* seen_before = ll_create(point2);
    struct llist* sb_head = seen_before;
    // make a list of nodes already visited
    queue* the_list = queue_create();
    struct llist* simple = ll_create(point2);
    queue_enqueue(the_list, simple);
    
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
/* Function checks to see if there are two distinct paths between given Nodes in a graph
 * @PARAM map -- graph to evaluate
 * @PARAM point1 @ point2 -- nodes to evaluate
 * @RETURN true if there are two disjointed paths between the nodes or if nodes are adjacent to each other. 
 * else false. 
 */

bool two_paths(graph* map, struct device* point1,  struct device* point2){
    // run dijkstra's
    
    struct llist* path1 = dijkstra(map, point1, point2);
    struct llist* p1_head = path1;
    int hops = 0;
    while (path1){
        hops ++;   
        path1 = path1->next;
    }
    // the nodes are adjacent to each other
    if (hops == 2){
        //printf("here1234\n");
        ll_disassemble(p1_head);
        return true;
    }
    path1 = p1_head;
    if ( ! path1 || !path1->data){
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
   
    struct llist* cur = path1->next;
    struct device* temp = NULL;
    struct device* prev = NULL;
    int count = 0; 
    struct llist* added_nodes = NULL;// = ll_create(NULL);
    do{
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
            break;
        }
        struct llist* head = adj_list;
        while ( adj_list && adj_list->data){
            graph_add_edge(doppelganger, temp, adj_list->data, 1, compare_device);
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
    }while (cur->next);
    graph_add_edge(doppelganger, cur->data, prev, 0, compare_device);
 
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
    
}

/* Function determines if it is possible to remove nodes from a graph such that the result 
 * will have two disjoint paths between all nodes. //NOTE: there is no pretty way to do this.
 *
 * Description of algorithm --
 *  If the graph is not a valid graph we must change something.
 *  If there are nodes with 0 or 1 edge -- they are not adequatly connect, and should be removed.
 *      remove them, until all nodes have at least two edges. Or only 2 nodes exist.
 *  Check the resulting graph to see if it is valid.
 *  If not there is at least 1 bottle-neck somewhere in the graph. 
 *  Find a bottle-neck and identify one element on each side of it. (call them points a and b)
 *  Identify all elements that could possibly make a valid graph with a and b.
 *  Check both possible graphs recursivly until a valid graph is found.
 *  Compare all graphs to see which is the largest.
 *
 *
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
        graph_destroy(map);
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
   // printf("here");
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
                    nodes = graph_node_count(map);
                    if(nodes == 2){
                        graph_disassemble(map);
                        devices = d_head;
                        ll_disassemble(al_head);
                        printf("here\n");
                        //ll_destroy(removed);
                        return removed;
                        //return devices;
                        //goto DONE;
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
        }else {
            printf("here\n");
            graph_disassemble(map);
            //ll_disassemble(removed);
            return removed;
        }
    //DONE: // TODO remove below line
    //done = true;
        
    }while(!done);
    
    devices = d_head;
    
    // if node > 3 check edge count for each node, 
        // if 1 || 0 add to remove list and remove from remaining nodes
    struct llist* best_graph = find_biggest_sub_network(map, devices);
    
    
    
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
     // clean up
    ll_disassemble(d_head);
    if ( !(d_head == best_graph )){ //make sure graphs are different
        ll_disassemble(best_graph);
    }
    ll_disassemble(ag_head);
    graph_disassemble(map);
    return removed;
        
}

/*Function finds the largest sub network within the given graph out of the given list of devices
 * @PARAM map -- graph to be search, may contain more points and edges than in the list of devices
 * @PARAM devices --list of devices that we want to find the largest possible network with.
 * @RETURN -- the largest network composed of ONLY the devices in the specified list such that all 
 * devices are either directally connected or have at least two disjointed paths between them.
 */
struct llist *find_biggest_sub_network(graph* map, struct llist* devices){
    if ( ! map || ! devices ){
        return NULL;
    }
    struct llist* d_head = devices;
    
    struct llist* TN1 = ll_clone(devices);
    struct llist* notConnected = is_valid_graph(map, TN1);// returns single element if valid 
                    //  else null or two elements that are NOT connected by 2 disjointed paths
    
    ll_disassemble(TN1);
        
    if( notConnected && notConnected->next ){ //we have two points, a and b, that
                //are not connected by two disjointed paths 
        
        struct llist* connected_to = NULL;
        struct llist* first = NULL;
        struct llist* second = NULL;
        ll_add(&first, notConnected->data);
        ll_add(&second, notConnected->next->data);
        // for both disjointed nodes identify their potential valid sub-networks.
        
        // while loop goes through and creates a list of all points for a and b 
        // such that a and b respectivly CAN get to them in 2 disjointed paths -- i.e. sub-network  
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
        
        
        
        // check to see if there is anything else that wasn't picked up -- and 
        // and generate a list of devices in this sub-network
        struct llist* diff1 = ll_diff(devices, first, compare_device);
        struct llist* difference = ll_diff(diff1, second, compare_device);
        struct llist* dif_head = difference;
        ll_disassemble(diff1);
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
                // recursivly call ourselves to ensure that the potential sub-networks are valid
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
        
    }else if ( notConnected ){ // original list of nodes is valid network. 
        ll_disassemble(notConnected);
        return devices;
    } else return NULL;
}

/* Function opens the file specified by path and searches for pcap files which specify 
 * the device has a batery level <= the given threshold
 * @PARAM path -- string representing the path of the file to open.
 * @PARAM life -- battery level threshold 
 * 
 *
 */
void check_bat_level( struct device* head, double life){
    if ( ! head || life < .0001 ){
        return;
    } 
    fprintf(stdout, "Low battery devices: \n");
    graph* lowBats = graph_create();
    while (head){
        if( (fabs(STATUS_PACKET- head->latitude)) < .1){
            //we have a battery status packet 
            if( head->battery - life <.1 ){
                // battery is less than specified level
                graph_add_node(lowBats, head, compare_device);
            }
        }
        head = head->next;
    }
    struct llist* lb_head = graph_list_nodes(lowBats);
    while (lb_head){
        fprintf(stdout, "(%d)\n", ((struct device*)lb_head->data)->device_id);
        lb_head = lb_head->next;
    }
    graph_disassemble(lowBats);
    
}

/* Function checks to see if two devices have the same device id
 * @PARAM first -- first device to check
 * @PARAM second -- second device to check
 */
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


/* Function checks to see if a graph is valid, (all nodes are either directally connected
 * or can be connected by two disjointed paths. )
 * @PARAM map -- graph structure to check
 * @PARAM devices -- list of devices in the graph to check.
 * @RETURNS: NULL if can't even try -- single element if valid and two elements that are
 *      not connected by two paths if not valid
 */
struct llist* is_valid_graph(graph* map, struct llist* devices){
    if( ! map || ! devices){
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
//        printf("(%d)\n", ((struct device*)devices->data)->device_id);
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
        
        // check to see if remaining elements are doubly connected
        while( to_check ){
            if ( ! two_paths(map, to_check->data, devices->data)){
                    // found 2 not doubbly connected return them
                ll_add(&rValue, to_check->data);
                ll_add(&rValue, devices->data);

                ll_disassemble(tc_head);
                return rValue;
            }
            to_check = to_check->next;
        }
        ll_disassemble(tc_head);
        devices = devices->next;
    }
    
    ll_add(&rValue, devices->data);
    //ll_disassemble(d_head);
    return rValue;
   
}

bool is_valid_data(struct device* test){
    if ( ! test ){
        return false;
    }
    bool rValue = test->altitude > -1500; // dead sea altitude - a little bit
    if (rValue) rValue = test->altitude < 29500; // mt everest + a bit 
    if (rValue) rValue = test->latitude > -90; // otherwise silly 
    if (rValue) rValue = test->latitude < 90; // otherwise silly
    if (rValue) rValue = test->longitude > -180; // otherwise silly
    if (rValue) rValue = test->longitude < 180; // otherwise silly
    return rValue;
}
/* Function takes a list of devices and constructs a weighted graph based upon the max
 *      connected distance specified in the file. //possible flourish make distance dynamic
 * @PARAM tree -- OCT-Tree of devices with their gps cordinates to make a graph 
 * @PARAM test -- a list of devices to place in the graph.
 * @RETURN -- a graph structure detailing the interconnected-ness of the devices.
 */

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
        printf("inserting (%d) \n", test->device_id);
//        printf("(%.6f) alt, (%.8f) lati, (%.8f) long\n", test->altitude, test->latitude, test->longitude);
   //     printTree((*tree), 0);
    //    int a;f
    //    scanf("%d", &a);
        if( ! (fabs(STATUS_PACKET- test->latitude) < 1)){
            if(is_valid_data(test) && insert(tree, test)){
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
    bool tellanyway = false;
    struct device* head = NULL;
    if (!strncmp(argv[1], "-", 1)){
        if(!strncmp(argv[1], "-p", 2)){
            
            head = decoder(argc-2, &argv[2]);
            double battery = strtod(argv[2], NULL);
            if ( battery <= 0 || battery > 100){
                fprintf(stderr, "ERROR: invalid percentage\n need 0 < number < 100");
                while (head){
                    struct device* temp = head->next;
                    free(head);
                    head = temp;
                }
                exit(0);
            }
            check_bat_level(head, battery);
            
        }else if(!strncmp(argv[1], "-l", 2) ){
            head = decoder(argc-1, &argv[1]);
            tellanyway = true;
        }
        else{
            fprintf(stdout, "ERROR: unrecognized option\n");
            exit(0);
        }
    }else{
        head = decoder(argc, argv);
    }
    struct oct_tree* tree = NULL;
    if (! head || ! head->next ){
        fprintf(stderr, "ERROR: graph should have more than one element.\n ");
        if( head ){
            free(head);
        }
        exit(0);
    }
    graph* map = test_makeGraph(head, &tree);
    struct llist* top = dijkstra(map, head, head->next);
    struct llist* t_head = top;
   
    struct llist* devices = graph_list_nodes(map);
    struct llist* check = is_valid_graph(map, devices);
    bool valid = false;
    if( check && !check->next){
        valid = true;
        printf("graph is valid\n");
    }


    if (!valid){
        
        struct llist* burn_with_fire = can_remove(map);
        struct llist* bwf_head = burn_with_fire;
        int changes = 0;
        while(burn_with_fire){
            changes ++;
            burn_with_fire = burn_with_fire->next;
        }
        burn_with_fire = bwf_head;
        int numNodes = graph_node_count(map); // 
        if ( changes <= (numNodes /2) || tellanyway){
            fprintf(stdout, " remove :\n");
            while(burn_with_fire){
            fprintf(stdout, " (%d) \n", ((struct device*)burn_with_fire->data)->device_id);
            burn_with_fire = burn_with_fire->next;
            }
        }else {
            fprintf(stdout, "Liam said to just say that too many changes are needed\n"
                    " Rerun with -l to display the answer anyway\n" );
        }
        
        printf("\n");
        ll_disassemble(bwf_head);
    }
    
    ll_disassemble(devices);
    ll_disassemble(check);
    ll_disassemble(t_head);
    oct_tree_disassemble(tree);
    graph_destroy(map);
   

    
    return 1;
}

