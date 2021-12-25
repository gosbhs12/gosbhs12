// Project includes
#include "globals.h"
#include "hardware.h"
#include "map.h"
#include "graphics.h"
#include "speech.h"
#include "hash_table.h"
// Functions in this file
int get_action (GameInputs inputs);
int update_game (int action);
void draw_game (int init);
void init_main_map();
int main ();
int action_but();
MapItem* check_item(); // check four side of the player
void start_game(); // start game page
void pause_game(); // to pause this game
/**
 * The main game state. Must include Player locations and previous locations for
 * drawing to work properly. Other items can be added as needed.
 */
static int monster_die = 0; // init monster is alive for quest
static int game_start = 0; // when game_start is 1, game is started.
static int running = 0; // when the player is running, running is 1
struct {
    int x,y;    // Current locations
    int px, py; // Previous locations
    int has_key; // To open door
    int omni; // player's option omni mode
    int hp; // player's stats for HP
    // You can add other properties for the player here
} Player;
char* start_quest = "Hi, Stranger. I have a favor for you. There is a monster upstair. Many people had got a pain from that for a long time. Could you please defeat it for me?";
char* on_quest = "Use stair to go up. Defeat the monster!";
char* stair1 = "This is the way to go to dungeon. Use spells to defeat monster.";
char* stair2 = "You want to go down.";
char* cast_spell_fire = "You cast fire spell. The Monster got damaged.";
char* cast_spell_water = "You cast water spell. ";
char* cast_spell_wind = "You cast wind spell.";
char* monster_ = "The monster fell down. Talk to the NPC.";
char* giving_key  = "WOW, you defeated the monster! I knew you can do it. This is the key to get out of here. GO AWAY!";
/**
 * Given the game inputs, determine what kind of update needs to happen.
 * Possbile return values are defined below.
 */
int spell_count =0;
#define NO_ACTION 0
#define ACTION_BUTTON 1
#define MENU_BUTTON 2
#define GO_LEFT 3
#define GO_RIGHT 4
#define GO_UP 5
#define GO_DOWN 6
#define OMNI_BUTTON 7
void start_game(){
    GameInputs in;
    while(1){
        in = read_inputs(); // read inputs
        wait_ms(50);
        if(in.b1 == 0){ // if the button1 is detected, game is started
            game_start = 1;
            break;            
        }
        else if(in.b2 == 0){ // if the button2 is detected, player quits game
            uLCD.cls(); // clear display
            uLCD.locate(1,3); //locate text 
            uLCD.textbackground_color(BLACK); // black background
            uLCD.color(RED); //white
            uLCD.printf("Game Over");
            uLCD.locate(1,4); // locate text below the Gameover text
            uLCD.textbackground_color(BLACK);
            uLCD.color(WHITE); //white
            uLCD.printf("Restart device to play game.");
            wait(1000); 
        }
    }
}
int get_action(GameInputs inputs)
{
    if( inputs.ax > 0.7 || inputs.ax < -0.7 || inputs.ay > 0.7 || inputs.ay < -0.7 ) running =1; // the steeper tilt accelerometer identified, the faster player run.
    else if( inputs.ax <= 0.7 || inputs.ax >= -0.7 || inputs.ay <= 0.7 || inputs.ay >= -0.7 ) running =0; // running mode off.
    if( inputs.ax > 0.3 ) return  GO_RIGHT; 
    else if( inputs.ax < -0.3) return GO_LEFT;
    else if( inputs.ay > 0.3 ) return GO_UP;
    else if( inputs.ay <-0.3 ) return GO_DOWN;
    else if( inputs.b1 == 0) return ACTION_BUTTON;
    else if( inputs.b2 == 0) return OMNI_BUTTON;
    else if( inputs.b3 == 0) return MENU_BUTTON;
    else return NO_ACTION;
}

/**
 * Update the game state based on the user action. For example, if the user
 * requests GO_UP, then this function should determine if that is possible by
 * consulting the map, and update the Player position accordingly.
 * 
 * Return values are defined below. FULL_DRAW indicates that for this frame,
 * draw_game should not optimize drawing and should draw every tile, even if
 * the player has not moved.
 */
