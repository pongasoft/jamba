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
#pragma once

namespace pongasoft::VST::GUI::Params {

//------------------------------------------------------------------------
// GUIParamCxMgr::registerOptionalParam
//------------------------------------------------------------------------
template<typename T>
GUIOptionalParam<T> GUIParamCxMgr::registerOptionalParam(ParamID iParamID,
                                                         Parameters::IChangeListener *iChangeListener)
{
  auto param = fGUIState->findParam(iParamID);
  std::shared_ptr<ITGUIParameter<T>> optionalParam = nullptr;

  if(param)
  {
    auto typedParam = param->cast<T>();
    if(typedParam)
    {
      optionalParam = std::move(typedParam);
    }
    else
    {
      DLOG_F(WARNING, "param [%d] is not of the requested type [%s]", iParamID, Utils::typeString<T>().c_str());
    }
  }

  // no vst or jmb parameter match => using default
  if(!optionalParam)
  {
    optionalParam = VstUtils::make_sfo<GUIValParameter<T>>(iParamID, T{});

#ifndef NDEBUG
    if(iParamID != UNDEFINED_PARAM_ID)
      DLOG_F(WARNING, "could not find any parameter (vst or jmb) with id [%d]... reverting to default", iParamID);
#endif
  }

  return __registerListener(GUIOptionalParam<T>(std::move(optionalParam)), iChangeListener);
}

//------------------------------------------------------------------------
// GUIParamCxMgr::registerVstParam
//------------------------------------------------------------------------
template<typename T>
GUIVstParam<T> GUIParamCxMgr::registerVstParam(ParamID iParamID, Parameters::IChangeListener *iChangeListener)
{
  auto param = fGUIState->getGUIVstParameter<T>(iParamID);

  return __registerListener(GUIVstParam<T>(std::move(param)), iChangeListener);
}

//------------------------------------------------------------------------
// GUIParamCxMgr::registerJmbParam
//------------------------------------------------------------------------
template<typename T>
GUIJmbParam<T> GUIParamCxMgr::registerJmbParam(ParamID iParamID, Parameters::IChangeListener *iChangeListener)
{
  auto param = fGUIState->getJmbParameter(iParamID);

  if(!param)
  {
    DLOG_F(WARNING, "jmb param [%d] not found", iParamID);
    return GUIJmbParam<T>{};
  }

  auto res = std::dynamic_pointer_cast<GUIJmbParameter<T>>(param);
  if(!res)
  {
    DLOG_F(WARNING, "jmb param [%d] is not of the requested type [%s]", iParamID, typeid(T).name());
  }

  return __registerListener(GUIJmbParam<T>(std::move(res)), iChangeListener);
}

//------------------------------------------------------------------------
// GUIParamCxMgr::__registerCallback
//------------------------------------------------------------------------
template<typename TParam>
inline TParam GUIParamCxMgr::__registerCallback(TParam iParam, Parameters::ChangeCallback iCallback, bool iInvokeCallback)
{
  if(!iCallback)
  {
    DLOG_F(WARNING, "Callback is empty... unlikely what you want to do");
    return iParam;
  }

  if(iParam.exists())
  {
    if(iInvokeCallback)
      iCallback();

    fParamCxs.emplace_back(iParam.connect(std::move(iCallback)));
  }

  return iParam;
}

//------------------------------------------------------------------------
// GUIParamCxMgr::__registerCallback1
//------------------------------------------------------------------------
template<typename TParam>
inline TParam GUIParamCxMgr::__registerCallback1(TParam iParam,
                                                 Parameters::ChangeCallback1<TParam> iCallback,
                                                 bool iInvokeCallback)
{
  if(!iCallback)
  {
    DLOG_F(WARNING, "Callback is empty... unlikely what you want to do");
    return iParam;
  }

  if(iParam.exists())
  {
    // Implementation note: iParam is copied on purpose as we do not know its lifespan (and iParam are simple
    // wrappers around a shared pointer so cheap/quick to copy)
    auto callback = [iParam, cb2 = std::move(iCallback)] () mutable { cb2(iParam); };

    if(iInvokeCallback)
      callback();

    fParamCxs.emplace_back(iParam.connect(std::move(callback)));
  }

  return iParam;
}

}