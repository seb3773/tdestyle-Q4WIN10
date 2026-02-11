/* Q4WIN10 widget style for TDE
   based on Plastik by Sandro Giessl
*/

#ifndef __Q4WIN10STYLE_H
#define __Q4WIN10STYLE_H

#include <tdestyle.h>
#include <tqbitmap.h>
#include <tqintcache.h>

#define u_arrow -4, 1, 2, 1, -3, 0, 1, 0, -2, -1, 0, -1, -1, -2
#define d_arrow -4, -2, 2, -2, -3, -1, 1, -1, -2, 0, 0, 0, -1, 1
#define l_arrow 0, -3, 0, 3, -1, -2, -1, 2, -2, -1, -2, 1, -3, 0
#define r_arrow -2, -3, -2, 3, -1, -2, -1, 2, 0, -1, 0, 1, 1, 0

#define QCOORDARRLEN(x) sizeof(x) / (sizeof(QCOORD) * 2)

class TQSettings;
class TQTab;
class TQTimer;

class Q4Win10Style : public TDEStyle {
  TQ_OBJECT

public:
  Q4Win10Style();
  virtual ~Q4Win10Style();
  
  
public slots:

  void applicationPolish(const TQStyleControlElementData &ceData,
                         ControlElementFlags elementFlags, void *);
  void polish(const TQStyleControlElementData &ceData,
              ControlElementFlags elementFlags, void *);
  void unPolish(const TQStyleControlElementData &ceData,
                ControlElementFlags elementFlags, void *);

  void drawTDEStylePrimitive(TDEStylePrimitive kpe, TQPainter *p,
                             const TQStyleControlElementData &ceData,
                             ControlElementFlags elementFlags, const TQRect &r,
                             const TQColorGroup &cg,
                             SFlags flags = Style_Default,
                             const TQStyleOption & = TQStyleOption::Default,
                             const TQWidget *widget = 0) const;

  void drawPrimitive(PrimitiveElement pe, TQPainter *p,
                     const TQStyleControlElementData &ceData,
                     ControlElementFlags elementFlags, const TQRect &r,
                     const TQColorGroup &cg, SFlags flags = Style_Default,
                     const TQStyleOption &opt = TQStyleOption::Default) const;

  void drawControl(ControlElement element, TQPainter *p,
                   const TQStyleControlElementData &ceData,
                   ControlElementFlags elementFlags, const TQRect &r,
                   const TQColorGroup &cg, SFlags flags = Style_Default,
                   const TQStyleOption & = TQStyleOption::Default,
                   const TQWidget *widget = 0) const;

  void drawControlMask(ControlElement, TQPainter *,
                       const TQStyleControlElementData &ceData,
                       ControlElementFlags elementFlags, const TQRect &,
                       const TQStyleOption &, const TQWidget * = 0) const;

  void drawComplexControl(ComplexControl control, TQPainter *p,
                          const TQStyleControlElementData &ceData,
                          ControlElementFlags elementFlags, const TQRect &r,
                          const TQColorGroup &cg, SFlags flags = Style_Default,
                          SCFlags controls = SC_All, SCFlags active = SC_None,
                          const TQStyleOption & = TQStyleOption::Default,
                          const TQWidget *widget = 0) const;

  int pixelMetric(PixelMetric m, const TQStyleControlElementData &ceData,
                  ControlElementFlags elementFlags,
                  const TQWidget *widget = 0) const;

  TQRect subRect(SubRect r, const TQStyleControlElementData &ceData,
                 const ControlElementFlags elementFlags,
                 const TQWidget *widget) const;

  TQRect querySubControlMetrics(
      ComplexControl control, const TQStyleControlElementData &ceData,
      ControlElementFlags elementFlags, SubControl subcontrol,
      const TQStyleOption &opt = TQStyleOption::Default,
      const TQWidget *widget = 0) const;

  void drawComplexControlMask(TQStyle::ComplexControl c, TQPainter *p,
                              const TQStyleControlElementData &ceData,
                              const ControlElementFlags elementFlags,
                              const TQRect &r,
                              const TQStyleOption &o = TQStyleOption::Default,
                              const TQWidget *w = 0) const;

  TQSize sizeFromContents(TQStyle::ContentsType t,
                          const TQStyleControlElementData &ceData,
                          ControlElementFlags elementFlags, const TQSize &s,
                          const TQStyleOption &o, const TQWidget *w = 0) const;

  int styleHint(StyleHint, const TQStyleControlElementData &ceData,
                ControlElementFlags elementFlags,
                const TQStyleOption & = TQStyleOption::Default,
                TQStyleHintReturn * = 0, const TQWidget * = 0) const;

protected:
  enum TabPosition {
    First = 0,
    Middle,
    Last,
    Single // only one tab!
  };

  enum ColorType {
    ButtonContour,
    DragButtonContour,
    DragButtonSurface,
    PanelContour,
    PanelLight,
    PanelLight2,
    PanelDark,
    PanelDark2,
    MouseOverHighlight,
    FocusHighlight,
    CheckMark
  };

  enum WidgetState {
    IsEnabled,
    IsPressed,     // implies IsEnabled
    IsHighlighted, // implies IsEnabled
    IsDisabled
  };

  // the only way i see to provide all these options
  // to renderContour/renderSurface...
  enum SurfaceFlags {
    Draw_Left = 0x00000001,
    Draw_Right = 0x00000002,
    Draw_Top = 0x00000004,
    Draw_Bottom = 0x00000008,
    Highlight_Left = 0x00000010,   // surface
    Highlight_Right = 0x00000020,  // surface
    Highlight_Top = 0x00000040,    // surface
    Highlight_Bottom = 0x00000080, // surface
    Is_Sunken = 0x00000100,        // surface
    Is_Horizontal = 0x00000200,    // surface
    Is_Highlight = 0x00000400,     // surface
    Is_Default = 0x00000800,       // surface
    Is_Disabled = 0x00001000,
    Round_UpperLeft = 0x00002000,
    Round_UpperRight = 0x00004000,
    Round_BottomLeft = 0x00008000,
    Round_BottomRight = 0x00010000,

