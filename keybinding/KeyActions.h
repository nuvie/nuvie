/*
 *  Copyright (C) 2012 The Nuvie Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef KEYACTIONS_H
#define KEYACTIONS_H

void ActionWalkWest(int *params);
void ActionWalkEast(int *params);
void ActionWalkNorth(int *params);
void ActionWalkSouth(int *params);
void ActionWalkNorthEast(int *params);
void ActionWalkSouthEast(int *params);
void ActionWalkNorthWest(int *params);
void ActionWalkSouthWest(int *params);

void ActionCast(int *params);
void ActionLook(int *params);
void ActionTalk(int *params);
void ActionUse(int *params);
void ActionGet(int *params);
void ActionMove(int *params); //PUSH EVENT
void ActionDrop(int *params);
void ActionToggleCombat(int *params);
void ActionAttack(int *params);
void ActionRest(int *params);

void ActionNewInventory(int *params);
void ActionInventory(int *params);
void ActionPartyView(int *params);
void ActionNextInventory(int *params);
void ActionPreviousInventory(int *params);

void ActionSoloMode(int *params);
void ActionPartyMode(int *params);

void ActionSaveDialog(int *params);
void ActionLoadLatestSave(int *params);
void ActionQuitDialog(int *params);

void ActionToggleCursor(int *params);
void ActionToggleCombatStrategy(int *params);

void ActionDoAction(int *params);
void ActionCancelAction(int *params);

void ActionMsgScrollUP(int *params);
void ActionMsgScrollDown(int *params);
void ActionShowKeys(int *params);
void ActionDecreaseDebug(int *params);
void ActionIncreaseDebug(int *params);

void ActionTest(int* params);

#endif