#define NO_RESULT 0
#define GAME_OVER 3
#define FULL_DRAW 2
int update_game(int action)
{
    // Save player previous location before updating
    Player.px = Player.x;
    Player.py = Player.y;
    MapItem* temp;
    MapItem* temp2;
    // Do different things based on the each action.
    // You can define functions like "go_up()" that get called for each case.
    switch(action) // int 0 3 4 5
    {
        case GO_UP:
        temp = get_north(Player.x, Player.y); // get north item
        temp2 = get_north(Player.x, Player.y-1); // get the second north item 
        if(temp->type == MONSTER){ // if the item is monster, 
             Player.hp -=1; //  lose player's hp
             wait(1);   
             if(Player.hp == 0) return GAME_OVER; // when the player's hp is low, Game over
        }
        if(Player.omni){ // omni mode ON
            if(running && temp2->walkable) Player.y -= 1;    // Player is running and the second north item is walkable, then move two large pixels
            Player.y -= 1; // one more large pixel
            return FULL_DRAW; 
            }else if(Player.omni == 0){ // not omni mode
                if(temp->walkable) { // Character can go north
                    if(running && temp2->walkable) Player.y -=1; // Player is running and the second north item is walkable, then move two large pixels
                    Player.y -= 1; 
                    return FULL_DRAW;
                }
            }
        break;
        case GO_LEFT:
        temp = get_east(Player.x, Player.y); // get east item
        temp2 = get_east(Player.x-1, Player.y); // get two space east item
        if(temp->type == MONSTER){ // when player is toward monster 
             Player.hp -=1; // hp reduced
             wait(1);
             if(Player.hp == 0) return GAME_OVER;
        }
        if(Player.omni){ // omni ON
            if(running && temp2->walkable) Player.x -=1; // Player is running and the second west item is walkable, then move two large pixels
            Player.x -= 1; // move one more, so move total two space 
            return FULL_DRAW; // return draw
            }else if(Player.omni == 0){ // omni off
                
                if(temp->walkable) { // Character can go left
                    if(running && temp2->walkable) Player.x -=1; // Player is running and the second west item is walkable, then move two large pixels
                    Player.x -= 1; // move one more, so move total two space
                    return FULL_DRAW;
                }
            }
                
        break;            
        case GO_DOWN:   
        temp = get_south(Player.x, Player.y);
        temp2 = get_south(Player.x, Player.y+1);
        if(temp->type == MONSTER){ // if the player is toward monster 
             Player.hp -=1; // reduced hp
             wait(1);
             if(Player.hp == 0) return GAME_OVER;
        }
        if(Player.omni){ // omni ON
            if(running && temp2->walkable) Player.y +=1; // Player is running and the second south item is walkable, then move two large pixels
            Player.y += 1; // move one more step
            return FULL_DRAW;
            }else if(Player.omni == 0){ // omni off
                if(temp->walkable) { // Character can go down
                    if(running && temp2->walkable) Player.y +=1; // Player is running and the second south item is walkable, then move two large pixels
                    Player.y += 1; // move one more step
                    return FULL_DRAW;
                }
            }
        break;
        case GO_RIGHT:  
        temp = get_west(Player.x, Player.y);
        temp2 = get_west(Player.x+1, Player.y);
        if(temp->type == MONSTER){
             Player.hp -=1;
             wait(1);
             if(Player.hp == 0) return GAME_OVER;
        }
        if(Player.omni){ // OMNI ON
            if(running && temp2->walkable) Player.x +=1; // Player is running and the second east item is walkable, then move two large pixels
            Player.x += 1; // move one more step
            return FULL_DRAW;
            }else if(Player.omni == 0){ // omni off
                
                if(temp->walkable) { // Character can go right
                    if(running && temp2->walkable) Player.x +=1; // Player is running and the second east item is walkable, then move two large pixels
                    Player.x += 1; // move one more step
                    return FULL_DRAW;
                }
            }
        
        break;
        case ACTION_BUTTON: 
        return action_but();
        // if there is something near player, get into text box
        case MENU_BUTTON: 
            pause_game(); // pause this game
            break;
        case OMNI_BUTTON: 
            Player.omni = !Player.omni ; // all wall set to walkable or walkable doesnt matter
            return FULL_DRAW; 
        
        default:        
        
        break;
    }
    return NO_RESULT;
}
MapItem* check_item(){
    MapItem* i1 = get_north(Player.x, Player.y); // get north item
    MapItem* i2 = get_east(Player.x, Player.y); // get east item
    MapItem* i3 = get_south(Player.x, Player.y); // get south item
    MapItem* i4 = get_west(Player.x, Player.y); // get west item
    if(i1->type == NPC ||i1->type == DOOR ||i1->type == STAIR ||i1->type == SPELL||i1->type == MONSTER){
        return i1; // if there is something on north, output north
    }else if(i2->type == NPC ||i2->type == DOOR ||i2->type == STAIR ||i2->type == SPELL||i2->type == MONSTER){
        return i2; // if there is something on east, output east
    }else if(i3->type == NPC ||i3->type == DOOR ||i3->type == STAIR ||i3->type == SPELL||i3->type == MONSTER){
        return i3; // if there is something on south, output south
    }else if(i4->type == NPC ||i4->type == DOOR ||i4->type == STAIR ||i4->type == SPELL||i4->type == MONSTER){
        return i4; // if there is something on west, output west
    }
    return NULL;
}
void pause_game(){ // if menu button is pressed, pause this game
    GameInputs in2 =read_inputs();
    bool pause = true;
    while( pause ){ // waiting for pause changing to 0
        in2 = read_inputs();
        wait_ms(200); // waiting delay for input error
        if(in2.b3 == 0){ // if menu button is pressed again, continue this game
            pause = false; // cancel pause
        }
        wait_ms(50);
    }
    return ;
}
int action_but()
{    
    MapItem* it = check_item(); // check item on four side (N, E, S, W)
    MapItem* me = get_here(Player.x, Player.y); // get player's place's item
    MapItem* monst = get_there(1, 25, 27); // monster
    if(it->type == NPC){ // Near NPC
            if(it->data == (int*) BEFORE_QUEST){ // the beginning of the quest
                long_speech(start_quest); // print long speech
                it->data = (int*) Q_STEP1; // Doing on quest
                return 2; // FULL_DRAW
            }else if( it->data == (int*) Q_STEP1 && monster_die ){ // monster is slained
                long_speech(giving_key); 
                Player.has_key = 1 ; // player gets a key
                it->data = (int*) AFTER_QUEST; // quest is finished
                return 2;
            }else{
                long_speech(on_quest);
                return 2;
            }            
    }
    if(it->type == STAIR){ // Near Stair
           if( get_active_map() == get_map(0) ){ // when player is on map[0]
           long_speech(stair1); // when the player going up to map[1]
           set_active_map(1);
           }else if( get_active_map() == get_map(1) ){ // when player is on map[1]
               long_speech(stair2); //when the player going down to map[0]
               set_active_map(0);
           }
           //check if the door is locked or not, then check if player has a key or not,
           // if has a key, open the door 
           return 2;
    }
    
    if(me->type == SPELL){ // Near spell
        spell_count++; // used one spell
        if(me->data == (int*)FIRE){ // if the spell is fire
            long_speech(cast_spell_fire);
            if(spell_count == 2){
                monster_die = 1;
                //monst->data = (char*)"DIE";  //monster's data updated
                monst->draw = draw_die;   //changed image
            }
        }else if(me->data == (int*)WATER){ // if the spell is water
            long_speech(cast_spell_water);
            if(spell_count == 2){ // when the two spell are used, monster is killed
                monster_die = 1;
                monst->draw = draw_die;
            }
        }else if(me->data == (int*)WIND){ // if the spell is wind
            long_speech(cast_spell_wind);
            if(spell_count == 2){
                monster_die =1;
                monst->draw = draw_die;
            }
        }
        return 2;
    }
    if(it->type == DOOR){ // Near door
        if(Player.has_key == 1){ // check if Player has a key
            it->data = (int*)UNLOCKED; // update door unlocked
            return GAME_OVER; // the game is over, player wins
        }
    }
    return 0;
}
/**
 * Entry point for frame drawing. This should be called once per iteration of
 * the game loop. This draws all tiles on the screen, followed by the status 
 * bars. Unless init is nonzero, this function will optimize drawing by only 
 * drawing tiles that have changed from the previous frame.
 */

