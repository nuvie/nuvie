// temp file to make cvs compile.
// FIX! Joseph you might want to revert to your version of this file.

static const U6ObjectType U6ObjectTypes[] = {

{ OBJ_U6_EGG,0,0,USE_EVENT_USE/*|USE_EVENT_LOAD*/,&U6UseCode::use_egg, 0 },
 
{ OBJ_U6_OAKEN_DOOR,   0xFF,0,USE_EVENT_USE,&U6UseCode::use_door, 0 },
{ OBJ_U6_WINDOWED_DOOR,0xFF,0,USE_EVENT_USE,&U6UseCode::use_door, 0 },
{ OBJ_U6_CEDAR_DOOR,   0xFF,0,USE_EVENT_USE,&U6UseCode::use_door, 0 },
{ OBJ_U6_STEEL_DOOR,   0xFF,0,USE_EVENT_USE,&U6UseCode::use_door, 0 },
{ OBJ_U6_KEY,          0xFF,0,USE_EVENT_USE,&U6UseCode::use_key, 0 },

{ OBJ_U6_SIGN,      0xFF,0,USE_EVENT_LOOK,&U6UseCode::look_sign, 0 },
{ OBJ_U6_BOOK,      0xFF,0,USE_EVENT_LOOK,&U6UseCode::look_sign, 0 },
{ OBJ_U6_SCROLL,    0xFF,0,USE_EVENT_LOOK,&U6UseCode::look_sign, 0 },
{ OBJ_U6_PICTURE,   0xFF,0,USE_EVENT_LOOK,&U6UseCode::look_sign, 0 },
{ OBJ_U6_SIGN_ARROW,0xFF,0,USE_EVENT_LOOK,&U6UseCode::look_sign, 0 },
{ OBJ_U6_TOMBSTONE, 0xFF,0,USE_EVENT_LOOK,&U6UseCode::look_sign, 0 },
{ OBJ_U6_CROSS,     0xFF,0,USE_EVENT_LOOK,&U6UseCode::look_sign, 0 },
{ OBJ_U6_CODEX,       0,0,USE_EVENT_LOOK,&U6UseCode::look_sign, 0 },

{ OBJ_U6_CRATE,        0,0,USE_EVENT_SEARCH,&U6UseCode::use_container, 0 },
{ OBJ_U6_CRATE,      0xFF,0,USE_EVENT_USE,   &U6UseCode::use_container, 0 },
{ OBJ_U6_BARREL,       0,0,USE_EVENT_SEARCH,&U6UseCode::use_container, 0 },
{ OBJ_U6_BARREL,     0xFF,0,USE_EVENT_USE,   &U6UseCode::use_container, 0 },
{ OBJ_U6_CHEST,        0,0,USE_EVENT_SEARCH,&U6UseCode::use_container, 0 },
{ OBJ_U6_CHEST,      0xFF,0,USE_EVENT_USE,   &U6UseCode::use_container, 0 },
{ OBJ_U6_SECRET_DOOR,0xFF,0,USE_EVENT_USE|USE_EVENT_SEARCH,&U6UseCode::use_secret_door, 0 },
{ OBJ_U6_BAG,        0xFF,0,USE_EVENT_SEARCH,&U6UseCode::use_container, 0 },
{ OBJ_U6_DRAWER,     0xFF,0,USE_EVENT_SEARCH,&U6UseCode::use_container, 0 },
{ OBJ_U6_STONE_LION,   1,0,USE_EVENT_SEARCH,&U6UseCode::use_container, 0 },
{ OBJ_U6_PLANT,        0,0,USE_EVENT_SEARCH,&U6UseCode::use_container, 0 },
{ OBJ_U6_GRAVE,        0,0,USE_EVENT_SEARCH,&U6UseCode::use_container, 0 },
{ OBJ_U6_DEAD_ANIMAL,0xFF,0,USE_EVENT_SEARCH,&U6UseCode::use_container, 0 },
{ OBJ_U6_DEAD_BODY,  0xFF,0,USE_EVENT_SEARCH,&U6UseCode::use_container, 0 },
{ OBJ_U6_DEAD_CYCLOPS, 0,0,USE_EVENT_SEARCH,&U6UseCode::use_container, 0 },
{ OBJ_U6_DEAD_GARGOYLE,0,0,USE_EVENT_SEARCH,&U6UseCode::use_container, 0 },
{ OBJ_U6_REMAINS,    0xFF,0,USE_EVENT_SEARCH,&U6UseCode::use_container, 0 },

{ OBJ_U6_V_PASSTHROUGH,0xFF,0,USE_EVENT_USE,&U6UseCode::use_passthrough, 0 },
{ OBJ_U6_H_PASSTHROUGH,0xFF,0,USE_EVENT_USE,&U6UseCode::use_passthrough, 0 },

{ OBJ_U6_LEVER, 0xFF,0,USE_EVENT_USE,&U6UseCode::use_switch, 0 },
{ OBJ_U6_SWITCH,0xFF,0,USE_EVENT_USE,&U6UseCode::use_switch, 0 },

{ OBJ_U6_CHURN, 0xFF,0,USE_EVENT_USE,&U6UseCode::use_churn, 0 },

{ OBJ_U6_CRANK, 0xFF,0,USE_EVENT_USE,&U6UseCode::use_crank, 0 },

{ OBJ_U6_FIREPLACE, 0xFF,0,USE_EVENT_USE,&U6UseCode::use_firedevice, 0 },
{ OBJ_U6_CANDLE,    0xFF,0,USE_EVENT_USE,&U6UseCode::use_firedevice, 0 },
{ OBJ_U6_CANDELABRA,0xFF,0,USE_EVENT_USE,&U6UseCode::use_firedevice, 0 },
{ OBJ_U6_BRAZIER,   0,0,USE_EVENT_USE,&U6UseCode::use_firedevice, 0 },
{ OBJ_U6_BRAZIER,   1,0,USE_EVENT_USE,&U6UseCode::use_firedevice, 0 },

{ OBJ_U6_ORB_OF_THE_MOONS,0xFF,0,USE_EVENT_USE,&U6UseCode::use_orb, 0 },
{ OBJ_U6_RED_GATE,  1,0,USE_EVENT_PASS,&U6UseCode::enter_red_moongate, 0 }, //FIX we only want to go through frame_n 1
{ OBJ_U6_LADDER,0xFF,0,USE_EVENT_USE,&U6UseCode::use_ladder, 0 },
{ OBJ_U6_CAVE,  0xFF,0,USE_EVENT_PASS,&U6UseCode::enter_dungeon, 0 },
{ OBJ_U6_HOLE,  0xFF,0,USE_EVENT_PASS,&U6UseCode::enter_dungeon, 0 },

{ OBJ_U6_CLOCK,     0xFF,0,USE_EVENT_LOOK,&U6UseCode::look_clock, 0 },
{ OBJ_U6_SUNDIAL,   0xFF,0,USE_EVENT_LOOK,&U6UseCode::look_clock, 0 },
{ OBJ_U6_MIRROR,    0xFF,0,USE_EVENT_LOOK,&U6UseCode::look_mirror, 0 },
{ OBJ_U6_WELL,      0xFF,0,USE_EVENT_USE,&U6UseCode::use_well, 0 },
{ OBJ_U6_POWDER_KEG,0xFF,0,USE_EVENT_USE|USE_EVENT_TIMED,&U6UseCode::use_powder_keg, 0 },
    
{ OBJ_U6_BEEHIVE,   0xFF,0,USE_EVENT_USE,&U6UseCode::use_beehive, 0 },
	
{ OBJ_U6_POTION,    0xFF,0,USE_EVENT_USE,&U6UseCode::use_potion, 0 },
{ OBJ_U6_BUTTER,      0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_WINE,        0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_MEAD,        0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_ALE,         0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_BREAD,       0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_MEAT_PORTION,0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_ROLLS,       0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_CAKE,        0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_CHEESE,      0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_RIBS,        0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_MEAT,        0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_GRAPES,      0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_HAM,         0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_GARLIC,      0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_SNAKE_VENOM, 0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_HORSE_CHOPS, 0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_JAR_OF_HONEY,0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },
{ OBJ_U6_DRAGON_EGG,  0,0,USE_EVENT_USE,&U6UseCode::use_food, 0 },

{ OBJ_U6_COW, 0xFF, 0, USE_EVENT_USE,&U6UseCode::use_cow, 0 },
{ OBJ_U6_HORSE, 0xFF,0,USE_EVENT_USE,&U6UseCode::use_horse, 0 },
{ OBJ_U6_HORSE_WITH_RIDER, 0xFF,0,USE_EVENT_USE,&U6UseCode::use_horse, 0 },
    
{ OBJ_U6_SHIP, 0xFF,0,USE_EVENT_USE,&U6UseCode::use_boat, 0 },
{ OBJ_U6_SKIFF,0xFF,0,USE_EVENT_USE,&U6UseCode::use_boat, 0 },
{ OBJ_U6_RAFT,   0,0,USE_EVENT_USE,&U6UseCode::use_boat, 0 },
//    add_usecode(OBJ_U6_BALLOON_BASKET,0,0,USE_EVENT_USE,&U6UseCode::use_balloon);
    
{ OBJ_U6_QUEST_GATE,  0,0,USE_EVENT_PASS,&U6UseCode::pass_quest_barrier, 0 },

{ OBJ_U6_VORTEX_CUBE, 0,0,USE_EVENT_USE,&U6UseCode::use_vortex_cube, 0 },
{ OBJ_U6_PULL_CHAIN,  0,0,USE_EVENT_USE,&U6UseCode::use_bell, 0 },
{ OBJ_U6_BELL,      0xFF,0,USE_EVENT_USE,&U6UseCode::use_bell, 0 },
{ OBJ_U6_SHOVEL,      0,0,USE_EVENT_USE,&U6UseCode::use_shovel, 0 },
{ OBJ_U6_FOUNTAIN,    0,0,USE_EVENT_USE,&U6UseCode::use_fountain, 0 },
{ OBJ_U6_RUBBER_DUCKY,0,0,USE_EVENT_USE,&U6UseCode::use_rubber_ducky, 0 },
{ OBJ_U6_CANNON,    0xFF,0,USE_EVENT_USE|USE_EVENT_MOVE,&U6UseCode::use_cannon, 0 },

{ OBJ_U6_PANPIPES,   0,0,USE_EVENT_USE,&U6UseCode::play_instrument, 0 },
{ OBJ_U6_HARPSICHORD,0,0,USE_EVENT_USE,&U6UseCode::play_instrument, 0 },
{ OBJ_U6_HARP,       0,0,USE_EVENT_USE,&U6UseCode::play_instrument, 0 },
{ OBJ_U6_LUTE,       0,0,USE_EVENT_USE,&U6UseCode::play_instrument, 0 },
{ OBJ_U6_XYLOPHONE,  0,0,USE_EVENT_USE,&U6UseCode::play_instrument, 0 },
{ OBJ_U6_NOTHING, 0xFF, 0, 0, NULL, 0}};

