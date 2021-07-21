/**
 * Sourcemod 1.7 Plugin Template
 */
#pragma semicolon 1
#include <sourcemod>

#include <sdktools>
#include <entitylump>

#pragma newdecls required

#define PLUGIN_VERSION "0.0.0"
public Plugin myinfo = {
	name = "Entity Lump Native Test",
	author = "nosoop",
	description = "A port of the Level KeyValues entity test to the Entity Lump extension",
	version = PLUGIN_VERSION,
	url = "https://github.com/nosoop"
}

#define OUTPUT_NAME "OnCapTeam2"

public void OnMapStart() {
	int captureArea = FindEntityByClassname(-1, "trigger_capture_area");
	
	if (!IsValidEntity(captureArea)) {
		LogMessage("---- %s", "No capture area");
		return;
	}
	
	int hammerid = GetEntProp(captureArea, Prop_Data, "m_iHammerID");
	EntityLumpEntry entry = FindEntityLumpEntryByHammerID(hammerid);
	
	if (!entry) {
		return;
	}
	
	int ttc = entry.FindKey("area_time_to_cap");
	if (ttc != -1) {
		entry.Update(ttc, NULL_STRING, "30");
	}
	
	LogMessage("---- %s", "Found a trigger_capture_area with keys:");
	
	for (int i, n = entry.Length; i < n; i++) {
		char keyBuffer[128], valueBuffer[128];
		entry.Get(i, keyBuffer, sizeof(keyBuffer), valueBuffer, sizeof(valueBuffer));
		
		LogMessage("%s -> %s", keyBuffer, valueBuffer);
	}
	
	LogMessage("---- %s", "List of " ... OUTPUT_NAME ... " outputs:");
	for (int k = -1; (k = entry.FindKey(OUTPUT_NAME, k)) != -1;) {
		char outputString[256];
		entry.Get(k, _, _, outputString, sizeof(outputString));
		
		char targetName[32], inputName[64], variantValue[32];
		float delay;
		int nFireCount;
		
		ParseEntityOutputString(outputString, targetName, sizeof(targetName),
				inputName, sizeof(inputName), variantValue, sizeof(variantValue),
				delay, nFireCount);
		
		LogMessage("target %s -> input %s (value %s, delay %.2f, refire %d)",
				targetName, inputName, variantValue, delay, nFireCount);
	}
	delete entry;
}

/**
 * Returns the first EntityLumpEntry with a matching hammerid.
 */
EntityLumpEntry FindEntityLumpEntryByHammerID(int hammerid) {
	for (int i, n = EntityLump.Length(); i < n; i++) {
		EntityLumpEntry entry = EntityLump.Get(i);
		
		int hidkey = entry.FindKey("hammerid");
		if (hidkey != -1) {
			char value[32];
			entry.Get(hidkey, .valbuf = value, .vallen = sizeof(value));
			
			if (StringToInt(value) == hammerid) {
				// EntityLump.Erase(i); // used this for testing the weakref handling
				return entry;
			}
		}
		delete entry;
	}
	return null;
}

/**
 * Parses an entity's output value (as formatted in the entity string).
 * Refer to https://developer.valvesoftware.com/wiki/AddOutput for the format.
 * 
 * @return True if the output string was successfully parsed, false if not.
 */
stock bool ParseEntityOutputString(const char[] output, char[] targetName, int targetNameLength,
		char[] inputName, int inputNameLength, char[] variantValue, int variantValueLength,
		float &delay, int &nFireCount) {
	int delimiter;
	char buffer[32];
	
	{
		// validate that we have something resembling an output string (four commas)
		int i, c, nDelim;
		while ((c = FindCharInString(output[i], ',')) != -1) {
			nDelim++;
			i += c + 1;
		}
		if (nDelim < 4) {
			return false;
		}
	}
	
	delimiter = SplitString(output, ",", targetName, targetNameLength);
	delimiter += SplitString(output[delimiter], ",", inputName, inputNameLength);
	delimiter += SplitString(output[delimiter], ",", variantValue, variantValueLength);
	
	delimiter += SplitString(output[delimiter], ",", buffer, sizeof(buffer));
	delay = StringToFloat(buffer);
	
	nFireCount = StringToInt(output[delimiter]);
	
	return true;
}
