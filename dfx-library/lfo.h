/*------------------------------------------------------------------------
Destroy FX Library (version 1.0) is a collection of foundation code 
for creating audio software plug-ins.  
Copyright (C) 2002-2009  Sophia Poirier

This program is free software:  you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation, either version 3 of the License, or 
(at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with this program.  If not, see <http://www.gnu.org/licenses/>.

To contact the author, please visit http://destroyfx.org/ 
and use the contact form.

Destroy FX is a sovereign entity comprised of Sophia Poirier and Tom Murphy 7.
Welcome to our LFO.
by Sophia Poirier  ][  January 2002
------------------------------------------------------------------------*/

#ifndef __DFX_LFO_H
#define __DFX_LFO_H

#include <math.h>
#include <stdlib.h>


//-------------------------------------------------------------------------------------
// these are the 8 LFO waveforms:
enum
{
	kLFOshape_sine,
	kLFOshape_triangle,
	kLFOshape_square,
	kLFOshape_saw,
	kLFOshape_reversesaw,
	kLFOshape_thorn,
	kLFOshape_random,
	kLFOshape_randominterpolating,

	numLFOshapes
};

//-------------------------------------------------------------------------------------
// constants and macros

//#define LFOshapeScaled(value)   ( (long)((value) * ((float)numLFOshapes-0.01f)) )
//#define LFOshapeUnscaled(step)   ( (float)(step) / ((float)(numLFOshapes-1)) )

const long NUM_LFO_POINTS = 512;
const float NUM_LFO_POINTS_FLOAT = (float)NUM_LFO_POINTS;	// to reduce casting later on
const float LFO_TABLE_STEP = 1.0f / (float)NUM_LFO_POINTS;	// to reduce division and encourage multiplication
const long SQUARE_HALF_POINT = NUM_LFO_POINTS / 2;	// the point in the table when the square waveform drops to zero

const long LFO_SMOOTH_DUR = 48;
const float LFO_SMOOTH_STEP = 1.0f / (float)LFO_SMOOTH_DUR;


//----------------------------------------------------------------------------- 
class LFO
{
public:
	LFO();
	~LFO();

	void reset();
	void fillLFOtables();

	void pickTheLFOwaveform();
	void getShapeName(char * outNameString);
	void getShapeNameIndexed(long inIndex, char * outNameString);
	const char * getShapeNameIndexed_ptr(long inIndex);

	void syncToTheBeat(long inSamplesToBar);

	// the LFO waveform tables
	float * sineTable, * triangleTable, * squareTable, * sawTable, * reverseSawTable, * thornTable;

	// the following are intended to be used as 0.0 - 1.0 VST parameter values:
	bool bOnOff;	// parameter value for turning the LFO on or off
	bool bTempoSync;	// parameter value for toggling tempo sync
	float fRate;	// parameter value for LFO rate (in Hz)
	float fTempoRate;	// parameter value for LFO rate (in cycles per beat)
	float fDepth;	// parameter value LFO depth
	long iShape;	// parameter value for LFO shape

	float position;	// this tracks the position in the LFO table
	float stepSize;	// size of the steps through the LFO table
	float * table;	// pointer to the LFO table
	float randomNumber;	// this stores random values for the random LFO waveforms
	float oldRandomNumber;	// this stores previous random values for the random interpolating LFO waveform
	float cycleRate;	// the rate in Hz of the LFO (only used for first layer LFOs)
	long smoothSamples;	// a counter for the position during a smoothing fade
	long granularityCounter;	// a counter for implementing LFO processing on a block basis
	long granularity;	// the number of samples to wait before processing


	//--------------------------------------------------------------------------------------
	// This function wraps around the LFO table position when it passes the cycle end.
	// It also sets up the smoothing counter if a discontiguous LFO waveform is being used.
	void updatePosition(long inNumSteps = 1)
	{
		// increment the LFO position tracker
		position += (stepSize * (float)inNumSteps);

		if (position >= NUM_LFO_POINTS_FLOAT)
		{
			// wrap around the position tracker if it has made it past the end of the LFO table
			position = fmodf(position, NUM_LFO_POINTS_FLOAT);
			// get new random LFO values, too
			oldRandomNumber = randomNumber;
			randomNumber = (float)rand() / (float)RAND_MAX;
			// set up the sample smoothing if a discontiguous waveform's cycle just ended
			switch (iShape)
			{
				case kLFOshape_square     :
				case kLFOshape_saw        :
				case kLFOshape_reversesaw :
				case kLFOshape_random     :
					smoothSamples = LFO_SMOOTH_DUR;
				default:
					break;
			}
		}
		else if (position < 0.0f)
			position = 0.0f;

		// special check for the square waveform - it also needs smoothing at the half point
		else if (iShape == kLFOshape_square)
		{
			// check to see if it has just passed the halfway point
			if ( ((long)position >= SQUARE_HALF_POINT) && 
				 ((long)(position - stepSize) < SQUARE_HALF_POINT) )
				smoothSamples = LFO_SMOOTH_DUR;
		}
	}

	//--------------------------------------------------------------------------------------
	// this function gets the current 0.0 - 1.0 output value of the LFO and increments its position
	float processLFO()
	{
	  float randiScalar, outValue;

		if (iShape == kLFOshape_randominterpolating)
		{
			// calculate how far into this LFO cycle we are so far, scaled from 0.0 to 1.0
			randiScalar = position * LFO_TABLE_STEP;
			// interpolate between the previous random number and the new one
			outValue = (randomNumber * randiScalar) + (oldRandomNumber * (1.0f-randiScalar));
		}
		//
		else if (iShape == kLFOshape_random)
			outValue = randomNumber;
		//
		else
			outValue = table[(long)position];

		return (outValue * fDepth);
	}

	//--------------------------------------------------------------------------------------
	// this scales the return of processLFO() from 0.0 - 1.0 output to 0.0 - 2.0 (oscillating around 1.0)
	float processLFOzero2two()
	{
		return ( (processLFO() * 2.0f) - fDepth + 1.0f );
	}

};


#endif
