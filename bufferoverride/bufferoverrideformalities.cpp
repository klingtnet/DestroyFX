/*------------------------------------------------------------------------
Copyright (C) 2001-2016  Sophia Poirier

This file is part of Buffer Override.

Buffer Override is free software:  you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation, either version 3 of the License, or 
(at your option) any later version.

Buffer Override is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with Buffer Override.  If not, see <http://www.gnu.org/licenses/>.

To contact the author, use the contact form at http://destroyfx.org/
------------------------------------------------------------------------*/

#include "bufferoverride.h"


#pragma mark _________init_________

// this macro does boring entry point stuff for us
DFX_EFFECT_ENTRY(BufferOverride)

//-----------------------------------------------------------------------------
// initializations and such
BufferOverride::BufferOverride(TARGET_API_BASE_INSTANCE_TYPE inInstance)
:	DfxPlugin(inInstance, kNumParameters, kNumPresets)	// 21 parameters, 16 presets
{
	buffers = NULL;
	outval = NULL;
	numBuffers = 0;

	// create instances of these classes
	tempoRateTable = new TempoRateTable(kNoExtremeTempoRates);
	divisorLFO = new LFO;
	bufferLFO = new LFO;

	const long numTempoRates = tempoRateTable->getNumTempoRates();
	const long unitTempoRateIndex = tempoRateTable->getNearestTempoRateIndex(1.0f);
	initparameter_f(kDivisor, "buffer divisor", 1.92, 1.92, 1.92, 222.0, kDfxParamUnit_divisor, kDfxParamCurve_squared);
	initparameter_f(kBufferSize_abs, "forced buffer size (free)", 90.0, 33.3, 1.0, 999.0, kDfxParamUnit_ms, kDfxParamCurve_squared);
	initparameter_list(kBufferSize_sync, "forced buffer size (sync)", unitTempoRateIndex, unitTempoRateIndex, numTempoRates, kDfxParamUnit_beats);
	initparameter_b(kBufferTempoSync, "forced buffer tempo sync", false, false);
	initparameter_b(kBufferInterrupt, "buffer interrupt", true, true);
	initparameter_f(kDivisorLFOrate_abs, "divisor LFO rate (free)", 0.3, 3.0, 0.03, 21.0, kDfxParamUnit_hz, kDfxParamCurve_squared);
	initparameter_list(kDivisorLFOrate_sync, "divisor LFO rate (sync)", unitTempoRateIndex, unitTempoRateIndex, numTempoRates, kDfxParamUnit_beats);
	initparameter_f(kDivisorLFOdepth, "divisor LFO depth", 0.0, 0.0, 0.0, 100.0, kDfxParamUnit_percent);
	initparameter_list(kDivisorLFOshape, "divisor LFO shape", 0, 0, numLFOshapes);
	initparameter_b(kDivisorLFOtempoSync, "divisor LFO tempo sync", false, false);
	initparameter_f(kBufferLFOrate_abs, "buffer LFO rate (free)", 3.0, 3.0, 0.03, 21.0, kDfxParamUnit_hz, kDfxParamCurve_exp);//kDfxParamCurve_squared);
	initparameter_list(kBufferLFOrate_sync, "buffer LFO rate (sync)", unitTempoRateIndex, unitTempoRateIndex, numTempoRates, kDfxParamUnit_beats);
	initparameter_f(kBufferLFOdepth, "buffer LFO depth", 0.0, 0.0, 0.0, 100.0, kDfxParamUnit_percent);
	initparameter_list(kBufferLFOshape, "buffer LFO shape", 0, 0, numLFOshapes);
	initparameter_b(kBufferLFOtempoSync, "buffer LFO tempo sync", false, false);
	initparameter_f(kSmooth, "smooth", 9.0, 3.0, 0.0, 100.0, kDfxParamUnit_percent);
	initparameter_f(kDryWetMix, "dry/wet mix", 100.0, 50.0, 0.0, 100.0, kDfxParamUnit_drywetmix);
	initparameter_f(kPitchbend, "pitchbend range", 6.0, 3.0, 0.0, PITCHBEND_MAX, kDfxParamUnit_semitones);
	initparameter_list(kMidiMode, "MIDI mode", kMidiMode_nudge, kMidiMode_nudge, kNumMidiModes);
	initparameter_f(kTempo, "tempo", 120.0, 120.0, 57.0, 480.0, kDfxParamUnit_bpm);
	initparameter_b(kTempoAuto, "sync to host tempo", true, true);

	// set the value strings for the LFO shape parameters
	for (int i = 0; i < numLFOshapes; i++)
	{
		const char * shapename = divisorLFO->getShapeNameIndexed_ptr(i);
		setparametervaluestring(kDivisorLFOshape, i, shapename);
		setparametervaluestring(kBufferLFOshape, i, shapename);
	}
	// set the value strings for the sync rate parameters
	for (int i = 0; i < tempoRateTable->getNumTempoRates(); i++)
	{
		const char * tname = tempoRateTable->getDisplay(i);
		setparametervaluestring(kBufferSize_sync, i, tname);
		setparametervaluestring(kDivisorLFOrate_sync, i, tname);
		setparametervaluestring(kBufferLFOrate_sync, i, tname);
	}
	// set the value strings for the MIDI mode parameter
	setparametervaluestring(kMidiMode, kMidiMode_nudge, "nudge");
	setparametervaluestring(kMidiMode, kMidiMode_trigger, "trigger");

	settailsize_seconds( 1.0 / (tempoRateTable->getScalar(0) * kMinAllowableBPS) );


	// give currentTempoBPS a value in case that's useful for a freshly opened GUI
	currentTempoBPS = getparameter_f(kTempo) / 60.0;

	setpresetname(0, "self-determined");	// default preset name
	initPresets();
}

