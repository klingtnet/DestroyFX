/*------------------------------------------------------------------------
Copyright (C) 2000-2009  Sophia Poirier

This file is part of Skidder.

Skidder is free software:  you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation, either version 3 of the License, or 
(at your option) any later version.

Skidder is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with Skidder.  If not, see <http://www.gnu.org/licenses/>.

To contact the author, use the contact form at http://destroyfx.org/
------------------------------------------------------------------------*/

#include "skidder.h"


// this macro does boring entry point stuff for us
DFX_EFFECT_ENTRY(Skidder)

//-----------------------------------------------------------------------------
// initializations and such
Skidder::Skidder(TARGET_API_BASE_INSTANCE_TYPE inInstance)
	: DfxPlugin(inInstance, kNumParameters, kNumPresets)	// 16 parameters, 16 presets
{
	noteTable = NULL;

	tempoRateTable = new TempoRateTable;


	// initialize the parameters
	long unitTempoRateIndex = tempoRateTable->getNearestTempoRateIndex(1.0f);
	long numTempoRates = tempoRateTable->getNumTempoRates();
	initparameter_f(kRate_abs, "rate (free)", 3.0, 3.0, 0.3, 21.0, kDfxParamUnit_hz, kDfxParamCurve_log);
	initparameter_list(kRate_sync, "rate (sync)", unitTempoRateIndex, unitTempoRateIndex, numTempoRates, kDfxParamUnit_beats);
	initparameter_f(kRateRandMin_abs, "rate random min (free)", 3.0, 3.0, 0.3, 21.0, kDfxParamUnit_hz, kDfxParamCurve_log);
	initparameter_list(kRateRandMin_sync, "rate random min (sync)", unitTempoRateIndex, unitTempoRateIndex, numTempoRates, kDfxParamUnit_beats);
	initparameter_b(kTempoSync, "tempo sync", false, false);
	initparameter_f(kPulsewidth, "pulsewidth", 0.5, 0.5, 0.001, 0.999, kDfxParamUnit_scalar);
	initparameter_f(kPulsewidthRandMin, "pulsewidth random min", 0.5, 0.5, 0.001, 0.999, kDfxParamUnit_scalar);
	initparameter_f(kSlope, "slope", 3.0, 3.0, 0.0, 300.0, kDfxParamUnit_ms, kDfxParamCurve_squared);
	initparameter_f(kPan, "stereo spread", 0.0, 0.6, 0.0, 1.0, kDfxParamUnit_scalar);
	initparameter_f(kFloor, "floor", 0.0, 0.0, 0.0, 1.0, kDfxParamUnit_lineargain, kDfxParamCurve_cubed);
	initparameter_f(kFloorRandMin, "floor random min", 0.0, 0.0, 0.0, 1.0, kDfxParamUnit_lineargain, kDfxParamCurve_cubed);
	initparameter_f(kNoise, "rupture", 0.0, 18732.0, 0.0, 18732.0, kDfxParamUnit_lineargain, kDfxParamCurve_squared);
	initparameter_list(kMidiMode, "MIDI mode", kMidiMode_none, kMidiMode_none, kNumMidiModes);
	initparameter_b(kVelocity, "velocity", false, false);
	initparameter_f(kTempo, "tempo", 120.0, 120.0, 39.0, 480.0, kDfxParamUnit_bpm);
	initparameter_b(kTempoAuto, "sync to host tempo", true, true);

	// set the value strings for the sync rate parameters
	for (int i=0; i < tempoRateTable->getNumTempoRates(); i++)
	{
		const char * tname = tempoRateTable->getDisplay(i);
		setparametervaluestring(kRate_sync, i, tname);
		setparametervaluestring(kRateRandMin_sync, i, tname);
	}
	// set the value strings for the MIDI modes
	setparametervaluestring(kMidiMode, kMidiMode_none, "none");
	setparametervaluestring(kMidiMode, kMidiMode_trigger, "trigger");
	setparametervaluestring(kMidiMode, kMidiMode_apply, "apply");


	setpresetname(0, "thip thip thip");	// default preset name

	// allocate memory for this array
	noteTable = (int*) malloc(NUM_NOTES * sizeof(int));

	// since we don't use pitchbend for anything special, 
	// allow it be assigned to control parameters
	dfxsettings->setAllowPitchbendEvents(true);

	addchannelconfig(-1, -1);	// N-in/N-out
	addchannelconfig(1, 2);	// 1-in/2-out

	// give currentTempoBPS a value in case that's useful for a freshly opened GUI
	currentTempoBPS = getparameter_f(kTempo) / 60.0;

	// start off with split CC automation of both range slider points
	rateDoubleAutomate = pulsewidthDoubleAutomate = floorDoubleAutomate = false;
}

