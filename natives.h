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

#ifndef _INCLUDE_LUMPMANAGER_NATIVES_H_
#define _INCLUDE_LUMPMANAGER_NATIVES_H_

/**
 * @file natives.h
 * @brief Declares native bindings for SourcePawn.
 */
 
class EntityLumpEntryHandler : public IHandleTypeDispatch {
public:
	void OnHandleDestroy(HandleType_t type, void* object);
};

extern HandleType_t g_EntityLumpEntryType;
extern EntityLumpEntryHandler g_EntityLumpEntryHandler;

cell_t sm_LumpManagerGet(IPluginContext *pContext, const cell_t *params);
cell_t sm_LumpManagerErase(IPluginContext *pContext, const cell_t *params);
cell_t sm_LumpManagerInsert(IPluginContext *pContext, const cell_t *params);
cell_t sm_LumpManagerAppend(IPluginContext *pContext, const cell_t *params);
cell_t sm_LumpManagerLength(IPluginContext *pContext, const cell_t *params);

cell_t sm_LumpEntryGet(IPluginContext *pContext, const cell_t *params);
cell_t sm_LumpEntryUpdate(IPluginContext *pContext, const cell_t *params);
// cell_t sm_LumpEntryInsert(IPluginContext *pContext, const cell_t *params);
// cell_t sm_LumpEntryErase(IPluginContext *pContext, const cell_t *params);
// cell_t sm_LumpEntryAppend(IPluginContext *pContext, const cell_t *params);
cell_t sm_LumpEntryFindKey(IPluginContext *pContext, const cell_t *params);
cell_t sm_LumpEntryLength(IPluginContext *pContext, const cell_t *params);

const sp_nativeinfo_t g_EntityLumpNatives[] = {
	{ "EntityLump.Get", sm_LumpManagerGet },
	{ "EntityLump.Erase", sm_LumpManagerErase },
	{ "EntityLump.Insert", sm_LumpManagerInsert },
	{ "EntityLump.Append", sm_LumpManagerAppend },
	{ "EntityLump.Length", sm_LumpManagerLength },
	
	{ "EntityLumpEntry.Get", sm_LumpEntryGet },
	{ "EntityLumpEntry.Update", sm_LumpEntryUpdate },
	// { "EntityLumpEntry.Insert", sm_LumpEntryInsert },
	// { "EntityLumpEntry.Erase", sm_LumpEntryErase },
	// { "EntityLumpEntry.Append", sm_LumpEntryAppend },
	{ "EntityLumpEntry.FindKey", sm_LumpEntryFindKey },
	{ "EntityLumpEntry.Length.get", sm_LumpEntryLength },
	
	{NULL, NULL},
};

#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
