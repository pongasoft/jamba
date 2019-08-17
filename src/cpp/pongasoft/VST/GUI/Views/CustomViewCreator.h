/*
 * Copyright (c) 2018 pongasoft
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

#include <vstgui4/vstgui/uidescription/iviewcreator.h>
#include <vstgui4/vstgui/uidescription/uiviewcreator.h>
#include <vstgui4/vstgui/uidescription/uiviewfactory.h>
#include <vstgui4/vstgui/uidescription/uiattributes.h>
#include <vstgui4/vstgui/uidescription/detail/uiviewcreatorattributes.h>
#include <vstgui4/vstgui/lib/crect.h>
#include <vstgui4/vstgui/lib/ccolor.h>
#include <vstgui4/vstgui/lib/cbitmap.h>
#include <map>
#include <memory>
#include <pongasoft/logging/logging.h>
#include <pongasoft/VST/GUI/Types.h>
#include <pongasoft/VST/GUI/LookAndFeel.h>
#include <pongasoft/Utils/StringUtils.h>
#include <pongasoft/VST/Types.h>

namespace pongasoft {
namespace VST {
namespace GUI {
namespace Views {

using namespace VSTGUI;

/**
 * Base abstract class for an attribute of a view
 */
class ViewAttribute
{
public:
  // Constructor
  explicit ViewAttribute(std::string iName) :
    fName(std::move(iName))
  {}

  /**
   * @return the type of this attribute => subclass will define it
   */
  virtual IViewCreator::AttrType getType() = 0;

  /**
   * Name of the attribute (which ends up being an attribute in the xml file)
   * Ex: <view back-color="~ BlackCColor" .../> => back-color
   */
  std::string getName() const
  {
    return fName;
  }

  /**
   * Extracts the value from iAttributes for getName() attribute and "apply" it on the view provided. Subclass
   * will handle the extraction based on type.
   *
   * @return false if view is not of the right type or could not extract
   */
  virtual bool apply(CView *iView, const UIAttributes &iAttributes, const IUIDescription *iDescription) = 0;

  /**
   * Extracts the value from the view for this attribute (getName()) and store it in oStringValue. Subclass
   * will handle the extraction based on type.
   *
   * @return false if view is not of the right type or could not extract
   */
  virtual bool getAttributeValue(CView *iView, const IUIDescription *iDescription, std::string &oStringValue) const = 0;

  /**
   * @return the list of values available (if attribute is a list attribute)
   */
  virtual bool getPossibleListValues(std::list<const std::string *> &iValues) const
  {
    return false;
  }

private:
  std::string fName;
};

/**
 * Factory method which creates the actual view. Can be specialized for specific cases
 * @return the new view
 */
template<typename TView>
inline TView *createCustomView(CRect const &iSize,
                               const UIAttributes &iAttributes,
                               const IUIDescription *iDescription) { return new TView(iSize); }

/**
 * Defines a map of `string` to attribute value */
template<typename T>
using AttrValMap = std::map<std::string, T>;

/**
 * Defines the type to initialize an [AttrValMap], for an example check [TCustomViewCreator::registerListAttribute]
 */
template<typename T>
using AttrValInitList = std::initializer_list<typename AttrValMap<T>::value_type>;

/**
 * Generic custom view creator base class. Inherit from it and call the various "registerXX" methods in the constructor.
 *
 * In case of inheritance, you do the following:
 *
 * class CustomView1 : public CControl { ... }
 * class CustomView2 : public CustomView1 { ... }
 *
 * class CustomView1Creator : public CustomViewCreator<CustomView1> {
 *   public:
 *     explicit CustomView1Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
 *        CustomViewCreator(iViewName, iDisplayName)
 *     {
 *       ... register CustomView1 attributes here ...
 *     }
 * }
 * class CustomView2Creator : public CustomViewCreator<CustomView2> {
 *   public:
 *     explicit CustomView2Creator(char const *iViewName = nullptr, char const *iDisplayName = nullptr) :
 *        CustomViewCreator(iViewName, iDisplayName)
 *     {
 *       registerAttributes(CustomView1Creator());
 *       ... register CustomView2 attributes here ...
 *     }
 * }
 */