//-------------------------------------------------------------------------
BufferOverride::~BufferOverride()
{
	if (divisorLFO != NULL)
		delete divisorLFO;
	if (bufferLFO != NULL)
		delete bufferLFO;
}

//-------------------------------------------------------------------------
long BufferOverride::initialize()
{
	inputGain.setSampleRate(getsamplerate());
	outputGain.setSampleRate(getsamplerate());
	return kDfxErr_NoError;
}

//-------------------------------------------------------------------------
void BufferOverride::cleanup()
{
}

//-------------------------------------------------------------------------
void BufferOverride::reset()
{
	// setting the values like this will restart the forced buffer in the next process()
	currentForcedBufferSize = 1;
	writePos = readPos = 1;
	minibufferSize = 1;
	prevMinibufferSize = 0;
	smoothcount = smoothDur = 0;
	sqrtFadeIn = sqrtFadeOut = 1.0f;

	if (divisorLFO != NULL)
		divisorLFO->reset();
	if (bufferLFO != NULL)
		bufferLFO->reset();

	oldNote = false;
	lastNoteOn = kInvalidMidi;
	lastPitchbend = kInvalidMidi;
	pitchbend = 1.0;
	oldPitchbend = 1.0;
	divisorWasChangedByMIDI = divisorWasChangedByHand = false;

	needResync = true;	// some hosts may call resume when restarting playback
}

//-------------------------------------------------------------------------
bool BufferOverride::createbuffers()
{
//printf("\n(pre) sr = %.0f, super_max_buffer = %ld, numBuffers = %ld\n", getsamplerate(), SUPER_MAX_BUFFER, numBuffers);
	const long oldmax = SUPER_MAX_BUFFER;
	SUPER_MAX_BUFFER = (long) ( getsamplerate() / (kMinAllowableBPS * tempoRateTable->getScalar(0)) );
	const unsigned long oldnum = numBuffers;
	numBuffers = getnumoutputs();

	const bool result1 = dfx_createbufferarray(&buffers, oldnum, oldmax, numBuffers, SUPER_MAX_BUFFER);
	const bool result2 = dfx_createbuffer(&outval, oldnum, numBuffers);
/*
bool result1 = true;
if (buffers == NULL)
{
buffers = (float**) malloc(sizeof(float*) * 16);
for (unsigned long i = 0; i < 4; i++) buffers[i] = (float*) malloc(sizeof(float) * SUPER_MAX_BUFFER);
}
bool result2 = true;
//if (outval == NULL) outval = (float*) malloc(sizeof(float) * 16);
*/

	if (!result1 || !result2)
		return false;

//printf("(post) sr = %.0f, super_max_buffer = %ld, numBuffers = %ld\n\n", getsamplerate(), SUPER_MAX_BUFFER, numBuffers);
	// we were successful if we reached this point
	return true;
}

//-------------------------------------------------------------------------
void BufferOverride::releasebuffers()
{
	dfx_releasebufferarray(&buffers, numBuffers);
	dfx_releasebuffer(&outval);

	numBuffers = 0;
}


#pragma mark _________presets_________

