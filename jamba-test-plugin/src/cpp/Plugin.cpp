/*
 * Copyright (c) 2019 pongasoft
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * @author Yan Pujante
 */

#include "Plugin.h"

namespace pongasoft::test::jamba {

using UTF8StringSerializer = UTF8StringParamSerializer<128>;

//------------------------------------------------------------------------
// JambaTestPluginParameters::JambaTestPluginParameters
//------------------------------------------------------------------------
JambaTestPluginParameters::JambaTestPluginParameters()
{
  // bypass
  fBypass =
    vst<BooleanParamConverter>(EJambaTestPluginParamID::kBypass, STR16 ("Bypass"))
      .defaultValue(false)
      .flags(ParameterInfo::kCanAutomate | ParameterInfo::kIsBypass)
      .shortTitle(STR16 ("Bypass"))
      .add();


  // fTab
  fTab =
    vst<DiscreteTypeParamConverter<ETabs>>(EJambaTestPluginParamID::kTab,
                                           STR16("Tab"),
                                           {
                                             {ETabs::kTabAll,                 STR16("All Controls")},
                                             {ETabs::kTabToggleButtonView,    STR16("ToggleButtonView")},
                                             {ETabs::kTabMomentaryButtonView, STR16("MomentaryButtonView")},
                                             {ETabs::kTabStepButtonView,      STR16("StepButtonView")}
                                           })
      .guiOwned()
      .add();

  //------------------------------------------------------------------------
  // various flavors of Vst parameters
  //------------------------------------------------------------------------

  // fRawVst
  fRawVst =
    raw(EJambaTestPluginParamID::kRawVst, STR16("RawVst"))
      .add();

  // fParamValueVst
  fParamValueVst =
    vstFromType<ParamValue>(EJambaTestPluginParamID::kParamValueVst, STR16("ParamValueVst"))
      .converter<RawParamConverter>()
      .add();


  // fInt32Vst
  fInt32Vst =
    vstFromType<int32>(EJambaTestPluginParamID::kInt32Vst, STR16("int32Vst"))
      .converter<DiscreteValueParamConverter<3, int32>>(STR16("int32Vst [%d]"))
      .add();

  // fInt64Vst
  fInt64Vst =
    vst<DiscreteValueParamConverter<5, int64>>(EJambaTestPluginParamID::kInt64Vst, STR16("int64Vst"),
                                               STR16("int64Vst [%d]")) // converter argument
      .add();

  // fBoolVst
  fBoolVst =
    vst<BooleanParamConverter>(EJambaTestPluginParamID::kBoolVst, STR16("boolVst"), STR16("Off (vst)"), STR16("On (vst)"))
      .add();

  // fEnumVst
  fEnumVst =
    vst<EnumParamConverter<EEnum, EEnum::kEnum3>>(EJambaTestPluginParamID::kEnumVst, STR16("EnumVst"),
                                                  {
                                                    STR16("kEnum0 (vst)"),
                                                    STR16("kEnum1 (vst)"),
                                                    STR16("kEnum2 (vst)"),
                                                    STR16("kEnum3 (vst)")
                                                  }) // converter argument
      .add();

  // fEnumClassVst
  fEnumClassVst =
    vst<DiscreteTypeParamConverter<EEnumClass>>(EJambaTestPluginParamID::kEnumClassVst, STR16("EnumClassVst"),
                                                {
                                                  {EEnumClass::kEnumClass0, STR16("kEC0 (vst)")},
                                                  {EEnumClass::kEnumClass1, STR16("kEC1 (vst)")},
                                                  {EEnumClass::kEnumClass2, STR16("kEC2 (vst)")},
                                                  {EEnumClass::kEnumClass3, STR16("kEC3 (vst)")},
                                                }) // converter argument
      .add();

  // fNonContiguousEnumVst
  fNonContiguousEnumVst =
    vst<DiscreteTypeParamConverter<ENonContiguousEnum>>(EJambaTestPluginParamID::kNonContiguousEnumVst, STR16("NonContiguousEnumVst"),
                                                {
                                                  {ENonContiguousEnum::kNonContiguousEnum0, STR16("kNCE0 (vst)")},
                                                  {ENonContiguousEnum::kNonContiguousEnum1, STR16("kNCE1 (vst)")},
                                                  {ENonContiguousEnum::kNonContiguousEnum2, STR16("kNCE2 (vst)")},
                                                  {ENonContiguousEnum::kNonContiguousEnum3, STR16("kNCE3 (vst)")},
                                                }) // converter argument
      .defaultValue(ENonContiguousEnum::kNonContiguousEnum0)
      .add();

  // fGainVst
  fGainVst =
    vst<GainParamConverter>(EJambaTestPluginParamID::kGainVst, STR16("GainVst"))
      .defaultValue(DEFAULT_GAIN)
      .add();

  setRTSaveStateOrder(PROCESSOR_STATE_VERSION,
                      fBypass,
                      fRawVst,
                      fParamValueVst,
                      fInt32Vst,
                      fInt64Vst,
                      fBoolVst,
                      fEnumVst,
                      fEnumClassVst,
                      fNonContiguousEnumVst,
                      fGainVst);

  //------------------------------------------------------------------------
  // various flavors of Jmb parameters
  //------------------------------------------------------------------------

  fParamValueJmb = jmbFromType<ParamValue>(EJambaTestPluginParamID::kParamValueJmb, STR16("ParamValueJmb"))
    .serializer<RawParamSerializer>()
    .add();

  fInt32Jmb = jmb<Int32ParamSerializer>(EJambaTestPluginParamID::kInt32Jmb, STR16("int32Jmb"))
    .add();

  fInt64Jmb = jmbFromType<int64>(EJambaTestPluginParamID::kInt64Jmb, STR16("int64Jmb"))
    .serializer<Int64ParamSerializer>()
    .add();

  fBoolJmb = jmbFromType<bool>(EJambaTestPluginParamID::kBoolJmb, STR16("boolJmb"))
    .serializer<BooleanParamSerializer>("Off (jmb)", "On (jmb)")
    .add();

  // fEnumClassJmb
  fEnumJmb =
    jmb<DiscreteTypeParamSerializer<EEnum>>(EJambaTestPluginParamID::kEnumJmb, STR16("EnumJmb"),
                                            {
                                              {EEnum::kEnum0, "kEnum0 (jmb)"},
                                              {EEnum::kEnum1, "kEnum1 (jmb)"},
                                              {EEnum::kEnum2, "kEnum2 (jmb)"},
                                              {EEnum::kEnum3, "kEnum3 (jmb)"},
                                            }) // converter argument
      .add();

  // fEnumClassJmb
  fEnumClassJmb =
    jmb<DiscreteTypeParamSerializer<EEnumClass>>(EJambaTestPluginParamID::kEnumClassJmb, STR16("EnumClassJmb"),
                                                {
                                                  {EEnumClass::kEnumClass0, "kEC0 (jmb)"},
                                                  {EEnumClass::kEnumClass1, "kEC1 (jmb)"},
                                                  {EEnumClass::kEnumClass2, "kEC2 (jmb)"},
                                                  {EEnumClass::kEnumClass3, "kEC3 (jmb)"},
                                                }) // converter argument
      .add();

  // fNonContiguousEnumJmb
  fNonContiguousEnumJmb =
    jmb<DiscreteTypeParamSerializer<ENonContiguousEnum>>(EJambaTestPluginParamID::kNonContiguousEnumJmb, STR16("NonContiguousEnumJmb"),
                                                         {
                                                           {ENonContiguousEnum::kNonContiguousEnum0, "kNCE0 (jmb)"},
                                                           {ENonContiguousEnum::kNonContiguousEnum1, "kNCE1 (jmb)"},
                                                           {ENonContiguousEnum::kNonContiguousEnum2, "kNCE2 (jmb)"},
                                                           {ENonContiguousEnum::kNonContiguousEnum3, "kNCE3 (jmb)"},
                                                         }) // converter argument
      .defaultValue(ENonContiguousEnum::kNonContiguousEnum0)
      .add();

  // fGainJmb
  fGainJmb = jmbFromType<Gain>(EJambaTestPluginParamID::kGainJmb, STR16("GainJmb"))
    .defaultValue(DEFAULT_GAIN)
    .serializer<GainParamSerializer>(4)
    .add();

  // fUTF8String
  fUTF8String =
    jmb<UTF8StringSerializer>(EJambaTestPluginParamID::kUTF8StringJmb, STR16("UTF8StringJmb"))
      .defaultValue("<empty>")
      .add();

  // same for GUI - note that if the GUI does not save anything then you don't need this
  setGUISaveStateOrder(CONTROLLER_STATE_VERSION,
                       fTab,
                       fParamValueJmb,
                       fInt32Jmb,
                       fInt64Jmb,
                       fBoolJmb,
                       fEnumJmb,
                       fEnumClassJmb,
                       fNonContiguousEnumJmb,
                       fGainJmb,
                       fUTF8String
  );
}

}