template<typename TView>
class TCustomViewCreator : public ViewCreatorAdapter
{
private:
  /**
   * Generic base class that implements the logic for a ViewAttribute that uses a getter and setter in TView
   *
   * @tparam T the type of the attribute
   */
  template<typename T, typename TGetter, typename TSetter>
  class TAttribute : public ViewAttribute
  {
  public:
    using Getter = TGetter;
    using Setter = TSetter;

    // Constructor
    TAttribute(std::string const &iName, Getter iGetter, Setter iSetter) :
      ViewAttribute(iName), fGetter{iGetter}, fSetter{iSetter} { }

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kUnknownType;
    }

    /**
     * Subclasses need to implement this method to convert a string (iAttributeValue) to a T. Returns true if the
     * conversion is possible in which case the result is written to oValue, false otherwise.
     */
    virtual bool fromString(IUIDescription const *iDescription, std::string const &iAttributeValue, T &oValue) const
    {
      return false;
    }

    /**
     * Subclasses need to implement this method to convert a T to a string. Returns true if the
     * conversion is possible in which case the result is written to oStringValue, false otherwise.
     */
    virtual bool toString(IUIDescription const *iDescription, T const &iValue, std::string &oStringValue) const
    {
      return false;
    }

    // apply => set a color to the view
    bool apply(CView *iView, const UIAttributes &iAttributes, const IUIDescription *iDescription) override
    {
      auto *tv = dynamic_cast<TView *>(iView);
      if(tv != nullptr)
      {
        auto attributeValue = iAttributes.getAttributeValue(getName());
        if(attributeValue)
        {
          T value;
          if(fromString(iDescription, *attributeValue, value))
          {
            (tv->*fSetter)(value);
            return true;
          }
        }
      }
      return false;
    }

    // getAttributeValue => get a color from the view
    bool getAttributeValue(CView *iView, const IUIDescription *iDescription, std::string &oStringValue) const override
    {
      auto *tv = dynamic_cast<TView *>(iView);
      if(tv != nullptr)
      {
        auto value = (tv->*fGetter)();
        return toString(iDescription, value, oStringValue);
      }
      return false;
    }

