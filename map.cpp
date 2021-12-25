#include "map.h"
#include "globals.h"
#include "graphics.h"

/**
 * The Map structure. This holds a HashTable for all the MapItems, along with
 * values for the width and height of the Map.
 * In this file you need to define how the map will be structured. IE how will
 * you put values into the map, pull them from the map. Remember a "Map" variable
 * is a hashtable plus two ints (see below) 
 * You will have more than one map variable, one will be the main map with it's own hashtable.
 * Then you'll have a second map with another hashtable
 * You should store objects into the hashtable with different properties (spells
 * etc)
 */
struct Map {
    HashTable* items;
    int w, h;
};

/**
 * Storage area for the maps.
 * This is a global variable, but can only be access from this file because it
 * is static.
 */
static Map map[2]; // two maps exist
static int active_map; // array number

/**
 * The first step in HashTable access for the map is turning the two-dimensional
 * key information (x, y) into a one-dimensional unsigned integer.
 * This function should uniquely map (x,y) onto the space of unsigned integers.
 */
static unsigned XY_KEY(int X, int Y) {
    // TODO: Fix me!
    unsigned int w = get_active_map()->w;
    
    return X+(Y*w);
}

/**
 * This is the hash function actually passed into createHashTable. It takes an
 * unsigned key (the output of XY_KEY) and turns it into a hash value (some
 * small non-negative integer).
 */
unsigned map_hash(unsigned key)
{
    return key % get_active_map()->w; // 
    // TODO: Fix me!
}

void maps_init()
{

    HashTable* hT0 = createHashTable( map_hash, 50);
    HashTable* hT1 = createHashTable( map_hash, 30);
    Map* newmap0 = (Map*)malloc( sizeof(Map) ); // 12 bytes
    Map* newmap1 = (Map*)malloc( sizeof(Map) ); // 12 bytes
    newmap0->items = hT0; // put created hashtable
    newmap0->w = 50; //width = 50
    newmap0->h = 50; // height = 50
    
    newmap1->items = hT1;
    newmap1->w = 30; // width = 30
    newmap1->h = 30; // height = 30
    map[0] = *newmap0;
    map[1] = *newmap1;
}

Map* get_active_map()
{
    // There's only one map
    return &map[active_map]; // return active map
}

Map* set_active_map(int m)
{
    active_map = m; // setting new active map
    return &map[m];
}
Map* get_map(int m){
        return &map[m]; // get the specific map
}

void print_map()
{
    // As you add more types, you'll need to add more items to this array.
    char lookup[] = {'W', 'P', 'S', 'D', 'L', 'M'};
    for(int y = 0; y < map_height(); y++)
    {
        for (int x = 0; x < map_width(); x++)
        {
            MapItem* item = get_here(x,y);
            if (item) pc.printf("%c", lookup[item->type]);
            else pc.printf(" ");
        }
        pc.printf("\r\n");
    }
}

int map_width()
{
    return get_active_map()->w; // return active map's width
}

int map_height()
{
    return get_active_map()->h; // return active map's height
}

int map_area()
{
    return get_active_map()->w * get_active_map()->h; // return active map's area
}
MapItem* get_north(int x, int y)
{
    unsigned int key = XY_KEY(x,y-1); // north coordinate
    MapItem* north_item = (MapItem*) getItem(get_active_map()->items, key);
    if(north_item == NULL) return NULL;
    else return north_item; // return the item
}//return mapItem to compare item's type.

MapItem* get_south(int x, int y)
{
    unsigned int key = XY_KEY(x, y+1); // south coordinate
    MapItem* south_item = (MapItem*) getItem(get_active_map()->items, key);
    if(south_item == NULL) return NULL; // empty
    else return south_item; // return the item
}

MapItem* get_east(int x, int y)
{
    unsigned int key = XY_KEY(x-1, y); // east coordinate
    MapItem* east_item = (MapItem*) getItem(get_active_map()->items, key);
    if(east_item == NULL) return NULL;
    else return east_item; // return the item
}

MapItem* get_west(int x, int y)
{
    unsigned int key = XY_KEY(x+1, y); // west coordinate
    MapItem* west_item = (MapItem*) getItem(get_active_map()->items, key);
    if(west_item == NULL) return NULL;
    else return west_item; // return the item
}

MapItem* get_here(int x, int y)
{
    unsigned int key = XY_KEY(x, y); // the specific place
    MapItem* here = (MapItem*) getItem(get_active_map()->items, key);
    if(here == NULL) return NULL;
    else return here; // return the item
}
MapItem* get_there( int i, int x, int y)
{
    unsigned int key = XY_KEY(x, y); // specific map's specific's location
    MapItem* there = (MapItem*) getItem(get_map(i)->items, key);
    if(there == NULL) return NULL;
    else return there;
}

void map_erase(int x, int y)
{
    MapItem* item = get_here(x, y); 
    unsigned int key = XY_KEY(x, y); // key is unsigned integer
    // if something is already in (x,y), delete it from items.
    if(item && item->data) 
        free(item->data);
    deleteItem(get_active_map()->items, key);
}

void add_wall(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = WALL;
        w1->draw = draw_wall;
        w1->walkable = false;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        pc.printf("key : %d\n", key);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // If something is already there, free it
    }
}

void add_plant(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = PLANT;
    w1->draw = draw_plant;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_NPC(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = NPC;
    w1->draw = draw_NPC;
    w1->walkable = false;
    w1->data = (int* ) BEFORE_QUEST; // data value to know a quest process
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_stair(int x, int y) // to transition map through stair
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = STAIR;
    w1->draw = draw_stair;
    w1->walkable = false; // not walkable
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_spell_f(int x, int y) // one of spell
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = SPELL;
    w1->draw = draw_fspell;
    w1->walkable = true;
    w1->data = (int* ) FIRE;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_spell_w(int x, int y) // one of spell
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = SPELL;
    w1->draw = draw_wspell;
    w1->walkable = true;
    w1->data = (int* ) WATER;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_spell_wind(int x, int y) // one of spell
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = SPELL;
    w1->draw = draw_w2spell;
    w1->walkable = true;
    w1->data = (int* ) WIND;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_mst(int x, int y){ // monster item
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = MONSTER;
    w1->draw = draw_mst;
    w1->walkable = false;
    w1->data = (char* )"ALIVE"; // data, ALIVE and DIED
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_door(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = DOOR;
    w1->draw = draw_door;
    w1->walkable = false;
    w1->data = (int* )LOCKED; // it is changed when the player opens it with a key
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_tree(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = TREE;
    w1->draw = draw_tree;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_wood(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = WOOD;
    w1->draw = draw_wood;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
void add_leaf(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = LEAF;
    w1->draw = draw_leaf;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}