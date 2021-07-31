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
#include "natives.h"

#include <algorithm>

HandleType_t g_EntityLumpEntryType = 0;
EntityLumpEntryHandler g_EntityLumpEntryHandler;

void EntityLumpEntryHandler::OnHandleDestroy(HandleType_t type, void* object) {
	if (type == g_EntityLumpEntryType) {
		delete reinterpret_cast<std::weak_ptr<EntityLumpEntry>*>(object);
	}
}

cell_t sm_LumpManagerGet(IPluginContext *pContext, const cell_t *params) {
	int index = params[1];
	if (index < 0 || index >= static_cast<int>(lumpmanager->Length())) {
		return pContext->ThrowNativeError("Invalid index %d", index);
	}
	
	std::weak_ptr<EntityLumpEntry>* pReference = new std::weak_ptr<EntityLumpEntry>;
	*pReference = lumpmanager->Get(index);
	
	return g_pHandleSys->CreateHandle(g_EntityLumpEntryType, pReference,
			pContext->GetIdentity(), myself->GetIdentity(), NULL);
}

cell_t sm_LumpManagerErase(IPluginContext *pContext, const cell_t *params) {
	if (!g_bLumpAvailableForWriting) {
		return pContext->ThrowNativeError("Cannot use EntityLump.Erase() while in read-only mode");
	}
	
	int index = params[1];
	if (index < 0 || index >= static_cast<int>(lumpmanager->Length())) {
		return pContext->ThrowNativeError("Invalid index %d", index);
	}
	
	lumpmanager->Erase(index);
	return 0;
}

cell_t sm_LumpManagerInsert(IPluginContext *pContext, const cell_t *params) {
	if (!g_bLumpAvailableForWriting) {
		return pContext->ThrowNativeError("Cannot use EntityLump.Insert() while in read-only mode");
	}
	
	int index = params[1];
	if (index < 0 || index > static_cast<int>(lumpmanager->Length())) {
		return pContext->ThrowNativeError("Invalid index %d", index);
	}
	
	lumpmanager->Insert(index);
	return 0;
}

cell_t sm_LumpManagerAppend(IPluginContext *pContext, const cell_t *params) {
	if (!g_bLumpAvailableForWriting) {
		return pContext->ThrowNativeError("Cannot use EntityLump.Append() while in read-only mode");
	}
	return lumpmanager->Append();
}

cell_t sm_LumpManagerLength(IPluginContext *pContext, const cell_t *params) {
	return lumpmanager->Length();
}

cell_t sm_LumpEntryGet(IPluginContext *pContext, const cell_t *params) {
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());
	HandleError err;
	
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	
	std::weak_ptr<EntityLumpEntry>* entryref = nullptr;
	if ((err = g_pHandleSys->ReadHandle(hndl, g_EntityLumpEntryType, &sec, (void**) &entryref)) != HandleError_None) {
		return pContext->ThrowNativeError("Invalid EntityLumpEntry handle %x (error: %d)", hndl, err);
	}
	
	if (entryref->expired()) {
		return pContext->ThrowNativeError("Invalid EntityLumpEntry handle %x (reference expired)", hndl);
	}
	
	auto entry = entryref->lock();
	
	int index = params[2];
	if (index < 0 || index >= static_cast<int>(entry->size())) {
		return pContext->ThrowNativeError("Invalid index %d", index);
	}
	
	const auto& pair = (*entry)[index];
	
	size_t nBytes;
	pContext->StringToLocalUTF8(params[3], params[4], pair.first.c_str(), &nBytes);
	pContext->StringToLocalUTF8(params[5], params[6], pair.second.c_str(), &nBytes);
	
	return 0;
}

cell_t sm_LumpEntryUpdate(IPluginContext *pContext, const cell_t *params) {
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());
	HandleError err;
	
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	
	std::weak_ptr<EntityLumpEntry>* entryref = nullptr;
	if ((err = g_pHandleSys->ReadHandle(hndl, g_EntityLumpEntryType, &sec, (void**) &entryref)) != HandleError_None) {
		return pContext->ThrowNativeError("Invalid EntityLumpEntry handle %x (error: %d)", hndl, err);
	}
	
	if (entryref->expired()) {
		return pContext->ThrowNativeError("Invalid EntityLumpEntry handle %x (reference expired)", hndl);
	}
	
	if (!g_bLumpAvailableForWriting) {
		return pContext->ThrowNativeError("Cannot use EntityLumpEntry.Update() while in read-only mode");
	}
	
	auto entry = entryref->lock();
	
	int index = params[2];
	if (index < 0 || index >= static_cast<int>(entry->size())) {
		return pContext->ThrowNativeError("Invalid index %d", index);
	}
	
	char *key, *value;
	pContext->LocalToStringNULL(params[3], &key);
	pContext->LocalToStringNULL(params[4], &value);
	
	auto& pair = (*entry)[index];
	if (key != nullptr) {
		pair.first = key;
	}
	if (value != nullptr) {
		pair.second = value;
	}
	
	return 0;
}