  private:
    Getter fGetter;
    Setter fSetter;
  };

  /**
   * ByValAttribute defines getter/setter by value (copy)
   */
  template<typename T>
  using ByValAttribute = TAttribute<T, T (TView::*)() const, void (TView::*)(T)>;

  /**
   * ByRefAttribute defines getter/setter by const reference
   */
  template<typename T>
  using ByRefAttribute = TAttribute<T, T const &(TView::*)() const, void (TView::*)(T const &)>;

  /**
   * Specialization for a tag attribute (vst type TagID). The view must have getter and setter as defined by the
   * types below.
   */
  class TagAttribute : public ByValAttribute<TagID>
  {
  public:
    TagAttribute(std::string const &iName,
                 typename ByValAttribute<TagID>::Getter iGetter,
                 typename ByValAttribute<TagID>::Setter iSetter) :
      ByValAttribute<TagID>(iName, iGetter, iSetter) {}

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kTagType;
    }

    // fromString
    bool fromString(IUIDescription const *iDescription, std::string const &iAttributeValue, TagID &oValue) const override
    {
      if(iAttributeValue.length() != 0)
      {
        auto tag = iDescription->getTagForName(iAttributeValue.c_str());
        if(tag == UNDEFINED_TAG_ID)
        {
          char *endPtr = nullptr;
          tag = (TagID) strtol(iAttributeValue.c_str(), &endPtr, 10);
          if(endPtr == iAttributeValue.c_str())
          {
            return false;
          }
        }
        oValue = tag;
        return true;
      }
      return false;
    }

    // toString
    bool toString(IUIDescription const *iDescription, const TagID &iValue, std::string &oStringValue) const override
    {
      if(iValue != UNDEFINED_TAG_ID)
      {
        UTF8StringPtr controlTag = iDescription->lookupControlTagName(iValue);
        if(controlTag)
        {
          oStringValue = controlTag;
          return true;
        }
      }

      return false;
    }
  };

  /**
   * Specialization for an Integer attribute (which can be any kind of integer, like short, int32_t, etc..).
   * The view must have getter and setter as defined by the types below.
   */
  template<typename TInt>
  class IntegerAttribute : public ByValAttribute<TInt>
  {
  public:
    using Getter = typename ByValAttribute<TInt>::Getter;
    using Setter = typename ByValAttribute<TInt>::Setter;

    // Constructor
    IntegerAttribute(std::string const &iName, Getter iGetter, Setter iSetter) :
      ByValAttribute<TInt>(iName, iGetter, iSetter) {}

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kIntegerType;
    }

    // fromString
    bool fromString(IUIDescription const *iDescription, std::string const &iAttributeValue, TInt &oValue) const override
    {
      char *endPtr = nullptr;
      auto value = static_cast<TInt>(strtol(iAttributeValue.c_str(), &endPtr, 10));
      if(endPtr == iAttributeValue.c_str())
      {
        DLOG_F(WARNING, "could not convert <%s> to an integer", iAttributeValue.c_str());
        return false;
      }
      oValue = value;
      return true;
    }

    // toString
    bool toString(IUIDescription const *iDescription, const TInt &iValue, std::string &oStringValue) const override
    {
      std::stringstream str;
      str << iValue;
      oStringValue = str.str();
      return true;
    }
  };

  /**
   * Specialization for an float attribute (which can be a double or a float, etc..).
   * The view must have getter and setter as defined by the types below.
   */
  template<typename TFloat>
  class FloatAttribute : public ByValAttribute<TFloat>
  {
  public:
    using Getter = typename ByValAttribute<TFloat>::Getter;
    using Setter = typename ByValAttribute<TFloat>::Setter;

    // Constructor
    FloatAttribute(std::string const &iName, Getter iGetter, Setter iSetter) :
      ByValAttribute<TFloat>(iName, iGetter, iSetter) {}

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kFloatType;
    }

    // fromString
    bool fromString(IUIDescription const *iDescription, std::string const &iAttributeValue, TFloat &oValue) const override
    {
      TFloat value;
      if(Utils::stringToFloat<TFloat>(iAttributeValue, value))
      {
        oValue = value;
        return true;
      }
      DLOG_F(WARNING, "could not convert <%s> to a float", iAttributeValue.c_str());
      return false;
    }

    // toString
    bool toString(IUIDescription const *iDescription, const TFloat &iValue, std::string &oStringValue) const override
    {
      std::stringstream str;
      str << iValue;
      oStringValue = str.str();
      return true;
    }
  };

  /**
   * Specialization for the color attribute. The view must have getter and setter as defined by the
   * types below.
   */
  class ColorAttribute : public ByRefAttribute<CColor>
  {
  public:

    // Constructor
    ColorAttribute(std::string const &iName,
                   typename ByRefAttribute<CColor>::Getter iGetter,
                   typename ByRefAttribute<CColor>::Setter iSetter) :
      ByRefAttribute<CColor>(iName, iGetter, iSetter) {}

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kColorType;
    }

    // fromString
    bool fromString(IUIDescription const *iDescription, std::string const &iAttributeValue, CColor &oValue) const override
    {
      CColor color;
      if(UIViewCreator::stringToColor(&iAttributeValue, color, iDescription))
      {
        oValue = color;
        return true;
      }
      return false;
    }

    // toString
    bool toString(IUIDescription const *iDescription, const CColor &iValue, std::string &oStringValue) const override
    {
      return UIViewCreator::colorToString(iValue, oStringValue, iDescription);
    }
  };

  /**
   * Specialization for the color attribute. The view must have getter and setter as defined by the
   * types below.
   */
  class GradientAttribute : public ByValAttribute<GradientPtr>
  {
  public:

    // Constructor
    GradientAttribute(std::string const &iName,
                   typename ByValAttribute<GradientPtr>::Getter iGetter,
                   typename ByValAttribute<GradientPtr>::Setter iSetter) :
      ByValAttribute<GradientPtr>(iName, iGetter, iSetter) {}

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kGradientType;
    }

    // fromString
    bool fromString(IUIDescription const *iDescription, std::string const &iAttributeValue, GradientPtr &oValue) const override
    {
      auto gradient = iDescription->getGradient(iAttributeValue.c_str());
      if(gradient)
      {
        oValue = gradient;
        return true;
      }
      return false;
    }

    // toString
    bool toString(IUIDescription const *iDescription, GradientPtr const &iValue, std::string &oStringValue) const override
    {
      if(iValue)
      {
        auto name = iDescription->lookupGradientName(iValue);
        if(name)
        {
          oStringValue = name;
          return true;
        }

      }
      return false;
    }
  };

  /**
   * Specialization for the boolean attribute. The view must have getter and setter as defined by the
   * types below.
   */
  class BooleanAttribute : public ByValAttribute<bool>
  {
  public:
    // Constructor
    BooleanAttribute(std::string const &iName,
                     typename ByValAttribute<bool>::Getter iGetter,
                     typename ByValAttribute<bool>::Setter iSetter) :
      ByValAttribute<bool>(iName, iGetter, iSetter) {}

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kBooleanType;
    }

    // fromString
    bool fromString(IUIDescription const *iDescription, std::string const &iAttributeValue, bool &oValue) const override
    {
      if(iAttributeValue == "true")
      {
        oValue = true;
        return true;
      }

      if(iAttributeValue == "false")
      {
        oValue = false;
        return true;
      }

      return false;
    }

    // toString
    bool toString(IUIDescription const *iDescription, const bool &iValue, std::string &oStringValue) const override
    {
      oStringValue = iValue ? "true" : "false";
      return true;
    }
  };

  /**
   * Specialization for a bitmap attribute. The view must have getter and setter as defined by the
   * types below.
   */
  class BitmapAttribute : public ByValAttribute<BitmapPtr>
  {
  public:
    // Constructor
    BitmapAttribute(std::string const &iName,
                    typename ByValAttribute<BitmapPtr>::Getter iGetter,
                    typename ByValAttribute<BitmapPtr>::Setter iSetter) :
      ByValAttribute<BitmapPtr>(iName, iGetter, iSetter) {}

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kBitmapType;
    }

    // fromString
    bool fromString(IUIDescription const *iDescription, std::string const &iAttributeValue, BitmapPtr &oValue) const override
    {
      BitmapPtr bitmap;
      if(UIViewCreator::stringToBitmap(&iAttributeValue, bitmap, iDescription))
      {
        oValue = bitmap;
        return true;
      }
      return false;
    }

    // toString
    bool toString(IUIDescription const *iDescription, BitmapPtr const &iValue, std::string &oStringValue) const override
    {
      if(iValue)
        return UIViewCreator::bitmapToString(iValue, oStringValue, iDescription);
      else
        return false;
    }
  };

  /**
   * Specialization for a bitmap attribute. The view must have getter and setter as defined by the
   * types below.
   */
  class FontAttribute : public ByValAttribute<FontPtr>
  {
  public:
    // Constructor
    FontAttribute(std::string const &iName,
                  typename ByValAttribute<FontPtr>::Getter iGetter,
                  typename ByValAttribute<FontPtr>::Setter iSetter) :
      ByValAttribute<FontPtr>(iName, iGetter, iSetter) {}

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kFontType;
    }

    // fromString
    bool fromString(IUIDescription const *iDescription, std::string const &iAttributeValue, FontPtr &oValue) const override
    {
      auto font = iDescription->getFont(iAttributeValue.c_str());
      if(font)
      {
        oValue = font;
        return true;
      }
      return false;
    }

    // toString
    bool toString(IUIDescription const *iDescription, FontPtr const &iValue, std::string &oStringValue) const override
    {
      if(iValue)
      {
        auto fontName = iDescription->lookupFontName(iValue);
        if(fontName)
        {
          oStringValue = fontName;
          return true;
        }
      }
      return false;
    }
  };

  /**
   * Specialization for the margin attribute. The view must have getter and setter as defined by the
   * types below.
   */
  class MarginAttribute : public ByRefAttribute<Margin>
  {
  public:
    MarginAttribute(std::string const &iName,
                    typename ByRefAttribute<Margin>::Getter iGetter,
                    typename ByRefAttribute<Margin>::Setter iSetter) :
      ByRefAttribute<Margin>(iName, iGetter, iSetter) {}

    // fromString
    bool fromString(IUIDescription const *iDescription, std::string const &iAttributeValue, Margin &oValue) const override
    {
      auto parts = Utils::splitFloats<CCoord>(iAttributeValue, ',');

      if(parts.empty())
        return false;

      // look for nan in the array
      if(std::find_if(parts.cbegin(), parts.cend(), [] (auto f) {return std::isnan(f);}) != parts.cend())
        return false;

      if(parts.size() == 1)
      {
        oValue = Margin{parts[0]};
      }
      else
      {
        if(parts.size() < 4)
          return false;

        oValue = Margin{parts[0], parts[1], parts[2], parts[3]};
      }

      return true;
    }

    // toString
    bool toString(IUIDescription const *iDescription, const Margin &iValue, std::string &oStringValue) const override
    {
      std::stringstream str;
      if(iValue.fTop == iValue.fRight && iValue.fTop == iValue.fBottom && iValue.fTop == iValue.fLeft)
        str << iValue.fTop;
      else
      {
        str << iValue.fTop;
        str << ",";
        str << iValue.fRight;
        str << ",";
        str << iValue.fBottom;
        str << ",";
        str << iValue.fLeft;
      }
      oStringValue = str.str();
      return true;
    }
  };

  /**
   * Specialization for the range attribute. The view must have getter and setter as defined by the
   * types below.
   */
  class RangeAttribute : public ByRefAttribute<Range>
  {
  public:
    RangeAttribute(std::string const &iName,
                   typename ByRefAttribute<Range>::Getter iGetter,
                   typename ByRefAttribute<Range>::Setter iSetter) :
      ByRefAttribute<Range>(iName, iGetter, iSetter) {}

    // fromString
    bool fromString(IUIDescription const *iDescription, std::string const &iAttributeValue, Range &oValue) const override
    {
      auto parts = Utils::splitFloats<CCoord>(iAttributeValue, ',');

      if(parts.empty())
        return false;

      // look for nan in the array
      if(std::find_if(parts.cbegin(), parts.cend(), [] (auto f) {return std::isnan(f);}) != parts.cend())
        return false;

      if(parts.size() == 1)
        oValue = Range{parts[0]};
      else
        oValue = Range{parts[0], parts[1]};

      return true;
    }

    // toString
    bool toString(IUIDescription const *iDescription, const Range &iValue, std::string &oStringValue) const override
    {
      std::stringstream str;
      if(iValue.fFrom == iValue.fTo)
        str << iValue.fFrom;
      else
      {
        str << iValue.fFrom;
        str << ",";
        str << iValue.fTo;
      }
      oStringValue = str.str();
      return true;
    }
  };

  /**
   * Specialization for a vector of strings. Note that the strings are assumed to be UTF8 encoded strings.
   */
  class VectorStringAttribute : public ByRefAttribute<std::vector<std::string>>
  {
    using super_type = ByRefAttribute<std::vector<std::string>>;

  public:
    // Constructor
    VectorStringAttribute(std::string const &iName,
                          typename super_type::Getter iGetter,
                          typename super_type::Setter iSetter,
                          char iDelimiter = ',',
                          bool iSkipEmptyEntries = false) :
      super_type(iName, iGetter, iSetter),
      fDelimiter{iDelimiter},
      fSkipEmptyEntries{iSkipEmptyEntries}
    {}

    // fromString
    bool fromString(IUIDescription const *iDescription,
                    std::string const &iAttributeValue,
                    std::vector<std::string> &oValue) const override
    {
      oValue = Utils::splitString(iAttributeValue, fDelimiter, fSkipEmptyEntries);
      return true;
    }

    // toString
    bool toString(IUIDescription const *iDescription,
                  const std::vector<std::string> &iValue,
                  std::string &oStringValue) const override
    {
      oStringValue.clear();
      int i = 0;
      for(auto &entry : iValue)
      {
        if(i > 0)
          oStringValue += fDelimiter;
        if(!entry.empty() || !fSkipEmptyEntries)
        {
          oStringValue += entry;
          i++;
        }
      }
      return true;
    }

  protected:
    char fDelimiter;
    bool fSkipEmptyEntries;
  };

  /**
   * Specialization for a list of possible values defined by the `AttributeMap`
   */
  template<typename T>
  class ListAttribute : public ByValAttribute<T>
  {
    using super_type = ByValAttribute<T>;

  public:
    // Constructor
    ListAttribute(std::string const &iName,
                  typename super_type::Getter iGetter,
                  typename super_type::Setter iSetter,
                  AttrValInitList<T> const &iAttributeValues) :
      super_type(iName, iGetter, iSetter),
      fAttributeValuesMap(iAttributeValues)
#ifndef NDEBUG
      ,fAttributeValuesList{iAttributeValues}
#endif
    {
    }

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kListType;
    }

    // fromString
    bool fromString(IUIDescription const *iDescription,
                    std::string const &iAttributeValue,
                    T &oValue) const override
    {
      if(fAttributeValuesMap.find(iAttributeValue) != fAttributeValuesMap.cend())
      {
        oValue = fAttributeValuesMap.at(iAttributeValue);
        return true;
      }

      DLOG_F(WARNING, "Attribute value '%s' is not valid for '%s'", iAttributeValue.c_str(), ViewAttribute::getName().c_str());
      return false;
    }

    // toString
    bool toString(IUIDescription const *iDescription,
                  T const &iValue,
                  std::string &oStringValue) const override
    {
      auto pos = std::find_if(std::begin(fAttributeValuesList),
                              std::end(fAttributeValuesList),
                              [&iValue](auto entry) -> bool {
                                return entry.second == iValue;
                              });
      if(pos != std::end(fAttributeValuesList))
      {
        oStringValue = pos->first;
        return true;
      }

      return false;
    }

