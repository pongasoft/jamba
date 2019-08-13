#include "version.h"

// Check https://developer.apple.com/library/archive/documentation/General/Conceptual/ExtensibilityPG/AudioUnit.html for various types

/* Bundle Identifier */
#define kAudioUnitBundleIdentifier	pongasoft.vst3.JambaTestPlugin.audiounit

/* Version Number (computed in version.h from version in CMakeLists.txt) */
#define kAudioUnitVersion			AU_VERSION_INT

/* Company Name + Effect Name */
#define kAUPluginName 				pongasoft: JambaTestPlugin

/* A product name for the audio unit, such as TremoloUnit */
#define kAUPluginDescription 		JambaTestPlugin

/*
  The specific variant of the Audio Unit. The four possible types and their values are:
  Effect (aufx), Generator (augn), Instrument (aumu), and Music Effect (aufm).
 */
#define kAUPluginType 				aufx

/* A subtype code for the audio unit, such as tmlo. This value must be exactly 4 alphanumeric characters. */
#define kAUPluginSubType 			unkw

/* A manufacturer code for the audio unit, such as Aaud. This value must be exactly 4 alphanumeric characters.
 * This value should be unique across audio units of the same manufacturer
 * Manufacturer OSType should have at least one non-lower case character */
#define kAUPluginManufacturer 		Jtp1

// Definitions for the resource file
#define kAudioUnitName				      "pongasoft: JambaTestPlugin" // same as kAUPluginName
#define kAudioUnitDescription	      "JambaTestPlugin" // same as kAUPluginDescription
#define kAudioUnitType				      'aufx' // same as kAUPluginType
#define kAudioUnitComponentSubType	'unkw' // same as kAUPluginSubType
#define kAudioUnitComponentManuf    'Jtp1' // same as kAUPluginManufacturer

#define kAudioUnitCarbonView		1		// if 0 no Carbon view support will be added