    Draw_AlphaBlend = 0x00020000
  };

  void renderContour(TQPainter *p, const TQRect &r,
                     const TQColor &backgroundColor,
                     const TQColor &contourColor,
                     const uint flags = Draw_Left | Draw_Right | Draw_Top |
                                        Draw_Bottom | Round_UpperLeft |
                                        Round_UpperRight | Round_BottomLeft |
                                        Round_BottomRight) const;

  void renderMask(TQPainter *p, const TQRect &r, const TQColor &color,
                  const uint flags = Draw_Left | Draw_Right | Draw_Top |
                                     Draw_Bottom | Round_UpperLeft |
                                     Round_UpperRight | Round_BottomLeft |
                                     Round_BottomRight) const;

  void renderSurface(TQPainter *p, const TQRect &r,
                     const TQColor &backgroundColor, const TQColor &buttonColor,
                     const TQColor &highlightColor, int intensity = 5,
                     const uint flags = Draw_Left | Draw_Right | Draw_Top |
                                        Draw_Bottom | Round_UpperLeft |
                                        Round_UpperRight | Round_BottomLeft |
                                        Round_BottomRight |
                                        Is_Horizontal) const;

  inline void renderPixel(TQPainter *p, const TQPoint &pos, const int alpha,
                          const TQColor &color,
                          const TQColor &background = TQColor(),
                          bool fullAlphaBlend = true) const;

  void renderButton(TQPainter *p, const TQRect &r, const TQColorGroup &g,
                    bool sunken = false, bool mouseOver = false,
                    bool horizontal = true, bool enabled = true,
                    bool tdehtmlMode = false, bool isDefault = false) const;

  void renderPanel(TQPainter *p, const TQRect &r, const TQColorGroup &g,
                   const bool pseudo3d = true, const bool sunken = true) const;

  void renderDot(TQPainter *p, const TQPoint &point, const TQColor &baseColor,
                 const bool thick = false, const bool sunken = false) const;

  void renderGradient(TQPainter *p, const TQRect &r, const TQColor &c1,
                      const TQColor &c2, bool horizontal = true) const;

  void renderTab(TQPainter *p, const TQRect &r, const TQColorGroup &g,
                 bool mouseOver = false, const bool selected = false,
                 const bool bottom = false, const TabPosition pos = Middle,
                 const bool triangular = false,
                 const bool cornerWidget = false) const;

  virtual void renderMenuBlendPixmap(KPixmap &pix, const TQColorGroup &cg,
                                     const TQPopupMenu *popup) const;

  virtual bool objectEventHandler(const TQStyleControlElementData &ceData,
                                  ControlElementFlags elementFlags,
                                  void *source, TQEvent *e);

protected slots:
  void tdehtmlWidgetDestroyed(TQObject *w);

  inline TQColor getColor(const TQColorGroup &cg, const ColorType t,
                          const bool enabled = true) const;
  inline TQColor getColor(const TQColorGroup &cg, const ColorType t,
                          const WidgetState s) const;

private:
  // Disable copy constructor and = operator
  Q4Win10Style(const Q4Win10Style &);
  Q4Win10Style &operator=(const Q4Win10Style &);

  bool kickerMode, kornMode;
  mutable bool flatMode;

  int _contrast;
  bool _scrollBarLines;

  bool _drawToolBarSeparator;
  bool _drawToolBarItemSeparator;
  bool _drawFocusRect;
  bool _drawTriangularExpander;
  bool _inputFocusHighlight;
  bool _customOverHighlightColor;
  bool _customFocusHighlightColor;
  bool _customCheckMarkColor;
  TQColor _overHighlightColor;
  TQColor _focusHighlightColor;
  TQColor _checkMarkColor;

  // track tdehtml widgets.
  TQMap<const TQWidget *, bool> tdehtmlWidgets;

  // Animation support removed

  // pixmap cache.
  enum CacheEntryType { cSurface, cGradientTile, cAlphaDot };
  struct CacheEntry {
    CacheEntryType type;
    int width;
    int height;
    TQRgb c1Rgb;
    TQRgb c2Rgb;
    bool horizontal;

    TQPixmap *pixmap;

    CacheEntry(CacheEntryType t, int w, int h, TQRgb c1, TQRgb c2 = 0,
               bool hor = false, TQPixmap *p = 0)
        : type(t), width(w), height(h), c1Rgb(c1), c2Rgb(c2), horizontal(hor),
          pixmap(p) {}

    ~CacheEntry() { delete pixmap; }

    int key() {
      // create an int key from the properties which is used to refer to entries
      // in the TQIntCache. the result may not be 100% correct as we don't have
      // so much space in one integer -- use
      // == operator after find to make sure we got the right one. :)
      return horizontal ^ (type << 1) ^ (width << 5) ^ (height << 10) ^
             (c1Rgb << 19) ^ (c2Rgb << 22);
    }

    bool operator==(const CacheEntry &other) {
      bool match = (type == other.type) && (width == other.width) &&
                   (height == other.height) && (c1Rgb == other.c1Rgb) &&
                   (c1Rgb == other.c1Rgb) && (horizontal == other.horizontal);
      return match;
    }
  };
  TQIntCache<CacheEntry> *pixmapCache;

  private:
    // For KPE_ListViewBranch

  // For KPE_ListViewBranch

};

#endif // __Q4WIN10STYLE_H