#ifndef NDEBUG
    // getPossibleListValues
    bool getPossibleListValues(std::list<const std::string *> &iValues) const override
    {
      for(auto const &p : fAttributeValuesList)
      {
        iValues.emplace_back(&p.first);
      }
      return true;
    }
#endif

  protected:
    AttrValMap<T> const fAttributeValuesMap;
#ifndef NDEBUG
    std::vector<typename AttrValMap<T>::value_type> const fAttributeValuesList;
#endif
  };

public:
  // Constructor
  explicit TCustomViewCreator(char const *iViewName = nullptr,
                              char const *iDisplayName = nullptr,
                              char const *iBaseViewName = VSTGUI::UIViewCreator::kCView) :
    fViewName{iViewName},
    fDisplayName{iDisplayName},
    fBaseViewName{iBaseViewName},
    fAttributes{}
  {
    // this allows for inheritance!
    if(iViewName != nullptr && iDisplayName != nullptr)
      VSTGUI::UIViewFactory::registerViewCreator(*this);
  }

  // Destructor
  ~TCustomViewCreator() override
  {
    // we simply clear the map since it holds shared pointers which will be discarded when they are no longer
    // held by another object
    fAttributes.clear();
  }

  // getViewName
  IdStringPtr getViewName() const override
  {
    return fViewName;
  }

  // getDisplayName
  UTF8StringPtr getDisplayName() const override
  {
    return fDisplayName;
  }

  // getBaseViewName
  IdStringPtr getBaseViewName() const override
  {
    return fBaseViewName;
  }

  /**
   * This method is called to register all the attributes from another CustomViewCreator (used in case of inheritance)
   */
  template<typename XView>
  void registerAttributes(TCustomViewCreator<XView> const &iOther)
  {
    for(auto attribute : iOther.fAttributes)
    {
      registerAttribute(attribute.second);
    }

    if(std::string(fBaseViewName) == std::string(VSTGUI::UIViewCreator::kCView))
      fBaseViewName = iOther.fBaseViewName;
  }

  /**
   * Registers a color attribute with the given name and getter/setter
   */
  void registerColorAttribute(std::string const &iName,
                              typename ColorAttribute::Getter iGetter,
                              typename ColorAttribute::Setter iSetter)
  {
    registerAttribute<ColorAttribute>(iName, iGetter, iSetter);
  }

  /**
   * Registers a gradient attribute with the given name and getter/setter
   */
  void registerGradientAttribute(std::string const &iName,
                              typename GradientAttribute::Getter iGetter,
                              typename GradientAttribute::Setter iSetter)
  {
    registerAttribute<GradientAttribute>(iName, iGetter, iSetter);
  }

  /**
   * Registers a bitmap attribute with the given name and getter/setter
   */
  void registerBitmapAttribute(std::string const &iName,
                              typename BitmapAttribute::Getter iGetter,
                              typename BitmapAttribute::Setter iSetter)
  {
    registerAttribute<BitmapAttribute>(iName, iGetter, iSetter);
  }

  /**
   * Registers a font attribute with the given name and getter/setter
   */
  void registerFontAttribute(std::string const &iName,
                             typename FontAttribute::Getter iGetter,
                             typename FontAttribute::Setter iSetter)
  {
    registerAttribute<FontAttribute>(iName, iGetter, iSetter);
  }

  /**
   * Registers a Margin attribute with the given name and getter/setter
   */
  void registerMarginAttribute(std::string const &iName,
                               typename MarginAttribute::Getter iGetter,
                               typename MarginAttribute::Setter iSetter)
  {
    registerAttribute<MarginAttribute>(iName, iGetter, iSetter);
  }

  /**
   * Registers a Range attribute with the given name and getter/setter
   */
  void registerRangeAttribute(std::string const &iName,
                               typename RangeAttribute::Getter iGetter,
                               typename RangeAttribute::Setter iSetter)
  {
    registerAttribute<RangeAttribute>(iName, iGetter, iSetter);
  }

  /**
   * Registers a Range attribute with the given name and getter/setter
   */
  void registerVectorStringAttribute(std::string const &iName,
                                     typename VectorStringAttribute::Getter iGetter,
                                     typename VectorStringAttribute::Setter iSetter,
                                     char iDelimiter = ',',
                                     bool iSkipEmptyEntries = false)
  {
    registerAttribute<VectorStringAttribute>(iName, iGetter, iSetter, iDelimiter, iSkipEmptyEntries);
  }

  /**
   * Registers a list attribute with the given name and getter/setter. This kind of attribute is represented in the
   * editor with a drop down list of values: for example "vertical" / "horizontal" and usually maps to an enum of some
   * kind (or in other words, in general `T` is an enum). The `iAttributeValues` parameter is an initializer list
   * of pairs (string,T) which makes it easy to initialize. This list is also used to populate the dropdown (in the
   * editor) in the order defined.
   *
   * Example:
   * ```
   * registerListAttribute<EOrientation>("orientation", &MyView::getOrientation, &MyView::setOrientation,
   *                                     {
   *                                       { "vertical", EOrientation::kVertical },
   *                                       { "horizontal", EOrientation::kHorizontal }
   *                                     }
   * ```
   */
  template<typename T>
  void registerListAttribute(std::string const &iName,
                             typename ListAttribute<T>::Getter iGetter,
                             typename ListAttribute<T>::Setter iSetter,
                             AttrValInitList<T> const &iAttributeValues)
  {
    registerAttribute<ListAttribute<T>>(iName, iGetter, iSetter, iAttributeValues);
  }

  /**
   * Registers a tag attribute with the given name and getter/setter
   */
  void registerTagAttribute(std::string const &iName,
                            typename TagAttribute::Getter iGetter,
                            typename TagAttribute::Setter iSetter)
  {
    registerAttribute<TagAttribute>(iName, iGetter, iSetter);
  }

  /**
   * Registers an Integer (any kind) attribute with the given name and getter/setter
   */
  template<typename TInt>
  void registerIntegerAttribute(std::string const &iName,
                                typename IntegerAttribute<TInt>::Getter iGetter,
                                typename IntegerAttribute<TInt>::Setter iSetter)
  {
    registerAttribute<IntegerAttribute<TInt>>(iName, iGetter, iSetter);
  }

  /**
   * Registers an int attribute with the given name and getter/setter
   */
  void registerIntAttribute(std::string const &iName,
                            typename IntegerAttribute<int32_t>::Getter iGetter,
                            typename IntegerAttribute<int32_t>::Setter iSetter)
  {
    registerIntegerAttribute<int32_t>(iName, iGetter, iSetter);
  }

  /**
   * Registers a float attribute with the given name and getter/setter
   */
  void registerFloatAttribute(std::string const &iName,
                              typename FloatAttribute<float>::Getter iGetter,
                              typename FloatAttribute<float>::Setter iSetter)
  {
    registerAttribute<FloatAttribute<float>>(iName, iGetter, iSetter);
  }

  /**
   * Registers a double attribute with the given name and getter/setter
   */
  void registerDoubleAttribute(std::string const &iName,
                               typename FloatAttribute<double>::Getter iGetter,
                               typename FloatAttribute<double>::Setter iSetter)
  {
    registerAttribute<FloatAttribute<double>>(iName, iGetter, iSetter);
  }

  /**
   * Registers a boolean attribute with the given name and getter/setter
   */
  void registerBooleanAttribute(std::string const &iName,
                                typename BooleanAttribute::Getter iGetter,
                                typename BooleanAttribute::Setter iSetter)
  {
    registerAttribute<BooleanAttribute>(iName, iGetter, iSetter);
  }

  /**
   * This is the factory method which will instantiate the view
   */
  CView *create(const UIAttributes &attributes, const IUIDescription *description) const override
  {
#ifdef JAMBA_DEBUG_LOGGING
    DLOG_F(INFO, "CustomViewCreator<%s>::create()", getViewName());
#endif

    return createCustomView<TView>(CRect(0, 0, 0, 0), attributes, description);
  }

  /**
   * Extract all the attribute values and apply them to the view. This is for example what happens when the xml file
   * is read to populate the view with the stored values
   */
  bool apply(CView *view, const UIAttributes &attributes, const IUIDescription *description) const override
  {
    auto *tv = dynamic_cast<TView *>(view);

    if(tv == nullptr)
      return false;

    for(auto attribute : fAttributes)
    {
      attribute.second->apply(tv, attributes, description);
    }

    return true;
  }

  // getAttributeNames
  bool getAttributeNames(std::list<std::string> &attributeNames) const override
  {
    for(auto attribute : fAttributes)
    {
      attributeNames.emplace_back(attribute.first);
    }
    return true;
  }

  // getAttributeType
  AttrType getAttributeType(const std::string &attributeName) const override
  {
    auto iter = fAttributes.find(attributeName);
    if(iter != fAttributes.cend())
    {
      return iter->second->getType();
    }
    return kUnknownType;
  }

  /**
   * This is used by the editor when instantiating a new view (drag & drop) and populating the various attributes
   * with the values coming from the view */
  bool getAttributeValue(CView *iView,
                         const std::string &iAttributeName,
                         std::string &oStringValue,
                         const IUIDescription *iDescription) const override
  {
    auto *cdv = dynamic_cast<TView *>(iView);

    if(cdv == nullptr)
      return false;

    auto iter = fAttributes.find(iAttributeName);
    if(iter != fAttributes.cend())
    {
      return iter->second->getAttributeValue(cdv, iDescription, oStringValue);
    }

    return false;
  }

