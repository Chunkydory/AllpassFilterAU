//...................................................................
//
//
//
// 			AllPassFilter
//
//
//
//__________________________________________________________________

From Wikipedia:
An all-pass filter is a signal processing filter that passes all frequencies equally in gain, but changes the phase relationship between various frequencies. It does this by varying its phase shift as a function of frequency. Generally, the filter is described by the frequency at which the phase shift crosses 90° (i.e., when the input and output signals go into quadrature – when there is a quarter wavelength of delay between them).

They are generally used to compensate for other undesired phase shifts that arise in the system, or for mixing with an unshifted version of the original to implement a notch comb filter.

They may also be used to convert a mixed phase filter into a minimum phase filter with an equivalent magnitude response or an unstable filter into a stable filter with an equivalent magnitude response.

//………………………………

This project is mainly me figuring out how to build simple Audio Units.
In this very early state, the first step was converting the TremoloUnit example project to a project for an all-pass filter. Still needs a bit of figuring out (which parameters to include, which not) and still needs all of the DSP implemented (the good bits :-) )

What to include when building? :
	
	- CoreAudio.framework
	- CoreServices.framework
	- AudioToolbox.framework
	- AudioUnit.framework
	
	?— AUPublic folder
	?- PublicUtility folder