void draw_game(int init)
{
    if( game_start == 0){ // when the game started at first
        uLCD.cls(); // clear display
        uLCD.locate(1,2); // locate x= 1 y = 2
        uLCD.textbackground_color(BLACK);
        uLCD.color(WHITE); //white
        uLCD.printf("Welcome to the Game!");
        uLCD.locate(1,4); // locate  x=1 y =4
        uLCD.color(BLUE);
        uLCD.printf("1. Start game");
        uLCD.locate(1,5); // locate x = 1 y=5
        uLCD.color(BLUE);
        uLCD.printf("2. Quit");
        start_game(); // waiting the player's input
    }
        
    if(init == 3){// GAME_OVER
        if(Player.hp == 0){ // GAME OVER with no HP
            uLCD.cls(); // when player lose
            uLCD.locate(1,3);
            uLCD.textbackground_color(BLACK);
            uLCD.color(RED); // red
            uLCD.printf("You lost.");
            uLCD.locate(1,4); // locate text to (1,4)
            uLCD.color(WHITE); // WHITE
            uLCD.printf("THE END!");
            wait(1000); 
        }
        //GAME_OVER when player won.
        uLCD.cls();
        uLCD.locate(1,3);
        uLCD.textbackground_color(BLACK);
        uLCD.color(WHITE); //white
        uLCD.printf("You won!");
        uLCD.locate(1,4);
        uLCD.color(RED);
        uLCD.printf("THE END!");
        uLCD.locate(1,5);
        uLCD.color(BLUE);
        uLCD.printf("Thank you! ");
        wait(1000);
    }
    // Draw game border first
    if(init) draw_border();
    
    // Iterate over all visible map tiles
    for (int i = -5; i <= 5; i++) // Iterate over columns of tiles
    {
        for (int j = -4; j <= 4; j++) // Iterate over one column of tiles
        {
            // Here, we have a given (i,j)
            
            // Compute the current map (x,y) of this tile
            int x = i + Player.x;
            int y = j + Player.y;
            
            // Compute the previous map (px, py) of this tile
            int px = i + Player.px;
            int py = j + Player.py;
                        
            // Compute u,v coordinates for drawing
            int u = (i+5)*11 + 3;
            int v = (j+4)*11 + 15;
            
            MapItem* here = get_here(x,y);
            // Figure out what to draw
            DrawFunc draw = NULL;
            if (init && i == 0 && j == 0) // Only draw the player on init
            {
                if(here->type == LEAF){ // if the player is on leaf or wood item
                    draw_leaf(u,v); // if the player down to LEAF item, draw leaf   
                }else if(here->type == WOOD){ draw_wood(u,v);    // if the player down to WOOD item, draw wood            
                }else draw_player(u, v, Player.has_key, running); // else  draw player
                continue;
            }
            else if (x >= 0 && y >= 0 && x < map_width() && y < map_height()) // Current (i,j) in the map
            {
                MapItem* curr_item = get_here(x, y);
                MapItem* prev_item = get_here(px, py);
                if (init || curr_item != prev_item) // Only draw if they're different
                {
                    if (curr_item) // There's something here! Draw it
                    {
                        draw = curr_item->draw;
                    }
                    else // There used to be something, but now there isn't
                    {
                        draw = draw_nothing;
                    }
                }
            }
            else if (init) // If doing a full draw, but we're out of bounds, draw the walls.
            {
                draw = draw_wall;
            }

            // Actually draw the tile
            if (draw) draw(u, v);
        }
    }

    // Draw status bars    
    draw_upper_status(Player.x, Player.y); // draw player's coordinate in upper bar
    draw_lower_status(Player.hp); // draw hp status in lower bar
}


