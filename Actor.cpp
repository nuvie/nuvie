/*
 *  Actor.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Thu Mar 20 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 */

#include "Map.h"

#include "Actor.h"

Actor::Actor(Map *m)
{
 map = m;
}
 
Actor::~Actor()
{
}
 
bool Actor::is_alive()
{
 return alive;
}