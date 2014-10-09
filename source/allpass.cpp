// This file defines the AllPassFilter class, as well as the AllPassFilterKernel helper class.

#include "allpas.h"

//........................................
AUDIOCOMPONENT_ENTRY(AUBaseFactory, AllPassFilter)
//........................................
// The COMPONENT_ENTRY macro is required for the Mac OS X Component Manager to recognize and
// use the audio unit

//........................................
// AllPassFilter::AllPassFilter
//........................................
// The constructor for new AllPassFilter audio units
AllPassFilter::AllPassFilter (AudioUnit component) : AUEffectBase (component) {

	// This method, defined in the AUBase superclass, insures that the required audio unit
	// elements are created and initialized.
	CreateElements ();
	
	// Invokes the use of an STL vector for parameter access.
	// See AUBase/AUScopeElement.cpp
	Globals () -> UseIndexedParameters (kNumberOfParameters);
	
	// During instantiation, sets up the parameters according to their defaults.
	// The parameter defaults should correspond to the settings for the default preset.
	SetParameter (
		kParameter_Frequency,
		kDefaultValue_AllPass_Freq
	);
	
	// Also during instantiation, sets the preset menu to indicate the default preset,
	// which corresponds to the default parameters. It's possible to set this so a
	// fresh audio unit indicates the wrong preset, so be careful to get it right.
	SetAFactoryPresetAsCurrent (
		kPresets [kPreset_Default]
	);
	
	#if AU_DEBUG_DIPATCHER
		mDebugDispatcher = new AUDebugDispatcher(this);
	#endif
}

//.....................................
// AllPassFilter::GetParameterInfo
//.....................................
// Called by the audio unit's view; provides information needed for the view to display the
// audio unit's parameters
ComponentResult AllPassFilter::GetParameterInfo (
			AudioUnitScope			inScope,
			AudioUnitParameterID	inParameterID,
			AudioUnitParameterInfo	&outParameterInfo
) {
	ComponentResult result = noErr;
	
	// Adds two flags to all parameters for the audio unit, indicating to the host application
	// that it should consider all the audio unit's parameters to be readable and writeable
	outParameterInfo.flags =
	kAudioUnitParameterFlag_IsWriteable | kAudioUnitParameterFlag_IsReadable;
	
	// All parameters for this audio unit are in the "global" scope
	if (inScope == kAudioUnitScope_Global) {
		switch(inParameterID) {
		
			case kParameter_Frequency:
			// Invoked when the view needs information for the kTremeloParam_Frequency
			// parameter; defines how to represent this parameter in the user interface.
				AUBase::FillInParameterName (
					outParameterInfo,
					kParamName_AllPass_Freq,
					false
				);
				outParameterInfo.unit				= kAudioUnitParameterUnit_Hertz;
					// Sets the unit of measurement for the Frequency parameter to Hertz
				outParameterInfo.minValue			= kMinimumValue_AllPass_Freq;
					// Sets the minimum value for the Frequency parameter
				outParameterInfo.maxValue			= kMaximumValue_AllPass_Freq;
				outParameterInfo.defaultValue		= kDefaultValue_AllPass_Freq;
				outParameterInfo.flags 				|= kAudioUnitParameterFlag_DisplayLogarithmic;
					// Adds a flag to indicate to the host that it should use a logarithmic
					// control for the frequency parameter
				break;
				
				default:
					result = kAudioUnitErr_InvalidParameter;
					break;
		}
	} else {
		result = kAudioUnitErr_InvalidParameter;
	}
	return result;
}

//...................................
// AllPassFilter::GetPropertyInfo
//...................................
ComponentResult TremoloUnit::GetPropertyInfo (
	// This audio unit doesn't define any custom properties, so it uses this generic code for
	// this method.
	AudioUnitPropertyID	inID,
	AudioUnitScope		inScope,
	AudioUnitElement	inElement,
	UInt32				&outDataSize,
	Boolean				&outWritable
) {
	return AUEffectBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}

//...................................
// AllPassFilter::GetProperty
//...................................
ComponentResult AllPassFilter::GetProperty (
	// This audio unit doesn't define any custom properties, so it uses this generic code for
	// this method.
	AudioUnitPropertyID inID,
	AudioUnitScope		inScope,
	AudioUnitElement	inElement,
	void				*outData
) {
	return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}
	