//-------------------------------------------------------------------------
void BufferOverride::initPresets()
{
	int i = 1;

	setpresetname(i, "drum roll");
	setpresetparameter_f(i, kDivisor, 4.0);
	setpresetparameter_i(i, kBufferSize_sync, tempoRateTable->getNearestTempoRateIndex(4.0f));
	setpresetparameter_b(i, kBufferTempoSync, true);
	setpresetparameter_f(i, kSmooth, 9.0);
	setpresetparameter_f(i, kDryWetMix, getparametermax_f(kDryWetMix));
	setpresetparameter_i(i, kMidiMode, kMidiMode_nudge);
	setpresetparameter_b(i, kTempoAuto, true);
	i++;

	setpresetname(i, "arpeggio");
	setpresetparameter_f(i, kDivisor, 37.0);
	setpresetparameter_f(i, kBufferSize_abs, 444.0);
	setpresetparameter_b(i, kBufferTempoSync, false);
	setpresetparameter_b(i, kBufferInterrupt, true);
	setpresetparameter_f(i, kDivisorLFOrate_abs, 0.3);
	setpresetparameter_f(i, kDivisorLFOdepth, 72.0);
	setpresetparameter_i(i, kDivisorLFOshape, kLFOshape_saw);
	setpresetparameter_b(i, kDivisorLFOtempoSync, false);
	setpresetparameter_f(i, kBufferLFOrate_abs, 0.27);
	setpresetparameter_f(i, kBufferLFOdepth, 63.0);
	setpresetparameter_i(i, kBufferLFOshape, kLFOshape_saw);
	setpresetparameter_b(i, kBufferLFOtempoSync, false);
	setpresetparameter_f(i, kSmooth, 4.2);
	setpresetparameter_f(i, kDryWetMix, getparametermax_f(kDryWetMix));
	setpresetparameter_i(i, kMidiMode, kMidiMode_nudge);
	i++;

	setpresetname(i, "laser");
	setpresetparameter_f(i, kDivisor, 170.0);
	setpresetparameter_f(i, kBufferSize_abs, 128.0);
	setpresetparameter_b(i, kBufferTempoSync, false);
	setpresetparameter_b(i, kBufferInterrupt, true);
	setpresetparameter_f(i, kDivisorLFOrate_abs, 9.0);
	setpresetparameter_f(i, kDivisorLFOdepth, 87.0);
	setpresetparameter_i(i, kDivisorLFOshape, kLFOshape_thorn);
	setpresetparameter_b(i, kDivisorLFOtempoSync, false);
	setpresetparameter_f(i, kBufferLFOrate_abs, 5.55);
	setpresetparameter_f(i, kBufferLFOdepth, 69.0);
	setpresetparameter_i(i, kBufferLFOshape, kLFOshape_reversesaw);
	setpresetparameter_b(i, kBufferLFOtempoSync, false);
	setpresetparameter_f(i, kSmooth, 20.1);
	setpresetparameter_f(i, kDryWetMix, getparametermax_f(kDryWetMix));
	setpresetparameter_i(i, kMidiMode, kMidiMode_nudge);
	i++;

	setpresetname(i, "sour melodies");
	setpresetparameter_f(i, kDivisor, 42.0);
	setpresetparameter_f(i, kBufferSize_abs, 210.0);
	setpresetparameter_b(i, kBufferTempoSync, false);
	setpresetparameter_b(i, kBufferInterrupt, true);
	setpresetparameter_f(i, kDivisorLFOrate_abs, 3.78);
	setpresetparameter_f(i, kDivisorLFOdepth, 90.0);
	setpresetparameter_i(i, kDivisorLFOshape, kLFOshape_random);
	setpresetparameter_b(i, kDivisorLFOtempoSync, false);
	setpresetparameter_f(i, kBufferLFOdepth, 0.0);
	setpresetparameter_f(i, kSmooth, 3.9);
	setpresetparameter_f(i, kDryWetMix, getparametermax_f(kDryWetMix));
	setpresetparameter_i(i, kMidiMode, kMidiMode_nudge);
	i++;

	setpresetname(i, "rerun");
	setpresetparameter_f(i, kDivisor, 9.0);
	setpresetparameter_f(i, kBufferSize_abs, 747.0);
	setpresetparameter_b(i, kBufferTempoSync, false);
	setpresetparameter_f(i, kDivisorLFOrate_abs, getparametermin_f(kDivisorLFOrate_abs));
	setpresetparameter_f(i, kDivisorLFOdepth, 0.0);
	setpresetparameter_i(i, kDivisorLFOshape, kLFOshape_triangle);
	setpresetparameter_b(i, kDivisorLFOtempoSync, false);
	setpresetparameter_f(i, kBufferLFOrate_abs, 0.174);
	setpresetparameter_f(i, kBufferLFOdepth, 21.0);
	setpresetparameter_i(i, kBufferLFOshape, kLFOshape_triangle);
	setpresetparameter_b(i, kBufferLFOtempoSync, false);
	setpresetparameter_f(i, kSmooth, 8.1);
	setpresetparameter_f(i, kDryWetMix, getparametermax_f(kDryWetMix));
	setpresetparameter_i(i, kMidiMode, kMidiMode_nudge);
	i++;

	setpresetname(i, "jiggy");
	setpresetparameter_f(i, kDivisor, 4.0);
	setpresetparameter_i(i, kBufferSize_sync, tempoRateTable->getNearestTempoRateIndex(4.0f));
	setpresetparameter_b(i, kBufferTempoSync, true);
	setpresetparameter_b(i, kBufferInterrupt, true);
	setpresetparameter_i(i, kDivisorLFOrate_sync, tempoRateTable->getNearestTempoRateIndex(0.5f));
	setpresetparameter_f(i, kDivisorLFOdepth, 84.0);
	setpresetparameter_i(i, kDivisorLFOshape, kLFOshape_square);
	setpresetparameter_b(i, kDivisorLFOtempoSync, true);
	setpresetparameter_f(i, kBufferLFOdepth, 0.0);
	setpresetparameter_f(i, kSmooth, 9.0);	// eh?
	setpresetparameter_f(i, kDryWetMix, 100.0);
	setpresetparameter_i(i, kMidiMode, kMidiMode_nudge);
	setpresetparameter_b(i, kTempoAuto, true);
	i++;

	setpresetname(i, "aliens are coming");
	setpresetparameter_f(i, kDivisor, 5.64);
	setpresetparameter_f(i, kBufferSize_abs, 31.25);
	setpresetparameter_b(i, kBufferTempoSync, false);
	setpresetparameter_b(i, kBufferInterrupt, true);
	setpresetparameter_f(i, kDivisorLFOdepth, 0.0);
	setpresetparameter_f(i, kBufferLFOrate_abs, 1.53);
	setpresetparameter_f(i, kBufferLFOdepth, 99.0);
	setpresetparameter_i(i, kBufferLFOshape, kLFOshape_sine);
	setpresetparameter_b(i, kBufferLFOtempoSync, false);
	setpresetparameter_f(i, kSmooth, 9.0);
	setpresetparameter_f(i, kDryWetMix, 100.0);
	setpresetparameter_i(i, kMidiMode, kMidiMode_nudge);
	i++;

	setpresetname(i, "\"echo\"");
	setpresetparameter_f(i, kDivisor, 2.001);
	setpresetparameter_f(i, kBufferSize_abs, 603.0);
	setpresetparameter_b(i, kBufferTempoSync, false);
	setpresetparameter_f(i, kDivisorLFOdepth, 0.0);
	setpresetparameter_f(i, kBufferLFOdepth, 0.0);
	setpresetparameter_f(i, kSmooth, getparametermax_f(kSmooth));
	setpresetparameter_f(i, kDryWetMix, getparametermax_f(kDryWetMix));
	setpresetparameter_i(i, kMidiMode, kMidiMode_nudge);
	i++;

	setpresetname(i, "squeegee");
	setpresetparameter_f(i, kDivisor, 27.0);
	setpresetparameter_f(i, kBufferSize_abs, 81.0);
	setpresetparameter_b(i, kBufferTempoSync, false);
	setpresetparameter_b(i, kBufferInterrupt, true);
	setpresetparameter_i(i, kDivisorLFOrate_sync, tempoRateTable->getNearestTempoRateIndex(2.0f));
	setpresetparameter_f(i, kDivisorLFOdepth, 33.3);
	setpresetparameter_i(i, kDivisorLFOshape, kLFOshape_sine);
	setpresetparameter_b(i, kDivisorLFOtempoSync, true);
	setpresetparameter_i(i, kBufferLFOrate_sync, 0);
	setpresetparameter_f(i, kBufferLFOdepth, 6.0);
	setpresetparameter_i(i, kBufferLFOshape, kLFOshape_saw);
	setpresetparameter_b(i, kBufferLFOtempoSync, true);
	setpresetparameter_f(i, kSmooth, 6.0);
	setpresetparameter_f(i, kDryWetMix, getparametermax_f(kDryWetMix));
	setpresetparameter_i(i, kMidiMode, kMidiMode_nudge);
	setpresetparameter_b(i, kTempoAuto, true);
	i++;

/*
char mook[DFX_PRESET_MAX_NAME_LENGTH];
for (i = 0; i < 396; i++)
{
	sprintf(mook, "%03d => %s", i, getpresetname_ptr(i));
	setpresetname(i, mook);
}
*/
/*
	setpresetname(i, "");
	setpresetparameter_f(i, kDivisor, );
	setpresetparameter_f(i, kBufferSize_abs, );
	setpresetparameter_i(i, kBufferSize_sync, tempoRateTable->getNearestTempoRateIndex(f));
	setpresetparameter_b(i, kBufferTempoSync, );
	setpresetparameter_b(i, kBufferInterrupt, );
	setpresetparameter_f(i, kDivisorLFOrate_abs, );
	setpresetparameter_i(i, kDivisorLFOrate_sync, tempoRateTable->getNearestTempoRateIndex(f));
	setpresetparameter_f(i, kDivisorLFOdepth, );
	setpresetparameter_i(i, kDivisorLFOshape, kLFOshape_);
	setpresetparameter_b(i, kDivisorLFOtempoSync, );
	setpresetparameter_f(i, kBufferLFOrate_abs, );
	setpresetparameter_i(i, kBufferLFOrate_sync, tempoRateTable->getNearestTempoRateIndex(f));
	setpresetparameter_f(i, kBufferLFOdepth, );
	setpresetparameter_i(i, kBufferLFOshape, kLFOshape_);
	setpresetparameter_b(i, kBufferLFOtempoSync, );
	setpresetparameter_f(i, kSmooth, );
	setpresetparameter_f(i, kDryWetMix, );
	setpresetparameter_d(i, kPitchbend, );
	setpresetparameter_i(i, kMidiMode, kMidiMode_);
	setpresetparameter_f(i, kTempo, );
	setpresetparameter_b(i, kTempoAuto, );
	i++;
*/
}