//-----------------------------------------------------------------------------------------
Skidder::~Skidder()
{
	// deallocate the memory from this array
	if (noteTable != NULL)
		free(noteTable);
	noteTable = NULL;
}

//-----------------------------------------------------------------------------------------
void Skidder::reset()
{
	state = kSkidState_Valley;
	valleySamples = 0;
	panGainL = panGainR = 1.0f;
	rms = 0.0;
	rmscount = 0;
	randomFloor = 0.0f;
	randomGainRange = 1.0f;

	resetMidi();

	needResync = true;	// some hosts may call resume when restarting playback
	waitSamples = 0;
}

//-----------------------------------------------------------------------------------------
void Skidder::processparameters()
{
	rateHz = getparameter_f(kRate_abs);
	rateIndex = getparameter_i(kRate_sync);
	rateSync = tempoRateTable->getScalar(rateIndex);
	rateRandMinHz = getparameter_f(kRateRandMin_abs);
	rateRandMinIndex = getparameter_i(kRateRandMin_sync);
	rateRandMinSync = tempoRateTable->getScalar(rateRandMinIndex);
	tempoSync = getparameter_b(kTempoSync);
	pulsewidth = getparameter_f(kPulsewidth);
	pulsewidthRandMin = getparameter_f(kPulsewidthRandMin);
	slopeSeconds = getparameter_f(kSlope) * 0.001;
	panWidth = getparameter_f(kPan);
	noise = getparameter_scalar(kNoise);
	midiMode = getparameter_i(kMidiMode);
	useVelocity = getparameter_b(kVelocity);
	floor = getparameter_f(kFloor);
	floorRandMin = getparameter_f(kFloorRandMin);
	userTempo = getparameter_f(kTempo);
	useHostTempo = getparameter_b(kTempoAuto);

	// get the "generic" values of these parameters for randomization
	rateHz_gen = getparameter_gen(kRate_abs);
	rateRandMinHz_gen = getparameter_gen(kRateRandMin_abs);
	floor_gen = getparameter_gen(kFloor);
	floorRandMin_gen = getparameter_gen(kFloorRandMin);

	// set needResync true if tempo sync mode has just been switched on
	if ( getparameterchanged(kTempoSync) && tempoSync )
		needResync = true;
	if (getparameterchanged(kRate_sync))
		needResync = true;
	if (getparameterchanged(kMidiMode))
	{
		// if we've just entered a MIDI mode, zero out all notes and reset waitSamples
// XXX fetch old value
//		if ( (oldMidiMode == kMidiMode_none) && 
//				(midiMode != kMidiMode_none) )
		{
			for (int i=0; i < NUM_NOTES; i++)
				noteTable[i] = 0;
			noteOff();
		}
	}

	gainRange = 1.0f - floor;	// the range of the skidding on/off gain
	if (tempoSync)
		useRandomRate = (rateRandMinSync < rateSync);
	else
		useRandomRate = (rateRandMinHz < rateHz);
	useRandomFloor = (floorRandMin < floor);
	useRandomPulsewidth = (pulsewidthRandMin < pulsewidth);
}
