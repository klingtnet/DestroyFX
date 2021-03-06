
/* Windowingstub, starring the Super Destroy FX Windowing System! */

#ifndef __DFX_EXEMPLAR_H
#define __DFX_EXEMPLAR_H

#include <dfxplugin.h>
#include "ANN/ANN.h"
#include "rfftw.h"

/* change these for your plugins */
#define PLUGIN Exemplar
#define NUM_PRESETS 16

#define BUFFERSIZESSIZE 14
#define MAXBUFFERSIZE 32768
const long buffersizes[BUFFERSIZESSIZE] = { 
  4, 8, 16, 32, 64, 128, 256, 512, 
  1024, 2048, 4096, 8192, 16384, 32768, 
};


// PLUGIN ## DSP
#define PLUGINCORE ExemplarDSP

/* the types of window shapes available for smoothity */
enum { WINDOW_TRIANGLE, 
       WINDOW_ARROW, 
       WINDOW_WEDGE, 
       WINDOW_COS, 
       NUM_WINDOWSHAPES,
       MAX_WINDOWSHAPES=16
};

enum { MODE_CAPTURE,
       MODE_MATCH,
       NUM_MODES, };

enum { FFTR_AUDIBLE,
       FFTR_ALL,
       NUM_FFTRS, };

/* the names of the parameters */
enum { P_BUFSIZE, P_SHAPE, 
       P_MODE, P_FFTRANGE,
       P_ERRORAMOUNT,
       NUM_PARAMS,
};


class PLUGIN : public DfxPlugin {
public:
  PLUGIN(TARGET_API_BASE_INSTANCE_TYPE inInstance);
  virtual ~PLUGIN();

private:
  /* set up the built-in presets */
  void makepresets();
};

class PLUGINCORE : public DfxPluginCore {
public:
  PLUGINCORE(DfxPlugin * inInstance);
  virtual ~PLUGINCORE();

  virtual void reset();
  virtual void processparameters();
  virtual void process(const float *in, float *out, unsigned long inNumFrames, bool replacing=true);

  long getwindowsize() { return third; }

 private:
 

  /* input and output buffers. out is framesize*2 samples long, in is framesize
     samples long. (for maximum framesize)
  */
  float * in0, * out0;

  /* bufsize is 3 * third, framesize is 2 * third 
     bufsize is used for outbuf.
  */
  long bufsize, framesize, third;

  void processw(float * in, float * out, long samples);

  int shape;

  /* third-sized tail of previous processed frame. already has mixing envelope
     applied.
   */
  float * prevmix;

  /* number of samples in in0 */
  int insize;

  /* number of samples and starting position of valid samples in out0 */
  int outsize;
  int outstart;



  /* Exemplar stuff */
  /* classifies a window */
  void classify(float * in, float & scale, ANNpoint & out, long samples);

  /* specific classifiers */
  void classify_haar(float * in, float & scale, ANNpoint & out, long samples);
  void classify_fft (float * in, float & scale, ANNpoint & out, long samples);


  bool capturemode;
  float erroramount;

#define CAPBUFFER 500000 /* 1000000 */
  float capsamples[CAPBUFFER];
  int ncapsamples; /* < CAPBUFFER */
  
#if 0
  /* an individual capture */
  struct caplet {
    /* its classification */
    ANNpoint p;
    /* its index in capsamples */
    int i;
    /* width? */
  };
#endif

  ANNpoint cap_point[CAPBUFFER];
  int cap_index[CAPBUFFER];
  float cap_scale[CAPBUFFER];
  int npoints;

  ANNkd_tree * nntree;

  /* for FFTW: current analysis plan */
  rfftw_plan plan;
  rfftw_plan rplan;

  /* result of ffts ( */
  float fftr[MAXBUFFERSIZE];

};

#endif
