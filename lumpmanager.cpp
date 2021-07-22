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

#include "lumpmanager.h"

#include <regex>
#include <sstream>
#include <string>

static EntityLumpManager s_LumpManager;
EntityLumpManager* lumpmanager = &s_LumpManager;

/**
 * Reads a string from the given input stream, up to, but excluding line ending characters.
 *
 * Function sourced from https://stackoverflow.com/a/6089413 (CC BY-SA 3.0)
 * Name and indentation has been modified
 */
std::istream& getline_any_eol(std::istream& is, std::string& t) {
	t.clear();

	// The characters in the stream are read one-by-one using a std::streambuf.
	// That is faster than reading them one-by-one using the std::istream.
	// Code that uses streambuf this way must be guarded by a sentry object.
	// The sentry object performs various tasks,
	// such as thread synchronization and updating the stream state.

	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();

	for(;;) {
		int c = sb->sbumpc();
		switch (c) {
		case '\n':
			return is;
		case '\r':
			if(sb->sgetc() == '\n')
				sb->sbumpc();
			return is;
		case std::streambuf::traits_type::eof():
			// Also handle the case when the last line has no line ending
			if(t.empty())
				is.setstate(std::ios::eofbit);
			return is;
		default:
			t += (char)c;
		}
	}
}

void EntityLumpManager::Parse(const char* pMapEntities) {
	// Pattern copied from alliedmodders/stripper-source/master/parser.cpp
	static std::regex keyValueLine("\"([^\"]+)\"\\s+\"([^\"]+)\"");
	
	m_Entities.clear();
	
	std::istringstream mapEntities(pMapEntities);
	
	EntityLumpEntry currentEntry;
	std::smatch match;
	
	// TODO rework the parser logic
	for (std::string line; !getline_any_eol(mapEntities, line).eof();) {
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

std::string EntityLumpManager::Dump() {
	std::ostringstream stream;
	for (const auto& entry : m_Entities) {
		// ignore empty entries
		if ((*entry).empty()) {
			continue;
		}
		stream << "{\n";
		for (const auto& pair : (*entry)) {
			stream << '"' << pair.first << "\"	\"" << pair.second << '"' << '\n';
		}
		stream << "}\n";
	}
	return stream.str();
}

std::weak_ptr<EntityLumpEntry> EntityLumpManager::Get(size_t index) {
	return m_Entities[index];
}

void EntityLumpManager::Erase(size_t index) {
	m_Entities.erase(m_Entities.begin() + index);
}

void EntityLumpManager::Insert(size_t index) {
	m_Entities.emplace(m_Entities.begin() + index);
}

size_t EntityLumpManager::Append() {
	return std::distance(m_Entities.begin(), m_Entities.emplace(m_Entities.end()));
}

size_t EntityLumpManager::Length() {
	return m_Entities.size();
}
