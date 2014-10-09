#ifndef __AllPassFilterVersion_h__
#define __AllPassFilterVersion_h__


#ifdef DEBUG
	#define kAllPassFilterVersion 0xFFFFFFFF
#else
	#define kAllPassFilterVersion 0x00010000	
#endif

// customized for each audio unit
#define AllPassFilter_COMP_SUBTYPE		'alpf'
#define AllPassFilter_COMP_MANF			'Niem'

#endif