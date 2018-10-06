#include "version.h"

// Check https://developer.apple.com/library/archive/documentation/General/Conceptual/ExtensibilityPG/AudioUnit.html for various types

/* Bundle Identifier */
#define kAudioUnitBundleIdentifier	[-company-].vst3.[-name-].audiounit

/* Version Number (computed in version.h from version in CMakeLists.txt) */
#define kAudioUnitVersion			AU_VERSION_INT

/* Company Name + Effect Name */
#define kAUPluginName 				[-company-]: [-name-]

/* A product name for the audio unit, such as TremoloUnit */
#define kAUPluginDescription 		[-name-]

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
#define kAUPluginManufacturer 		[-audio_unit_manufacturer_code-]

// Definitions for the resource file
#define kAudioUnitName				      "[-company-]: [-name-]" // same as kAUPluginName
#define kAudioUnitDescription	      "[-name-]" // same as kAUPluginDescription
#define kAudioUnitType				      'aufx' // same as kAUPluginType
#define kAudioUnitComponentSubType	'unkw' // same as kAUPluginSubType
#define kAudioUnitComponentManuf    '[-audio_unit_manufacturer_code-]' // same as kAUPluginManufacturer

#define kAudioUnitCarbonView		1		// if 0 no Carbon view support will be added
