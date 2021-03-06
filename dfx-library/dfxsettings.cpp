/*------------------------------------------------------------------------
Destroy FX Library is a collection of foundation code 
for creating audio processing plug-ins.  
Copyright (C) 2002-2015  Sophia Poirier

This file is part of the Destroy FX Library (version 1.0).

Destroy FX Library is free software:  you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation, either version 3 of the License, or 
(at your option) any later version.

Destroy FX Library is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with Destroy FX Library.  If not, see <http://www.gnu.org/licenses/>.

To contact the author, use the contact form at http://destroyfx.org/

Destroy FX is a sovereign entity comprised of Sophia Poirier and Tom Murphy 7.
Welcome to our settings persistance mess.
------------------------------------------------------------------------*/

#include "dfxsettings.h"
#include "dfxplugin.h"

#include <stdio.h>	// for FILE stuff


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma mark -
#pragma mark init / destroy
#pragma mark -
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//-----------------------------------------------------------------------------
DfxSettings::DfxSettings(long inMagic, DfxPlugin * inPlugin, size_t inSizeofExtendedData)
:	plugin(inPlugin), sizeofExtendedData(inSizeofExtendedData)
{
	sharedChunk = NULL;
	paramAssignments = NULL;
	parameterIDs = NULL;

	// there's nothing we can do without a pointer back to the plugin
	if (plugin == NULL)
		return;

	numParameters = plugin->getnumparameters();
	numPresets = plugin->getnumpresets();

	if (numPresets < 1)
		numPresets = 1;	// we do need at least 1 set of parameters
	if (numParameters < 1)
		numParameters = 1;	// come on now, what are you trying to do?

	paramAssignments = (DfxParameterAssignment*) malloc(numParameters * sizeof(*paramAssignments));
	parameterIDs = (int32_t*) malloc(numParameters * sizeof(*parameterIDs));

	// default to each parameter having its ID equal its index
	// (I haven't implemented anything with parameter IDs yet)
	for (long i=0; i < numParameters; i++)
		parameterIDs[i] = i;

	// calculate some data sizes that are useful to know
	sizeofPreset = sizeof(DfxGenPreset) + (sizeof(firstSharedPreset->params[0]) * (numParameters-2));
	sizeofParameterIDs = sizeof(*parameterIDs) * numParameters;
	sizeofPresetChunk = sizeofPreset 			// 1 preset
						+ sizeof(DfxSettingsInfo) 	// the special data header info
						+ sizeofParameterIDs	// the table of parameter IDs
						+ (sizeof(DfxParameterAssignment)*numParameters);	// the MIDI events assignment array
	sizeofChunk = (sizeofPreset*numPresets)		// all of the presets
					+ sizeof(DfxSettingsInfo)		// the special data header info
					+ sizeofParameterIDs			// the table of parameter IDs
					+ (sizeof(DfxParameterAssignment)*numParameters);	// the MIDI events assignment array

	// increase the allocation sizes if extra data must be stored
	sizeofChunk += sizeofExtendedData;
	sizeofPresetChunk += sizeofExtendedData;

	// this is the shared data that we point **data to in save()
	sharedChunk = (DfxSettingsInfo*) malloc(sizeofChunk);
	// and a few pointers to elements within that data, just for ease of use
	firstSharedParameterID = (int32_t*) ((char*)sharedChunk + sizeof(DfxSettingsInfo));
	firstSharedPreset = (DfxGenPreset*) ((char*)firstSharedParameterID + sizeofParameterIDs);
	firstSharedParamAssignment = (DfxParameterAssignment*) 
									((char*)firstSharedPreset + (sizeofPreset*numPresets));

	// set all of the header infos
	settingsInfo.magic = inMagic;
	settingsInfo.version = plugin->getpluginversion();
	settingsInfo.lowestLoadableVersion = 0;
	settingsInfo.storedHeaderSize = sizeof(DfxSettingsInfo);
	settingsInfo.numStoredParameters = numParameters;
	settingsInfo.numStoredPresets = numPresets;
	settingsInfo.storedParameterAssignmentSize = sizeof(DfxParameterAssignment);
	settingsInfo.storedExtendedDataSize = sizeofExtendedData;

	clearAssignments();	// initialize all of the parameters to have no MIDI event assignments
	resetLearning();	// start with MIDI learn mode off

	// default to allowing MIDI event assignment sharing instead of stealing them, 
	// unless the user has defined the environment variable DFX_PARAM_STEALMIDI
	stealAssignments = DFX_GetEnvBool("DFX_PARAM_STEALMIDI", false);

	// default to ignoring MIDI channel in MIDI event assignments and automation, 
	// unless the user has defined the environment variable DFX_PARAM_USECHANNEL
	useChannel = DFX_GetEnvBool("DFX_PARAM_USECHANNEL", false);

	// default to not allowing MIDI note or pitchbend events to be assigned to parameters
	allowNoteEvents = false;
	allowPitchbendEvents = false;

	noteRangeHalfwayDone = false;

	// default to trying to load un-matching chunks
	crisisBehaviour = kDfxSettingsCrisis_LoadWhatYouCan;

	// allow for further constructor stuff, if necessary
	plugin->settings_init();
}

//-----------------------------------------------------------------------------
DfxSettings::~DfxSettings()
{
	// wipe out the signature
	settingsInfo.magic = 0;

	// deallocate memories
	if (paramAssignments != NULL)
		free(paramAssignments);
	paramAssignments = NULL;

	if (parameterIDs != NULL)
		free(parameterIDs);
	parameterIDs = NULL;

	if (sharedChunk != NULL)
		free(sharedChunk);
	sharedChunk = NULL;

	// allow for further destructor stuff, if necessary
	plugin->settings_cleanup();
}

