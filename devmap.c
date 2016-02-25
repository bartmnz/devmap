// slashies --outline
#include "octTree.h"
//#define CONNECT_DISTANCE = 5;

/* Function opens the file located at the specified path(s) TODO one file o multiple?
 *      and parses through looking for GPS packets. Returns a list of all valid GPS 
 *      data found in input.
 * @PARAM path -- string representing file path of input file.
 * @RETURN -- list of devices that were in input file 
 */
struct device* collect_GPS_data(const char* path ){
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
 *      the first element in each list will be examined.
 * @PARAM to_check -- first device to compare.
 * @PARAM against -- second device to compare.
 * @RETURN -- true if they are connected else false.
 */
bool is_connected(struct device* to_check, struct device* against){
    if ( ! to_check || ! against ){
        return false;
    }
    return true;
}

/* Function determines if there are two disjointed paths between the specified devices
 * @PARAM points -- linked list of devices the head is the start node and the second 
 *      element is the end point.
 * @PARAM graph -- the graph to traverse looking for the paths.
 * @RETURN -- true if two seperate paths exist between the nodes else false.
 */
bool two_disjoint_paths(struct device* points, struct graph* graph){
    if ( ! points || ! graph ){
        return false;
    }
    return true;
}

/* Function determines if it is possible to remove nodes from a graph such that the result 
 * will have two disjoint paths between all nodes. //NOTE: there is no pretty way to do this.
 * @PARAM graph -- graph to check for possibilities 
 * @RETURN -- a list of nodes to remove to satisfy the condition, NULL if not possible
 */
struct device* can_remove(struct graph* graph){
    if ( ! graph ){
        return NULL;
    }
    struct device* compile = NULL;
    return compile;
}

/* Function opens the file specified by path and searches for pcap files which specify 
 * the device has a batery level <= the given threshold
 * @PARAM path -- string representing the path of the file to open.
 * @PARAM life -- battery level threshold 
 * @RETURN -- a list of devices such that (device battery level <= life)
 *
 */
struct device* check_bat_level(const char* path, double life){
    if ( ! path || life < .0001 ){
        return NULL;
    } 
    struct device* compile = NULL;
    return compile;
}

/* Main -- you know what this does. If not: please close this before you break something!
 * @AUTHOR -- BARTMNZ 
 * @VERSION -- beta
 */
int main(int argc, char* argv[]){
    if ( argc < 2 || ! argv ){
        return 0;
    }
    return 1;
}
