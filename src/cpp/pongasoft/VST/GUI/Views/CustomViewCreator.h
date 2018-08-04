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

#if VSTGUI_LIVE_EDITING
#define EDITOR_MODE 1
#endif

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

private:
  std::string fName;
};

/**
 * Interface which is implemented by views interested in being notified during the creation/parameter apply phase */
class CustomViewInitializer
{
public:
  virtual void afterCreate(UIAttributes const &iAttributes, IUIDescription const *iDescription) = 0;
  virtual void beforeApply(UIAttributes const &iAttributes, IUIDescription const *iDescription) = 0;
  virtual void afterApply(UIAttributes const &iAttributes, IUIDescription const *iDescription) = 0;
};

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
   * Specialization for a tag attribute (vst type int32_t). The view must have getter and setter as defined by the
   * types below.
   */
  class TagAttribute : public ViewAttribute
  {
  public:
    using Getter = int32_t (TView::*)() const;
    using Setter = void (TView::*)(int32_t);

    TagAttribute(std::string const &iName,
                 Getter iGetter,
                 Setter iSetter) :
      ViewAttribute(iName),
      fGetter{iGetter},
      fSetter{iSetter}
    {
    }

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kTagType;
    }

    // apply => set the tag value
    bool apply(CView *iView, const UIAttributes &iAttributes, const IUIDescription *iDescription) override
    {
      auto *tv = dynamic_cast<TView *>(iView);
      if(tv != nullptr)
      {
        auto controlTagAttr = iAttributes.getAttributeValue(getName());
        if(controlTagAttr)
        {
          if(controlTagAttr->length() != 0)
          {
            int32_t tag = iDescription->getTagForName(controlTagAttr->c_str());
            if(tag == -1)
            {
              char *endPtr = nullptr;
              tag = (int32_t) strtol(controlTagAttr->c_str(), &endPtr, 10);
              if(endPtr == controlTagAttr->c_str())
              {
                return false;
              }
            }
            (tv->*fSetter)(tag);
            return true;
          }
        }
      }
      return false;
    }

    // getAttributeValue => get the tag value
    bool getAttributeValue(CView *iView, const IUIDescription *iDescription, std::string &oStringValue) const override
    {
      auto *tv = dynamic_cast<TView *>(iView);
      if(tv != nullptr)
      {
        int32_t tag = (tv->*fGetter)();
        if(tag != -1)
        {
          UTF8StringPtr controlTag = iDescription->lookupControlTagName(tag);
          if(controlTag)
          {
            oStringValue = controlTag;
            return true;
          }
        }
      }
      return false;
    }

  private:
    Getter fGetter;
    Setter fSetter;
  };

  /**
   * Specialization for an Integer attribute (which can be any kind of integer, like short, int32_t, etc..).
   * The view must have getter and setter as defined by the types below.
   */
  template<typename TInt>
  class IntegerAttribute : public ViewAttribute
  {
  public:
    using Getter = TInt (TView::*)() const;
    using Setter = void (TView::*)(TInt);

    IntegerAttribute(std::string const &iName,
                     Getter iGetter,
                     Setter iSetter) :
      ViewAttribute(iName),
      fGetter{iGetter},
      fSetter{iSetter}
    {
    }

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kIntegerType;
    }

    // apply => set the tag value
    bool apply(CView *iView, const UIAttributes &iAttributes, const IUIDescription *iDescription) override
    {
      auto *tv = dynamic_cast<TView *>(iView);
      if(tv != nullptr)
      {
        auto integerAttr = iAttributes.getAttributeValue(getName());
        if(integerAttr)
        {
          char *endPtr = nullptr;
          auto value = static_cast<TInt>(strtol(integerAttr->c_str(), &endPtr, 10));
          if(endPtr == integerAttr->c_str())
          {
            DLOG_F(WARNING, "could not convert <%s> to an integer", integerAttr->c_str());
            return false;
          }
          (tv->*fSetter)(value);
        }
      }
      return false;
    }

    // getAttributeValue => get the integer value
    bool getAttributeValue(CView *iView, const IUIDescription *iDescription, std::string &oStringValue) const override
    {
      auto *tv = dynamic_cast<TView *>(iView);
      if(tv != nullptr)
      {
        TInt value = (tv->*fGetter)();
        std::stringstream str;
        str << value;
        oStringValue = str.str();
        return true;
      }
      return false;
    }

  private:
    Getter fGetter;
    Setter fSetter;
  };

  /**
   * Specialization for the color attribute. The view must have getter and setter as defined by the
   * types below.
   */
  class ColorAttribute : public ViewAttribute
  {
  public:
    using Getter = const CColor &(TView::*)() const;
    using Setter = void (TView::*)(CColor const &);

    ColorAttribute(std::string const &iName,
                   Getter iGetter,
                   Setter iSetter) :
      ViewAttribute(iName),
      fGetter{iGetter},
      fSetter{iSetter}
    {
    }

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kColorType;
    }

    // apply => set a color to the view
    bool apply(CView *iView, const UIAttributes &iAttributes, const IUIDescription *iDescription) override
    {
      auto *tv = dynamic_cast<TView *>(iView);
      if(tv != nullptr)
      {
        auto colorAttr = iAttributes.getAttributeValue(getName());
        if(colorAttr)
        {
          CColor color;
          if(UIViewCreator::stringToColor(colorAttr, color, iDescription))
          {
            (tv->*fSetter)(color);
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
        return UIViewCreator::colorToString((tv->*fGetter)(), oStringValue, iDescription);
      }
      return false;
    }

  private:
    Getter fGetter;
    Setter fSetter;
  };

  /**
   * Specialization for the boolean attribute. The view must have getter and setter as defined by the
   * types below.
   */
  class BooleanAttribute : public ViewAttribute
  {
  public:
    using Getter = bool (TView::*)() const;
    using Setter = void (TView::*)(bool);

    BooleanAttribute(std::string const &iName,
                   Getter iGetter,
                   Setter iSetter) :
      ViewAttribute(iName),
      fGetter{iGetter},
      fSetter{iSetter}
    {
    }

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kBooleanType;
    }

    // apply => set a color to the view
    bool apply(CView *iView, const UIAttributes &iAttributes, const IUIDescription *iDescription) override
    {
      auto *tv = dynamic_cast<TView *>(iView);
      if(tv != nullptr)
      {
        if(iAttributes.getAttributeValue(getName()))
        {
          bool value;
          if(iAttributes.getBooleanAttribute(getName(), value))
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
        oStringValue = (tv->*fGetter)() ? "true" : "false";
        return true;
      }
      return false;
    }

  private:
    Getter fGetter;
    Setter fSetter;
  };

  /**
   * Specialization for a bitmap attribute. The view must have getter and setter as defined by the
   * types below.
   */
  class BitmapAttribute : public ViewAttribute
  {
  public:
    using Getter = BitmapPtr(TView::*)() const;
    using Setter = void (TView::*)(BitmapPtr);

    BitmapAttribute(std::string const &iName,
                   Getter iGetter,
                   Setter iSetter) :
      ViewAttribute(iName),
      fGetter{iGetter},
      fSetter{iSetter}
    {
    }

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kBitmapType;
    }

    // apply => set a bitmap to the view
    bool apply(CView *iView, const UIAttributes &iAttributes, const IUIDescription *iDescription) override
    {
      auto *tv = dynamic_cast<TView *>(iView);
      if(tv != nullptr)
      {
        auto bitmapAttr = iAttributes.getAttributeValue(getName());
        if(bitmapAttr)
        {
          CBitmap *bitmap;
          if(UIViewCreator::stringToBitmap(bitmapAttr, bitmap, iDescription))
          {
            (tv->*fSetter)(bitmap);
            return true;
          }
        }
        else
        {
          (tv->*fSetter)(nullptr);
          return true;
        }
      }
      return false;
    }

    // getAttributeValue => get a bitmap from the view
    bool getAttributeValue(CView *iView, const IUIDescription *iDescription, std::string &oStringValue) const override
    {
      auto *tv = dynamic_cast<TView *>(iView);
      if(tv != nullptr)
      {
        auto bitmap = (tv->*fGetter)();
        if(bitmap)
          return UIViewCreator::bitmapToString(bitmap, oStringValue, iDescription);
      }
      return false;
    }

  private:
    Getter fGetter;
    Setter fSetter;
  };

  /**
 * Specialization for a bitmap attribute. The view must have getter and setter as defined by the
 * types below.
 */
  class FontAttribute : public ViewAttribute
  {
  public:
    using Getter = FontPtr(TView::*)() const;
    using Setter = void (TView::*)(FontPtr);

    FontAttribute(std::string const &iName,
                    Getter iGetter,
                    Setter iSetter) :
      ViewAttribute(iName),
      fGetter{iGetter},
      fSetter{iSetter}
    {
    }

    // getType
    IViewCreator::AttrType getType() override
    {
      return IViewCreator::kFontType;
    }

    // apply => set a font to the view
    bool apply(CView *iView, const UIAttributes &iAttributes, const IUIDescription *iDescription) override
    {
      auto *tv = dynamic_cast<TView *>(iView);
      if(tv != nullptr)
      {
        auto fontAttr = iAttributes.getAttributeValue(getName());
        if(fontAttr)
        {
          auto font = iDescription->getFont(fontAttr->c_str());
          if(font)
          {
            (tv->*fSetter)(font);
            return true;
          }
        }
        else
        {
          (tv->*fSetter)(nullptr);
          return true;
        }
      }
      return false;
    }

    // getAttributeValue => get a font from the view
    bool getAttributeValue(CView *iView, const IUIDescription *iDescription, std::string &oStringValue) const override
    {
      auto *tv = dynamic_cast<TView *>(iView);
      if(tv != nullptr)
      {
        auto font = (tv->*fGetter)();
        if(font)
        {
          auto fontName = iDescription->lookupFontName(font);
          if(fontName)
            oStringValue = fontName;
        }
        return true;
      }
      return false;
    }

  private:
    Getter fGetter;
    Setter fSetter;
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

    auto tv = new TView(CRect(0, 0, 0, 0));

    auto *cvi = dynamic_cast<CustomViewInitializer *>(tv);
    if(cvi != nullptr)
      cvi->afterCreate(attributes, description);

    return tv;
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

    auto *cvi = dynamic_cast<CustomViewInitializer *>(tv);

    if(cvi != nullptr)
      cvi->beforeApply(attributes, description);

    for(auto attribute : fAttributes)
    {
      attribute.second->apply(tv, attributes, description);
    }

    if(cvi != nullptr)
      cvi->afterApply(attributes, description);

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
  template<typename TViewAttribute>
  void registerAttribute(std::string const &iName,
                         typename TViewAttribute::Getter iGetter,
                         typename TViewAttribute::Setter iSetter)
  {
    std::shared_ptr<ViewAttribute> cva;
    cva.reset(new TViewAttribute(iName, iGetter, iSetter));
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