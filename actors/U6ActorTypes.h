
/*
 *  U6ActorTypes.h
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Oct 01 2003.
 *  Copyright (c) Nuvie Team 2003. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */
 
 /*
base_obj_n
frames_per_direction
tiles_per_direction
tiles_per_frame
tile_start_offset
dead_obj_n
dead_frame_n
can_laydown
can_sit
tile_type
movetype
twitch_rand
str
dex
intelligence
body_armor_class
attack_hands
hp
alignment
*/

const U6ActorType u6ActorTypes[] = {
// 4x1 tile actors                                                                                            STR, DEX, INT,  AC, DMG,  HP, ALIGNMENT
 {OBJ_U6_INSECTS,          0, 0, 1, 0, OBJ_U6_NOTHING, 0, false,false, ACTOR_ST, MOVETYPE_U6_AIR_LOW,     20,   1,  40,   1,   0,   1,   1, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_GIANT_SQUID,      0, 0, 1, 0, OBJ_U6_NOTHING, 0, false, false, ACTOR_ST, MOVETYPE_U6_WATER_HIGH, 50,  24,  20,   8,   0,  20,  50, ACTOR_ALIGNMENT_EVIL},
 {OBJ_U6_GHOST,            0, 0, 1, 0, OBJ_U6_NOTHING, 0, false, false, ACTOR_ST, MOVETYPE_U6_LAND,       45,   1,  10,   9,   0,  12,  20, ACTOR_ALIGNMENT_EVIL},
 {OBJ_U6_ACID_SLUG,        0, 0, 1, 0, OBJ_U6_NOTHING, 0, false, false, ACTOR_ST, MOVETYPE_U6_LAND,       50,   5,   5,   2,   0,   1,  10, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_WISP,             0, 0, 1, 0, OBJ_U6_NOTHING, 0, false, false, ACTOR_ST, MOVETYPE_U6_AIR_LOW,    50,   1,  40,  20,   0,   1,  40, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_GIANT_BAT,        0, 0, 1, 0, OBJ_U6_BLOOD, 0, false, false, ACTOR_ST, MOVETYPE_U6_AIR_LOW,      30,   5,  25,   4,   0,   6,   5, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_REAPER,           0, 0, 1, 0, OBJ_U6_BLOOD, 0, false, false, ACTOR_ST, MOVETYPE_U6_LAND,         40,  21,  20,  16,   4,  20,  30, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_GREMLIN,          0, 0, 1, 0, OBJ_U6_BLOOD, 0, false, false, ACTOR_ST, MOVETYPE_U6_LAND,         20,   2,  26,   7,   0,   1,   5, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_GAZER,            0, 0, 1, 0, OBJ_U6_BLOOD, 0, false, false, ACTOR_ST, MOVETYPE_U6_LAND,         30,   2,  10,  21,   0,   1,  20, ACTOR_ALIGNMENT_EVIL},
 {OBJ_U6_BIRD,             0, 0, 1, 0, OBJ_U6_BLOOD, 0, false, false, ACTOR_ST, MOVETYPE_U6_AIR_LOW,      20,   1,  28,   4,   0,   1,   3, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_CORPSER,          0, 0, 1, 0, OBJ_U6_BLOOD, 0, false, false, ACTOR_ST, MOVETYPE_U6_LAND,         50,  17,   6,   3,   0,  15,  40, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_RABBIT,           0, 0, 1, 0, OBJ_U6_BLOOD, 0, false, false, ACTOR_ST, MOVETYPE_U6_LAND,         25,   3,   9,   3,   0,   2,   5, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_ROT_WORMS,        0, 0, 1, 0, OBJ_U6_BLOOD, 0, false, false, ACTOR_ST, MOVETYPE_U6_LAND,         50,   3,  17,   2,   0,   2,   5, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_HYDRA,            0, 0, 1, 0, OBJ_U6_BLOOD, 0, false, false, ACTOR_MT, MOVETYPE_U6_LAND,          5,  26,  11,   2,   0,  15,  50, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_MOUSE,            1, 1, 1, 0, OBJ_U6_MOUSE, 0, false, false, ACTOR_ST, MOVETYPE_U6_LAND,          0,   2,  25,   3,   0,   2,   5, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_CAT,              1, 1, 1, 0, OBJ_U6_BLOOD, 0, false, false, ACTOR_ST, MOVETYPE_U6_LAND,         40,   4,  18,   8,   0,   2,   6, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_TANGLE_VINE,      0, 0, 1, 0, OBJ_U6_TANGLE_VINE, 0, false, false, ACTOR_ST, MOVETYPE_U6_LAND,    0,  20,   5,   1,   4,   1,  30, ACTOR_ALIGNMENT_CHAOTIC},

 // 4x2
 {OBJ_U6_SEA_SERPENT,      2, 2, 1, 0, OBJ_U6_NOTHING, 0, false,false, ACTOR_ST, MOVETYPE_U6_WATER_HIGH,  60,  17,  17,   9,   2,  30,  70, ACTOR_ALIGNMENT_EVIL},
 {OBJ_U6_GIANT_RAT,        2, 2, 1, 0, OBJ_U6_BLOOD, 0, false,false, ACTOR_ST, MOVETYPE_U6_LAND,          40,   5,  13,   4,   0,   6,   8, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_SHEEP,            2, 2, 1, 0, OBJ_U6_BLOOD, 0, false,false, ACTOR_ST, MOVETYPE_U6_LAND,          35,   5,  10,   4,   0,   2,   6, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_DOG,              2, 2, 1, 0, OBJ_U6_BLOOD, 0, false,false, ACTOR_ST, MOVETYPE_U6_LAND,          35,   5,  16,   8,   0,   3,   6, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_DEER,             2, 2, 1, 0, OBJ_U6_BLOOD, 0, false,false, ACTOR_ST, MOVETYPE_U6_LAND,          20,  11,  18,   5,   0,   2,   8, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_WOLF,             2, 2, 1, 0, OBJ_U6_BLOOD, 0, false,false, ACTOR_ST, MOVETYPE_U6_LAND,          20,   8,  16,   8,   0,   4,   8, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_SNAKE,            2, 2, 1, 0, OBJ_U6_BLOOD, 0, false,false, ACTOR_ST, MOVETYPE_U6_LAND,          20,   3,   7,   6,   1,   2,   5, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_GIANT_SPIDER,     2, 2, 1, 0, OBJ_U6_BLOOD, 0, false,false, ACTOR_ST, MOVETYPE_U6_LAND,          20,   9,  11,   4,   0,   8,  10, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_DRAKE,            2, 2, 1, 0, OBJ_U6_DRAKE, 0, false,false, ACTOR_ST, MOVETYPE_U6_AIR_LOW,       15,  22,  22,  13,   4,  10,  50, ACTOR_ALIGNMENT_EVIL},
 {OBJ_U6_MONGBAT,          2, 2, 1, 0, OBJ_U6_BLOOD, 0, false,false, ACTOR_ST, MOVETYPE_U6_LAND,          15,  20,  27,  13,   4,  20,  30, ACTOR_ALIGNMENT_EVIL},
 {OBJ_U6_DAEMON,           2, 2, 1, 0, OBJ_U6_DEAD_BODY, 0, true,false, ACTOR_ST, MOVETYPE_U6_LAND,       30,  35,  26,  31,  10,  20, 100, ACTOR_ALIGNMENT_EVIL},
 {OBJ_U6_SKELETON,         2, 2, 1, 0, OBJ_U6_DEAD_BODY, 9, true,false, ACTOR_ST, MOVETYPE_U6_LAND,       30,  10,  14,   7,   0,   6,  20, ACTOR_ALIGNMENT_EVIL},
 {OBJ_U6_HEADLESS,         2, 2, 1, 0, OBJ_U6_DEAD_BODY, 1, true,false, ACTOR_ST, MOVETYPE_U6_LAND,       30,  19,  13,   8,   2,  12,  20, ACTOR_ALIGNMENT_EVIL},
 {OBJ_U6_TROLL,            2, 2, 1, 0, OBJ_U6_DEAD_BODY, 0, true,false, ACTOR_ST, MOVETYPE_U6_LAND,       40,  18,  14,   9,   4,   6,  15, ACTOR_ALIGNMENT_EVIL},
 {OBJ_U6_CYCLOPS,          2, 8, 4, 0, OBJ_U6_DEAD_CYCLOPS, 3, true,false, ACTOR_QT, MOVETYPE_U6_LAND,    45,  27,   8,   7,   4,  20,  40, ACTOR_ALIGNMENT_EVIL},

 // 4x3 fix dead frame
 {OBJ_U6_WINGED_GARGOYLE,  3, 12, 4, 0, OBJ_U6_DEAD_GARGOYLE, 3, true,false, ACTOR_QT, MOVETYPE_U6_LAND,  60,  15,  32,  33,  10,   6,  50, ACTOR_ALIGNMENT_EVIL},
 {OBJ_U6_GARGOYLE,         3, 3, 1, 0, OBJ_U6_DEAD_BODY, 0, true,false, ACTOR_ST, MOVETYPE_U6_LAND,       50,  25,  19,   8,   5,  15,  30, ACTOR_ALIGNMENT_EVIL},

 // 4x5
 {OBJ_U6_FIGHTER,          3, 4, 1, 0, OBJ_U6_DEAD_BODY, 6, true, true, ACTOR_ST, MOVETYPE_U6_LAND,       50,  20,  17,  11,   0,   6,  20, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_SWASHBUCKLER,     3, 4, 1, 0, OBJ_U6_DEAD_BODY, 5, true, true, ACTOR_ST, MOVETYPE_U6_LAND,       50,  15,  18,  12,   0,   4,  15, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_MAGE,             3, 4, 1, 0, OBJ_U6_DEAD_BODY, 3, true, true, ACTOR_ST, MOVETYPE_U6_LAND,       50,  10,  14,  22,   0,   4,  30, ACTOR_ALIGNMENT_EVIL},
 {OBJ_U6_VILLAGER,         3, 4, 1, 0, OBJ_U6_DEAD_BODY, 2, true, true, ACTOR_ST, MOVETYPE_U6_LAND,       50,  11,  12,  12,   0,   4,   8, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_MERCHANT,         3, 4, 1, 0, OBJ_U6_DEAD_BODY, 2, true, true, ACTOR_ST, MOVETYPE_U6_LAND,       50,  12,  12,  18,   0,   4,   8, ACTOR_ALIGNMENT_EVIL},
 {OBJ_U6_CHILD,            3, 4, 1, 0, OBJ_U6_DEAD_BODY, 8, true, true, ACTOR_ST, MOVETYPE_U6_LAND,       30,   8,   8,   9,   0,   2,   6, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_GUARD,            3, 4, 1, 0, OBJ_U6_DEAD_BODY, 4, true, true, ACTOR_ST, MOVETYPE_U6_LAND,       35,  30,  30,   9,   0,   8, 100, ACTOR_ALIGNMENT_EVIL},
 {OBJ_U6_JESTER,           3, 4, 1, 0, OBJ_U6_DEAD_BODY, 8, true, true, ACTOR_ST, MOVETYPE_U6_LAND,        5,  14,  16,  12,   0,   4,   8, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_PEASANT,          3, 4, 1, 0, OBJ_U6_DEAD_BODY, 5, true, true, ACTOR_ST, MOVETYPE_U6_LAND,       50,  11,  12,  12,   0,   4,   8, ACTOR_ALIGNMENT_NEUTRAL}, //not sure of stats here
 {OBJ_U6_FARMER,           3, 4, 1, 0, OBJ_U6_DEAD_BODY, 8, true, true, ACTOR_ST, MOVETYPE_U6_LAND,       40,  15,  15,  10,   0,   4,   8, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_MUSICIAN,         3, 4, 1, 0, OBJ_U6_DEAD_BODY, 7, true, true, ACTOR_ST, MOVETYPE_U6_LAND,       50,  12,  16,  14,   0,   4,   8, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_WOMAN,            3, 4, 1, 0, OBJ_U6_DEAD_BODY, 3, true, true, ACTOR_ST, MOVETYPE_U6_LAND,       50,  10,  14,  15,   0,   4,   8, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_LORD_BRITISH,     3, 4, 1, 0, OBJ_U6_DEAD_BODY, 2, true, true, ACTOR_ST, MOVETYPE_U6_LAND,       60,  30,  30,  30,  30, 255, 255, ACTOR_ALIGNMENT_GOOD}, //does LB have a dead frame!? ;)
 {OBJ_U6_AVATAR,           3, 4, 1, 0, OBJ_U6_DEAD_BODY, 7, true, true, ACTOR_ST, MOVETYPE_U6_LAND,       50,  25,  25,  25,   0,   4,  30, ACTOR_ALIGNMENT_GOOD},

 {OBJ_U6_MUSICIAN_PLAYING, 2, 2, 1, 0, OBJ_U6_NOTHING, 0, false, false, ACTOR_ST, MOVETYPE_U6_LAND,        3,  12,  16,  14,   0,   4,   8, ACTOR_ALIGNMENT_NEUTRAL},

 {OBJ_U6_SHIP,             1, 2, 2, 8, OBJ_U6_NOTHING, 0, false, false, ACTOR_MT, MOVETYPE_U6_WATER_HIGH,  0,  30,  30,  30,  30,  30,  30, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_SKIFF,            1, 1, 1, 0, OBJ_U6_NOTHING, 0, false, false, ACTOR_ST, MOVETYPE_U6_WATER_LOW,   0,   0,   0,   0,   0,   0,   0, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_INFLATED_BALLOON, 0, 0, 0, 4, OBJ_U6_NOTHING, 0, false, false, ACTOR_ST, MOVETYPE_U6_AIR_LOW,     0,   0,   0,   0,   0,   0,   0, ACTOR_ALIGNMENT_NEUTRAL},

 {OBJ_U6_GIANT_SCORPION,   2, 2, 1, 0, OBJ_U6_BLOOD, 0, false, false, ACTOR_DT, MOVETYPE_U6_LAND,         30,  13,  13,   3,   3,   8,  20, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_GIANT_ANT,        2, 2, 1, 0, OBJ_U6_BLOOD, 0, false, false, ACTOR_DT, MOVETYPE_U6_LAND,         30,  16,  15,   2,   3,   8,  10, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_COW,              2, 2, 1, 0, OBJ_U6_BLOOD, 0, false, false, ACTOR_DT, MOVETYPE_U6_LAND,         40,  14,   7,   5,   0,   3,  10, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_ALLIGATOR,        2, 2, 1, 0, OBJ_U6_BLOOD, 0, false, false, ACTOR_DT, MOVETYPE_U6_LAND,         30,  20,  10,   3,   6,  10,  30, ACTOR_ALIGNMENT_CHAOTIC},
 {OBJ_U6_HORSE,            2, 2, 1, 0, OBJ_U6_HORSE_CARCASS, 1, false, false, ACTOR_DT, MOVETYPE_U6_LAND, 20,  22,  27,   7,   0,   3,  15, ACTOR_ALIGNMENT_NEUTRAL},
 {OBJ_U6_HORSE_WITH_RIDER, 2, 2, 1, 0, OBJ_U6_BLOOD, 0, false, false, ACTOR_DT, MOVETYPE_U6_LAND,         20,  22,  27,   7,   0,   3,  15, ACTOR_ALIGNMENT_NEUTRAL},

 {OBJ_U6_DRAGON,           2, 2, 1, 0, OBJ_U6_NOTHING, 0, false, false, ACTOR_MT, MOVETYPE_U6_AIR_LOW,    10,  40,  35,  27,  12,  30, 150, ACTOR_ALIGNMENT_EVIL},
 {OBJ_U6_SILVER_SERPENT,   1, 2, 1, 0, OBJ_U6_NOTHING, 0, false, false, ACTOR_MT, MOVETYPE_U6_LAND,        0,  60,  21,   8,  15,  60, 200, ACTOR_ALIGNMENT_CHAOTIC},

 // 2x1 FIXME
 {OBJ_U6_RAFT,             0, 1, 1, 0, OBJ_U6_NOTHING, 0, false, false, ACTOR_ST, MOVETYPE_U6_WATER_LOW,   0,   0,   0,   0,   0,   0,   0, ACTOR_ALIGNMENT_NEUTRAL}, // FIX might need to fix this
// {OBJ_U6_TANGLE_VINE_POD,  0, 0, 1, 0, OBJ_U6_TANGLE_VINE_POD, 0, false, false, ACTOR_ST, MOVETYPE_U6_LAND,40, 20,  25,   1,   4,   1,  30, ACTOR_ALIGNMENT_CHAOTIC},

 {OBJ_U6_NOTHING, 0, 0, 1, 0, OBJ_U6_NOTHING, 0, false, false, ACTOR_ST, MOVETYPE_U6_LAND, 0, 0, 0, 0, 0, 0, 0, ACTOR_ALIGNMENT_NEUTRAL} //end indicator
};