#pragma mark _________parameters_________

//-------------------------------------------------------------------------
void BufferOverride::processparameters()
{
	divisor = getparameter_f(kDivisor);
	bufferSizeMs = getparameter_f(kBufferSize_abs);
	bufferSizeSync = tempoRateTable->getScalar(getparameter_i(kBufferSize_sync));
	bufferTempoSync = getparameter_b(kBufferTempoSync);
	bufferInterrupt = getparameter_b(kBufferInterrupt);
	divisorLFO->fRate = getparameter_f(kDivisorLFOrate_abs);
	divisorLFO->fTempoRate = tempoRateTable->getScalar(getparameter_i(kDivisorLFOrate_sync));
	divisorLFO->fDepth = getparameter_scalar(kDivisorLFOdepth);
	divisorLFO->iShape = getparameter_i(kDivisorLFOshape);
	divisorLFO->bTempoSync = getparameter_b(kDivisorLFOtempoSync);
	bufferLFO->fRate = getparameter_f(kBufferLFOrate_abs);
	bufferLFO->fTempoRate = tempoRateTable->getScalar(getparameter_i(kBufferLFOrate_sync));
	bufferLFO->fDepth = getparameter_scalar(kBufferLFOdepth);
	bufferLFO->iShape = getparameter_i(kBufferLFOshape);
	bufferLFO->bTempoSync = getparameter_b(kBufferLFOtempoSync);
	smooth = getparameter_scalar(kSmooth);
	dryWetMix = getparameter_scalar(kDryWetMix);
	pitchbendRange = getparameter_f(kPitchbend);
	midiMode = getparameter_i(kMidiMode);
	userTempo = getparameter_f(kTempo);
	useHostTempo = getparameter_b(kTempoAuto);

	if (getparameterchanged(kDivisor))
		// tell MIDI trigger mode to respect this change
		divisorWasChangedByHand = true;
	if (getparameterchanged(kBufferSize_sync))
		// make sure the cycles match up if the tempo rate has changed
		needResync = true;
	if ( getparameterchanged(kBufferTempoSync) && bufferTempoSync )
		// set needResync true if tempo sync mode has just been switched on
		needResync = true;
	if (getparameterchanged(kMidiMode))
	{
		// reset all notes to off if we're switching into MIDI trigger mode
		if (midiMode == kMidiMode_trigger)
		{
			midistuff->removeAllNotes();
			divisorWasChangedByHand = false;
		}
	}
	if ( getparameterchanged(kTempoAuto) && useHostTempo)
		// set needResync true if host sync has just been switched on
		needResync = true;
}
