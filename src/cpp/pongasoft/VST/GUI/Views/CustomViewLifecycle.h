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

namespace pongasoft::VST::GUI::Views {

/**
 * This interface defines some methods that are important in the lifecycle of a custom view. For the moment only
 * one method is implemented, but more can be added as the needs arise.
 */
class ICustomViewLifecycle
{
public:
  /**
   * Called after the attributes have been applied by the framework.
   *
   * The %VST SDK always apply all attributes attached to the view as a block (loop). Whether it is at loading time
   * (after loading the xml, and instantiating the view) or in the editor when modifying attributes.
   * This ends up calling "setters" on the view (via the `Creator`). This call is invoked after all attributes have
   * been set which allow to handle a consistent set of attributes. By default the jamba framework will call
   * `registerParameters` from this api because at this point in the lifecycle all attributes have been set (which
   * include paramIDs/tagIDs and are required for registering parameters).
   */
  virtual void afterApplyAttributes() {};
};

}