#include "AUEffectBase.h"				//routines from apple :-)
#include "AllPassVersion.h"				//defines version no. company name etc

//----- Not sure what this does
#if AU_DEBUG_DISPATCHER
	#include "AUDebugDispatcher.h"
#endif

#ifndef __AllPass_h__
#define __AllPass_h__
//----------

//........................
// Constancs for parameters and presets go here
//........................

static CFStringRef kParamName_AllPass_Freq 		= CFSTR("Frequency");		//name of the "frequency" paramter (knob, slider)
static const float kDefaultValue_AllPass_Freq	= 50.0;						// default value for "frequency" parameter
static const float kMinimumValue_AllPass_Freq 	= 2.0;						// minimum value for "frequency" parameter
static const float kMaximumValue_AllPass_Freq 	= 20.0;						// maxium value

enum {	//defines constants for identifying the parameters
	kParameter_Frequency		= 0	
};
//..........................


class AllPassFilter : public AUEffectBase {

public:
	AllPassFilter(AudioUnit component);
	
#if AU_DEBUG_DISPATCHER
	virtual ~AllPassFilter () { delete mDebugDispatcher; }		// don't know what this is for :-)
#endif
	
	virtual AUKernelBase* NewKernel () { return new AllPassFilterKernel(this);}		// DSP goes in the AllPassFilterKernel helper class, defined in AllPassFilter.cpp
	
	virtual ComponentResult GetParameterValueStrings (		//A component result is just a signed integer.
			AudioUnitScope			inScope.
			AudioUnitParameterID 	inParameterID,
			CFArrayRef				*outStrings
	);
	
	virtual ComponentResult GetParameterInfo (
			AudioUnitScope			inScope,
			AudioUnitParameterID	inParameterID,
			AudioUnitParameterInfo	&outParameterInfo
	);
	
	virtual ComponentResult GetPropertyInfo (
			AudioUnitPropertyID		inID,
			AudioUnitSScope			inScope,
			AudioUnitElement		inElement,
			UInt32					&outDataSize,
			Boolean					&outWritable
	);
	
	virtual ComponentResult GetProperty (
			AudioUnitPropertyID		inID,
			AudioUnitScope			inScope,
			AudioUnitElement		inElement,
			void					*outData
	);
	
	// report that the audio unit supports the kAudioUnitProperty_TailTime property
	virtual bool SupportsTail() { return true; }
	
	//provide version info
	virtual ComponentResult Version() { return kAllPassVersion; }		// defined in AllPassVersion.h
		
protected:
	class AllPassFilterKernel : public AUKernelBase {
		public:
			AllPassFilterKernel (AUEffectBase *inAudioUnit);
			
			// *Required* overrides for the process method for this effect
			// processes one channel of interleaved samples
			virtual void Process (
				const Float32	*inSourceP,
				Float32			*inDestP,
				UInt32			inFramesToProcess,
				UInt32			inNumChannels,	//equal to 1
				bool			&ioSilence
			);
			
			virtual void Reset ();
			
			private:		// variables used by the AU go here such as:
				// EXAMPLES
				enum	{kWaveArraySize = 2000};	// The number of points in the wave table.
				float	mSine [kWaveArraySize];		// The wave table for a sine wave 
				float 	mSquare [kWaveArraySize];	// the wave table for a square wave
				float	*waveArrayPointer;			// points to the wave table to use for the current audio input buffer.
				Float32 mSSampleFrequency;			// the "sample rate" of the audio signal being processed
				long 	mSamplesProcessed;			// the number of samples processed since the audio unit
													//	started rendering or since this variable was last
													//  reset to 0. We have to keep track of this because
													//  we vary the effect continuously and independently 
													//  of the input buffer size.
				enum {sampleLimit = (int) 10E6		// to keep the value of mSamplesProcesed within a
													//  reasonable limit. 10E6 is equivalent to the number
													//  of samples in 100 seconds of 96 khz audio.
				float mCurrentScale					// There are two scaling factors to allow the audio unit
													//  to switch to a new scaling factor at the beginning
													//  of the waveform, no matter when the user 
													//  changes the frequency. mCurrentScale is
													//  the scaling factor in use.
				float mNextScale					// the scaling factor that the user most recently requested
													//  by moving the frequency slider.
	};
};