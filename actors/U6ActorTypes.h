
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


const U6ActorType u6ActorTypes[] = {
// 4x1 tile actors  
 {OBJ_U6_INSECTS,0, 0, 1, OBJ_U6_NOTHING, 0, false,false, 20},
 {OBJ_U6_GIANT_SQUID,0, 0, 1, OBJ_U6_NOTHING, 0, false, false, 50},
 {OBJ_U6_GHOST,0, 0, 1, OBJ_U6_NOTHING, 0, false, false, 45},
 {OBJ_U6_ACID_SLUG,0, 0, 1, OBJ_U6_NOTHING, 0, false, false, 50},
 {OBJ_U6_WISP,0, 0, 1, OBJ_U6_NOTHING, 0, false, false, 50},
 {OBJ_U6_GIANT_BAT,0, 0, 1, OBJ_U6_BLOOD, 0, false, false, 30},
 {OBJ_U6_REAPER,0, 0, 1, OBJ_U6_BLOOD, 0, false, false, 40},
 {OBJ_U6_GREMLIN,0, 0, 1, OBJ_U6_BLOOD, 0, false, false, 20},
 {OBJ_U6_GAZER,0, 0, 1, OBJ_U6_BLOOD, 0, false, false, 30},
 {OBJ_U6_BIRD,0, 0, 1, OBJ_U6_BLOOD, 0, false, false, 20},
 {OBJ_U6_CORPSER,0, 0, 1, OBJ_U6_BLOOD, 0, false, false, 50},
 {OBJ_U6_RABBIT,0, 0, 1, OBJ_U6_BLOOD, 0, false, false, 25},
 {OBJ_U6_ROT_WORMS,0, 0, 1, OBJ_U6_BLOOD, 0, false, false, 50},
 {OBJ_U6_HYDRA,0, 0, 1, OBJ_U6_BLOOD, 0, false, false, 50},
 {OBJ_U6_MOUSE,1, 1, 1, OBJ_U6_BLOOD, 0, false, false, 0},
 {OBJ_U6_CAT,1, 1, 1, OBJ_U6_BLOOD, 0, false, false, 40},

 // 4x2
 {OBJ_U6_SEA_SERPENT,2, 2, 1, OBJ_U6_NOTHING, 0, false,false, 60},
 {OBJ_U6_GIANT_RAT,2, 2, 1, OBJ_U6_BLOOD, 0, false,false, 40},
 {OBJ_U6_SHEEP,2, 2, 1, OBJ_U6_BLOOD, 0, false,false, 35},
 {OBJ_U6_DOG,2, 2, 1, OBJ_U6_BLOOD, 0, false,false, 35},
 {OBJ_U6_DEER,2, 2, 1, OBJ_U6_BLOOD, 0, false,false, 20},
 {OBJ_U6_WOLF,2, 2, 1, OBJ_U6_BLOOD, 0, false,false, 20},
 {OBJ_U6_SNAKE,2, 2, 1, OBJ_U6_BLOOD, 0, false,false, 20},
 {OBJ_U6_GIANT_SPIDER,2, 2, 1, OBJ_U6_BLOOD, 0, false,false, 20},
 {OBJ_U6_DRAKE,2, 2, 1, OBJ_U6_BLOOD, 0, false,false, 15},
 {OBJ_U6_MONGBAT,2, 2, 1, OBJ_U6_BLOOD, 0, false,false, 15},
 {OBJ_U6_DAEMON,2, 2, 1, OBJ_U6_DEAD_BODY, 0, true,false, 30},
 {OBJ_U6_SKELETON,2, 2, 1, OBJ_U6_DEAD_BODY, 9, true,false, 30},
 {OBJ_U6_HEADLESS,2, 2, 1, OBJ_U6_DEAD_BODY, 1, true,false, 30},
 {OBJ_U6_TROLL,2, 2, 1, OBJ_U6_DEAD_BODY, 0, true,false, 40},
 {OBJ_U6_CYCLOPS,2, 8, 4, OBJ_U6_DEAD_CYCLOPS, 3, true,false, 45},

 // 4x3 fix dead frame
 {OBJ_U6_WINGED_GARGOYLE,3, 12, 4, OBJ_U6_DEAD_GARGOYLE, 3, true,false, 60},
 {OBJ_U6_GARGOYLE,3, 3, 1, OBJ_U6_DEAD_BODY, 0, true,false, 50},

 // 4x5
 {OBJ_U6_FIGHTER,3, 4, 1, OBJ_U6_DEAD_BODY, 6, true, true, 50},
 {OBJ_U6_SWASHBUCKLER,3, 4, 1, OBJ_U6_DEAD_BODY, 5, true, true, 50},
 {OBJ_U6_MAGE,3, 4, 1, OBJ_U6_DEAD_BODY, 3, true, true, 50},
 {OBJ_U6_VILLAGER,3, 4, 1, OBJ_U6_DEAD_BODY, 2, true, true, 50},
 {OBJ_U6_MERCHANT,3, 4, 1, OBJ_U6_DEAD_BODY, 2, true, true, 50},
 {OBJ_U6_CHILD,3, 4, 1, OBJ_U6_DEAD_BODY, 8, true, true, 30},
 {OBJ_U6_GUARD,3, 4, 1, OBJ_U6_DEAD_BODY, 4, true, true, 35},
 {OBJ_U6_JESTER,3, 4, 1, OBJ_U6_DEAD_BODY, 8, true, true, 5},
 {OBJ_U6_PEASANT,3, 4, 1, OBJ_U6_DEAD_BODY, 5, true, true, 50},
 {OBJ_U6_FARMER,3, 4, 1, OBJ_U6_DEAD_BODY, 8, true, true, 40},
 {OBJ_U6_MUSICIAN,3, 4, 1, OBJ_U6_DEAD_BODY, 7, true, true, 50},
 {OBJ_U6_WOMAN,3, 4, 1, OBJ_U6_DEAD_BODY, 3, true, true, 50},
 {OBJ_U6_LORD_BRITISH,3, 4, 1, OBJ_U6_DEAD_BODY, 2, true, true, 60}, //does LB have a dead frame!? ;)
 {OBJ_U6_AVATAR,3, 4, 1, OBJ_U6_DEAD_BODY, 7, true, true, 50},

 {OBJ_U6_MUSICIAN_PLAYING, 2, 2, 1, OBJ_U6_NOTHING, 0, false, false, 3},
 
 {OBJ_U6_GIANT_SCORPION, 2, 2, 1, OBJ_U6_BLOOD, 0, false, false, 30},
 {OBJ_U6_GIANT_ANT, 2, 2, 1, OBJ_U6_BLOOD, 0, false, false, 30},
 {OBJ_U6_COW, 2, 2, 1, OBJ_U6_BLOOD, 0, false, false, 40},
 {OBJ_U6_ALLIGATOR, 2, 2, 1, OBJ_U6_BLOOD, 0, false, false, 30},
 {OBJ_U6_HORSE, 2, 2, 1, OBJ_U6_BLOOD, 0, false, false, 20},

 {OBJ_U6_NOTHING, 0, 0, 1, OBJ_U6_NOTHING, 0, false, false, 0} //end indicator
};

