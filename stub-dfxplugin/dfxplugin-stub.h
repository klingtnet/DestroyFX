/*------------------------------------------------------------------------
Destroy FX is a sovereign entity comprised of Sophia Poirier and Tom Murphy 7.  
This is our unexciting, but informative, demonstration DfxPlugin.
written by Sophia Poirier, October 2002
------------------------------------------------------------------------*/

#ifndef __DFXPLUGIN_STUB_H
#define __DFXPLUGIN_STUB_H


#include "dfxplugin.h"


//----------------------------------------------------------------------------- 
// these are the plugin parameters:
enum
{
	kFloatParam,
	kIntParam,
	kIndexParam,
	kBooleanParam,

	kNumParameters
};


//----------------------------------------------------------------------------- 
// constants and helpfuls

const long kNumPresets = 16;

// audio buffer size in seconds
const double kBufferSize_seconds = 3.0;
// function for getting samples from seconds
long buffersize_sec2samples(float inSeconds)
{
	return (long) (inSeconds * getsamplerate_f());
}

// the different states of the indexed parameter
enum {
	kIndexParamState1,
	kIndexParamState2,
	kIndexParamState3,
	kNumIndexParamStates
};


//----------------------------------------------------------------------------- 
class DfxStub : public DfxPlugin
{
public:
	DfxStub(TARGET_API_BASE_INSTANCE_TYPE inInstance);
	virtual ~DfxStub();

	virtual long initialize();
	virtual void cleanup();
	virtual void reset();

#if !TARGET_PLUGIN_USES_DSPCORE
	virtual void processaudio(const float ** in, float ** out, unsigned long inNumFrames, bool replacing=true);
	virtual void processparameters();

	virtual bool createbuffers();
	virtual void releasebuffers();
	virtual void clearbuffers();
#endif

private:
	void initPresets();

#if !TARGET_PLUGIN_USES_DSPCORE
	// handy usable copies of the parameters
	float floatParam;
	long intParam, indexParam;
	bool booleanParam;

	// stuff needed for audio processing
	float ** buffers;	// a 2-dimensional array of audio buffers
	unsigned long numbuffers;	// number of buffers allocated in the 2D buffer array
	long buffersize;	// size of each buffer in the audio buffers array
	long bufferpos;	// position in the buffer
#endif

};



#if TARGET_PLUGIN_USES_DSPCORE
//----------------------------------------------------------------------------- 
class DfxStubDSP : public DfxPluginCore
{
public:
	DfxStubDSP(TARGET_API_CORE_INSTANCE_TYPE *inInstance);
	virtual ~DfxStubDSP();

	virtual void reset();
	virtual bool createbuffers();
	virtual void releasebuffers();
	virtual void clearbuffers();

	virtual void processparameters();
	virtual void process(const float * in, float * out, unsigned long inNumFrames, bool replacing=true);

private:
	// handy usable copies of the parameters
	float floatParam;
	long intParam, indexParam;
	bool booleanParam;

	// stuff needed for audio processing
	float * buffer;	// an audio buffer
	long buffersize;	// size of the audio buffer
	long bufferpos;	// position in the buffer
};
#endif
// end TARGET_PLUGIN_USES_DSPCORE


#endif
// defining __DFXPLUGIN_STUB