/**
 * Initialize the main world map. Add walls around the edges, interior chambers,
 * and plants in the background so you can see motion. Note: using the similar
 * procedure you can init the secondary map(s).
 */
void init_main_map()
{
    // "Random" plants
    Map* map = set_active_map(0); // set active map to first
    for(int i = map_width() + 3; i < map_area(); i += 43)
    {
        add_plant(i % map_width(), i / map_width());
    }
    add_stair(27, 17);  // add stair item
    add_wall(0,              0,              HORIZONTAL, map_width()); // add border walls
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    add_NPC(30, 20); // add NPC in first map
    add_tree(25,22); // add bottom of tree
    add_wood(25,21); // add mid of tree
    add_leaf(25,20); // add top of tree
    print_map(); 
    
    Map* map2 = set_active_map(1); // set active map to second
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    add_wall(1, 7, HORIZONTAL, 20 );// wall 
    add_wall(1, 20, HORIZONTAL, 22 );
    add_wall(14, 7, VERTICAL, 17);
    add_wall(23, 7, VERTICAL, 17 );
    add_stair(27, 17); // add stair item to connect to the first map
    add_door(10, 20); // add door item for ending this game
    add_spell_f( 17, 15); // add fire spell
    add_spell_w( 11, 15); // add water spell
    add_spell_wind( 20, 16); // add wind spell
    add_mst(25, 27); // add monster 
    print_map();
}

/**
 * Program entry point! This is where it all begins.
 * This function orchestrates all the parts of the game. Most of your
 * implementation should be elsewhere - this holds the game loop, and should
 * read like a road map for the rest of the code.
 */
int main()
{
    // First things first: initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");
    
    // Initialize the maps
    maps_init(); // done
    init_main_map(); 
    
    // Initialize game state
    set_active_map(0);
    Player.x = Player.y = 20; // player's coordinate
    Player.has_key =0; // player has no key at first
    Player.omni =0; // init omni value
    Player.hp = 10; // init Player's hp
    // Initial drawing
    draw_game(true);
    
    // Main game loop
    while(1)
    {
        // Timer to measure game update speed
        Timer t; t.start();
        
        int action = get_action( read_inputs() );     // 2. Determine action (get_action)
        int update = update_game(action);       // 3. Update game (update_game)
        if(update == GAME_OVER) {               // 3b. Check for game over
            draw_game(GAME_OVER);
            break;
        }
         // pc.printf("%s\n", get_there(1,25,27)->data);
        draw_game(true);                        // 4. Draw frame (draw_game)
        
        // 5. Frame delay
        t.stop();
        int dt = t.read_ms();
        if (dt < 100) wait_ms(100 - dt);
    }
}
