/*
 * Copyright (c) 2020 pongasoft
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

#ifndef JAMBA_IDIALOGHANDLER_H
#define JAMBA_IDIALOGHANDLER_H

#include <string>
#include <pluginterfaces/base/ftypes.h>

namespace pongasoft::VST::GUI {

/**
 * Defines the interface to show or dismiss a modal/dialog window which is a window that captures all events and must
 * be dismissed (by calling `IDialogHandler::dismissDialog`). Typical usages are for About windows or alert messages.
 *
 * The primary way to use it in the code is via the gui state (which implements this interface)
 *
 * ```
 * // Example from some controller (aboutButton is a TextButtonView)
 * aboutButton->setOnClickListener([this] {
 *   fState->showDialog("about_dialog");
 * });
 *
 * // From another controller tied to the about_dialog view (set via the sub-controller attribute)
 * dismissButton->setOnClickListener([this] {
 *   fState->dismissDialog();
 * });
 * ```
 */
class IDialogHandler
{
public:
  //! Destructor
  virtual ~IDialogHandler() = default;

  /**
   * This method is called with the name of the template to use for the dialog. It should be a template defined in the
   * xml file (`.uidesc`) as a top level template.
   *
   * @note Because the view is modal it must include a way for the user to dismiss it, like a button.
   *
   * @param iTemplateName the name of the top level template to use
   * @return `true` if the dialog was shown and `false` if there was a problem (for example `iTemplateName` does not
   *         refer to a valid top level template name)
   */
  virtual bool showDialog(std::string iTemplateName) = 0;

  /**
   * Dismisses the currently shown dialog.
   *
   * @return `true` if there was a dialog shown or `false` otherwise
   */
  virtual bool dismissDialog() = 0;
};

}

#endif //JAMBA_IDIALOGHANDLER_H