// A list of readiable objects and their readied location.

static const struct {uint16 obj_n; uint8 readiable_location;}
 readiable_objects[] = {
{OBJ_U6_LEATHER_HELM, ACTOR_HEAD },
{OBJ_U6_CHAIN_COIF, ACTOR_HEAD },
{OBJ_U6_IRON_HELM, ACTOR_HEAD },
{OBJ_U6_SPIKED_HELM, ACTOR_HEAD },
{OBJ_U6_WINGED_HELM, ACTOR_HEAD },
{OBJ_U6_BRASS_HELM, ACTOR_HEAD },
{OBJ_U6_GARGOYLE_HELM, ACTOR_HEAD },
{OBJ_U6_MAGIC_HELM, ACTOR_HEAD },
{OBJ_U6_WOODEN_SHIELD, ACTOR_ARM },
{OBJ_U6_CURVED_HEATER, ACTOR_ARM },
{OBJ_U6_WINGED_SHIELD, ACTOR_ARM },
{OBJ_U6_KITE_SHIELD, ACTOR_ARM },
{OBJ_U6_SPIKED_SHIELD, ACTOR_ARM },
{OBJ_U6_BLACK_SHIELD, ACTOR_ARM },
{OBJ_U6_DOOR_SHIELD, ACTOR_ARM },
{OBJ_U6_MAGIC_SHIELD, ACTOR_ARM },
{OBJ_U6_CLOTH_ARMOUR, ACTOR_BODY },
{OBJ_U6_LEATHER_ARMOR, ACTOR_BODY },
{OBJ_U6_RING_MAIL, ACTOR_BODY },
{OBJ_U6_SCALE_MAIL, ACTOR_BODY },
{OBJ_U6_CHAIN_MAIL, ACTOR_BODY },
{OBJ_U6_PLATE_MAIL, ACTOR_BODY },
{OBJ_U6_MAGIC_ARMOUR, ACTOR_BODY },
{OBJ_U6_SPIKED_COLLAR, ACTOR_BODY },
{OBJ_U6_GUILD_BELT, ACTOR_BODY },
{OBJ_U6_GARGOYLE_BELT, ACTOR_BODY },
{OBJ_U6_LEATHER_BOOTS, ACTOR_FOOT },
{OBJ_U6_SWAMP_BOOTS, ACTOR_FOOT },

{OBJ_U6_SLING, ACTOR_ARM },
{OBJ_U6_CLUB, ACTOR_ARM },
{OBJ_U6_MAIN_GAUCHE, ACTOR_ARM },
{OBJ_U6_SPEAR, ACTOR_ARM },
{OBJ_U6_THROWING_AXE, ACTOR_ARM },
{OBJ_U6_DAGGER, ACTOR_ARM },
{OBJ_U6_MACE, ACTOR_ARM },
{OBJ_U6_MORNING_STAR, ACTOR_ARM },
{OBJ_U6_BOW, ACTOR_ARM },
{OBJ_U6_CROSSBOW, ACTOR_ARM },
{OBJ_U6_SWORD, ACTOR_ARM },
{OBJ_U6_TWO_HANDED_HAMMER, ACTOR_ARM },
{OBJ_U6_TWO_HANDED_AXE, ACTOR_ARM },
{OBJ_U6_TWO_HANDED_SWORD, ACTOR_ARM },
{OBJ_U6_HALBERD, ACTOR_ARM },
{OBJ_U6_GLASS_SWORD, ACTOR_ARM },
{OBJ_U6_BOOMERANG, ACTOR_ARM },
{OBJ_U6_TRIPLE_CROSSBOW, ACTOR_ARM },

{OBJ_U6_MAGIC_BOW, ACTOR_ARM },
{OBJ_U6_SPELLBOOK, ACTOR_ARM },

{OBJ_U6_ANKH_AMULET, ACTOR_NECK },
{OBJ_U6_SNAKE_AMULET, ACTOR_NECK },
{OBJ_U6_AMULET_OF_SUBMISSION, ACTOR_NECK },

{OBJ_U6_STAFF, ACTOR_ARM },
{OBJ_U6_LIGHTNING_WAND, ACTOR_ARM },
{OBJ_U6_FIRE_WAND, ACTOR_ARM },
{OBJ_U6_STORM_CLOAK, ACTOR_BODY },
{OBJ_U6_RING, ACTOR_HAND },
{OBJ_U6_FLASK_OF_OIL, ACTOR_ARM },

{OBJ_U6_TORCH, ACTOR_ARM },

{OBJ_U6_SCYTHE, ACTOR_ARM },
{OBJ_U6_PITCHFORK, ACTOR_ARM },
{OBJ_U6_RAKE, ACTOR_ARM },
{OBJ_U6_PICK, ACTOR_ARM },
{OBJ_U6_SHOVEL, ACTOR_ARM },
{OBJ_U6_HOE, ACTOR_ARM },

{OBJ_U6_CLEAVER, ACTOR_ARM },
{OBJ_U6_KNIFE, ACTOR_ARM },

{OBJ_U6_LUTE, ACTOR_ARM },

{OBJ_U6_PLIERS, ACTOR_ARM },
{OBJ_U6_HAMMER, ACTOR_ARM },

{OBJ_U6_PROTECTION_RING, ACTOR_HAND },
{OBJ_U6_REGENERATION_RING, ACTOR_HAND },
{OBJ_U6_INVISIBILITY_RING, ACTOR_HAND },

{OBJ_U6_NOTHING, ACTOR_NOT_READIABLE} }; // this last element terminates the array.

