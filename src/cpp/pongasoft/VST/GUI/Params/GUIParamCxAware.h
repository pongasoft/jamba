/*
 * Copyright (c) 2018-2019 pongasoft
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

#include "ParamAware.h"

namespace pongasoft::VST::GUI::Params {

/**
 * Code has been moved to ParamAware class.
 *
 * @deprecated Use ParamAware instead
 */
class [[deprecated("Use ParamAware instead")]] GUIParamCxAware : public ParamAware
{
};


/**
 * @deprecated Use ParamAwareView instead
 */
template<typename TView>
class [[deprecated("Use ParamAwareView instead")]] ViewGUIParamCxAware : public ParamAwareView<TView>
{
};

}