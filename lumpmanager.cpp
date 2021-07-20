/**
 * vim: set ts=4 :
 * =============================================================================
 * Entity Lump Manager
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"
#include "lumpmanager.h"

#include <regex>
#include <sstream>
#include <string>

static EntityLumpManager s_LumpManager;
EntityLumpManager* lumpmanager = &s_LumpManager;

void EntityLumpManager::Parse(const char* pMapEntities) {
	// Pattern copied from alliedmodders/stripper-source/master/parser.cpp
	static std::regex keyValueLine("\"([^\"]+)\"\\s+\"([^\"]+)\"");
	
	m_Entities.clear();
	
	std::stringstream mapEntities(pMapEntities);
	
	EntityLumpEntry currentEntry;
	std::smatch match;
	for (std::string line; std::getline(mapEntities, line);) {
		if (line == "{") {
			// TODO we should just be able to assert that this is empty, right?
			currentEntry.clear();
		} else if (line == "}") {
			m_Entities.push_back(std::make_shared<EntityLumpEntry>(currentEntry));
		} else if (std::regex_match(line, match, keyValueLine)) {
			currentEntry.emplace_back(match[1].str(), match[2].str());
		}
	}
}