// A list of readiable objects and their readied location.

static const struct {uint16 obj_n; uint8 readiable_location; uint8 defence; uint8 attack; }
 readiable_objects[] = {
{OBJ_U6_LEATHER_HELM, ACTOR_HEAD, 1, 0 },
{OBJ_U6_CHAIN_COIF, ACTOR_HEAD, 2, 0 },
{OBJ_U6_IRON_HELM, ACTOR_HEAD, 3, 0 },
{OBJ_U6_SPIKED_HELM, ACTOR_HEAD, 3, 4 },
{OBJ_U6_WINGED_HELM, ACTOR_HEAD, 2, 0 },
{OBJ_U6_BRASS_HELM, ACTOR_HEAD, 2, 0 },
{OBJ_U6_GARGOYLE_HELM, ACTOR_HEAD, 3, 0 },
{OBJ_U6_MAGIC_HELM, ACTOR_HEAD, 5, 0 },
{OBJ_U6_WOODEN_SHIELD, ACTOR_ARM, 2, 0 },
{OBJ_U6_CURVED_HEATER, ACTOR_ARM, 3, 0 },
{OBJ_U6_WINGED_SHIELD, ACTOR_ARM, 3, 0 },
{OBJ_U6_KITE_SHIELD, ACTOR_ARM, 3, 0 },
{OBJ_U6_SPIKED_SHIELD, ACTOR_ARM, 2, 4 },
{OBJ_U6_BLACK_SHIELD, ACTOR_ARM, 2, 0 },
{OBJ_U6_DOOR_SHIELD, ACTOR_ARM, 4, 0 },
{OBJ_U6_MAGIC_SHIELD, ACTOR_ARM, 5, 0 },
{OBJ_U6_CLOTH_ARMOUR, ACTOR_BODY, 1, 0 },
{OBJ_U6_LEATHER_ARMOR, ACTOR_BODY, 2, 0 },
{OBJ_U6_RING_MAIL, ACTOR_BODY, 3, 0 },
{OBJ_U6_SCALE_MAIL, ACTOR_BODY, 4, 0 },
{OBJ_U6_CHAIN_MAIL, ACTOR_BODY, 5, 0 },
{OBJ_U6_PLATE_MAIL, ACTOR_BODY, 7, 0 },
{OBJ_U6_MAGIC_ARMOUR, ACTOR_BODY, 10, 0 },
{OBJ_U6_SPIKED_COLLAR, ACTOR_BODY, 2, 0 },
{OBJ_U6_GUILD_BELT, ACTOR_BODY, 0, 0 },
{OBJ_U6_GARGOYLE_BELT, ACTOR_BODY, 0, 0 },
{OBJ_U6_LEATHER_BOOTS, ACTOR_FOOT, 0, 0 },
{OBJ_U6_SWAMP_BOOTS, ACTOR_FOOT, 0, 0 },

{OBJ_U6_SLING, ACTOR_ARM, 0, 6 },
{OBJ_U6_CLUB, ACTOR_ARM, 0, 8 },
{OBJ_U6_MAIN_GAUCHE, ACTOR_ARM, 0, 8 },
{OBJ_U6_SPEAR, ACTOR_ARM, 0, 10 },
{OBJ_U6_THROWING_AXE, ACTOR_ARM, 0, 10 },
{OBJ_U6_DAGGER, ACTOR_ARM, 0, 6 },
{OBJ_U6_MACE, ACTOR_ARM, 0, 15 },
{OBJ_U6_MORNING_STAR, ACTOR_ARM, 0, 15 },
{OBJ_U6_BOW, ACTOR_ARM, 0, 10 },
{OBJ_U6_CROSSBOW, ACTOR_ARM, 0, 12 },
{OBJ_U6_SWORD, ACTOR_ARM, 0, 15 },
{OBJ_U6_TWO_HANDED_HAMMER, ACTOR_ARM, 0, 20 },
{OBJ_U6_TWO_HANDED_AXE, ACTOR_ARM, 0, 20 },
{OBJ_U6_TWO_HANDED_SWORD, ACTOR_ARM, 0, 20 },
{OBJ_U6_HALBERD, ACTOR_ARM, 0, 30 },
{OBJ_U6_GLASS_SWORD, ACTOR_ARM, 0, 255 },
{OBJ_U6_BOOMERANG, ACTOR_ARM, 0, 8 },
{OBJ_U6_TRIPLE_CROSSBOW, ACTOR_ARM, 0, 12 * 3 }, // ?? how to handle this

{OBJ_U6_MAGIC_BOW, ACTOR_ARM, 0, 20 },
{OBJ_U6_SPELLBOOK, ACTOR_ARM, 0, 0 },

{OBJ_U6_ANKH_AMULET, ACTOR_NECK, 0, 0 },
{OBJ_U6_SNAKE_AMULET, ACTOR_NECK, 0, 0 },
{OBJ_U6_AMULET_OF_SUBMISSION, ACTOR_NECK, 0, 0 },

{OBJ_U6_STAFF, ACTOR_ARM, 0, 4 },
{OBJ_U6_LIGHTNING_WAND, ACTOR_ARM, 0, 30 },
{OBJ_U6_FIRE_WAND, ACTOR_ARM, 0, 20 },
{OBJ_U6_STORM_CLOAK, ACTOR_BODY, 0, 0 },
{OBJ_U6_RING, ACTOR_HAND, 0, 0 },
{OBJ_U6_FLASK_OF_OIL, ACTOR_ARM, 0, 4 },

{OBJ_U6_TORCH, ACTOR_ARM, 0, 0 },

{OBJ_U6_SCYTHE, ACTOR_ARM, 0, 0 },
{OBJ_U6_PITCHFORK, ACTOR_ARM, 0, 0 },
{OBJ_U6_RAKE, ACTOR_ARM, 0, 0 },
{OBJ_U6_PICK, ACTOR_ARM, 0, 0 },
{OBJ_U6_SHOVEL, ACTOR_ARM, 0, 0 },
{OBJ_U6_HOE, ACTOR_ARM, 0, 0 },

{OBJ_U6_ROLLING_PIN, ACTOR_ARM, 0, 2 },

{OBJ_U6_CLEAVER, ACTOR_ARM, 0, 4 },
{OBJ_U6_KNIFE, ACTOR_ARM, 0, 4 },

{OBJ_U6_LUTE, ACTOR_ARM, 0, 0 },

{OBJ_U6_PLIERS, ACTOR_ARM, 0, 0 },
{OBJ_U6_HAMMER, ACTOR_ARM, 0, 0 },

{OBJ_U6_PROTECTION_RING, ACTOR_HAND, 0, 0 },
{OBJ_U6_REGENERATION_RING, ACTOR_HAND, 0, 0 },
{OBJ_U6_INVISIBILITY_RING, ACTOR_HAND, 0, 0 },

{OBJ_U6_NOTHING, ACTOR_NOT_READIABLE, 0, 0 } }; // this last element terminates the array.