#ifndef NDEBUG
  /**
   * This is used by the editor to populate the drop down list for list attributes. Since the editor is only
   * available in debug mode, there is no reason to keep this implementation for release... (default returns `false`) */
  bool getPossibleListValues(const std::string &iAttributeName, std::list<const std::string *> &iValues) const override
  {
    auto iter = fAttributes.find(iAttributeName);
    if(iter != fAttributes.cend())
    {
      return iter->second->getPossibleListValues(iValues);
    }
    return false;
  }
#endif

private:

  // somehow this is required...
  template<typename XView>
  friend class TCustomViewCreator;

  /**
   * Internal method to register an attribute... check that names are not duplicate!
   */
  void registerAttribute(std::shared_ptr<ViewAttribute> iAttribute)
  {
    // making sure there are no duplicates (cannot use loguru here!)
    assert(fAttributes.find(iAttribute->getName()) == fAttributes.cend());
    fAttributes[iAttribute->getName()] = iAttribute;
  }

  /**
   * Generic register attribute
   */
  template<typename TViewAttribute, typename... Args>
  void registerAttribute(std::string const &iName,
                         typename TViewAttribute::Getter iGetter,
                         typename TViewAttribute::Setter iSetter,
                         Args&& ...iArgs)
  {
    std::shared_ptr<ViewAttribute> cva;
    cva.reset(new TViewAttribute(iName, iGetter, iSetter, std::forward<Args>(iArgs)...));
    registerAttribute(cva);
  }


  char const *fViewName;
  char const *fDisplayName;
  char const *fBaseViewName;

  // use a map of shared pointers so that they can easily be copied (see registerAttributes)
  std::map<std::string, std::shared_ptr<ViewAttribute>> fAttributes;
};

/**
 * Convenient definition which assumes that TBaseView has a Creator inner class. Use TCustomViewCreator otherwise.
 */
template<typename TView, typename TBaseView>
class CustomViewCreator : public TCustomViewCreator<TView>
{
public:
  explicit CustomViewCreator(char const *iViewName = nullptr,
                             char const *iDisplayName = nullptr,
                             char const *iBaseViewName = VSTGUI::UIViewCreator::kCView) :
    TCustomViewCreator<TView>(iViewName, iDisplayName, iBaseViewName)
  {
    TCustomViewCreator<TView>::registerAttributes(typename TBaseView::Creator());
  }
};

}
}
}
}