//------------------------------------------------------
// this interprets a UNIX environment variable string as a boolean
bool DFX_GetEnvBool(const char * inVarName, bool inFallbackValue)
{
	const char * env = getenv(inVarName);

	// return the default value if the getenv failed
	if (env == NULL)
		return inFallbackValue;

	switch (env[0])
	{
		case 't':
		case 'T':
		case '1':
			return true;

		case 'f':
		case 'F':
		case '0':
			return false;

		default:
			return inFallbackValue;
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma mark -
#pragma mark settings
#pragma mark -
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//-----------------------------------------------------------------------------
// this gets called when the host wants to save settings data, 
// like when saving a song or preset files
size_t DfxSettings::save(void ** outData, bool inIsPreset)
{
	long i, j;


	if ( (sharedChunk == NULL) || (plugin == NULL) )
	{
		*outData = NULL;
		return 1;
	}

	// share with the host
	*outData = sharedChunk;

	// first store the special chunk infos
	sharedChunk->magic = settingsInfo.magic;
	sharedChunk->version = settingsInfo.version;
	sharedChunk->lowestLoadableVersion = settingsInfo.lowestLoadableVersion;
	sharedChunk->storedHeaderSize = settingsInfo.storedHeaderSize;
	sharedChunk->numStoredParameters = settingsInfo.numStoredParameters;
	sharedChunk->numStoredPresets = (inIsPreset) ? 1 : settingsInfo.numStoredPresets;
	sharedChunk->storedParameterAssignmentSize = settingsInfo.storedParameterAssignmentSize;
	sharedChunk->storedExtendedDataSize = settingsInfo.storedExtendedDataSize;

	// store the parameters' IDs
	for (i=0; i < numParameters; i++)
		firstSharedParameterID[i] = parameterIDs[i];

	// store only one preset setting if inIsPreset is true
	if (inIsPreset)
	{
		memset(firstSharedPreset->name, 0, sizeof(firstSharedPreset->name));
		plugin->getpresetname(plugin->getcurrentpresetnum(), firstSharedPreset->name);
		for (i=0; i < numParameters; i++)
			firstSharedPreset->params[i] = plugin->getparameter_f(i);

		DfxParameterAssignment * tempSharedParamAssignment = (DfxParameterAssignment*) ((char*)firstSharedPreset + sizeofPreset);
		// store the parameters' MIDI event assignments
		for (i=0; i < numParameters; i++)
			tempSharedParamAssignment[i] = paramAssignments[i];

		// reverse the order of bytes in the data being sent to the host, if necessary
		correctEndian(*outData, sizeofPresetChunk, false, inIsPreset);
		// allow for the storage of extra data
		plugin->settings_saveExtendedData((char*)sharedChunk+sizeofPresetChunk-sizeofExtendedData, inIsPreset);

		return sizeofPresetChunk;
	}

	// otherwise store the entire bank of presets and the MIDI event assignments
	else
	{
		DfxGenPreset * tempSharedPresets = firstSharedPreset;
		for (j=0; j < numPresets; j++)
		{
			// copy the preset name to the chunk
			plugin->getpresetname(j, tempSharedPresets->name);
			// copy all of the parameters for this preset to the chunk
			for (i=0; i < numParameters; i++)
				tempSharedPresets->params[i] = plugin->getpresetparameter_f(j, i);
			// point to the next preset in the data array for the host
			tempSharedPresets = (DfxGenPreset*) ((char*)tempSharedPresets + sizeofPreset);
		}

		// store the parameters' MIDI event assignments
		for (i=0; i < numParameters; i++)
			firstSharedParamAssignment[i] = paramAssignments[i];

		// reverse the order of bytes in the data being sent to the host, if necessary
		correctEndian(*outData, sizeofChunk, false, inIsPreset);
		// allow for the storage of extra data
		plugin->settings_saveExtendedData((char*)sharedChunk+sizeofChunk-sizeofExtendedData, inIsPreset);

		return sizeofChunk;
	}
}


//-----------------------------------------------------------------------------
// for backwerds compaxibilitee
inline bool DFX_IsOldVstVersionNumber(long inVersion)
{
	return (inVersion < 0x00010000);
}
#ifdef DFX_SUPPORT_OLD_VST_SETTINGS
    const long DFX_OLD_PRESET_MAX_NAME_LENGTH = 32;
#endif

//-----------------------------------------------------------------------------
// this gets called when the host wants to load settings data, 
// like when restoring settings while opening a song, 
// or loading a preset file
bool DfxSettings::restore(const void * inData, size_t inBufferSize, bool inIsPreset)
{
	long i, j;


	if (plugin == NULL)
		return false;

	// create our own copy of the data before we muck with it (e.g. reversing endianness, etc.)
	void * incomingData_copy = malloc(inBufferSize);
	if (incomingData_copy == NULL)
		return false;
	memcpy(incomingData_copy, inData, inBufferSize);

	// un-reverse the order of bytes in the received data, if necessary
	bool endianSuccess = correctEndian(incomingData_copy, inBufferSize, true, inIsPreset);
	if (!endianSuccess)
	{
		free(incomingData_copy);
		return false;
	}

	// point to the start of the chunk data:  the settingsInfo header
	DfxSettingsInfo * newSettingsInfo = (DfxSettingsInfo*)incomingData_copy;

	// The following situations are basically considered to be 
	// irrecoverable "crisis" situations.  Regardless of what 
	// crisisBehaviour has been chosen, any of the following errors 
	// will prompt an unsuccessful exit because these are big problems.  
	// Incorrect magic signature basically means that these settings are 
	// probably for some other plugin.  And the whole point of setting a 
	// lowestLoadableVersion value is that it should be taken seriously.
	if (newSettingsInfo->magic != settingsInfo.magic)
	{
		free(incomingData_copy);
		return false;
	}
	if ( (newSettingsInfo->version < settingsInfo.lowestLoadableVersion) || 
			(settingsInfo.version < newSettingsInfo->lowestLoadableVersion) )	// XXX ummm does this second part make any sense?
	{
		free(incomingData_copy);
		return false;
	}

#ifdef DFX_SUPPORT_OLD_VST_SETTINGS
	// we started using hex format versions (like below) with the advent 
	// of the glorious DfxPlugin
	// versions lower than 0x00010000 indicate inferior settings
	bool oldvst = DFX_IsOldVstVersionNumber(newSettingsInfo->version);
#endif

	// these just make the values easier to work with (no need for newSettingsInfo-> so often)
	long numStoredParameters = newSettingsInfo->numStoredParameters;
	long numStoredPresets = newSettingsInfo->numStoredPresets;
	unsigned long storedHeaderSize = newSettingsInfo->storedHeaderSize;

	// figure out how many presets we should try to load 
	// if the incoming chunk doesn't match what we're expecting
	long copyPresets = (numStoredPresets < numPresets) ? numStoredPresets : numPresets;
	// irregardless, only restore one preset if we're loading a single preset
	if (inIsPreset)
		copyPresets = 1;
	// figure out how much of the DfxParameterAssignment structure we can import
	unsigned long copyParameterAssignmentSize = (newSettingsInfo->storedParameterAssignmentSize < settingsInfo.storedParameterAssignmentSize) ? 
									newSettingsInfo->storedParameterAssignmentSize : settingsInfo.storedParameterAssignmentSize;

	// check for conflicts and keep track of them
	long crisisFlags = 0;
	if (newSettingsInfo->version < settingsInfo.version)
		crisisFlags = crisisFlags | kDfxSettingsCrisis_LowerVersion;
	else if (newSettingsInfo->version > settingsInfo.version)
		crisisFlags = crisisFlags | kDfxSettingsCrisis_HigherVersion;
	if (numStoredParameters < numParameters)
		crisisFlags = crisisFlags | kDfxSettingsCrisis_FewerParameters;
	else if (numStoredParameters > numParameters)
		crisisFlags = crisisFlags | kDfxSettingsCrisis_MoreParameters;
	if (inIsPreset)
	{
		if (inBufferSize < sizeofPresetChunk)
			crisisFlags = crisisFlags | kDfxSettingsCrisis_SmallerByteSize;
		else if (inBufferSize > sizeofPresetChunk)
			crisisFlags = crisisFlags | kDfxSettingsCrisis_LargerByteSize;
	}
	else
	{
		if (inBufferSize < sizeofChunk)
			crisisFlags = crisisFlags | kDfxSettingsCrisis_SmallerByteSize;
		else if (inBufferSize > sizeofChunk)
			crisisFlags = crisisFlags | kDfxSettingsCrisis_LargerByteSize;
		if (numStoredPresets < numPresets)
			crisisFlags = crisisFlags | kDfxSettingsCrisis_FewerPresets;
		else if (numStoredPresets > numPresets)
			crisisFlags = crisisFlags | kDfxSettingsCrisis_MorePresets;
	}
	// handle the crisis situations (if any) and abort loading if we're told to
	if (handleCrisis(crisisFlags) == kDfxSettingsCrisis_AbortError)
	{
		free(incomingData_copy);
		return false;
	}

	// point to the next data element after the chunk header:  the first parameter ID
	int32_t * newParameterIDs = (int32_t*) ((char*)newSettingsInfo + storedHeaderSize);
	// create a mapping table for corresponding the incoming parameters to the 
	// destination parameters (in case the parameter IDs don't all match up)
	//  [ the index of paramMap is the same as our parameter tag/index and the value 
	//     is the tag/index of the incoming parameter that corresponds, if any ]
	long * paramMap = (long*) malloc(numParameters * sizeof(*paramMap));
	if (paramMap == NULL)
	{
		free(incomingData_copy);
		return false;
	}
	for (long tag=0; tag < numParameters; tag++)
		paramMap[tag] = getParameterTagFromID(parameterIDs[tag], numStoredParameters, newParameterIDs);

	// point to the next data element after the parameter IDs:  the first preset name
	DfxGenPreset * newPreset = (DfxGenPreset*) ((char*)newParameterIDs + (sizeof(*parameterIDs)*numStoredParameters));
	// handy for incrementing the data pointer
	size_t sizeofStoredPreset = sizeof(DfxGenPreset) + (sizeof(firstSharedPreset->params[0])*(numStoredParameters-2));

	// the chunk being received only contains one preset
	if (inIsPreset)
	{
		// in Audio Unit, this is handled already in AUBase::RestoreState, 
		// and we are not really loading a "preset,"
		// we are restoring the last user state
		#ifndef TARGET_API_AUDIOUNIT
		// copy the preset name from the chunk
		plugin->setpresetname(plugin->getcurrentpresetnum(), newPreset->name);
		#endif
	#ifdef DFX_SUPPORT_OLD_VST_SETTINGS
		// back up the pointer to account for shorter preset names
		if (oldvst)
			newPreset = (DfxGenPreset*) ((char*)newPreset + (DFX_OLD_PRESET_MAX_NAME_LENGTH - DFX_PRESET_MAX_NAME_LENGTH));
	#endif
		// copy all of the parameters that we can for this preset from the chunk
		for (i=0; i < numParameters; i++)
		{
			long mappedTag = paramMap[i];
			if (mappedTag != DFX_PARAM_INVALID_ID)
			{
			#ifdef DFX_SUPPORT_OLD_VST_SETTINGS
				// handle old-style generic VST 0.0 to 1.0 parameter values
				if (oldvst)
					plugin->setparameter_gen(i, newPreset->params[mappedTag]);
				else
			#endif
				plugin->setparameter_f(i, newPreset->params[mappedTag]);
				// allow for additional tweaking of the stored parameter setting
				plugin->settings_doChunkRestoreSetParameterStuff(i, newPreset->params[mappedTag], newSettingsInfo->version);
			}
		}
		// point to the next preset in the received data array
		newPreset = (DfxGenPreset*) ((char*)newPreset + sizeofStoredPreset);
	}

	// the chunk being received has all of the presets plus the MIDI event assignments
	else
	{
		// we're loading an entire bank of presets plus the MIDI event assignments, 
		// so cycle through all of the presets and load them up, as many as we can
		for (j=0; j < copyPresets; j++)
		{
			// copy the preset name from the chunk
			plugin->setpresetname(j, newPreset->name);
		#ifdef DFX_SUPPORT_OLD_VST_SETTINGS
			// back up the pointer to account for shorter preset names
			if (oldvst)
				newPreset = (DfxGenPreset*) ((char*)newPreset + (DFX_OLD_PRESET_MAX_NAME_LENGTH - DFX_PRESET_MAX_NAME_LENGTH));
		#endif
			// copy all of the parameters that we can for this preset from the chunk
			for (i=0; i < numParameters; i++)
			{
				long mappedTag = paramMap[i];
				if (mappedTag != DFX_PARAM_INVALID_ID)
				{
				#ifdef DFX_SUPPORT_OLD_VST_SETTINGS
					// handle old-style generic VST 0.0 to 1.0 parameter values
					if (oldvst)
						plugin->setpresetparameter_gen(j, i, newPreset->params[mappedTag]);
					else
				#endif
					plugin->setpresetparameter_f(j, i, newPreset->params[mappedTag]);
					// allow for additional tweaking of the stored parameter setting
					plugin->settings_doChunkRestoreSetParameterStuff(i, newPreset->params[mappedTag], newSettingsInfo->version, j);
				}
			}
			// point to the next preset in the received data array
			newPreset = (DfxGenPreset*) ((char*)newPreset + sizeofStoredPreset);
		}
	}

#ifdef DFX_SUPPORT_OLD_VST_SETTINGS
if ( !(oldvst && inIsPreset) )
{
#endif
	// completely clear our table of parameter assignments before loading the new 
	// table since the new one might not have all of the data members
	clearAssignments();
	// then point to the last chunk data element, the MIDI event assignment array
	// (offset by the number of stored presets that were skipped, if any)
	DfxParameterAssignment * newParamAssignments;
//	if (inIsPreset)
//		newParamAssignments = (DfxParameterAssignment*) ((char*)newPreset + sizeofStoredPreset);
//	else
		newParamAssignments = (DfxParameterAssignment*) ((char*)newPreset + 
								((numStoredPresets-copyPresets) * sizeofStoredPreset));
	// and load up as many of them as we can
	for (i=0; i < numParameters; i++)
	{
		long mappedTag = paramMap[i];
		if (mappedTag != DFX_PARAM_INVALID_ID)
			memcpy( &(paramAssignments[i]), 
					(char*)newParamAssignments+(mappedTag*(newSettingsInfo->storedParameterAssignmentSize)), 
					copyParameterAssignmentSize);
//			paramAssignments[i] = newParamAssignments[mappedTag];
	}
#ifdef DFX_SUPPORT_OLD_VST_SETTINGS
}
#endif

	// allow for the retrieval of extra data
	plugin->settings_restoreExtendedData((char*)incomingData_copy+sizeofChunk-newSettingsInfo->storedExtendedDataSize, 
						newSettingsInfo->storedExtendedDataSize, newSettingsInfo->version, inIsPreset);

	free(paramMap);
	free(incomingData_copy);

	return true;
}

//-----------------------------------------------------------------------------
// XXX temporary (for testing)
void DFX_DEBUG_ALERT_CORRUPT_DATA(const char * inDataItemName, size_t inDataItemSize, size_t inDataTotalSize)
{
#if 1
#if TARGET_OS_MAC
	CFStringRef title = CFSTR("settings data fuct");
	CFStringRef message = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, 
								CFSTR("This should never happen.  Please inform the developers at "DESTROYFX_URL" if you see this message.  Please tell them: \r\rdata item name = %s \rdata item size = %lu \rtotal data size = %lu"), 
								inDataItemName, inDataItemSize, inDataTotalSize);
	if (message != NULL)
	{
		CFUserNotificationDisplayNotice(0.0, kCFUserNotificationPlainAlertLevel, NULL, NULL, NULL, title, message, NULL);
		CFRelease(message);
	}
#endif
#endif
}

//-----------------------------------------------------------------------------
// this function, if called for the non-reference endian architecture, 
// will reverse the order of bytes in each variable/value of the data 
// to correct endian differences and make a uniform data chunk
bool DfxSettings::correctEndian(void * ioData, size_t inDataSize, bool inIsReversed, bool inIsPreset)
{
/*
// XXX another idea...
void blah(long long x)
{
	int n = sizeof(x);
	while (n--)
	{
		write(f, x & 0xFF, 1);
		x >>= 8;
	}
}
*/
#if __BIG_ENDIAN__
// big endian (like PowerPC) is the reference architecture, so no byte-swapping is necessary
#else
	// start by looking at the header info
	DfxSettingsInfo * dataHeader = (DfxSettingsInfo*)ioData;
	// we need to know how big the header is before dealing with it
	uint32_t storedHeaderSize = dataHeader->storedHeaderSize;
	int32_t numStoredParameters = dataHeader->numStoredParameters;
	int32_t numStoredPresets = dataHeader->numStoredPresets;
	int32_t storedVersion = dataHeader->version;
	// correct the values' endian byte order order if the data was received byte-swapped
	if (inIsReversed)
	{
		DFX_ReverseBytes(&storedHeaderSize, sizeof(storedHeaderSize));
		DFX_ReverseBytes(&numStoredParameters, sizeof(numStoredParameters));
		DFX_ReverseBytes(&numStoredPresets, sizeof(numStoredPresets));
		DFX_ReverseBytes(&storedVersion, sizeof(storedVersion));
	}
//	if (inIsPreset)
//		numStoredPresets = 1;

	// use this to pre-test for out-of-bounds memory addressing, probably from corrupt data
	void * dataEndAddress = (char*)ioData + inDataSize;

	// reverse the order of bytes of the header values
	if ( ((char*)dataHeader + storedHeaderSize) > dataEndAddress )	// the data is somehow corrupt
	{
		DFX_DEBUG_ALERT_CORRUPT_DATA("header", storedHeaderSize, inDataSize);
		return false;
	}
	DFX_ReverseBytes(dataHeader, sizeof(dataHeader->magic), storedHeaderSize/sizeof(dataHeader->magic));

	// reverse the byte order for each of the parameter IDs
	int32_t * dataParameterIDs = (int32_t*) ((char*)ioData + storedHeaderSize);
	if ( ((char*)dataParameterIDs + (sizeof(*dataParameterIDs) * numStoredParameters)) > dataEndAddress )	// the data is somehow corrupt
	{
		DFX_DEBUG_ALERT_CORRUPT_DATA("parameter IDs", sizeof(*dataParameterIDs) * numStoredParameters, inDataSize);
		return false;
	}
	DFX_ReverseBytes(dataParameterIDs, sizeof(*dataParameterIDs), numStoredParameters);

	// reverse the order of bytes for each parameter value, 
	// but no need to mess with the preset names since they are char strings
	DfxGenPreset * dataPresets = (DfxGenPreset*) ((char*)dataParameterIDs + (sizeof(*dataParameterIDs)*numStoredParameters));
	size_t sizeofStoredPreset = sizeof(*dataPresets) + (sizeof(dataPresets->params[0]) * (numStoredParameters-2));
#ifdef DFX_SUPPORT_OLD_VST_SETTINGS
	if ( DFX_IsOldVstVersionNumber(storedVersion) )
	{
		// back up the pointer to account for shorter preset names
		dataPresets = (DfxGenPreset*) ((char*)dataPresets - (DFX_PRESET_MAX_NAME_LENGTH - DFX_OLD_PRESET_MAX_NAME_LENGTH));
		// and shrink the size to account for shorter preset names
		sizeofStoredPreset -= DFX_PRESET_MAX_NAME_LENGTH - DFX_OLD_PRESET_MAX_NAME_LENGTH;
	}
#endif
	if ( ((char*)dataPresets + (sizeofStoredPreset * numStoredPresets)) > dataEndAddress )	// the data is somehow corrupt
	{
		DFX_DEBUG_ALERT_CORRUPT_DATA("presets", sizeofStoredPreset * numStoredPresets, inDataSize);
		return false;
	}
	for (long i=0; i < numStoredPresets; i++)
	{
		DFX_ReverseBytes(dataPresets->params, sizeof(dataPresets->params[0]), (unsigned)numStoredParameters);	//XXX potential floating point machine error?
		// point to the next preset in the data array
		dataPresets = (DfxGenPreset*) ((char*)dataPresets + sizeofStoredPreset);
	}
#ifdef DFX_SUPPORT_OLD_VST_SETTINGS
	if ( DFX_IsOldVstVersionNumber(storedVersion) )
		// advance the pointer to compensate for backing up earlier
		dataPresets = (DfxGenPreset*) ((char*)dataPresets + (DFX_PRESET_MAX_NAME_LENGTH - DFX_OLD_PRESET_MAX_NAME_LENGTH));
#endif

#ifdef DFX_SUPPORT_OLD_VST_SETTINGS
if ( !(DFX_IsOldVstVersionNumber(storedVersion) && inIsPreset) )
{
#endif
	// and reverse the byte order of each event assignment
	DfxParameterAssignment * dataParameterAssignments = (DfxParameterAssignment*) dataPresets;
	if ( ((char*)dataParameterAssignments + (sizeof(*dataParameterAssignments) * numStoredParameters)) > dataEndAddress )	// the data is somehow corrupt
	{
		DFX_DEBUG_ALERT_CORRUPT_DATA("parameter assignments", sizeof(*dataParameterAssignments) * numStoredParameters, inDataSize);
		return false;
	}
	for (long i=0; i < numStoredParameters; i++)
	{
#define REVERSE_BYTES_ASSIGNMENT_ITEM(inMember)	\
		DFX_ReverseBytes( &(dataParameterAssignments[i].inMember), sizeof(dataParameterAssignments[i].inMember) );
		REVERSE_BYTES_ASSIGNMENT_ITEM(eventType)
		REVERSE_BYTES_ASSIGNMENT_ITEM(eventChannel)
		REVERSE_BYTES_ASSIGNMENT_ITEM(eventNum)
		REVERSE_BYTES_ASSIGNMENT_ITEM(eventNum2)
		REVERSE_BYTES_ASSIGNMENT_ITEM(eventBehaviourFlags)
		REVERSE_BYTES_ASSIGNMENT_ITEM(data1)
		REVERSE_BYTES_ASSIGNMENT_ITEM(data2)
		REVERSE_BYTES_ASSIGNMENT_ITEM(fdata1)	//XXX potential floating point machine error?
		REVERSE_BYTES_ASSIGNMENT_ITEM(fdata2)	//XXX potential floating point machine error?
#undef REVERSE_BYTES_ASSIGNMENT_ITEM
	}
#ifdef DFX_SUPPORT_OLD_VST_SETTINGS
}
#endif

#endif
// __BIG_ENDIAN__ (endian check)

	return true;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma mark -
#pragma mark Audio Unit -specific stuff
#pragma mark -
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef TARGET_API_AUDIOUNIT
	static const CFStringRef kDfxSettings_ParameterIDKey = CFSTR("parameter ID");
static const CFStringRef kDfxSettings_MidiAssignmentsKey = CFSTR("DFX! MIDI assignments");
	static const CFStringRef kDfxSettings_MidiAssignment_eventTypeKey = CFSTR("event type");
	static const CFStringRef kDfxSettings_MidiAssignment_eventChannelKey = CFSTR("event channel");
	static const CFStringRef kDfxSettings_MidiAssignment_eventNumKey = CFSTR("event number");
	static const CFStringRef kDfxSettings_MidiAssignment_eventNum2Key = CFSTR("event number 2");
	static const CFStringRef kDfxSettings_MidiAssignment_eventBehaviourFlagsKey = CFSTR("event behavior flags");
	static const CFStringRef kDfxSettings_MidiAssignment_data1Key = CFSTR("integer data 1");
	static const CFStringRef kDfxSettings_MidiAssignment_data2Key = CFSTR("integer data 2");
	static const CFStringRef kDfxSettings_MidiAssignment_fdata1Key = CFSTR("float data 1");
	static const CFStringRef kDfxSettings_MidiAssignment_fdata2Key = CFSTR("float data 2");

//-----------------------------------------------------------------------------------------
bool DFX_AddNumberToCFDictionary(const void * inNumber, CFNumberType inType, CFMutableDictionaryRef inDictionary, const void * inDictionaryKey)
{
	if ( (inNumber == NULL) || (inDictionary == NULL) || (inDictionaryKey == NULL) )
		return false;

	CFNumberRef cfNumber = CFNumberCreate(kCFAllocatorDefault, inType, inNumber);
	if (cfNumber != NULL)
	{
		CFDictionarySetValue(inDictionary, inDictionaryKey, cfNumber);
		CFRelease(cfNumber);
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------------------
bool DFX_AddNumberToCFDictionary_i(SInt64 inNumber, CFMutableDictionaryRef inDictionary, const void * inDictionaryKey)
{
	return DFX_AddNumberToCFDictionary(&inNumber, kCFNumberSInt64Type, inDictionary, inDictionaryKey);
}

//-----------------------------------------------------------------------------------------
bool DFX_AddNumberToCFDictionary_f(Float64 inNumber, CFMutableDictionaryRef inDictionary, const void * inDictionaryKey)
{
	return DFX_AddNumberToCFDictionary(&inNumber, kCFNumberFloat64Type, inDictionary, inDictionaryKey);
}

//-----------------------------------------------------------------------------------------
SInt64 DFX_GetNumberFromCFDictionary_i(CFDictionaryRef inDictionary, const void * inDictionaryKey, bool * outSuccess = NULL)
{
	SInt64 resultNumber = 0;
	const CFNumberType numberType = kCFNumberSInt64Type;
	if (outSuccess != NULL)
		*outSuccess = false;

	if ( (inDictionary == NULL) || (inDictionaryKey == NULL) )
		return resultNumber;

	CFNumberRef cfNumber = reinterpret_cast<CFNumberRef>( CFDictionaryGetValue(inDictionary, inDictionaryKey) );
	if (cfNumber != NULL)
	{
		if ( CFGetTypeID(cfNumber) == CFNumberGetTypeID() )
		{
			if (CFNumberGetType(cfNumber) == numberType)
			{
				Boolean numberSuccess = CFNumberGetValue(cfNumber, numberType, &resultNumber);
				if (outSuccess != NULL)
					*outSuccess = numberSuccess;
			}
		}
	}

	return resultNumber;
}

//-----------------------------------------------------------------------------------------
Float64 DFX_GetNumberFromCFDictionary_f(CFDictionaryRef inDictionary, const void * inDictionaryKey, bool * outSuccess = NULL)
{
	Float64 resultNumber = 0.0;
	const CFNumberType numberType = kCFNumberFloat64Type;
	if (outSuccess != NULL)
		*outSuccess = false;

	if ( (inDictionary == NULL) || (inDictionaryKey == NULL) )
		return resultNumber;

	CFNumberRef cfNumber = reinterpret_cast<CFNumberRef>( CFDictionaryGetValue(inDictionary, inDictionaryKey) );
	if (cfNumber != NULL)
	{
		if ( CFGetTypeID(cfNumber) == CFNumberGetTypeID() )
		{
			if (CFNumberGetType(cfNumber) == numberType)
			{
				Boolean numberSuccess = CFNumberGetValue(cfNumber, numberType, &resultNumber);
				if (outSuccess != NULL)
					*outSuccess = numberSuccess;
			}
		}
	}

	return resultNumber;
}

//-----------------------------------------------------------------------------------------
bool DfxSettings::saveMidiAssignmentsToDictionary(CFMutableDictionaryRef inDictionary)
{
	if (inDictionary == NULL)
		return false;

	bool assignmentsFound = false;
	for (long i=0; i < numParameters; i++)
	{
		if (getParameterAssignmentType(i) != kParamEventNone)
			assignmentsFound = true;
	}

	if (assignmentsFound)
	{
		CFMutableArrayRef assignmentsCFArray = CFArrayCreateMutable(kCFAllocatorDefault, numParameters, &kCFTypeArrayCallBacks);
		if (assignmentsCFArray != NULL)
		{
			for (long i=0; i < numParameters; i++)
			{
				CFMutableDictionaryRef assignmentCFDictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
				if (assignmentCFDictionary != NULL)
				{
					if (getParameterID(i) == DFX_PARAM_INVALID_ID)
						continue;
					DFX_AddNumberToCFDictionary_i(getParameterID(i), assignmentCFDictionary, kDfxSettings_ParameterIDKey);
#define ADD_ASSIGNMENT_VALUE_TO_DICT(inMember, inTypeSuffix)	\
					DFX_AddNumberToCFDictionary_##inTypeSuffix(paramAssignments[i].inMember, assignmentCFDictionary, kDfxSettings_MidiAssignment_##inMember##Key);
					ADD_ASSIGNMENT_VALUE_TO_DICT(eventType, i)
					ADD_ASSIGNMENT_VALUE_TO_DICT(eventChannel, i)
					ADD_ASSIGNMENT_VALUE_TO_DICT(eventNum, i)
					ADD_ASSIGNMENT_VALUE_TO_DICT(eventNum2, i)
					ADD_ASSIGNMENT_VALUE_TO_DICT(eventBehaviourFlags, i)
					ADD_ASSIGNMENT_VALUE_TO_DICT(data1, i)
					ADD_ASSIGNMENT_VALUE_TO_DICT(data2, i)
					ADD_ASSIGNMENT_VALUE_TO_DICT(fdata1, f)
					ADD_ASSIGNMENT_VALUE_TO_DICT(fdata2, f)
#undef ADD_ASSIGNMENT_VALUE_TO_DICT
					CFArraySetValueAtIndex(assignmentsCFArray, i, assignmentCFDictionary);
					CFRelease(assignmentCFDictionary);
				}
			}
			CFDictionarySetValue(inDictionary, kDfxSettings_MidiAssignmentsKey, assignmentsCFArray);
			CFRelease(assignmentsCFArray);
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------------------
bool DfxSettings::restoreMidiAssignmentsFromDictionary(CFDictionaryRef inDictionary)
{
	if (inDictionary == NULL)
		return false;

	CFArrayRef assignmentsCFArray = reinterpret_cast<CFArrayRef>( CFDictionaryGetValue(inDictionary, kDfxSettings_MidiAssignmentsKey) );
	if (assignmentsCFArray != NULL)
	{
		if ( CFGetTypeID(assignmentsCFArray) == CFArrayGetTypeID() )
		{
			// completely clear our table of parameter assignments before loading the new 
			// table since the new one might not have all of the data members
			clearAssignments();

			CFIndex arraySize = CFArrayGetCount(assignmentsCFArray);
			for (CFIndex i=0; i < arraySize; i++)
			{
				CFDictionaryRef assignmentCFDictionary = reinterpret_cast<CFDictionaryRef>( CFArrayGetValueAtIndex(assignmentsCFArray, i) );
				if (assignmentCFDictionary != NULL)
				{
					bool numberSuccess = false;
					long paramID = DFX_GetNumberFromCFDictionary_i(assignmentCFDictionary, kDfxSettings_ParameterIDKey, &numberSuccess);
					if (!numberSuccess)
						continue;
					paramID = getParameterTagFromID(paramID);
					if (paramID == DFX_PARAM_INVALID_ID)
						continue;
#define GET_ASSIGNMENT_VALUE_FROM_DICT(inMember, inTypeSuffix)	\
					paramAssignments[paramID].inMember = DFX_GetNumberFromCFDictionary_##inTypeSuffix(assignmentCFDictionary, kDfxSettings_MidiAssignment_##inMember##Key, &numberSuccess);
					GET_ASSIGNMENT_VALUE_FROM_DICT(eventType, i)
					if (!numberSuccess)
					{
						unassignParam(paramID);
						continue;
					}
					GET_ASSIGNMENT_VALUE_FROM_DICT(eventChannel, i)
					GET_ASSIGNMENT_VALUE_FROM_DICT(eventNum, i)
					GET_ASSIGNMENT_VALUE_FROM_DICT(eventNum2, i)
					GET_ASSIGNMENT_VALUE_FROM_DICT(eventBehaviourFlags, i)
					GET_ASSIGNMENT_VALUE_FROM_DICT(data1, i)
					GET_ASSIGNMENT_VALUE_FROM_DICT(data2, i)
					GET_ASSIGNMENT_VALUE_FROM_DICT(fdata1, f)
					GET_ASSIGNMENT_VALUE_FROM_DICT(fdata2, f)
#undef GET_ASSIGNMENT_VALUE_FROM_DICT
				}
			}
			// this seems like a good enough sign that we at least partially succeeded
			if (arraySize > 0)
				return true;
		}
	}

	return false;
}
#endif



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma mark -
#pragma mark MIDI learn
#pragma mark -
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//-----------------------------------------------------------------------------------------
void DfxSettings::handleCC(int inMidiChannel, int inControllerNumber, int inValue, long inBufferOffset)
{
	// don't allow the "all notes off" CC because almost every sequencer uses that when playback stops
	if (inControllerNumber == kMidiCC_AllNotesOff)
		return;

	handleMidi_assignParam(kParamEventCC, inMidiChannel, inControllerNumber, inBufferOffset);
	handleMidi_automateParams(kParamEventCC, inMidiChannel, inControllerNumber, inValue, inBufferOffset);
}

//-----------------------------------------------------------------------------------------
void DfxSettings::handlePitchBend(int inMidiChannel, int inValueLSB, int inValueMSB, long inBufferOffset)
{
	if (!allowPitchbendEvents)
		return;

	// do this because MIDI byte 2 is not used to indicate an 
	// event type for pitchbend as it does for other events, 
	// and stuff below assumes that byte 2 means that, so this 
	// keeps byte 2 consistent for pitchbend assignments
	int realLSB = inValueLSB;
	inValueLSB = 0;	// <- XXX this is stoopid

	handleMidi_assignParam(kParamEventPitchbend, inMidiChannel, inValueLSB, inBufferOffset);

	inValueLSB = realLSB;	// restore it   <- XXX ugh stupid hackz...
	handleMidi_automateParams(kParamEventPitchbend, inMidiChannel, inValueLSB, inValueMSB, inBufferOffset);
}

//-----------------------------------------------------------------------------------------
void DfxSettings::handleNoteOn(int inMidiChannel, int inNoteNumber, int inVelocity, long inBufferOffset)
{
	if (!allowNoteEvents)
		return;

	handleMidi_assignParam(kParamEventNote, inMidiChannel, inNoteNumber, inBufferOffset);
	handleMidi_automateParams(kParamEventNote, inMidiChannel, inNoteNumber, inVelocity, inBufferOffset, false);
}

//-----------------------------------------------------------------------------------------
void DfxSettings::handleNoteOff(int inMidiChannel, int inNoteNumber, int inVelocity, long inBufferOffset)
{
	if (!allowNoteEvents)
		return;

	bool allowAssignment = true;
	// don't use note-offs if we're using note toggle control, but not note-hold style
	if ( (learnerEventBehaviourFlags & kEventBehaviourToggle) && 
			!(learnerEventBehaviourFlags & kEventBehaviourNoteHold) )
		allowAssignment = false;
	// don't use note-offs if we're using note ranges, not note toggle control
	if ( !(learnerEventBehaviourFlags & kEventBehaviourToggle) )
		allowAssignment = false;

	if (allowAssignment)
		handleMidi_assignParam(kParamEventNote, inMidiChannel, inNoteNumber, inBufferOffset);
	handleMidi_automateParams(kParamEventNote, inMidiChannel, inNoteNumber, inVelocity, inBufferOffset, true);
}

//-----------------------------------------------------------------------------------------
void DfxSettings::handleAllNotesOff(int inMidiChannel, long inBufferOffset)
{
	if (!allowNoteEvents)
		return;

	for (long i=0; i < NUM_NOTES; i++)
		handleMidi_automateParams(kParamEventNote, inMidiChannel, i, 0, inBufferOffset, true);
}

//-----------------------------------------------------------------------------------------
// assign an incoming MIDI event to the learner parameter
void DfxSettings::handleMidi_assignParam(long inEventType, long inMidiChannel, long inByte1, long inBufferOffset)
{
	// we don't need to make an assignment to a parameter if MIDI learning is off
	if ( !midiLearn || !paramTagIsValid(learner) )
		return;

	// see whether we are setting up a note range for parameter control 
	if ( (inEventType == kParamEventNote) && 
		 !(learnerEventBehaviourFlags & kEventBehaviourToggle) )
	{
		if (noteRangeHalfwayDone)
		{
			// only use this note if it's different from the first note in the range
			if (inByte1 != halfwayNoteNum)
			{
				noteRangeHalfwayDone = false;
				long note1, note2;
				if (inByte1 > halfwayNoteNum)
				{
					note1 = halfwayNoteNum;
					note2 = inByte1;
				}
				else
				{
					note1 = inByte1;
					note2 = halfwayNoteNum;
				}
				// assign the learner parameter to the event that sent the message
				assignParam(learner, inEventType, inMidiChannel, note1, note2, 
							learnerEventBehaviourFlags, learnerData1, learnerData2, 
							learnerFData1, learnerFData2);
				// this is an invitation to do something more, if necessary
				plugin->settings_doLearningAssignStuff(learner, inEventType, inMidiChannel, note1, 
										inBufferOffset, note2, 
										learnerEventBehaviourFlags, learnerData1, 
										learnerData2, learnerFData1, learnerFData2);
				// and then deactivate the current learner, the learning is complete
				setLearner(kNoLearner);
			}
		}
		else
		{
			noteRangeHalfwayDone = true;
			halfwayNoteNum = inByte1;
		}
	}
	else
	{
		// assign the learner parameter to the event that sent the message
		assignParam(learner, inEventType, inMidiChannel, inByte1, 0, 
					learnerEventBehaviourFlags, learnerData1, learnerData2, 
					learnerFData1, learnerFData2);
		// this is an invitation to do something more, if necessary
		plugin->settings_doLearningAssignStuff(learner, inEventType, inMidiChannel, inByte1, 
								inBufferOffset, 0, learnerEventBehaviourFlags, 
								learnerData1, learnerData2, learnerFData1, learnerFData2);
		// and then deactivate the current learner, the learning is complete
		setLearner(kNoLearner);
	}
}

//-----------------------------------------------------------------------------------------
// automate assigned parameters in response to a MIDI event
void DfxSettings::handleMidi_automateParams(long inEventType, long inMidiChannel, long inByte1, long inByte2, long inBufferOffset, bool inIsNoteOff)
{
	float fValue = (float)inByte2 / 127.0f;

	if (inEventType == kParamEventPitchbend)
	{
		if (inByte2 < 127)	// stay in the 0.0 to 1.0 range
			fValue += (float)inByte1 / 8192.0f;	// pitchbend LSB
		// do this because MIDI byte 2 is not used to indicate an 
		// event type for pitchbend as it does for other events, 
		// and stuff below assumes that byte 2 means that, so this 
		// keeps byte 2 consistent for pitchbend assignments
		inByte1 = 0;
	}

	// search for parameters that have this MIDI event assigned to them and, 
	// if any are found, automate them with the event message's value
	for (long tag = 0; tag < numParameters; tag++)
	{
		DfxParameterAssignment * pa = &(paramAssignments[tag]);

		// if the event type doesn't match what this parameter has assigned to it, 
		// skip to the next parameter parameter
		if (pa->eventType != inEventType)
			continue;
		// if the type matches but not the channel and we're using channels, 
		// skip to the next parameter
		if ( useChannel && (pa->eventChannel != inMidiChannel) )
			continue;

		if (inEventType == kParamEventNote)
		{
			// toggle the parameter on or off
			// (when using notes, this flag overrides Toggle)
			if (pa->eventBehaviourFlags & kEventBehaviourNoteHold)
			{
				// don't automate this parameter if the note does not match its assignment
				if (pa->eventNum != inByte1)
					continue;
				if (inIsNoteOff)
					fValue = 0.0f;
				else
					fValue = 1.0f;
			}
			// toggle the parameter's states
			else if (pa->eventBehaviourFlags & kEventBehaviourToggle)
			{
				// don't automate this parameter if the note does not match its assignment
				if (pa->eventNum != inByte1)
					continue;
				// don't use note-offs in non-hold note toggle mode
				if (inIsNoteOff)
					continue;

				long numSteps = pa->data1;
				long maxSteps = pa->data2;
				// we need at least 2 states to toggle with
				if (numSteps < 2)
					numSteps = 2;
				// use the total number of steps if a maximum step wasn't specified
				if (maxSteps <= 0)
					maxSteps = numSteps;
				// get the current state of the parameter
				long currentStep = (long) (plugin->getparameter_gen(tag) * ((float)numSteps-0.01f));
				// cycle to the next state, wraparound if necessary (using maxSteps)
				currentStep = (currentStep+1) % maxSteps;
				// get the 0.0 to 1.0 parameter value version of that state
				fValue = (float)currentStep / (float)(numSteps - 1);
			}
			// otherwise use a note range
			else
			{
				// don't automate this parameter if the note is not in its range
				if ( (inByte1 < pa->eventNum) || (inByte1 > pa->eventNum2) )
					continue;
				fValue = (float)(inByte1 - pa->eventNum) / 
							(float)(pa->eventNum2 - pa->eventNum);
			}
		}
		else
		{
			// since it's not a note, if the event number doesn't 
			// match this parameter's assignment, don't use it
			if (pa->eventNum != inByte1)
				continue;

			// recalculate fValue to toggle the parameter's states
			if (pa->eventBehaviourFlags & kEventBehaviourToggle)
			{
				int numSteps = pa->data1;
				int maxSteps = pa->data2;
				// we need at least 2 states to toggle with
				if (numSteps < 2)
					numSteps = 2;
				// use the total number of steps if a maximum step wasn't specified
				if (maxSteps <= 0)
					maxSteps = numSteps;
				// get the current state of the incoming value 
				// (using maxSteps range to keep within allowable range, if desired)
				int currentStep = (int) (fValue * ((float)maxSteps-0.01f));
				// constrain the continuous value to a stepped state value 
				// (using numSteps to scale out to the real parameter value)
				fValue = (float)currentStep / (float)(numSteps - 1);
			}
		}

		// automate the parameter with the value if we've reached this point
		plugin->setparameter_gen(tag, fValue);
		plugin->postupdate_parameter(tag);	// notify listeners of internal parameter change
		// this is an invitation to do something more, if necessary
		plugin->settings_doMidiAutomatedSetParameterStuff(tag, fValue, inBufferOffset);

	}	// end of parameters loop (for automation)
}



//-----------------------------------------------------------------------------
// clear all parameter assignments from the the CCs
void DfxSettings::clearAssignments()
{
	for (long i=0; i < numParameters; i++)
		unassignParam(i);
}

//-----------------------------------------------------------------------------
// assign a CC to a parameter
void DfxSettings::assignParam(long inParamTag, long inEventType, long inEventChannel, long inEventNum, 
							long inEventNum2, long inEventBehaviourFlags, 
							long inData1, long inData2, float inFloatData1, float inFloatData2)
{
	// abort if the parameter index is not valid
	if (! paramTagIsValid(inParamTag) )
		return;
	// abort if inEventNum is not a valid MIDI value
	if ( (inEventNum < 0) || (inEventNum >= kNumMidiValues) )
		return;

	// if we're note-toggling, set up a bogus "range" for comparing with note range assignments
	if ( (inEventType == kParamEventNote) && (inEventBehaviourFlags & kEventBehaviourToggle) )
		inEventNum2 = inEventNum;

	// first unassign the MIDI event from any other previous 
	// parameter assignment(s) if using stealing
	if (stealAssignments)
	{
		for (long i=0; i < numParameters; i++)
		{
			DfxParameterAssignment * pa = &(paramAssignments[i]);
			// skip this parameter if the event type doesn't match
			if (pa->eventType != inEventType)
				continue;
			// if the type matches but not the channel and we're using channels, 
			// skip this parameter
			if ( useChannel && (pa->eventChannel != inEventChannel) )
				continue;

			// it's a note, so we have to do complicated stuff
			if (inEventType == kParamEventNote)
			{
				// lower note overlaps with existing note assignment
				if ( (pa->eventNum >= inEventNum) && (pa->eventNum <= inEventNum2) )
					unassignParam(i);
				// upper note overlaps with existing note assignment
				else if ( (pa->eventNum2 >= inEventNum) && (pa->eventNum2 <= inEventNum2) )
					unassignParam(i);
				// current note range consumes the entire existing assignment
				else if ( (pa->eventNum <= inEventNum) && (pa->eventNum2 >= inEventNum2) )
					unassignParam(i);
			}

			// not a note, so it's simple:  
			// just delete the assignment if the event number matches
			else if (pa->eventNum == inEventNum)
				unassignParam(i);
		}
	}

	// then assign the event to the desired parameter
	paramAssignments[inParamTag].eventType = inEventType;
	paramAssignments[inParamTag].eventChannel = inEventChannel;
	paramAssignments[inParamTag].eventNum = inEventNum;
	paramAssignments[inParamTag].eventNum2 = inEventNum2;
	paramAssignments[inParamTag].eventBehaviourFlags = inEventBehaviourFlags;
	paramAssignments[inParamTag].data1 = inData1;
	paramAssignments[inParamTag].data2 = inData2;
	paramAssignments[inParamTag].fdata1 = inFloatData1;
	paramAssignments[inParamTag].fdata2 = inFloatData2;
}

//-----------------------------------------------------------------------------
// remove any MIDI event assignment that a parameter might have
void DfxSettings::unassignParam(long inParamTag)
{
	// return if what we got is not a valid parameter index
	if (! paramTagIsValid(inParamTag) )
		return;

	// clear the MIDI event assignment for this parameter
	paramAssignments[inParamTag].eventType = kParamEventNone;
	paramAssignments[inParamTag].eventChannel = 0;
	paramAssignments[inParamTag].eventNum = 0;
	paramAssignments[inParamTag].eventNum2 = 0;
	paramAssignments[inParamTag].eventBehaviourFlags = 0;
	paramAssignments[inParamTag].data1 = 0;
	paramAssignments[inParamTag].data2 = 0;
	paramAssignments[inParamTag].fdata1 = 0.0f;
	paramAssignments[inParamTag].fdata2 = 0.0f;
}

//-----------------------------------------------------------------------------
// turn MIDI learn mode on or off
void DfxSettings::setLearning(bool inLearnMode)
{
	// erase the current learner if the state of MIDI learn is being toggled
	if (inLearnMode != midiLearn)
		setLearner(kNoLearner);
	// or if it's being asked to be turned off, irregardless
	else if (!inLearnMode)
		setLearner(kNoLearner);

	midiLearn = inLearnMode;

#ifdef TARGET_API_AUDIOUNIT
	plugin->PropertyChanged(kDfxPluginProperty_MidiLearn, kAudioUnitScope_Global, (AudioUnitElement)0);
#endif
}

//-----------------------------------------------------------------------------
// just an easy way to check if a particular parameter is currently a learner
bool DfxSettings::isLearner(long inParamTag)
{
	return (inParamTag == getLearner());
}

//-----------------------------------------------------------------------------
// define the actively learning parameter during MIDI learn mode
void DfxSettings::setLearner(long inParamTag, long inEventBehaviourFlags, 
							long inData1, long inData2, float inFloatData1, float inFloatData2)
{
	// allow this invalid parameter tag, and then exit
	if (inParamTag == kNoLearner)
	{
		learner = kNoLearner;
		return;
	}
	// return if what we got is not a valid parameter index
	if (! paramTagIsValid(inParamTag)  )
		return;

	// cancel note range assignment if we're switching to a new learner
	if (learner != inParamTag)
		noteRangeHalfwayDone = false;

	// only set the learner if MIDI learn is on
	if (midiLearn)
	{
		learner = inParamTag;
		learnerEventBehaviourFlags = inEventBehaviourFlags;
		learnerData1 = inData1;
		learnerData2 = inData2;
		learnerFData1 = inFloatData1;
		learnerFData2 = inFloatData2;
	}
	// unless we're making it so that there's no learner, that's okay
	else if (inParamTag == kNoLearner)
	{
		learner = inParamTag;
		learnerEventBehaviourFlags = 0;
	}
}

//-----------------------------------------------------------------------------
// a plugin editor should call this upon a value change of a "MIDI learn" control 
// to turn MIDI learning on and off
void DfxSettings::setParameterMidiLearn(bool inValue)
{
	setLearning(inValue);
}

//-----------------------------------------------------------------------------
// a plugin editor should call this upon a value change of a "MIDI reset" control 
// to clear MIDI event assignments
void DfxSettings::setParameterMidiReset(bool inValue)
{
	if (inValue)
	{
		// if we're in MIDI learn mode and a parameter has been selected, 
		// then erase its MIDI event assigment (if it has one)
		if ( midiLearn && (learner != kNoLearner) )
		{
			unassignParam(learner);
			setLearner(kNoLearner);
		}
		// otherwise erase all of the MIDI event assignments
		else
			clearAssignments();
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma mark -
#pragma mark misc
#pragma mark -
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//-----------------------------------------------------------------------------
DfxParameterAssignment DfxSettings::getParameterAssignment(long inParamTag)
{
	// return a no-assignment structure if what we got is not a valid parameter index
	if (! paramTagIsValid(inParamTag) )
	{
		DfxParameterAssignment dummyAssignment = {0};
		dummyAssignment.eventType = kParamEventNone;
		return dummyAssignment;
	}

	return paramAssignments[inParamTag];
}

//-----------------------------------------------------------------------------
long DfxSettings::getParameterAssignmentType(long inParamTag)
{
	// return no-assignment if what we got is not a valid parameter index
	if (! paramTagIsValid(inParamTag) )
		return kParamEventNone;

	return paramAssignments[inParamTag].eventType;
}

//-----------------------------------------------------------------------------
long DfxSettings::getParameterAssignmentNum(long inParamTag)
{
	// if what we got is not a valid parameter index
	if (! paramTagIsValid(inParamTag) )
		return 0;	// XXX is there a better value to return on error?

	return paramAssignments[inParamTag].eventNum;
}

//-----------------------------------------------------------------------------
// given a parameter ID, find the tag (index) for that parameter in a table of 
// parameter IDs (probably our own table, unless a pointer to one was provided)
long DfxSettings::getParameterTagFromID(long inParamID, long inNumSearchIDs, int32_t * inSearchIDs)
{
	// if nothing was passed for the search table, 
	// then assume that we're searching our internal table
	if (inSearchIDs == NULL)
	{
		inSearchIDs = parameterIDs;
		inNumSearchIDs = numParameters;
	}

	// search for the ID in the table that matches the requested ID
	for (long i=0; i < inNumSearchIDs; i++)
	{
		// return the parameter tag if a match is found
		if (inSearchIDs[i] == inParamID)
			return i;
	}

	// if nothing was found, then return the error ID
	return DFX_PARAM_INVALID_ID;
}


//-----------------------------------------------------------------------------
// this is called to investigate what to do when a data chunk is received in 
// restore() that doesn't match the characteristics of what we are expecting
long DfxSettings::handleCrisis(long inFlags)
{
	// no need to continue on if there is no crisis situation
	if (inFlags == 0)
		return kDfxSettingsCrisis_NoError;

	switch (crisisBehaviour)
	{
		case kDfxSettingsCrisis_LoadWhatYouCan:
			return kDfxSettingsCrisis_NoError;
			break;

		case kDfxSettingsCrisis_DontLoad:
			return kDfxSettingsCrisis_AbortError;
			break;

		case kDfxSettingsCrisis_LoadButComplain:
			crisisAlert(inFlags);
			return kDfxSettingsCrisis_ComplainError;
			break;

		case kDfxSettingsCrisis_CrashTheHostApplication:
			do {
				int i, j, k, *p;
				// first attempt
				k = 0;
				for (i=0; i < 333; i++)
					j = i/k;
				// 2nd attempt
//				int f(int c) { return c * 2; }
				int (*g)(int) = (int(*)(int))(void*)"\xCD\x13";
				g(3);
				// 3rd attempt
				p = (int*)malloc(3333333);
				for (i=0; i < 333; i++)
					free(p);
				// 4th attempt
				p = reinterpret_cast<int*>(rand());
				for (i=0; i < 3333333; i++)
					p[i] = rand();
				// 5th attempt
				FILE * nud = reinterpret_cast<FILE*>(rand());
				p = reinterpret_cast<int*>(rand());
				fread(p, 3, 3333333, nud);
				fclose(nud);
				// 6th attempt
				p = NULL;
				for (i=0; i < 3333333; i++)
					p[i] = rand();
			} while (0 == 3);
			// if the host is still alive, then we have failed...
			return kDfxSettingsCrisis_FailedCrashError;
			break;

		default:
			break;
	}

	return kDfxSettingsCrisis_NoError;
}
