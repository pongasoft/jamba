#ifndef VST3_JAMBA_[-name-]_RT_PROCESSOR_H
#define VST3_JAMBA_[-name-]_RT_PROCESSOR_H

#include <pongasoft/VST/RT/RTProcessor.h>
#include "../Plugin.h"

namespace [-namespace-]::RT {

using namespace pongasoft::VST::RT;

//------------------------------------------------------------------------
// [-name-]Processor - Real Time Processor
//------------------------------------------------------------------------
class [-name-]Processor : public RTProcessor
{
public:
  //------------------------------------------------------------------------
  // UUID() method used to create the processor
  //------------------------------------------------------------------------
  static inline ::Steinberg::FUID UUID() { return [-name-]ProcessorUID; };

  //------------------------------------------------------------------------
  // Factory method used to create the processor
  //------------------------------------------------------------------------
  static FUnknown *createInstance(void * /*iContext*/) { return (IAudioProcessor *) new [-name-]Processor(); }

public:
  // Constructor
  explicit [-name-]Processor();

  // Destructor
  ~[-name-]Processor() override;

  // getRTState
  RTState *getRTState() override { return &fState; }

  /** Called at first after constructor (setup input/output) */
  tresult PLUGIN_API initialize(FUnknown *context) override;

  // Called at the end before destructor
  tresult PLUGIN_API terminate() override;

  // This is where the setup happens which depends on sample rate, etc..
  tresult PLUGIN_API setupProcessing(ProcessSetup &setup) override;

protected:

  // genericProcessInputs<SampleType>
  template<typename SampleType>
  tresult genericProcessInputs(ProcessData &data);

  // processInputs32Bits
  tresult processInputs32Bits(ProcessData &data) override { return genericProcessInputs<Sample32>(data); }

  // processInputs64Bits
  tresult processInputs64Bits(ProcessData &data) override { return genericProcessInputs<Sample64>(data); }

private:
  // The processor gets a copy of the parameters (defined in Plugin.h)
  [-name-]Parameters fParams;

  // The state
  [-name-]RTState fState;
};

}

#endif // VST3_JAMBA_[-name-]_RT_PROCESSOR_H

