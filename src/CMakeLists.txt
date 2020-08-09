cmake_minimum_required (VERSION 3.14)

#------------------------------------------------------------------------
# Jamba compile Options
#------------------------------------------------------------------------
option(JAMBA_DEBUG_LOGGING "Enable debug logging for jamba framework" OFF)

#------------------------------------------------------------------------
# Defining files to include to generate the library
#------------------------------------------------------------------------
set(JAMBA_CPP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/cpp)
set(LOGURU_IMPL ${JAMBA_CPP_SOURCES}/pongasoft/logging/loguru.cpp)

set(JAMBA_sources_h
    ${JAMBA_CPP_SOURCES}/pongasoft/logging/logging.h
    ${JAMBA_CPP_SOURCES}/pongasoft/logging/loguru.hpp

    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Clock/Clock.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Collection/CircularBuffer.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Concurrent/Concurrent.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Concurrent/SpinLock.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Constants.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Cpp17.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Disposable.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Lerp.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Metaprogramming.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Misc.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/Operators.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/StringUtils.h
    ${JAMBA_CPP_SOURCES}/pongasoft/Utils/StringUtils.cpp

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/Debug/ParamDisplay.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/Debug/ParamLine.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/Debug/ParamTable.h

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/AudioBuffer.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/AudioUtils.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/FObjectCx.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/MessageHandler.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/MessageProducer.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/Messaging.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/NormalizedState.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/ParamConverters.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/ParamDef.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/Parameters.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/ParamSerializers.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/PluginFactory.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/SampleRateBasedClock.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/Timer.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/Types.h

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/VstUtils/ExpiringDataCache.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/VstUtils/Utils.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/VstUtils/FastWriteMemoryStream.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/VstUtils/ReadOnlyMemoryStream.h

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/RT/RTParameter.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/RT/RTProcessor.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/RT/RTJmbOutParameter.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/RT/RTJmbInParameter.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/RT/RTState.h

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIJmbParameter.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIOptionalParam.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIParamCx.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIParamCxMgr.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIParamCxMgr.hpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIParamSerializers.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIRawVstParameter.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIVstParameter.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIValParameter.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/IGUIParameter.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/IGUIParameter.hpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/ParamAware.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/ParamAware.hpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/VstParameters.h

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/CustomController.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/CustomControlView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/CustomView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/CustomViewCreator.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/CustomViewFactory.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/CustomViewLifecycle.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/DebugParamDisplayView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/DiscreteButtonView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/GlobalKeyboardHook.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/ImageView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/JambaViews.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/MomentaryButtonView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/ParamDisplayView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/ParamImageView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/StateAware.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/TextButtonView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/TextEditView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/ScrollbarView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/ScrollbarView.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/SelfContainedViewListener.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/StepButtonView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/StepPadView.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/SwitchViewContainer.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/ToggleButtonView.h

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/DrawContext.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/GUIController.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/GUIState.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/GUIUtils.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/IDialogHandler.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/LookAndFeel.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Types.h
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/ParamAwareViews.h
    )

set(JAMBA_sources_cpp
    ${LOGURU_IMPL}

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/Debug/ParamDisplay.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/Debug/ParamLine.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/Debug/ParamTable.cpp

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/FObjectCx.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/MessageHandler.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/Parameters.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/NormalizedState.cpp

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/RT/RTParameter.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/RT/RTProcessor.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/RT/RTState.cpp

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/VstUtils/FastWriteMemoryStream.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/VstUtils/ReadOnlyMemoryStream.cpp

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIJmbParameter.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIParamCx.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIParamCxMgr.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/GUIRawVstParameter.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Params/ParamAware.cpp

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/CustomControlView.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/CustomView.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/CustomViewFactory.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/DiscreteButtonView.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/ImageView.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/MomentaryButtonView.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/ParamDisplayView.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/ParamImageView.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/StepButtonView.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/StepPadView.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/SwitchViewContainer.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/TextButtonView.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/TextEditView.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/Views/ToggleButtonView.cpp

    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/DrawContext.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/GUIController.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/GUIState.cpp
    ${JAMBA_CPP_SOURCES}/pongasoft/VST/GUI/ParamAwareViews.cpp

    )

if (JAMBA_ENABLE_VST2)
  set(JAMBA_vst2_sources
      # VST3 hosting sources
      ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/connectionproxy.cpp
      ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/eventlist.cpp
      ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/hostclasses.cpp
      ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/parameterchanges.cpp
      ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/pluginterfacesupport.cpp
      ${VST3_SDK_ROOT}/public.sdk/source/vst/hosting/processdata.cpp

      # JAMBA overrides
      ${JAMBA_CPP_SOURCES}/public.sdk/source/vst/basewrapper/basewrapper.cpp
      ${JAMBA_CPP_SOURCES}/public.sdk/source/vst/basewrapper/basewrapper.h
      ${JAMBA_CPP_SOURCES}/public.sdk/source/vst/vst2wrapper/vst2wrapper.cpp
      ${JAMBA_CPP_SOURCES}/public.sdk/source/vst/vst2wrapper/vst2wrapper.h

      # VST2 sources
      ${VST2_SDK_ROOT}/public.sdk/source/vst2.x/audioeffect.cpp
      ${VST2_SDK_ROOT}/public.sdk/source/vst2.x/audioeffectx.cpp
      )
endif ()

set(JAMBA_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
configure_file(${JAMBA_CPP_SOURCES}/pongasoft/logging/jamba_version.h.in ${JAMBA_GENERATED_DIR}/jamba_version.h)

if (JAMBA_DEBUG_LOGGING)
  message(STATUS "Enabling debug logging for jamba framework")
  add_definitions(-DJAMBA_DEBUG_LOGGING)
endif ()

add_library(jamba STATIC ${JAMBA_sources_cpp} ${JAMBA_vst2_sources} ${JAMBA_sources_h})
target_include_directories(jamba PUBLIC ${JAMBA_CPP_SOURCES} ${VSTGUI_ROOT}/vstgui4 ${JAMBA_GENERATED_DIR})
if (JAMBA_ENABLE_VST2)
  target_include_directories(jamba PUBLIC ${VST2_SDK_ROOT})
endif()
target_compile_definitions(jamba PUBLIC $<$<CONFIG:Debug>:VSTGUI_LIVE_EDITING=1>)

set(JAMBA_CPP_SOURCES ${JAMBA_CPP_SOURCES} PARENT_SCOPE)