cell_t sm_LumpEntryInsert(IPluginContext *pContext, const cell_t *params) {
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());
	HandleError err;
	
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	
	std::weak_ptr<EntityLumpEntry>* entryref = nullptr;
	if ((err = g_pHandleSys->ReadHandle(hndl, g_EntityLumpEntryType, &sec, (void**) &entryref)) != HandleError_None) {
		return pContext->ThrowNativeError("Invalid EntityLumpEntry handle %x (error: %d)", hndl, err);
	}
	
	if (entryref->expired()) {
		return pContext->ThrowNativeError("Invalid EntityLumpEntry handle %x (reference expired)", hndl);
	}
	
	if (!g_bLumpAvailableForWriting) {
		return pContext->ThrowNativeError("Cannot use EntityLumpEntry.Insert() while in read-only mode");
	}
	
	auto entry = entryref->lock();
	
	int index = params[2];
	if (index < 0 || index > static_cast<int>(entry->size())) {
		return pContext->ThrowNativeError("Invalid index %d", index);
	}
	
	char *key, *value;
	pContext->LocalToString(params[3], &key);
	pContext->LocalToString(params[4], &value);
	
	entry->emplace(entry->begin() + index, key, value);
	
	return 0;
}

cell_t sm_LumpEntryErase(IPluginContext *pContext, const cell_t *params) {
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());
	HandleError err;
	
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	
	std::weak_ptr<EntityLumpEntry>* entryref = nullptr;
	if ((err = g_pHandleSys->ReadHandle(hndl, g_EntityLumpEntryType, &sec, (void**) &entryref)) != HandleError_None) {
		return pContext->ThrowNativeError("Invalid EntityLumpEntry handle %x (error: %d)", hndl, err);
	}
	
	if (entryref->expired()) {
		return pContext->ThrowNativeError("Invalid EntityLumpEntry handle %x (reference expired)", hndl);
	}
	
	if (!g_bLumpAvailableForWriting) {
		return pContext->ThrowNativeError("Cannot use EntityLumpEntry.Erase() while in read-only mode");
	}
	
	auto entry = entryref->lock();
	
	int index = params[2];
	if (index < 0 || index >= static_cast<int>(entry->size())) {
		return pContext->ThrowNativeError("Invalid index %d", index);
	}
	
	entry->erase(entry->begin() + index);
	
	return 0;
}

cell_t sm_LumpEntryAppend(IPluginContext *pContext, const cell_t *params) {
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());
	HandleError err;
	
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	
	std::weak_ptr<EntityLumpEntry>* entryref = nullptr;
	if ((err = g_pHandleSys->ReadHandle(hndl, g_EntityLumpEntryType, &sec, (void**) &entryref)) != HandleError_None) {
		return pContext->ThrowNativeError("Invalid EntityLumpEntry handle %x (error: %d)", hndl, err);
	}
	
	if (entryref->expired()) {
		return pContext->ThrowNativeError("Invalid EntityLumpEntry handle %x (reference expired)", hndl);
	}
	
	if (!g_bLumpAvailableForWriting) {
		return pContext->ThrowNativeError("Cannot use EntityLumpEntry.Append() while in read-only mode");
	}
	
	auto entry = entryref->lock();
	
	char *key, *value;
	pContext->LocalToString(params[2], &key);
	pContext->LocalToString(params[3], &value);
	
	entry->emplace_back(key, value);
	
	return 0;
}

cell_t sm_LumpEntryFindKey(IPluginContext *pContext, const cell_t *params) {
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());
	HandleError err;
	
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	
	std::weak_ptr<EntityLumpEntry>* entryref = nullptr;
	if ((err = g_pHandleSys->ReadHandle(hndl, g_EntityLumpEntryType, &sec, (void**) &entryref)) != HandleError_None) {
		return pContext->ThrowNativeError("Invalid EntityLumpEntry handle %x (error: %d)", hndl, err);
	}
	
	if (entryref->expired()) {
		return pContext->ThrowNativeError("Invalid EntityLumpEntry handle %x (reference expired)", hndl);
	}
	
	// start from the index after the current one
	int start = params[3] + 1;
	
	auto entry = entryref->lock();
	
	if (start < 0 || start >= static_cast<int>(entry->size())) {
		return -1;
	}
	
	char *key;
	pContext->LocalToString(params[2], &key);
	
	auto matches_key = [&key](std::pair<std::string,std::string> pair) {
		return pair.first == key;
	};
	
	auto result = std::find_if(entry->begin() + start, entry->end(), matches_key);
	
	if (result == entry->end()) {
		return -1;
	}
	return std::distance(entry->begin(), result);
}

cell_t sm_LumpEntryLength(IPluginContext *pContext, const cell_t *params) {
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());
	HandleError err;
	
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	
	std::weak_ptr<EntityLumpEntry>* entryref = nullptr;
	if ((err = g_pHandleSys->ReadHandle(hndl, g_EntityLumpEntryType, &sec, (void**) &entryref)) != HandleError_None) {
		return pContext->ThrowNativeError("Invalid EntityLumpEntry handle %x (error: %d)", hndl, err);
	}
	
	if (entryref->expired()) {
		return pContext->ThrowNativeError("Invalid EntityLumpEntry handle %x (reference expired)", hndl);
	}
	
	auto entry = entryref->lock();
	return entry->size();
}