//.....................................
// AllPassFilter::AllPassFilterKernel::AllPassFilterKernel()
//.....................................
// This is the constructor for the AllPassFilterKernel helper class, which holds the DSP code 
//  for the audio unit. AllPassFilter is an n-to-n audio unit; one kernel object gets built for 
//  each channel in the audio unit.
//
// The first line of the method consists of the constructor method declarator and constructor-
//  initializer. In addition to calling the appropriate superclasses, this code initializes two 
//  member variables:
//
// mCurrentScale:		a factor for correlating points in the current wave table to
//						the audio signal sampling frequency.
// mSamplesProcessed:	a global count of samples processed. it allows the effect
//						to be continuous over data input buffer boundaries
//
AllPassFilter::AllPassFilterKernel::AllPassFilterKernel (AUEffectBase *inAudioUnit) : AuKernelBase(inAudioUnit),
	mSamplesProcessed (0), mCurrentScale (0)
{
	//...........
	// code for filling wave tables & generating oscillators and such
	// goes here, for example:
	// 	for (int i = 0; i < kWaveArraySize; ++i) {
	//	double radians = i * 2.0 * M_PI / kWaveArraySize;
	//	mSine [i] = (sin (radians) + 1.0) * 0.5;
	//  }
	//...........
	
	// Gets the samples per second of the audio stream provided to the audio unit. 
	// Obtaining this value here in the constructor assumes that the sample rate
	// will not change during one instantiation of the audio unit.
	mSampleFrequency = GetSampleRate ();
}
//.......................................
//	AllPassFilter::AllPassFilterKernel::Reset()
//.......................................
// Because we're calculating each output sample based on a unique input sample, there's no 
// need to clear any buffers. We simply reinitialize the variables that were initialized on
// instantiation of the kernel object.
void AllPassFilter::AllPassFilterKernel::Reset() {
	mCurrentScale		= 0;
	mSamplesProcessed	= 0;
}

//........................................
// AllPassFilter::AllPassFilterKernel::Process
//........................................
// This method contains the DSP code!

void AllPassFilter::AllPassFilterKernel::Process (
	const Float32	*inSourceP,			// The audio sample input buffer.
	Float32			*inDestP,			// the audio sample output buffer.
	UInt32			inSamplesToProcess,	// the number of samples in the input buffer
	UInt32			inNumChannels,		// the number of input channels. this is always equal to 1
										//   because there is always one kernel object instantiated
										//   per channel of audio.
	bool 			&ioSilence			// A boolean flag indicating wether the input to the audio
										//   unit consists of silence, with a TRUE value indicating
										//   silence.
){
	// Ignores the request to perform the Process method if the input to the audio unit is silence.
	if (!iosilence){
	
		// Assigns a pointer variable to the start of the audio sample input buffer.
		const Float32 *sourceP = inSourceP
		
		// assigns a pointer variable to the start of the audio sample output buffer.
		Float32 *destP = inDestP,
				inputSample,			// the current audio sample to process
				outputSample,			// the current audio output sample resulting from one iteration of the 
										//   processing loop.
				allpassFrequency,		// the freq requested by the user via the au view
				rawGain;				// the gain for the current audio sample
				
		// once per input buffer, get the frequency in hz from the user
		// via the au view
		allpassFrequency = GetParameter (kParameter_Frequency);
		
		// get the rest of the parameters -->
		
		// performs bounds checking on the parameters.
		if (allpassFrequency	< kMinimumValue_AllPass_Freq)
			allpassFrequency	= kMinimumValue_AllPass_Freq;
		if (allpassFrequency	> kMaximumValue_AllPass_Freq)
			allpassFrequency 	= kMaximumValue_AllPass_Freq;
			
		// Calculates the number of audio samples per cycle of frequency (from tremeloeffect)
		samplesPerCycle = mSampleFrequency / allpassFrequency;
		
		// calculates the scaling factor to use for applying the wave table to the current sampling
		// frequency and (tremolo) frequency.
		mnNextScale		= kWaveArraySize / samplesPerCycle;
		
		// The sample processing loop: processes the current batch of samples, one sample at a time.
		for (int i = inSamplesToProcess; i > 0; --i) {
		
			// The following statement calculates the position in the wave table ("index") to 
			// use for the current sample. This position, along with the calculation of 
			// mNextScale, is the only subtle math for this audio unit.
			//
			// "index" is the position marker in the wave table. The wave table is an array; 
			//		index varies from 0 to kWaveArraySize.
			//
			//	"index" is also the number of samples processed since the last 
			//	counter reset, divided by the number of samples that play during one pass 
			//	through the wave table, modulo the size of the wave table (see "An explanation...",
			//  above).
			int index = static_cast<long>(mSamplesProcessed * mCurrentScale) % kWaveArraySize;

			// If the user has moved the frequency slider, changes the scale factor
			// at the next positive zero crossing of the sine wave and resets the 
			// mSamplesProcessed value so it stays in sync with the index position.
			if ((mNextScale != mCurrentScale) && (index == 0)) {
				mCurrentScale = mNextScale;
				mSamplesProcessed = 0;
			}

			// If the audio unit runs for a long time without the user moving the
			// frequency slider, resets the mSamplesProcessed value at the 
			// next positive zero crossing of the sine wave.
			if ((mSamplesProcessed >= sampleLimit) && (index == 0))
				mSamplesProcessed = 0;

			// Gets the raw tremolo gain from the appropriate wave table.
			rawGain = waveArrayPointer [index];
			
			// Gets the next input sample.
			inputSample			= *sourceP;
			
			// Calculates the next output sample.
			outputSample		= (inputSample * rawGain);
			
			// Stores the output sample in the output buffer.
			*destP				= outputSample;
			
			// Advances to the next sample location in the input and output buffers.
			sourceP				+= 1;
			destP				+= 1;
			
			// Increments the global samples counter.
			mSamplesProcessed	+= 1;
		}
	}
}