// obj_n, defence, attack, hit_range, attack_type, missle_tile_num, thrown_obj_n, breaks_on_contact

const CombatType u6combat_hand = {OBJ_U6_NOTHING, 0, 4, 1, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false};

const CombatType u6combat_objects[] = {
{OBJ_U6_LEATHER_HELM, 1, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_CHAIN_COIF, 2, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_IRON_HELM, 3, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_SPIKED_HELM, 3, 4, 1, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_WINGED_HELM, 2, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_BRASS_HELM, 2, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_GARGOYLE_HELM, 3, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_MAGIC_HELM, 5, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_WOODEN_SHIELD, 2, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_CURVED_HEATER, 3, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_WINGED_SHIELD, 3, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_KITE_SHIELD, 3, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_SPIKED_SHIELD, 2, 4, 1, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_BLACK_SHIELD, 2, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_DOOR_SHIELD, 4, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_MAGIC_SHIELD, 5, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_CLOTH_ARMOUR, 1, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_LEATHER_ARMOR, 2, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_RING_MAIL, 3, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_SCALE_MAIL, 4, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_CHAIN_MAIL, 5, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_PLATE_MAIL, 7, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_MAGIC_ARMOUR, 10, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_SPIKED_COLLAR, 2, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },

{OBJ_U6_SLING, 0, 6, 3, ATTACK_TYPE_MISSLE, TILE_U6_SLING_STONE, OBJ_U6_NOTHING, false },
{OBJ_U6_CLUB, 0, 8, 1, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_MAIN_GAUCHE, 0, 8, 1, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_SPEAR, 0, 10, 4, ATTACK_TYPE_THROWN, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_THROWING_AXE, 0, 10, 3, ATTACK_TYPE_THROWN, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_DAGGER, 0, 6, 2, ATTACK_TYPE_THROWN, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_MACE, 0, 15, 1, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_MORNING_STAR, 0, 15, 2, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_BOW, 0, 10, 5, ATTACK_TYPE_MISSLE, TILE_U6_ARROW, OBJ_U6_ARROW, false },
{OBJ_U6_CROSSBOW, 0, 12, 7, ATTACK_TYPE_MISSLE, TILE_U6_BOLT, OBJ_U6_BOLT, false },
{OBJ_U6_SWORD, 0, 15, 1, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_TWO_HANDED_HAMMER, 0, 20, 1, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_TWO_HANDED_AXE, 0, 20, 1, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_TWO_HANDED_SWORD, 0, 20, 1, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_HALBERD, 0, 30, 2, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_GLASS_SWORD, 0, 255, 1, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, true },
{OBJ_U6_BOOMERANG, 0, 8, 0, ATTACK_TYPE_THROWN, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_TRIPLE_CROSSBOW, 0, 12 * 3, 0, ATTACK_TYPE_MISSLE, TILE_U6_BOLT, OBJ_U6_NOTHING, false }, // ?? how to handle this

{OBJ_U6_MAGIC_BOW, 0, 20, 0, ATTACK_TYPE_MISSLE, TILE_U6_ARROW, OBJ_U6_NOTHING, false },

{OBJ_U6_STAFF, 0, 4, 0, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_LIGHTNING_WAND, 0, 30, 0, ATTACK_TYPE_MISSLE, TILE_U6_LIGHTNING, OBJ_U6_NOTHING, false },
{OBJ_U6_FIRE_WAND, 0, 20, 0, ATTACK_TYPE_MISSLE, TILE_U6_FIREBALL, OBJ_U6_NOTHING, false },
{OBJ_U6_FLASK_OF_OIL, 0, 4, 0, ATTACK_TYPE_THROWN, 0, OBJ_U6_NOTHING, false },

{OBJ_U6_ROLLING_PIN, 0, 2, 1, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false },

{OBJ_U6_CLEAVER, 0, 4, 1, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false },
{OBJ_U6_KNIFE, 0, 4, 1, ATTACK_TYPE_HAND, 0, OBJ_U6_NOTHING, false },

{OBJ_U6_PROTECTION_RING, 5, 0, 0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false },

{OBJ_U6_NOTHING, 0, 0 ,0, ATTACK_TYPE_NONE, 0, OBJ_U6_NOTHING, false} }; // this last element terminates the array.


