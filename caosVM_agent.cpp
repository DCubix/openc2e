/*
 *  caosVM_agent.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun May 30 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#include "caosVM.h"
#include "openc2e.h"
#include "Vehicle.h"
#include "World.h"
#include <iostream>
using std::cerr;

/**
 RTAR (command) family (integer) genus (integer) species (integer)

 set targ to random agent with given family/genus/species
 */ 
void caosVM::c_RTAR() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)
	targ.reset();
	// todo
}

/**
 NEW: SIMP (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)

 create a new simple agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, at the screen depth given by plane
*/
void caosVM::c_NEW_SIMP() {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)

	SimpleAgent *a = new SimpleAgent(family, genus, species, plane, first_image, image_count);
	a->setImage(sprite_file);
	world.addAgent(a);
	targ.reset();
	targ.setAgent(a);
}

/**
 NEW: COMP (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)

 create a new composite agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, with the first part at the screen depth given by plane
*/
void caosVM::c_NEW_COMP() {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)

	CompoundAgent *a = new CompoundAgent(family, genus, species, plane, first_image, image_count);
	a->setImage(sprite_file);
	world.addAgent(a);
	targ.reset();
	targ.setAgent(a);
}

/**
 NEW: VHCL (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)

 create a new vehicle agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, with the first part at the screen depth given by plane
*/
void caosVM::c_NEW_VHCL() {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)

	Vehicle *a = new Vehicle(family, genus, species, plane, first_image, image_count);
	a->setImage(sprite_file);
	world.addAgent(a);
	targ.reset();
	targ.setAgent(a);
}

/**
 TARG (agent)

 return TARG
*/
void caosVM::v_TARG() {
	VM_VERIFY_SIZE(0)
	result = targ;
	if (!targ.hasAgent()) targ.setAgent(0); // todo: we shouldn't need to do this
	result.setVariable(&targ);
}

/**
 OWNR (agent)
 
 return OWNR
*/
void caosVM::v_OWNR() {
	VM_VERIFY_SIZE(0)
	result.setAgent(0);
	cerr << "unimplemented: OWNR\n";
}

/**
 NULL (agent)

 return null (zero) agent
*/
void caosVM::v_NULL() {
	VM_VERIFY_SIZE(0)
	result.setAgent(0);
}

/**
 POSE (command) pose (integer)
*/
void caosVM::c_POSE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(pose)
	cerr << "unimplemented: POSE\n";
}

/**
 ATTR (command) attr (integer)

 set attributes of TARG agent
*/
void caosVM::c_ATTR() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(attr)
	assert(targ.hasAgent());
	targ.agentValue->setAttributes(attr);
}

/**
 ATTR (integer)

 return attributes of TARG agent
*/
void caosVM::v_ATTR() {
	VM_VERIFY_SIZE(0)
	assert(targ.hasAgent());
	result.setInt(targ.agentValue->getAttributes());
}

/**
 TICK (command) tickrate (integer)
*/
void caosVM::c_TICK() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(tickrate)
	cerr << "unimplemented: TICK\n";
}

/**
 BHVR (command) bhvr (integer)
*/
void caosVM::c_BHVR() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(bhvr)
	cerr << "unimplemented: BHVR\n";
}

/**
 TARG (command) agent (agent)

 set TARG to given agent
*/
void caosVM::c_TARG() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(a)
	targ.reset();
	targ.setAgent(a);
}

/**
 FROM (agent)
*/
void caosVM::v_FROM() {
	VM_VERIFY_SIZE(0)
	result.setAgent(0);
	cerr << "unimplemented: FROM\n";
}

/**
 POSE (integer)
*/
void caosVM::v_POSE() {
	VM_VERIFY_SIZE(0)
	cerr << "unimplemented: POSE\n";
}

/**
 KILL (command) agent (agent)

 destroy the agent in question. you can't destroy PNTR.
 remember, use DEAD first for creatures!
*/
void caosVM::c_KILL() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(a)
	cerr << "unimplemented: KILL\n";
}

/**
 NEXT (command)
*/
void caosVM::c_NEXT() {
	VM_VERIFY_SIZE(0)
	cerr << "unimplemented: NEXT\n";
}

/**
 SCRX (command) event (integer) species (integer) genus (integer) family (integer)

 delete the event script in question
*/
void caosVM::c_SCRX() {
	VM_VERIFY_SIZE(4)
	VM_PARAM_INTEGER(family)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(event)
	cerr << "unimplemented: SCRX\n";
}

/**
 ANIM (command) poselist (byte-string)

 set the animation string for TARG, in the format '1 2 3 4'
 if it ends with '255', loop back to beginning; if it ends with '255 X', loop back to frame X

 <i>todo: compound agent stuff</i>
*/
void caosVM::c_ANIM() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(poselist)

	// todo: compound agent stuff
	assert(targ.hasAgent());

	// todo: !!
	SimpleAgent *a = (SimpleAgent *)targ.agentValue;
	a->animation.clear();

	std::string oh;
	for (unsigned int i = 0; i < poselist.size(); i++) {
		if (poselist[i] != ' ') {
			if (!isdigit(poselist[i])) throw badParamException();
			oh += poselist[i];
		} else {
			unsigned int j = (unsigned int)atoi(oh.c_str());
			a->animation.push_back(j);
			oh.clear();
		}
	}
	if (!oh.empty()) {
		unsigned int j = (unsigned int)atoi(oh.c_str());
		a->animation.push_back(j);
	}
	if (!a->animation.empty()) { a->setFrameNo(0); }
	if (a->animation.empty()) { std::cerr << "warning: ANIM produced an empty animation string\n"; }
}

/**
 ABBA (integer)
*/
void caosVM::v_ABBA() {
	VM_VERIFY_SIZE(0)
	cerr << "unimplemented: ABBA\n";
}

/**
 BASE (command) index (integer)
*/
void caosVM::c_BASE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(index)
	cerr << "unimplemented: BASE\n";
}

/**
 BASE (integer)
*/
void caosVM::v_BASE() {
	VM_VERIFY_SIZE(0)
	cerr << "unimplemented: BASE\n";
}

/**
 BHVR (integer)
*/
void caosVM::v_BHVR() {
	VM_VERIFY_SIZE(0)
	cerr << "unimplemented: BHVR\n";
}

/**
 CARR (agent)
*/
void caosVM::v_CARR() {
	VM_VERIFY_SIZE(0)
	result.setAgent(0);
	cerr << "unimplemented: CARR\n";
}

/**
 FMLY (integer)

 return family of TARG agent
*/
void caosVM::v_FMLY() {
	VM_VERIFY_SIZE(0)
	assert(targ.hasAgent());
	result.setInt(targ.agentValue->family);
}

/**
 GNUS (integer)

 return genus of TARG agent
*/
void caosVM::v_GNUS() {
	VM_VERIFY_SIZE(0)
	assert(targ.hasAgent());
	result.setInt(targ.agentValue->genus);
}

/**
 SPCS (integer)

 return species of TARG agent
*/
void caosVM::v_SPCS() {
	VM_VERIFY_SIZE(0)
	assert(targ.hasAgent());
	result.setInt(targ.agentValue->species);
}

/**
 PLNE (integer)

 return plane (z-order) of TARG agent
*/
void caosVM::v_PLNE() {
	VM_VERIFY_SIZE(0)
	assert(targ.hasAgent());
	result.setInt(targ.agentValue->zorder);
}

/**
 PNTR (agent)

 return the pointer agent (the hand)
*/
void caosVM::v_PNTR() {
	VM_VERIFY_SIZE(0)
	result.setAgent(world.hand());
}
