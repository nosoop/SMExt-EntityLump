/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
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
#include "natives.h"

#include <chrono>
#include <string>

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

EntityLumpExt g_EntityLumpExtension;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_EntityLumpExtension);

SH_DECL_HOOK6(IServerGameDLL, LevelInit, SH_NOATTRIB, false, bool, const char *, const char *, const char *, const char *, bool, bool);
SH_DECL_HOOK0(IVEngineServer, GetMapEntitiesString, SH_NOATTRIB, 0, const char *);

std::string g_strMapEntities;

static EntityLumpManager s_LumpManager;
EntityLumpManager* lumpmanager = &s_LumpManager;

IForward* fwdMapEntitiesParsed;

bool EntityLumpExt::SDK_OnLoad(char *error, size_t maxlen, bool late) {
	SH_ADD_HOOK(IServerGameDLL, LevelInit, gamedll, SH_MEMBER(&g_EntityLumpExtension, &EntityLumpExt::Hook_LevelInit), false);
	SH_ADD_HOOK(IVEngineServer, GetMapEntitiesString, engine, SH_MEMBER(&g_EntityLumpExtension, &EntityLumpExt::Hook_GetMapEntitiesString), false);
	
	sharesys->AddNatives(myself, g_EntityLumpNatives);
	g_EntityLumpEntryType = g_pHandleSys->CreateType("EntityLumpEntry", &g_EntityLumpEntryHandler, 0, NULL, NULL, myself->GetIdentity(), NULL);
	
	fwdMapEntitiesParsed = forwards->CreateForward("OnMapEntitiesParsed", ET_Ignore, 0, NULL);
	
	if (late) {
		lumpmanager->Parse(engine->GetMapEntitiesString());
	}
	
	return true;
}

void EntityLumpExt::SDK_OnUnload() {
	SH_REMOVE_HOOK(IServerGameDLL, LevelInit, gamedll, SH_MEMBER(&g_EntityLumpExtension, &EntityLumpExt::Hook_LevelInit), false);
	SH_REMOVE_HOOK(IVEngineServer, GetMapEntitiesString, engine, SH_MEMBER(&g_EntityLumpExtension, &EntityLumpExt::Hook_GetMapEntitiesString), false);
	
	g_pHandleSys->RemoveType(g_EntityLumpEntryType, myself->GetIdentity());
	
	forwards->ReleaseForward(fwdMapEntitiesParsed);
}

bool EntityLumpExt::Hook_LevelInit(char const *pMapName, char const *pMapEntities, char const *pOldLevel, char const *pLandmarkName, bool loadGame, bool background)
{
	auto parsestart = std::chrono::high_resolution_clock::now();

	lumpmanager->Parse(pMapEntities);

	auto parsestop = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> parsetime = parsestop - parsestart;
	rootconsole->ConsolePrint("Parsing %d entities took %f seconds", lumpmanager->Length(), parsetime.count());

	cell_t result{};
	fwdMapEntitiesParsed->Execute(&result);

	auto dumpstart = std::chrono::high_resolution_clock::now();
	g_strMapEntities = lumpmanager->Dump();
	auto dumpstop = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> dumptime = dumpstop - dumpstart;
	rootconsole->ConsolePrint("Dumping map entities took %f seconds", dumptime.count());

	RETURN_META_VALUE(MRES_IGNORED, true);
}

const char* EntityLumpExt::Hook_GetMapEntitiesString() {
	if (!g_strMapEntities.empty()) {
		RETURN_META_VALUE(MRES_SUPERCEDE, g_strMapEntities.c_str());
	}
	RETURN_META_VALUE(MRES_IGNORED, NULL);
}