const short int u6combat_hitrange_tbl[][121] = {
{
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,1,1,1,0,0,0,0,
0,0,0,0,1,1,1,0,0,0,0,
0,0,0,0,1,1,1,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0},
{
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,1,1,1,1,1,0,0,0,
0,0,0,1,1,1,1,1,0,0,0,
0,0,0,1,1,1,1,1,0,0,0,
0,0,0,1,1,1,1,1,0,0,0,
0,0,0,1,1,1,1,1,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0},
{
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,1,1,1,1,1,0,0,0,
0,0,1,1,1,1,1,1,1,0,0,
0,0,1,1,1,1,1,1,1,0,0,
0,0,1,1,1,1,1,1,1,0,0,
0,0,1,1,1,1,1,1,1,0,0,
0,0,1,1,1,1,1,1,1,0,0,
0,0,0,1,1,1,1,1,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0},
{
0,0,0,0,0,0,0,0,0,0,0,
0,0,0,1,1,1,1,1,0,0,0,
0,0,1,1,1,1,1,1,1,0,0,
0,1,1,1,1,1,1,1,1,1,0,
0,1,1,1,1,1,1,1,1,1,0,
0,1,1,1,1,1,1,1,1,1,0,
0,1,1,1,1,1,1,1,1,1,0,
0,1,1,1,1,1,1,1,1,1,0,
0,0,1,1,1,1,1,1,1,0,0,
0,0,0,1,1,1,1,1,0,0,0,
0,0,0,0,0,0,0,0,0,0,0},
{
0,0,0,1,1,1,1,1,0,0,0,
0,0,1,1,1,1,1,1,1,0,0,
0,1,1,1,1,1,1,1,1,1,0,
1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,
0,1,1,1,1,1,1,1,1,1,0,
0,0,1,1,1,1,1,1,1,0,0,
0,0,0,1,1,1,1,1,0,0,0}};


