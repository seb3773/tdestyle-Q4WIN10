/* Plastik widget style for KDE 3
   Copyright (C) 2003 Sandro Giessl <ceebx@users.sourceforge.net>

   based on the KDE style "dotNET":

   Copyright (C) 2001-2002, Chris Lee <clee@kde.org>
                            Carsten Pfeiffer <pfeiffer@kde.org>
                            Karol Szwed <gallium@kde.org>
   Drawing routines completely reimplemented from KDE3 HighColor, which was
   originally based on some stuff from the KDE2 HighColor.

   based on drawing routines of the style "Keramik":

   Copyright (c) 2002 Malte Starostik <malte@kde.org>
             (c) 2002,2003 Maksim Orlovich <mo002j@mail.rochester.edu>
   based on the KDE3 HighColor Style
   Copyright (C) 2001-2002 Karol Szwed      <gallium@kde.org>
             (C) 2001-2002 Fredrik H�glund  <fredrik@kde.org>
   Drawing routines adapted from the KDE2 HCStyle,
   Copyright (C) 2000 Daniel M. Duley       <mosfet@kde.org>
             (C) 2000 Dirk Mueller          <mueller@kde.org>
             (C) 2001 Martijn Klingens      <klingens@kde.org>
   Progressbar code based on TDEStyle,
   Copyright (C) 2001-2002 Karol Szwed <gallium@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#include <kpixmap.h>
#include <tqbitmap.h>
#include <tqapplication.h>
#include <tqcheckbox.h>
#include <tqcleanuphandler.h>
#include <tqcursor.h>
#include <tqcombobox.h>
#include <tqdrawutil.h>
#include <tqheader.h>
#include <tqimage.h>
#include <tqlistview.h>
#include <tqlineedit.h>
#include <tqlistbox.h>
#include <tqmenubar.h>
#include <tqpainter.h>
#include <tqpointarray.h>
#include <tqpopupmenu.h>
#include <tqprogressbar.h>
#include <tqpushbutton.h>
#include <tqradiobutton.h>
#include <tqregion.h>
#include <tqscrollbar.h>
#include <tqsettings.h>
#include <tqslider.h>
#include <tqstylefactory.h>
#include <tqstyleplugin.h>
#include <tqtabbar.h>
#include <tqtabwidget.h>
#include <tqtimer.h>
#include <tqtoolbar.h>
#include <tqtoolbutton.h>
#include <tqvariant.h>

// Fast division by 255 using integer bit-shifts (exact for x in [0, 65535])
static inline int div255(int x) {
  return (x + 1 + (x >> 8)) >> 8;
}

static TQColor alphaBlendColors(const TQColor &bgColor, const TQColor &fgColor,
                                const int a) {
  TQRgb rgb = bgColor.rgb();
  TQRgb rgb_b = fgColor.rgb();
  int alpha = a;
  if (alpha > 255)
    alpha = 255;
  if (alpha < 0)
    alpha = 0;
  int inv_alpha = 255 - alpha;

  return TQColor(
      tqRgb(div255(tqRed(rgb_b) * inv_alpha + tqRed(rgb) * alpha),
            div255(tqGreen(rgb_b) * inv_alpha + tqGreen(rgb) * alpha),
            div255(tqBlue(rgb_b) * inv_alpha + tqBlue(rgb) * alpha)));
}

#include <X11/Xlib.h>
#include <X11/Xatom.h>

// Helper to set X11 property for Communication with Decoration
static void setMenuBarHeightProperty(TQWidget *menuBar, int height) {
    if (!menuBar || !menuBar->topLevelWidget()) return;
    WId winId = menuBar->topLevelWidget()->winId();
    Display *dpy = tqt_xdisplay();
    static Atom atom = None;
    if (atom == None) {
        atom = XInternAtom(dpy, "_Q4WIN10_MENUBAR_HEIGHT", False);
    }
    
    if (height > 0) {
        long data = height;
        XChangeProperty(dpy, winId, atom, XA_CARDINAL, 32, PropModeReplace, 
                        (unsigned char*)&data, 1);
    } else {
        XDeleteProperty(dpy, winId, atom);
    }
}

#include "q4win10style.h"
#include <tdeglobal.h>
#include <kinstance.h>
#include <tdeconfig.h>
#include "q4win10style.moc"


// some bitmaps for the radio button so it's easier to handle the circle
// stuff... 13x13
// Radio button bitmaps removed

// Checkmarks now use Unicode characters, bits removed
const uint CHECKMARKSIZE = 9; // 9x9

// radio mark
// Radio mark bitmaps removed (replaced by drawing)

// popupmenu item constants...
static const int itemHMargin = 6;
static const int itemFrame = 2;
static const int arrowHMargin = 6;
static const int rightBorder = 12;

// -- Style Plugin Interface -------------------------
class Q4Win10StylePlugin : public TQStylePlugin {
public:
  Q4Win10StylePlugin() {}
  ~Q4Win10StylePlugin() {}

  TQStringList keys() const { return TQStringList() << "q4win10" << "Q4WIN10" << "Q4Win10"; }

  TQStyle *create(const TQString &key) {
    if (key.lower() == "q4win10")
      return new Q4Win10Style;
    return 0;
  }
};

TQ_EXPORT_PLUGIN(Q4Win10StylePlugin)
// -- end --

Q4Win10Style::Q4Win10Style()
    : TDEStyle(AllowMenuTransparency, WindowsStyleScrollBar), kickerMode(false),
      kornMode(false), m_darkMode(false), m_win11Mode(false) {


  // Hardcoded options for flat Windows 10 style - no config panel
  _contrast = 6;
  _scrollBarLines = false; // No scroll bar grip lines
  // _animateProgressBar removed
  _drawToolBarSeparator = false;     // No toolbar separator
  _drawToolBarItemSeparator = false; // No toolbar item separator
  _drawFocusRect = false;            // No focus rectangles
  _drawTriangularExpander = false;   // Square expanders
  _inputFocusHighlight = false;      // No active text field highlight
  _customOverHighlightColor = false;
  _overHighlightColor = TQColor("black");
  _customFocusHighlightColor = false;
  _focusHighlightColor = TQColor("black");
  _customCheckMarkColor = false;
  _checkMarkColor = TQColor("black");



  // Animation disabled - no timer needed
  
  readConfig();
}

void Q4Win10Style::readConfig() {
  // 1. Safe default values
  m_darkMode = false;
  m_win11Mode = false;

  // 2. In non-TDE (pure TQt3) applications, instantiate a static fallback TDEInstance
  // so TDEGlobal::dirs() and TDEConfig never dereference a NULL pointer.
  static TDEInstance *s_instance = NULL;
  if (!TDEGlobal::_instance && !s_instance) {
    s_instance = new TDEInstance("q4win10");
  }

  // 3. Guaranteed safe configuration reading
  if (TDEGlobal::_instance) {
    TDEConfig config("q4win10stylerc");
    config.reparseConfiguration();
    config.setGroup("Settings");
    m_darkMode = config.readBoolEntry("darkMode", false);
    m_win11Mode = config.readBoolEntry("win11Mode", false);
  }
}

// Animations disabled - updateProgressPos removed

Q4Win10Style::~Q4Win10Style() {
}

void Q4Win10Style::applicationPolish(const TQStyleControlElementData &ceData,
                                     ControlElementFlags, void *ptr) {
  if (ceData.widgetObjectTypes.contains("TQApplication")) {
    TQApplication *app = reinterpret_cast<TQApplication *>(ptr);

    if (!qstrcmp(app->argv()[0], "kicker"))
      kickerMode = true;
    else if (!qstrcmp(app->argv()[0], "korn"))
      kornMode = true;
  }
}

void Q4Win10Style::polish(const TQStyleControlElementData &ceData,
                          ControlElementFlags elementFlags, void *ptr) {
  if (ceData.widgetObjectTypes.contains("TQWidget")) {
    TQWidget *widget = reinterpret_cast<TQWidget *>(ptr);

    if (!strcmp(widget->name(), "__tdehtml")) { // is it a tdehtml widget...?
      tdehtmlWidgets[widget] = true;
      connect(widget, TQT_SIGNAL(destroyed(TQObject *)), this,
              TQT_SLOT(tdehtmlWidgetDestroyed(TQObject *)));
    }

    // use tqt_cast where possible to check if the widget inheits one of the
    // classes. might improve performance compared to TQObject::inherits()
    if (::tqt_cast<TQPushButton *>(widget) ||
        ::tqt_cast<TQComboBox *>(widget) ||
        ::tqt_cast<TQSpinWidget *>(widget) || ::tqt_cast<TQSlider *>(widget) ||
        ::tqt_cast<TQCheckBox *>(widget) ||
        ::tqt_cast<TQRadioButton *>(widget) ||
        ::tqt_cast<TQToolButton *>(widget) ||
        widget->inherits("TQSplitterHandle")) {
      //         widget->setBackgroundMode(PaletteBackground);
      installObjectEventHandler(ceData, elementFlags, ptr, this);
    } else if (::tqt_cast<TQLineEdit *>(widget)) {
      installObjectEventHandler(ceData, elementFlags, ptr, this);
    } else if (::tqt_cast<TQTabBar *>(widget)) {
      widget->setMouseTracking(true);
      installObjectEventHandler(ceData, elementFlags, ptr, this);
    } else if (TQMenuBar *menuBar = ::tqt_cast<TQMenuBar *>(widget)) {
      // Minimal setup: Just palette and background mode.
      // We rely on standard QMenuBar event handling (Zen Motif approach).
      
      // Create a specific palette for the Menu Bar:
      // We want the standard robust repaint behavior of PaletteButton,
      // but visually we want it to be the Standard Base Color (usually White).
      // We retrieve pal.active().base() which is dynamic (not hardcoded).
      TQPalette pal = menuBar->palette();
      TQColor baseColor = pal.active().base(); 
      pal.setColor(TQColorGroup::Button, baseColor);
      pal.setColor(TQColorGroup::Background, baseColor);
      menuBar->setPalette(pal);

      menuBar->setBackgroundMode(TQt::PaletteButton);
      
      // Communicate Menu Bar Height to Window Decoration (for seamless borders)
      setMenuBarHeightProperty(menuBar, menuBar->height());
    } else if (TQPopupMenu *popup = ::tqt_cast<TQPopupMenu *>(widget)) {
      widget->setBackgroundMode(TQt::PaletteBackground);
      if (m_win11Mode) {
        installObjectEventHandler(ceData, elementFlags, ptr, this);
      }
    } else if (!qstrcmp(widget->name(), "tde toolbar widget")) {
      installObjectEventHandler(ceData, elementFlags, ptr, this);
    }

    // Animations disabled
  }

  // Dark Mode: override disabled text color for visibility on dark backgrounds
  if (m_darkMode && ceData.widgetObjectTypes.contains("TQWidget")) {
    TQWidget *widget = reinterpret_cast<TQWidget *>(ptr);
    TQPalette pal = widget->palette();
    // Use a middle-dark grey (~95) for disabled text (User requested darker)
    TQColor disabledText(95, 95, 95);
    pal.setColor(TQPalette::Disabled, TQColorGroup::Text, disabledText);
    pal.setColor(TQPalette::Disabled, TQColorGroup::ButtonText, disabledText);
    pal.setColor(TQPalette::Disabled, TQColorGroup::Foreground, disabledText);
    pal.setColor(TQPalette::Disabled, TQColorGroup::Mid, disabledText); // Used by some styles for disabled text
    widget->setPalette(pal);
  }

  TDEStyle::polish(ceData, elementFlags, ptr);
}

void Q4Win10Style::unPolish(const TQStyleControlElementData &ceData,
                            ControlElementFlags elementFlags, void *ptr) {
  if (ceData.widgetObjectTypes.contains("TQWidget")) {
    TQWidget *widget = reinterpret_cast<TQWidget *>(ptr);

    if (!strcmp(widget->name(), "__tdehtml")) { // is it a tdehtml widget...?
      tdehtmlWidgets.remove(widget);
    }

    // use tqt_cast to check if the widget inheits one of the classes.
    if (::tqt_cast<TQPushButton *>(widget) ||
        ::tqt_cast<TQComboBox *>(widget) ||
        ::tqt_cast<TQSpinWidget *>(widget) || ::tqt_cast<TQSlider *>(widget) ||
        ::tqt_cast<TQCheckBox *>(widget) ||
        ::tqt_cast<TQRadioButton *>(widget) ||
        ::tqt_cast<TQToolButton *>(widget) ||
        ::tqt_cast<TQLineEdit *>(widget) ||
        widget->inherits("TQSplitterHandle")) {
      removeObjectEventHandler(ceData, elementFlags, ptr, this);
    } else if (::tqt_cast<TQTabBar *>(widget)) {
      widget->setMouseTracking(false);
      removeObjectEventHandler(ceData, elementFlags, ptr, this);
    } else if (::tqt_cast<TQMenuBar *>(widget)) {
        TQMenuBar *menuBar = ::tqt_cast<TQMenuBar *>(widget);
        setMenuBarHeightProperty(menuBar, 0); // Clear property
    } else if (::tqt_cast<TQPopupMenu *>(widget)) {
      widget->setBackgroundMode(PaletteBackground);
    } else if (!qstrcmp(widget->name(), "tde toolbar widget")) {
      removeObjectEventHandler(ceData, elementFlags, ptr, this);
    }

    // progAnimWidgets removed
  }

  TDEStyle::unPolish(ceData, elementFlags, ptr);
}

void Q4Win10Style::tdehtmlWidgetDestroyed(TQObject *obj) {
  tdehtmlWidgets.remove(static_cast<TQWidget *>(obj));
}

void Q4Win10Style::renderContour(TQPainter *p, const TQRect &r,
                                 const TQColor &backgroundColor,
                                 const TQColor &contour,
                                 const uint flags) const {
  if ((r.width() <= 0) || (r.height() <= 0))
    return;

  const bool drawLeft = flags & Draw_Left;
  const bool drawRight = flags & Draw_Right;
  const bool drawTop = flags & Draw_Top;
  const bool drawBottom = flags & Draw_Bottom;
  const bool disabled = flags & Is_Disabled;

  TQColor contourColor;
  if (disabled) {
    if (contour == backgroundColor)
      contourColor = backgroundColor;
    else
      contourColor = m_darkMode ? backgroundColor.light(115) : backgroundColor.dark(150);
  } else {
    contourColor = contour;
  }

  p->setPen(contourColor);

  if (drawLeft)
    p->drawLine(r.left(), r.top(), r.left(), r.bottom());
  if (drawRight)
    p->drawLine(r.right(), r.top(), r.right(), r.bottom());
  if (drawTop)
    p->drawLine(r.left(), r.top(), r.right(), r.top());
  if (drawBottom)
    p->drawLine(r.left(), r.bottom(), r.right(), r.bottom());
}

void Q4Win10Style::renderMask(TQPainter *p, const TQRect &r,
                              const TQColor &color, const uint flags) const {
  if ((r.width() <= 0) || (r.height() <= 0))
    return;

  const bool roundUpperLeft = flags & Round_UpperLeft;
  const bool roundUpperRight = flags & Round_UpperRight;
  const bool roundBottomLeft = flags & Round_BottomLeft;
  const bool roundBottomRight = flags & Round_BottomRight;

  p->fillRect(TQRect(r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2),
              color);

  p->setPen(color);
  // sides
  p->drawLine(roundUpperLeft ? r.x() + 1 : r.x(), r.y(),
              roundUpperRight ? r.right() - 1 : r.right(), r.y());
  p->drawLine(roundBottomLeft ? r.x() + 1 : r.x(), r.bottom(),
              roundBottomRight ? r.right() - 1 : r.right(), r.bottom());
  p->drawLine(r.x(), roundUpperLeft ? r.y() + 1 : r.y(), r.x(),
              roundBottomLeft ? r.bottom() - 1 : r.bottom());
  p->drawLine(r.right(), roundUpperLeft ? r.y() + 1 : r.y(), r.right(),
              roundBottomLeft ? r.bottom() - 1 : r.bottom());
}

void Q4Win10Style::renderSurface(TQPainter *p, const TQRect &r,
                                 const TQColor &backgroundColor,
                                 const TQColor &buttonColor,
                                 const TQColor &highlightColor, int intensity,
                                 const uint flags) const {
  if ((r.width() <= 0) || (r.height() <= 0))
    return;

  const bool disabled = flags & Is_Disabled;
  const bool sunken = flags & Is_Sunken;
  const bool highlight = (flags & Is_Highlight) && !disabled && !sunken;

  const bool drawLeft = flags & Draw_Left;
  const bool drawRight = flags & Draw_Right;
  const bool drawTop = flags & Draw_Top;
  const bool drawBottom = flags & Draw_Bottom;

  TQColor fillColor;
  if (disabled) {
    fillColor = backgroundColor;
  } else if (sunken) {
    fillColor = buttonColor.dark(110);
  } else if (highlight) {
    fillColor = highlightColor; // Use the actual highlight color passed in
  } else {
    fillColor = buttonColor;
  }

  int x = r.x();
  int y = r.y();
  int w = r.width();
  int h = r.height();

  if (drawLeft) {
    x++;
    w--;
  }
  if (drawTop) {
    y++;
    h--;
  }
  if (drawRight) {
    w--;
  }
  if (drawBottom) {
    h--;
  }

  if (w > 0 && h > 0) {
    if (m_win11Mode && (flags & (Round_UpperLeft | Round_UpperRight | Round_BottomLeft | Round_BottomRight))) {
      int radius = 8; // Internal radius is 8px (outer border is 9px)
      p->save();
      // Expand clip by 1px to allow antialiasing to breathe
      p->setClipRect(x - 1, y - 1, w + 2, h + 2);
      
      TQRect roundRect(x, y, w, h);
      int ext = radius * 2;
      
      // Extension logic: if a corner is square, extend in BOTH directions forming it
      // to move the rounding arcs far away from the clip area.
      if (!(flags & Round_UpperLeft)) {
          // Placeholder handled by combined extensions below
      }

      if (!(flags & Round_UpperLeft) && !(flags & Round_BottomLeft)) roundRect.setLeft(roundRect.left() - ext);
      if (!(flags & Round_UpperRight) && !(flags & Round_BottomRight)) roundRect.setRight(roundRect.right() + ext);
      if (!(flags & Round_UpperLeft) && !(flags & Round_UpperRight)) roundRect.setTop(roundRect.top() - ext);
      if (!(flags & Round_BottomLeft) && !(flags & Round_BottomRight)) roundRect.setBottom(roundRect.bottom() + ext);

      // Special cases for single-corner rounding (e.g. SpinWidget buttons)
      if ((flags & Round_UpperRight) && !(flags & Round_BottomRight) && !(flags & Round_UpperLeft)) {
          // Only top-right rounded: extend left and bottom
          roundRect.setLeft(x - ext);
          roundRect.setBottom(y + h + ext);
      }
      if ((flags & Round_BottomRight) && !(flags & Round_UpperRight) && !(flags & Round_BottomLeft)) {
          // Only bottom-right rounded: extend left and top
          roundRect.setLeft(x - ext);
          roundRect.setTop(y - ext);
      }
      if ((flags & Round_UpperLeft) && !(flags & Round_BottomLeft) && !(flags & Round_UpperRight)) {
          // Only top-left rounded: extend right and bottom
          roundRect.setRight(x + w + ext);
          roundRect.setBottom(y + h + ext);
      }
      if ((flags & Round_BottomLeft) && !(flags & Round_UpperLeft) && !(flags & Round_BottomRight)) {
          // Only bottom-left rounded: extend right and top
          roundRect.setRight(x + w + ext);
          roundRect.setTop(y - ext);
      }

      int xr = (roundRect.width() > 0) ? (200 * radius / roundRect.width()) : 0;
      int yr = (roundRect.height() > 0) ? (200 * radius / roundRect.height()) : 0;

      p->setPen(TQt::NoPen);
      p->setBrush(fillColor);
      p->drawRoundRect(roundRect, xr, yr);

      p->restore();
    } else {
      p->fillRect(x, y, w, h, fillColor);
    }
  }
}

void Q4Win10Style::renderButton(TQPainter *p, const TQRect &r,
                                const TQColorGroup &g, bool sunken,
                                bool mouseOver, bool horizontal, bool enabled,
                                bool tdehtmlMode, bool isDefault) const {
  // small fix for the kicker buttons...
  if (kickerMode)
    enabled = true;

  const TQPen oldPen(p->pen());

  uint contourFlags = Draw_Left | Draw_Right | Draw_Top | Draw_Bottom;
  if (!enabled)
    contourFlags |= Is_Disabled;
  if (tdehtmlMode)
    contourFlags |= Draw_AlphaBlend;

  uint surfaceFlags = Draw_Left | Draw_Right | Draw_Top | Draw_Bottom;
  if (horizontal)
    surfaceFlags |= Is_Horizontal;
  if (!enabled)
    surfaceFlags |= Is_Disabled;
  else {
    if (sunken)
      surfaceFlags |= Is_Sunken;
    else {
      if (mouseOver) {
        surfaceFlags |= Is_Highlight;
        if (horizontal) {
          surfaceFlags |= Highlight_Top;
          surfaceFlags |= Highlight_Bottom;
        } else {
          surfaceFlags |= Highlight_Left;
          surfaceFlags |= Highlight_Right;
        }
      }
    }
  }

  // Logic: Win11 Mode = Rounded Corners. Win10 Mode (Default) = Square/Flat.
  // Note: flatMode was a legacy override.
  bool rounded = m_win11Mode;

  if (rounded) {
    // Win11: true rounded corners via drawRoundRect (radius = 4px)
    // xRound/yRound are percentages of w/h in TQt3: radius_px * 200 / dimension
    const int radius = 6; // pixels

    // Determine fill color
    TQColor fillColor;
    if (!enabled)
      fillColor = g.background();
    else if (sunken)
      fillColor = g.button().dark(110);
    else if (mouseOver)
      fillColor = getColor(g, MouseOverHighlight);
    else
      fillColor = g.button();

    // Determine border color
    TQColor borderColor;
    if (!enabled)
      borderColor = m_darkMode ? g.background().light(130) : g.background().dark(130);
    else if (mouseOver || isDefault)
      borderColor = getColor(g, FocusHighlight);
    else if (sunken)
      borderColor = m_darkMode ? g.button().light(110) : g.button().dark(130);
    else
      borderColor = m_darkMode ? g.button().light(125) : g.button().dark(105 + _contrast * 3);

    // Compute xRound/yRound as percentage (TQt3 convention: 200*r/dim)
    int xRound = (r.width()  > 0) ? (200 * radius / r.width())  : 0;
    int yRound = (r.height() > 0) ? (200 * radius / r.height()) : 0;

    // Fill
    p->setPen(TQt::NoPen);
    p->setBrush(fillColor);
    p->drawRoundRect(r, xRound, yRound);

    // Border
    p->setPen(borderColor);
    p->setBrush(TQt::NoBrush);
    p->drawRoundRect(r, xRound, yRound);
  } else {
    renderContour(p, r, g.background(), g.button().dark(105 + _contrast * 3),
                  contourFlags);
    renderSurface(
        p, TQRect(r.left() + 1, r.top() + 1, r.width() - 2, r.height() - 2),
        g.background(), g.button(), getColor(g, MouseOverHighlight),
        _contrast / 2, surfaceFlags);
  }

  p->setPen(oldPen);
}


void Q4Win10Style::renderPanel(TQPainter *p, const TQRect &r,
                               const TQColorGroup &g, const bool pseudo3d,
                               const bool sunken) const {
  int x, x2, y, y2, w, h;
  r.rect(&x, &y, &w, &h);
  r.coords(&x, &y, &x2, &y2);

  if (kickerMode && p->device() &&
      p->device()->devType() == TQInternal::Widget &&
      TQCString(static_cast<TQWidget *>(p->device())->className()) ==
          "FittsLawFrame") {
    const TQCOORD corners[] = {x, y2, x, y, x2, y, x2, y2};
    if (m_darkMode) {
      p->setPen(g.background().light(145));
      p->drawPolyline(TQPointArray(4, corners), 0, 4);
    } else {
      if (sunken) {
        const TQCOORD sunkenCorners[] = {x2, y, x2, y2, x, y2, x, y};
        p->setPen(g.background().dark());
        p->drawConvexPolygon(TQPointArray(4, sunkenCorners));
        p->setPen(g.background().light());
        p->drawPolyline(TQPointArray(4, sunkenCorners), 0, 3);
      } else {
        p->setPen(g.background().dark());
        p->drawPolygon(TQPointArray(4, corners));
        p->setPen(g.background().light());
        p->drawPolyline(TQPointArray(4, corners), 0, 3);
      }
    }
  } else {
    renderContour(p, r, g.background(), getColor(g, PanelContour));

  }
}

void Q4Win10Style::renderMenuBlendPixmap(
    KPixmap &pix, const TQColorGroup &cg,
    const TQPopupMenu * /* popup */) const {
  pix.fill(cg.background().light(105));
}

void Q4Win10Style::renderTab(TQPainter *p, const TQRect &r,
                             const TQColorGroup &g, bool mouseOver,
                             const bool selected, const bool bottom,
                             const TabPosition pos, const bool triangular,
                             const bool cornerWidget) const {
  const bool reverseLayout = TQApplication::reverseLayout();

  const bool isFirst = (pos == First) || (pos == Single);
  const bool isLast = (pos == Last);
  const bool isSingle = (pos == Single);

  // 1. Colors
  TQColor fillColor;
  TQColor borderColor;

  if (selected) {
    // Active tab: standard background color (blends seamlessly with the panel)
    fillColor = g.background();
    borderColor = m_darkMode ? g.background().light(145) : g.background().dark(130);
  } else if (mouseOver) {
    fillColor = getColor(g, MouseOverHighlight);
    borderColor = g.background().dark(120);
  } else {
    // Inactive tab: subtle darker shade in light mode, lighter in dark mode
    fillColor = m_darkMode ? g.background().light(130) : g.background().dark(110);
    borderColor = g.background().dark(120);
  }

  // 2. Geometry: Inactive tabs are 2px shorter, pushed away from the panel
  TQRect drawRect = r;
  if (!selected) {
    if (!bottom) {
      // Top tabs: panel is at bottom, so push down the top by 2px (gap at top)
      drawRect = TQRect(r.left(), r.top() + 2, r.width(), r.height() - 2);
    } else {
      // Bottom tabs: panel is at top, so pull up the bottom by 2px (gap at bottom)
      drawRect = TQRect(r.left(), r.top(), r.width(), r.height() - 2);
    }
  }

  if (m_win11Mode) {
    // Clear parent cell to prevent leftover rendering when tabs switch states/heights
    p->fillRect(r, g.background());

    // --- Windows 11 Mode: 6px rounded top/bottom corners ---
    const int radius = 6;
    int xRound = (drawRect.width()  > 0) ? (200 * radius / drawRect.width())  : 0;
    int yRound = (drawRect.height() > 0) ? (200 * radius / drawRect.height()) : 0;

    // Fill rounded rect
    p->setPen(TQt::NoPen);
    p->setBrush(fillColor);
    p->drawRoundRect(drawRect, xRound, yRound);

    // Square off unwanted corners (bottom for top tabs, top for bottom tabs)
    if (!bottom) {
      p->fillRect(drawRect.left(), drawRect.bottom() - radius + 1, radius, radius, fillColor);
      p->fillRect(drawRect.right() - radius + 1, drawRect.bottom() - radius + 1, radius, radius, fillColor);
    } else {
      p->fillRect(drawRect.left(), drawRect.top(), radius, radius, fillColor);
      p->fillRect(drawRect.right() - radius + 1, drawRect.top(), radius, radius, fillColor);
    }

    // Draw border with surgical clipping
    p->save();
    if (!bottom) {
      p->setClipRect(drawRect.x(), drawRect.y(), drawRect.width(), drawRect.height() - radius + 1);
      p->setPen(borderColor);
      p->setBrush(TQt::NoBrush);
      p->drawRoundRect(drawRect, xRound, yRound);
      p->restore();

      p->setPen(borderColor);
      p->drawLine(drawRect.left(), drawRect.bottom() - radius + 1, drawRect.left(), drawRect.bottom());
      p->drawLine(drawRect.right(), drawRect.bottom() - radius + 1, drawRect.right(), drawRect.bottom());
      if (!selected) {
        p->drawLine(drawRect.left(), drawRect.bottom(), drawRect.right(), drawRect.bottom());
      }
    } else {
      p->setClipRect(drawRect.x(), drawRect.y() + radius - 1, drawRect.width(), drawRect.height() - radius + 1);
      p->setPen(borderColor);
      p->setBrush(TQt::NoBrush);
      p->drawRoundRect(drawRect, xRound, yRound);
      p->restore();

      p->setPen(borderColor);
      p->drawLine(drawRect.left(), drawRect.top(), drawRect.left(), drawRect.top() + radius - 1);
      p->drawLine(drawRect.right(), drawRect.top(), drawRect.right(), drawRect.top() + radius - 1);
      if (!selected) {
        p->drawLine(drawRect.left(), drawRect.top(), drawRect.right(), drawRect.top());
      }
    }
  } else {
    // --- Windows 10 Mode: Sharp, flat square tabs ---
    // Fill flat rectangle
    p->fillRect(drawRect, fillColor);

    p->setPen(borderColor);
    if (!bottom) {
      // Top tabs: draw top, left, right borders
      p->drawLine(drawRect.left(), drawRect.top(), drawRect.right(), drawRect.top());
      p->drawLine(drawRect.left(), drawRect.top(), drawRect.left(), drawRect.bottom());
      p->drawLine(drawRect.right(), drawRect.top(), drawRect.right(), drawRect.bottom());
      // Inactive tab: close bottom border; Active tab: leave open to merge seamlessly with panel
      if (!selected) {
        p->drawLine(drawRect.left(), drawRect.bottom(), drawRect.right(), drawRect.bottom());
      }
    } else {
      // Bottom tabs: draw bottom, left, right borders
      p->drawLine(drawRect.left(), drawRect.bottom(), drawRect.right(), drawRect.bottom());
      p->drawLine(drawRect.left(), drawRect.top(), drawRect.left(), drawRect.bottom());
      p->drawLine(drawRect.right(), drawRect.top(), drawRect.right(), drawRect.bottom());
      // Inactive tab: close top border; Active tab: leave open to merge seamlessly with panel
      if (!selected) {
        p->drawLine(drawRect.left(), drawRect.top(), drawRect.right(), drawRect.top());
      }
    }
  }
}

void Q4Win10Style::drawTDEStylePrimitive(
    TDEStylePrimitive kpe, TQPainter *p,
    const TQStyleControlElementData &ceData, ControlElementFlags elementFlags,
    const TQRect &r, const TQColorGroup &cg, SFlags flags,
    const TQStyleOption &opt, const TQWidget *widget) const {
  // some "global" vars...
  const bool enabled = (flags & Style_Enabled);

  //  SLIDER
  //  ------
  switch (kpe) {
  case KPE_SliderGroove: {
    bool horizontal = ceData.orientation == TQt::Horizontal;
    TQColor selectionColor = cg.highlight();
    if (!enabled)
      selectionColor = selectionColor.light(140);

    if (horizontal) {
      int center = r.y() + r.height() / 2;
      // Windows 10 style: Thin blue path
      p->fillRect(r.left(), center - 1, r.width(), 2, selectionColor);
    } else {
      int center = r.x() + r.width() / 2;
      // Windows 10 style: Thin blue path
      p->fillRect(center - 1, r.top(), 2, r.height(), selectionColor);
    }
    break;
  }

  case KPE_SliderHandle: {
    const bool mouseOver = flags & Style_MouseOver;
    const bool pressed = (flags & Style_Active) || (flags & Style_Down);

    // Windows 10 style: Selection Color blue handle
    TQColor handleColor = enabled ? cg.highlight() : cg.highlight().light(140);

    if (mouseOver || pressed) {
      handleColor = handleColor.light(110);
    }

    if (m_win11Mode) {
        // Win11: Circular handle with central dot
        p->save();
        // TQt3 doesn't support setRenderHint(Antialiasing) directly
        
        // Use a smaller dimension as requested (reduction of ~3px in radius -> 6px in diameter)
        // Previous was 16px, then 10px, then 11px, then 14px, now 16px.
        int dim = TQMIN(r.width(), r.height());
        if (dim > 15) dim = 15; 
        if (dim < 8) dim = 8; // Don't go too small either        
        // Center alignment
        int cx = r.center().x();
        int cy = r.center().y();

        // Background circle
        TQColor bgColor = m_darkMode ? TQColor(69, 69, 69) : TQColor(255, 255, 255);
        TQColor splitColor = m_darkMode ? TQColor(160, 160, 160) : TQColor(180, 180, 180); // Border color

        // Fake antialiasing: draw a slightly larger "soft" outline first
        // Color is a mix between border and panel background (cg.background())
        // TQt3 TQColor doesn't handle alpha well depending on the display, 
        // let's use a manual blend.
        TQColor blendColor;
        int r1, g1, b1, r2, g2, b2;
        splitColor.rgb(&r1, &g1, &b1);
        cg.background().rgb(&r2, &g2, &b2);
        blendColor.setRgb((r1+r2)/2, (g1+g2)/2, (b1+b2)/2);

        p->setPen(blendColor);
        p->setBrush(TQt::NoBrush);
        p->drawEllipse(cx - dim/2 - 1, cy - dim/2 - 1, dim + 2, dim + 2);

        // Outer circle (main border)
        p->setPen(splitColor);
        p->setBrush(bgColor);
        p->drawEllipse(cx - dim/2, cy - dim/2, dim, dim);

        // Inner dot (blue/highlight)
        int dotSize = dim / 2; 
        // Ensure dot is visible but not too large
        if (dotSize < 4) dotSize = 4;
        if (dotSize > 8) dotSize = 8;

        p->setPen(TQt::NoPen);
        p->setBrush(handleColor); // Use the highlighting color for the dot
        p->drawEllipse(cx - dotSize/2, cy - dotSize/2, dotSize, dotSize);

        p->restore();
    } else {
        // Centered rectangle drawing (Win10 / Default)
        p->fillRect(r, handleColor);

        // Subtle border
        p->setPen(handleColor.dark(120));
        p->drawRect(r);
    }

    break;
  }

  case KPE_ListViewExpander: {
    int radius = (r.width() - 4) / 2;
    int centerx = r.x() + r.width() / 2;
    int centery = r.y() + r.height() / 2;

    renderContour(p, r, cg.base(), cg.dark(),
                  Draw_Left | Draw_Right | Draw_Top | Draw_Bottom |
                      Round_UpperLeft | Round_UpperRight | Round_BottomLeft |
                      Round_BottomRight);

    p->setPen(cg.text());
    if (!_drawTriangularExpander) {
      // plus or minus
      p->drawLine(centerx - radius, centery, centerx + radius, centery);
      if (flags & Style_On) // Collapsed = On
        p->drawLine(centerx, centery - radius, centerx, centery + radius);
    } else if (_drawTriangularExpander) {
      if (flags & Style_On)
        drawPrimitive(PE_ArrowRight, p, ceData, elementFlags,
                      TQRect(r.x() + 1, r.y() + 1, r.width(), r.height()), cg,
                      ButtonContour, flags);
      if (flags & Style_Off)
        drawPrimitive(PE_ArrowDown, p, ceData, elementFlags,
                      TQRect(r.x() + 1, r.y() + 1, r.width(), r.height()), cg,
                      ButtonContour, flags);
    }

    break;
  }

  // copied and slightly modified from TDEStyle.


  default:
    TDEStyle::drawTDEStylePrimitive(kpe, p, ceData, elementFlags, r, cg, flags,
                                    opt, widget);
  }
}

void Q4Win10Style::drawPrimitive(PrimitiveElement pe, TQPainter *p,
                                 const TQStyleControlElementData &ceData,
                                 ControlElementFlags elementFlags,
                                 const TQRect &r, const TQColorGroup &cg,
                                 SFlags flags, const TQStyleOption &opt) const {
  bool down = flags & Style_Down;
  bool on = flags & Style_On;
  bool sunken = flags & Style_Sunken;
  bool horiz = flags & Style_Horizontal;
  const bool enabled = flags & Style_Enabled;
  const bool mouseOver = flags & Style_MouseOver;

  bool hasFocus = flags & Style_HasFocus;

  int x = r.x();
  int y = r.y();
  int w = r.width();
  int h = r.height();

  int x2, y2;
  r.coords(&x, &y, &x2, &y2);

  switch (pe) {
    // BUTTONS
    // -------
  case PE_FocusRect: {
    if (_drawFocusRect)
      p->drawWinFocusRect(r);
    break;
  }

  case PE_HeaderSectionMenu:
  case PE_HeaderSection: {
    // the taskbar buttons seems to be painted with PE_HeaderSection but I
    // want them look like normal buttons (at least for now. :) )
    if (!kickerMode) {
      // detect if this is the left most header item
      bool isFirst = false;
      if (p->device() && p->device()->devType() == TQInternal::Widget) {
        TQHeader *header = ::tqt_cast<TQHeader *>(static_cast<TQWidget *>(p->device()));
        if (header) {
          isFirst = header->mapToIndex(header->sectionAt(r.x())) == 0;
        }
      }

      uint contourFlags = Draw_Right | Draw_Top | Draw_Bottom;
      if (isFirst)
        contourFlags |= Draw_Left;
      if (!enabled)
        contourFlags |= Is_Disabled;
      renderContour(p, r, cg.background(), getColor(cg, ButtonContour),
                    contourFlags);

      uint surfaceFlags =
          Draw_Left | Draw_Right | Draw_Top | Draw_Bottom | Is_Horizontal;
      if (!enabled)
        surfaceFlags |= Is_Disabled;
      else {
        if (on || down)
          surfaceFlags |= Is_Sunken;
        else {
          if (mouseOver) {
            surfaceFlags |= Is_Highlight | Highlight_Top | Highlight_Bottom;
          }
        }
      }
      renderSurface(p,
                    TQRect(isFirst ? r.left() + 1 : r.left(), r.top() + 1,
                           isFirst ? r.width() - 2 : r.width() - 1,
                           r.height() - 2),
                    cg.background(), cg.button(),
                    getColor(cg, MouseOverHighlight), _contrast, surfaceFlags);

      break;
    }
  }
  case PE_ButtonBevel:
  case PE_ButtonTool:
  case PE_ButtonDropDown:
  case PE_ButtonCommand: {
    bool tdehtmlMode =
        opt.isDefault() ? false : tdehtmlWidgets.contains(opt.widget());
    renderButton(p, r, cg, (on || down), mouseOver, true, enabled, tdehtmlMode,
                 (flags & Style_ButtonDefault));
    break;
  }

  case PE_ButtonDefault: {
    uint contourFlags = Draw_Left | Draw_Right | Draw_Top | Draw_Bottom |
                        Round_UpperLeft | Round_UpperRight | Round_BottomLeft |
                        Round_BottomRight;
    if (!enabled)
      contourFlags |= Is_Disabled;
    renderContour(p, r, cg.background(), cg.background().dark(120),
                  contourFlags);
    break;
  }

  case PE_SpinWidgetPlus:
  case PE_SpinWidgetMinus: {
    p->setPen(cg.buttonText());

    int l = TQMIN(w - 2, h - 2);
    // make the length even so that we get a nice symmetric plus...
    if (l % 2 != 0)
      --l;
    TQPoint c = r.center();

    p->drawLine(c.x() - l / 2, c.y(), c.x() + l / 2, c.y());
    if (pe == PE_SpinWidgetPlus) {
      p->drawLine(c.x(), c.y() - l / 2, c.x(), c.y() + l / 2);
    }
    break;
  }

  case PE_ScrollBarSlider: {
    // Windows 10 style: slider uses button background color
    TQColor sliderColor;
    if (m_darkMode) {
      // Dark Mode: Slider is lighter than background for visibility
      if (flags & (Style_Active | Style_Down))
        sliderColor = cg.button().light(190); // Even lighter when active/pressed
      else
        sliderColor = cg.button().light(160);
    } else {
      // Light Mode: Slider is darker than background
      if (flags & (Style_Active | Style_Down))
        sliderColor = cg.button().dark(150); // Much darker when active/pressed
      else
        sliderColor = cg.button().dark(125); // Darker than standard button
    }

    const TQColor borderColor = sliderColor;

    if (m_win11Mode) {
      // Win11: Rounded slider (4px radius)
      p->save();

      // Fix for artifacts: Fill the entire slider rect with the track color first
      // to erase any "squared" leftovers from previous paints or surrounding pixels.
      // This mimics transparency for the rounded corners.
      TQColor trackColor = cg.background();
      if ((flags & Style_Down) || (flags & Style_Sunken)) { 
         // If track is sunken/active (though usually slider is on top of page), match page color
         trackColor = trackColor.dark(105); 
      }
      p->fillRect(r, trackColor);

      p->setPen(borderColor);
      p->setBrush(sliderColor);
      
      // Compute rounded percentages
      int xRound = (r.width() > 0) ? (200 * 4 / r.width()) : 0;
      int yRound = (r.height() > 0) ? (200 * 4 / r.height()) : 0;
      
      p->drawRoundRect(r, xRound, yRound);
      p->restore();
    } else {
      // Draw border
      p->setPen(borderColor);
      p->drawRect(r.x(), r.y(), r.width(), r.height());

      // Fill slider
      if (r.width() > 2 && r.height() > 2) {
        p->fillRect(r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2,
                    sliderColor);
      }
    }

    break;
  }

  case PE_ScrollBarAddPage:
  case PE_ScrollBarSubPage: {
    // Windows 10 style: darker gray track for contrast with white slider
    TQColor trackColor = cg.background();
    if (on || down) {
      trackColor = trackColor.dark(105);
    }
    p->fillRect(r, trackColor);
    break;
  }

    // SCROLLBAR BUTTONS
    // -----------------
  case PE_ScrollBarSubLine: {
    if (m_win11Mode)
      break; // Hide buttons in Win11 mode

    uint contourFlags = Draw_Left | Draw_Right | Draw_Top | Draw_Bottom;
    uint surfaceFlags = Draw_Left | Draw_Right | Draw_Top | Draw_Bottom;
    if (down)
      surfaceFlags |= Is_Sunken;
    if (!enabled) {
      contourFlags |= Is_Disabled;
      surfaceFlags |= Is_Disabled;
    }
    if (horiz) {
      contourFlags |= Round_UpperLeft | Round_BottomLeft | Is_Horizontal;
      surfaceFlags |= Round_UpperLeft | Round_BottomLeft | Is_Horizontal;
    } else {
      contourFlags |= Round_UpperLeft | Round_UpperRight;
      surfaceFlags |= Round_UpperLeft | Round_UpperRight;
    }

    TQColor btnBorder;
    if (down)
      btnBorder = cg.background().dark(114);
    else if (flags & Style_MouseOver)
      btnBorder = getColor(cg, MouseOverHighlight);
    else
      btnBorder = cg.background();

    renderContour(p, r, cg.background(), btnBorder, contourFlags);
    renderSurface(
        p, r,
        cg.background(), cg.background(), getColor(cg, MouseOverHighlight),
        _contrast + 3, surfaceFlags);

    p->setPen(cg.foreground());
    drawPrimitive((horiz ? PE_ArrowLeft : PE_ArrowUp), p, ceData, elementFlags,
                  r, cg, flags);
    break;
  }

  case PE_ScrollBarAddLine: {
    if (m_win11Mode)
      break; // Hide buttons in Win11 mode

    uint contourFlags = Draw_Left | Draw_Right | Draw_Top | Draw_Bottom;
    uint surfaceFlags = Draw_Left | Draw_Right | Draw_Top | Draw_Bottom;
    if (down)
      surfaceFlags |= Is_Sunken;
    if (!enabled) {
      contourFlags |= Is_Disabled;
      surfaceFlags |= Is_Disabled;
    }
    if (horiz) {
      contourFlags |= Round_UpperRight | Round_BottomRight | Is_Horizontal;
      surfaceFlags |= Round_UpperRight | Round_BottomRight | Is_Horizontal;
    } else {
      contourFlags |= Round_BottomLeft | Round_BottomRight;
      surfaceFlags |= Round_BottomLeft | Round_BottomRight;
    }

    TQColor btnBorder;
    if (down)
      btnBorder = cg.background().dark(114);
    else if (flags & Style_MouseOver)
      btnBorder = getColor(cg, MouseOverHighlight);
    else
      btnBorder = cg.background();

    renderContour(p, r, cg.background(), btnBorder, contourFlags);
    renderSurface(
        p, r,
        cg.background(), cg.background(), getColor(cg, MouseOverHighlight),
        _contrast + 3, surfaceFlags);

    p->setPen(cg.foreground());
    drawPrimitive((horiz ? PE_ArrowRight : PE_ArrowDown), p, ceData,
                  elementFlags, r, cg, flags);
    break;
  }

    // CHECKBOXES
    // ----------


  case PE_IndicatorMask: {
    p->fillRect(r, color1);
    break;
  }

    // RADIOBUTTONS
    // ------------


  case PE_ExclusiveIndicatorMask: {
    // Square mask for radio button is enough for click detection
    p->fillRect(r, color1);
    break;
  }

    // GENERAL PANELS
    // --------------
  case PE_Splitter: {
    // highlight on mouse over
    TQColor color = (mouseOver)
                        ? TQColor(cg.background().light(100 + _contrast))
                        : cg.background();
    p->fillRect(r, color);
    break;
  }

  case PE_PanelGroupBox:
  case PE_GroupBoxFrame: {
    if (opt.isDefault() || opt.lineWidth() <= 0)
      break;
    renderPanel(p, r, cg, false);

    break;
  }

  case PE_WindowFrame:
  case PE_Panel: {
    if (opt.isDefault() || opt.lineWidth() <= 0)
      break;
    renderPanel(p, r, cg, true, sunken);
    break;
  }

  case PE_PanelLineEdit: {
    bool isReadOnly = false;
    bool isEnabled = true;
    // panel is highlighted by default if it has focus, but if we have access to
    // the widget itself we can try to avoid highlighting in case it's readOnly
    // or disabled.
    if (p->device() && p->device()->devType() == TQInternal::Widget) {
      TQLineEdit *lineEdit = ::tqt_cast<TQLineEdit *>(static_cast<TQWidget *>(p->device()));
      if (lineEdit) {
        isReadOnly = lineEdit->isReadOnly();
        isEnabled = lineEdit->isEnabled();
      }
    }

    uint contourFlags = Draw_Left | Draw_Right | Draw_Top | Draw_Bottom |
                        Round_UpperLeft | Round_UpperRight | Round_BottomLeft |
                        Round_BottomRight;

    // HACK!!
    //
    // In order to draw nice edges in tdehtml, we need to paint alpha-blended.
    // On the other hand, we can't paint alpha-blended in normal widgets.
    //
    // In this place there is no reliable way to detect if we are in tdehtml;
    // the only thing we know is that tdehtml buffers its widgets into a pixmap.
    // So when the paint device is a TQPixmap, chances are high that we are in
    // tdehtml. It's possible that this breaks other things, so let's see how it
    // works...
    if (p->device() && p->device()->devType() == TQInternal::Pixmap) {
      contourFlags += Draw_AlphaBlend;
    }

    if (m_win11Mode) {
      TQColor borderColor;
      if (_inputFocusHighlight && hasFocus && !isReadOnly && isEnabled)
        borderColor = getColor(cg, FocusHighlight, enabled);
      else
        borderColor = getColor(cg, PanelContour);

      p->save();
      
      // CRITICAL: Clear the square corners with the parent background color
      p->fillRect(r, cg.background());

      // Fill background (Base color for text entry)
      p->setPen(TQt::NoPen);
      p->setBrush(cg.base());
      int radius = 9;
      int xRound = (r.width() > 0) ? (200 * radius / r.width()) : 0;
      int yRound = (r.height() > 0) ? (200 * radius / r.height()) : 0;
      p->drawRoundRect(r, xRound, yRound);

      // Border
      p->setPen(borderColor);
      p->setBrush(TQt::NoBrush);
      p->drawRoundRect(r, xRound, yRound);

      p->restore();
    } else {
      if (_inputFocusHighlight && hasFocus && !isReadOnly && isEnabled) {
        renderContour(p, r, cg.background(),
                      getColor(cg, FocusHighlight, enabled), contourFlags);
      } else {
        renderContour(p, r, cg.background(), getColor(cg, PanelContour),
                      contourFlags);
      }
    }
    break;
  }

  case PE_StatusBarSection: {
    // Windows 10 style: no border/separator on status bar sections
    break;
  }

  case PE_TabBarBase: // Still not sure what this one does
  case PE_PanelTabWidget: {
    renderPanel(p, r, cg, false, sunken);
    break;
  }

  case PE_PanelPopup: {
    if (m_win11Mode) {
      int radius = 9;
      int xRound = (r.width() > 0) ? (200 * radius / r.width()) : 0;
      int yRound = (r.height() > 0) ? (200 * radius / r.height()) : 0;

      p->setPen(TQt::NoPen);
      p->setBrush(cg.background());
      p->drawRoundRect(r, xRound, yRound);

      p->setPen(getColor(cg, PanelContour));
      p->setBrush(TQt::NoBrush);
      p->drawRoundRect(r, xRound, yRound);
    } else {
      p->fillRect(r, cg.background());
      renderContour(p, r, cg.background(), getColor(cg, PanelContour),
                    Draw_Left | Draw_Right | Draw_Top | Draw_Bottom);
    }
    break;
  }

    // MENU / TOOLBAR PANEL
    // --------------------
  case PE_PanelMenuBar: {
    // Use explicit cg.base() (Standard Background Color) for Menu Bar.
    // This is the semantic color for text fields, etc. - NOT hardcoded white.
    p->fillRect(r, cg.base());
    break;
  }

  case PE_PanelDockWindow: {
    // Removed side-effect state mutation.
    p->fillRect(r, cg.background());

    if (_drawToolBarSeparator) {
      if (r.width() > r.height()) {
        p->setPen(getColor(cg, PanelLight));
        p->drawLine(r.left(), r.top(), r.right(), r.top());
        p->setPen(getColor(cg, PanelDark));
        p->drawLine(r.left(), r.bottom(), r.right(), r.bottom());
      } else {
        p->setPen(getColor(cg, PanelLight));
        p->drawLine(r.left(), r.top(), r.left(), r.bottom());
        p->setPen(getColor(cg, PanelDark));
        p->drawLine(r.right(), r.top(), r.right(), r.bottom());
      }
    }
    break;
  }

    // TOOLBAR/DOCK WINDOW HANDLE
    // --------------------------
  case PE_DockWindowResizeHandle: {
    renderButton(p, r, cg);
    break;
  }

  case PE_DockWindowHandle: {
    // Narrower Windows 10 style grip: 2px solid rectangle
    // Using cg.button() color (standard button color)
    int handleWidth = 2;
    TQRect grip;
    if (horiz) { // vertical handle for horizontal toolbar
      grip = TQRect(r.x() + (r.width() - handleWidth) / 2, r.top() + 2,
                    handleWidth, r.height() - 4);
    } else { // horizontal handle for vertical toolbar
      grip = TQRect(r.left() + 2, r.y() + (r.height() - handleWidth) / 2,
                    r.width() - 4, handleWidth);
    }
    p->fillRect(grip, cg.button());
    break;
  }

    // TOOLBAR SEPARATOR
    // -----------------
  case PE_DockWindowSeparator: {
    p->fillRect(r, cg.background());

    if (_drawToolBarItemSeparator) {
      if (horiz) {
        int center = r.left() + r.width() / 2;
        p->setPen(getColor(cg, PanelDark));
        p->drawLine(center - 1, r.top() + 3, center - 1, r.bottom() - 3);
        if (!m_darkMode) {
            p->setPen(getColor(cg, PanelLight));
            p->drawLine(center, r.top() + 3, center, r.bottom() - 3);
        }
      } else {
        int center = r.top() + r.height() / 2;
        p->setPen(getColor(cg, PanelDark));
        p->drawLine(r.x() + 3, center - 1, r.right() - 3, center - 1);
        if (!m_darkMode) {
            p->setPen(getColor(cg, PanelLight));
            p->drawLine(r.x() + 3, center, r.right() - 3, center);
        }
      }
    }
    break;
  }

  case PE_Indicator:
  case PE_ExclusiveIndicator: {
    const bool isRadio = (pe == PE_ExclusiveIndicator);

    if (m_win11Mode) {
      // Win11 Mode: Flat design with blue fill when checked
      const bool isChecked = (flags & Style_On);
      const bool isTristate = (flags & Style_NoChange);
      const bool isDown = (flags & Style_Down);
      const bool isHover = (flags & Style_MouseOver) && enabled;

      TQColor borderColor, fillColor, dotColor;

      if (!enabled) {
        borderColor = TQColor(150, 150, 150);
        fillColor = TQColor(220, 220, 220);
        dotColor = TQColor(150, 150, 150);
      } else if (isChecked || isTristate) {
        fillColor = TQColor(0, 120, 215);
        if (isHover || isDown)
          fillColor = TQColor(0, 90, 158);
        borderColor = fillColor;
        dotColor = TQColor("white");
      } else {
        fillColor = m_darkMode ? TQColor(30, 30, 30) : TQColor("white");
        if (isHover)
          borderColor = TQColor(0, 120, 215);
        else if (isDown)
          borderColor = TQColor(0, 90, 158);
        else
          borderColor = m_darkMode ? TQColor(150, 150, 150) : TQColor(51, 51, 51);
        dotColor = TQColor("white");
      }

      p->save();
      if (isRadio) {
        p->setPen(TQPen(borderColor));
        p->setBrush(fillColor);
        p->drawEllipse(r);
        if (isChecked) {
          p->setBrush(dotColor);
          p->setPen(TQt::NoPen);
          p->drawEllipse(r.center().x() - 2, r.center().y() - 2, 5, 5);
        }
      } else {
        // Win11 Checkbox: 3px rounded corners
        int radius = 3;
        int xRound = (r.width() > 0) ? (200 * radius / r.width()) : 0;
        int yRound = (r.height() > 0) ? (200 * radius / r.height()) : 0;

        p->setPen(TQPen(borderColor));
        p->setBrush(fillColor);
        p->drawRoundRect(r, xRound, yRound);

        // Draw Checkmark / Dash
        if (isChecked || isTristate) {
          drawPrimitive(PE_CheckMark, p, ceData, elementFlags, r, cg, flags);
        }
      }
      p->restore();
      break;
    }

    // Default mode (Win10 / Dark): Original Plastik-style rendering
    if (isRadio) {
      // Original radio button rendering
      const int diam = 13;
      int cx = r.x() + (r.width() - diam) / 2;
      int cy = r.y() + (r.height() - diam) / 2;
      if (cx < r.x()) cx = r.x();
      if (cy < r.y()) cy = r.y();

      p->setPen(TQt::NoPen);
      p->setBrush(enabled ? cg.base() : cg.background());
      p->drawEllipse(cx, cy, diam, diam);

      TQColor borderColor;
      if (!enabled)
        borderColor = cg.background().dark(120);
      else if (mouseOver)
        borderColor = TQColor(0x3C, 0x7F, 0xB1);
      else
        borderColor = TQColor(51, 51, 51);
      p->setPen(borderColor);
      p->setBrush(TQt::NoBrush);
      p->drawEllipse(cx, cy, diam, diam);

      if (flags & Style_On || flags & Style_Down) {
        int dotSize = 7;
        int dotX = cx + (diam - dotSize) / 2;
        int dotY = cy + (diam - dotSize) / 2;
        TQColor dotColor = enabled ? TQColor(0, 0, 0) : cg.shadow();
        p->setPen(TQt::NoPen);
        p->setBrush(dotColor);
        p->drawEllipse(dotX, dotY, dotSize, dotSize);
      }
    } else {
      // Win10 Mode checkbox: original Plastik-style (white bg + checkmark)
      TQColor contentColor = enabled ? cg.base() : cg.background();

      uint contourFlags = Draw_Left | Draw_Right | Draw_Top | Draw_Bottom;
      if (!enabled)
        contourFlags |= Is_Disabled;
      renderContour(p, r, cg.background(), getColor(cg, ButtonContour), contourFlags);

      uint surfaceFlags = Draw_Left | Draw_Right | Draw_Top | Draw_Bottom | Is_Horizontal;
      if (!enabled) {
        surfaceFlags |= Is_Disabled;
      } else if (mouseOver) {
        contentColor = alphaBlendColors(contentColor, getColor(cg, MouseOverHighlight), 240);
        surfaceFlags |= Is_Highlight;
        surfaceFlags |= Highlight_Left | Highlight_Right | Highlight_Top | Highlight_Bottom;
      }
      renderSurface(
          p, TQRect(r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2),
          cg.background(), contentColor, getColor(cg, MouseOverHighlight),
          enabled ? _contrast + 3 : (_contrast / 2), surfaceFlags);

      drawPrimitive(PE_CheckMark, p, ceData, elementFlags, r, cg, flags);
    }
    break;
  }

  case PE_CheckMark: {
    // Checkmark color resolution:
    // - In Win11 mode: checkmark is white on top of the blue checkbox fill (0, 120, 215)
    // - In Win10 mode: checkmark is drawn on top of the standard box background (cg.base()), following cg.text() (black in light themes)
    TQColor checkmarkColor;
    if (!enabled) {
      checkmarkColor = cg.background().dark(120);
    } else if (m_win11Mode) {
      checkmarkColor = TQColor("white");
    } else if (m_darkMode) {
      checkmarkColor = TQColor("white");
    } else {
      checkmarkColor = cg.text().isValid() ? cg.text() : TQColor(0, 0, 0);
      if (checkmarkColor == TQColor("white") || tqGray(checkmarkColor.rgb()) > 200) {
        checkmarkColor = TQColor(0, 0, 0);
      }
    }

    if (flags & Style_Down) {
      checkmarkColor = checkmarkColor.dark(120);
    }

    if (flags & Style_On) {
      // Draw Vector Checkmark using Lines (No Font usage to avoid bugs)
      // Shape: ✓
      int cx = r.center().x();
      int cy = r.center().y();

      p->save();
      p->setPen(checkmarkColor);
      // Draw 1px lines twice for crisp 2px line thickness
      // Left stroke
      p->drawLine(cx - 3, cy, cx - 1, cy + 2);
      p->drawLine(cx - 3, cy + 1, cx - 1, cy + 3);
      // Right stroke
      p->drawLine(cx - 1, cy + 2, cx + 4, cy - 3);
      p->drawLine(cx - 1, cy + 3, cx + 4, cy - 2);
      p->restore();

    } else if (flags & Style_Off) {
      // empty - nothing to draw
    } else {
      // tristate - draw a horizontal line / dash
      TQColor dashColor = (!enabled) ? cg.background().dark(120) : ((m_win11Mode || m_darkMode) ? TQColor("white") : (cg.text().isValid() ? cg.text() : TQColor(0, 0, 0)));

      p->fillRect(r.x() + 4, r.y() + r.height() / 2 - 1, r.width() - 8, 2,
                  dashColor);
    }

    break;
  }

  case PE_MenuItemIndicatorFrame:
  case PE_MenuItemIndicatorIconFrame: {
    // Windows 10 style: checkmarks are flat in menus, no recessed frame/box.
    break;
  }
  case PE_MenuItemIndicatorCheck: {
    int x, y, w, h;
    r.rect(&x, &y, &w, &h);
    int checkcol = styleHint(SH_MenuIndicatorColumnWidth, ceData, elementFlags,
                             opt, NULL, NULL);
    TQRect cr = visualRect(TQRect(x + 2, y + 2, checkcol - 1, h - 4), r);
    // Draw the checkmark
    SFlags cflags = Style_On;
    if (enabled)
      cflags |= Style_Enabled;
    drawPrimitive(PE_CheckMark, p, ceData, elementFlags, cr, cg, cflags);
    break;
  }

  case PE_SpinWidgetUp:
  case PE_SpinWidgetDown:
  case PE_HeaderArrow:
  case PE_ArrowUp:
  case PE_ArrowDown:
  case PE_ArrowLeft:
  case PE_ArrowRight: {
    TQPointArray a(3);
    int cx = r.center().x();
    int cy = r.center().y();
    
    // Windows 10 style Symmetrical Checkmark (V-shape)
    // Fixed integer coordinates for perfect alignment.
    // Standard size: 6x3 pixels. SpinWidget: 4x2 pixels.
    
    switch (pe) {
    case PE_SpinWidgetUp:
      a.setPoint(0, cx - 2, cy + 1);
      a.setPoint(1, cx,     cy - 1);
      a.setPoint(2, cx + 2, cy + 1);
      break;
    case PE_ArrowUp:
      a.setPoint(0, cx - 3, cy + 1);
      a.setPoint(1, cx,     cy - 2); // Tip
      a.setPoint(2, cx + 3, cy + 1);
      break;
      
    case PE_SpinWidgetDown:
      a.setPoint(0, cx - 2, cy - 1);
      a.setPoint(1, cx,     cy + 1);
      a.setPoint(2, cx + 2, cy - 1);
      break;
    case PE_ArrowDown:
      a.setPoint(0, cx - 3, cy - 2);
      a.setPoint(1, cx,     cy + 1); // Tip
      a.setPoint(2, cx + 3, cy - 2);
      break;
      
    case PE_ArrowLeft: {
      int leg = TQMIN(4, (r.height() - 2) / 2);
      if (leg < 2) leg = 2;
      a.setPoint(0, cx + (leg - 1), cy - leg);
      a.setPoint(1, cx - 1,         cy);
      a.setPoint(2, cx + (leg - 1), cy + leg);
      break;
    }
    case PE_ArrowRight: {
      int leg = TQMIN(4, (r.height() - 2) / 2);
      if (leg < 2) leg = 2;
      a.setPoint(0, cx - (leg - 1), cy - leg);
      a.setPoint(1, cx + 1,         cy);
      a.setPoint(2, cx - (leg - 1), cy + leg);
      break;
    }
      
    default:
      // Fallback
      if (flags & Style_Up) {
          a.setPoint(0, cx - 3, cy + 1);
          a.setPoint(1, cx,     cy - 2);
          a.setPoint(2, cx + 3, cy + 1);
      } else {
          a.setPoint(0, cx - 3, cy - 2);
          a.setPoint(1, cx,     cy + 1);
          a.setPoint(2, cx + 3, cy - 2);
      }
    }

    if (p->pen() == TQt::NoPen) {
      if (flags & Style_Enabled) {
          if (flags & Style_Down)
             p->setPen(cg.highlightedText()); 
          else
             p->setPen(cg.buttonText());
      } else {
        p->setPen(cg.mid());
      }
    }
    
    // For side arrows (Left/Right), use two explicit drawLine pairs 
    // offset horizontally to match the "blue/black" reference.
    if (pe == PE_ArrowLeft) {
        // Stroke 1 (Tip at cx)
        p->drawLine(a[0], a[1]);
        p->drawLine(a[1], a[2]);
        // Stroke 2 (Offset 1px Right)
        p->drawLine(a[0].x() + 1, a[0].y(), a[1].x() + 1, a[1].y());
        p->drawLine(a[1].x() + 1, a[1].y(), a[2].x() + 1, a[2].y());
    } else if (pe == PE_ArrowRight) {
        // Stroke 1 (Tip at cx)
        p->drawLine(a[0], a[1]);
        p->drawLine(a[1], a[2]);
        // Stroke 2 (Offset 1px Left)
        p->drawLine(a[0].x() - 1, a[0].y(), a[1].x() - 1, a[1].y());
        p->drawLine(a[1].x() - 1, a[1].y(), a[2].x() - 1, a[2].y());
    } else {
        // Draw 1st instance (Up/Down/Spin)
        p->drawPolyline(a);
        
        // Draw 2nd instance (Double Stroke - Vertical)
        // ONLY for Up/Down arrows to increase boldness
        if (pe == PE_ArrowUp || pe == PE_ArrowDown) {
            a.translate(0, 1);
            p->drawPolyline(a);
        }
    }


    break;
  }

  default: {
    return TDEStyle::drawPrimitive(pe, p, ceData, elementFlags, r, cg, flags,
                                   opt);
  }
  }
}

void Q4Win10Style::drawControl(ControlElement element, TQPainter *p,
                               const TQStyleControlElementData &ceData,
                               ControlElementFlags elementFlags,
                               const TQRect &r, const TQColorGroup &cg,
                               SFlags flags, const TQStyleOption &opt,
                               const TQWidget *widget) const {
  const bool reverseLayout = TQApplication::reverseLayout();

  const bool enabled = (flags & Style_Enabled);

  switch (element) {

    // PROGRESSBAR - Windows 10 flat style
    // -----------
  case CE_ProgressBarGroove: {
    // Windows 10 style: #E6E6E6 gray background, #ADADAD border
    const TQColor bg(0xE6, 0xE6, 0xE6);     // Light gray background
    const TQColor border(0xBC, 0xBC, 0xBC); // Subtle gray border

    // Fill entire groove with gray
    p->fillRect(r, bg);

    // Draw 1px border around groove
    p->setPen(border);
    p->drawRect(r.x(), r.y(), r.width(), r.height());
    break;
  }

  case CE_ProgressBarContents: {
    int steps = ceData.totalSteps;

    // Windows 10 style: #06B025 green progress
    const TQColor fg(0x06, 0xB0, 0x25); // Windows 10 green

    if (steps == 0) {
      // Busy indicator - simple moving block
      static const int barWidth = 40;
      int maxProgress = r.width() - barWidth;
      if (maxProgress < 1)
        maxProgress = 1;
      int progress = ceData.currentStep % (2 * maxProgress);
      if (progress < 0)
        progress = 0;
      if (progress > maxProgress)
        progress = maxProgress - (progress - maxProgress);

      // Draw solid moving block - no gap
      p->fillRect(r.x() + progress, r.y(), barWidth, r.height(), fg);
    } else {
      // Standard progress bar
      double percent =
          static_cast<double>(ceData.currentStep) / static_cast<double>(steps);
      int w = static_cast<int>(r.width() * percent);
      if (w < 1 && ceData.currentStep > 0)
        w = 1;

      // Draw green progress - no gap, fills edge to edge
      if (w > 0) {
        TQRect Rfilled(reverseLayout ? r.right() - w + 1 : r.left(), r.top(), w,
                       r.height());
        p->fillRect(Rfilled, fg);
      }
    }

    break;
  }

    // TABS
    // ----
  case CE_TabBarTab: {
    bool cornerWidget = false;
    if (!ceData.tabBarData
             .cornerWidgets[TQStyleControlElementTabBarData::CWL_TopLeft]
             .widgetObjectTypes.isEmpty()) {
      cornerWidget = true;
    }
    TQTabBar::Shape tbs = ceData.tabBarData.shape;
    bool selected = false;
    if (flags & Style_Selected)
      selected = true;
    TabPosition pos;
    if (ceData.tabBarData.tabCount == 1) {
      pos = Single;
    } else if (ceData.tabBarData.identIndexMap[opt.tab()->identifier()] == 0) {
      pos = First;
    } else if (ceData.tabBarData.identIndexMap[opt.tab()->identifier()] ==
               (ceData.tabBarData.tabCount - 1)) {
      pos = Last;
    } else {
      pos = Middle;
    }

    bool mouseOver = false;
    if (opt.tab() == opt.hoverTab()) {
      mouseOver = true;
      flags |= Style_MouseOver;
    }

    switch (tbs) {
    case TQTabBar::TriangularAbove:
      //                     renderTriangularTab(p, r, cg, (flags &
      //                     Style_MouseOver), selected, false, pos);
      renderTab(p, r, cg, mouseOver, selected, false, pos, true, cornerWidget);
      break;
    case TQTabBar::RoundedAbove:
      renderTab(p, r, cg, mouseOver, selected, false, pos, false, cornerWidget);
      break;
    case TQTabBar::TriangularBelow:
      //                     renderTriangularTab(p, r, cg, (flags &
      //                     Style_MouseOver), selected, true, pos);
      renderTab(p, r, cg, mouseOver, selected, true, pos, true, cornerWidget);
      break;
    case TQTabBar::RoundedBelow:
      renderTab(p, r, cg, mouseOver, selected, true, pos, false, cornerWidget);
      break;
    }
    break;
  }

  case CE_TabBarLabel: {
    if (!(flags & Style_Selected)) {
      TQColor grey = m_darkMode ? TQColor(160, 160, 160) : TQColor(125, 125, 125);
      
      const TQTab* tab = opt.tab();
      TQString text = ceData.text;
      const TQPixmap* icon = ceData.fgPixmap.isNull() ? 0 : &ceData.fgPixmap;
      
      TQPixmap tabPix;
      if (tab) {
        if (text.isEmpty()) text = tab->text();
        if (tab->iconSet()) {
          tabPix = tab->iconSet()->pixmap(TQIconSet::Small, (elementFlags & CEF_IsEnabled) ? TQIconSet::Normal : TQIconSet::Disabled);
          if (!tabPix.isNull())
            icon = &tabPix;
        }
      }

      drawItem(p, r, AlignCenter | ShowPrefix, cg, (elementFlags & CEF_IsEnabled),
               icon, text, -1, &grey);
    } else {
      TDEStyle::drawControl(element, p, ceData, elementFlags, r, cg, flags, opt,
                            widget);
    }
    break;
  }

  case CE_ToolButtonLabel: {
    TQRect rect = r;
    TQt::ArrowType arrowType = opt.isDefault() ? TQt::DownArrow : opt.arrowType();

    int shiftX = 0;
    int shiftY = 0;
    if (flags & (Style_Down | Style_On)) {
      shiftX = pixelMetric(PM_ButtonShiftHorizontal, ceData, elementFlags, widget);
      shiftY = pixelMetric(PM_ButtonShiftVertical, ceData, elementFlags, widget);
    }

    if (!opt.isDefault()) {
      PrimitiveElement pe;
      switch (arrowType) {
      case TQt::LeftArrow:  pe = PE_ArrowLeft;  break;
      case TQt::RightArrow: pe = PE_ArrowRight; break;
      case TQt::UpArrow:    pe = PE_ArrowUp;    break;
      default:
      case TQt::DownArrow:  pe = PE_ArrowDown;  break;
      }

      rect.moveBy(shiftX, shiftY);
      drawPrimitive(pe, p, ceData, elementFlags, rect, cg, flags, opt);
    } else {
      TQColor btext;
      if (!(elementFlags & CEF_IsEnabled)) {
        btext = m_darkMode ? TQColor(95, 95, 95) : cg.mid();
      } else {
        btext = m_darkMode ? cg.buttonText().light(150) : (ceData.fgColor.isValid() ? ceData.fgColor : cg.buttonText());
      }

      if (ceData.iconSet.isNull() && !ceData.text.isNull() && !(elementFlags & CEF_UsesTextLabel)) {
        int alignment = AlignCenter | ShowPrefix;
        if ((!styleHint(SH_UnderlineAccelerator, ceData, elementFlags, TQStyleOption::Default, 0, widget)) ||
            ((styleHint(SH_HideUnderlineAcceleratorWhenAltUp, ceData, elementFlags, TQStyleOption::Default, 0, widget)) && (!acceleratorsShown())))
          alignment |= NoAccel;

        rect.moveBy(shiftX, shiftY);
        drawItem(p, rect, alignment, cg, flags & Style_Enabled,
                 (ceData.fgPixmap.isNull() ? NULL : &ceData.fgPixmap),
                 ceData.text, ceData.text.length(), &btext);
      } else {
        TQPixmap pm;
        if (!ceData.iconSet.isNull()) {
          TQIconSet::Size size = (elementFlags & CEF_UsesBigPixmap) ? TQIconSet::Large : TQIconSet::Small;
          TQIconSet::State state = (elementFlags & CEF_IsOn) ? TQIconSet::On : TQIconSet::Off;
          TQIconSet::Mode mode;
          if (!(elementFlags & CEF_IsEnabled))
            mode = TQIconSet::Disabled;
          else if ((flags & (Style_Down | Style_On)) || ((flags & Style_Raised) && (flags & Style_AutoRaise)))
            mode = TQIconSet::Active;
          else
            mode = TQIconSet::Normal;
          pm = ceData.iconSet.pixmap(size, mode, state);
        } else if (!ceData.fgPixmap.isNull()) {
          pm = ceData.fgPixmap;
        }

        if (elementFlags & CEF_UsesTextLabel) {
          p->setFont(ceData.font);
          TQRect pr = rect, tr = rect;
          int alignment = ShowPrefix;
          if ((!styleHint(SH_UnderlineAccelerator, ceData, elementFlags, TQStyleOption::Default, 0, widget)) ||
              ((styleHint(SH_HideUnderlineAcceleratorWhenAltUp, ceData, elementFlags, TQStyleOption::Default, 0, widget)) && (!acceleratorsShown())))
            alignment |= NoAccel;

          if (ceData.toolButtonTextPosition == TQToolButton::Under) {
            int fh = p->fontMetrics().height();
            pr.addCoords(0, 1, 0, -fh - 3);
            tr.addCoords(0, pr.bottom(), 0, -3);
            pr.moveBy(shiftX, shiftY);
            if (!pm.isNull())
              drawItem(p, pr, AlignCenter, cg, true, &pm, TQString::null);
            alignment |= AlignCenter;
          } else {
            pr.setWidth(pm.isNull() ? 0 : (pm.width() + 8));
            tr.addCoords(pr.right(), 0, 0, 0);
            pr.moveBy(shiftX, shiftY);
            if (!pm.isNull())
              drawItem(p, pr, AlignCenter, cg, true, &pm, TQString::null);
            alignment |= AlignLeft | AlignVCenter;
          }

          tr.moveBy(shiftX, shiftY);
          drawItem(p, tr, alignment, cg, flags & Style_Enabled, NULL,
                   ceData.textLabel, ceData.textLabel.length(), &btext);
        } else {
          rect.moveBy(shiftX, shiftY);
          if (!pm.isNull())
            drawItem(p, rect, AlignCenter, cg, true, &pm, TQString::null);
          else if (!ceData.text.isNull())
            drawItem(p, rect, AlignCenter | ShowPrefix, cg, flags & Style_Enabled,
                     NULL, ceData.text, ceData.text.length(), &btext);
        }
      }
    }
    break;
  }

  case CE_PushButton: {
    TQPushButton *button = (TQPushButton *)widget;

    const bool isDefault = enabled && (elementFlags & CEF_IsDefault);

    TQColorGroup g2 = cg;
    if (isDefault)
      g2.setColor(TQColorGroup::Background, cg.background().dark(120));
    drawPrimitive(
        PE_ButtonBevel, p, ceData, elementFlags,
        isDefault ? TQRect(r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2)
                  : r,
        g2, flags, TQStyleOption(button));

    if (isDefault) {
      drawPrimitive(PE_ButtonDefault, p, ceData, elementFlags, r, cg, flags);
    }

    break;
  }

  case CE_PushButtonLabel: {
    int x, y, w, h;
    r.rect(&x, &y, &w, &h);

    bool active = (elementFlags & CEF_IsOn) || (elementFlags & CEF_IsDown);
    bool cornArrow = false;

    // Shift button contents if pushed.
    if (active) {
      x += pixelMetric(PM_ButtonShiftHorizontal, ceData, elementFlags, widget);
      y += pixelMetric(PM_ButtonShiftVertical, ceData, elementFlags, widget);
      flags |= Style_Sunken;
    }

    // Does the button have a popup menu?
    if (elementFlags & CEF_IsMenuWidget) {
      int dx =
          pixelMetric(PM_MenuButtonIndicator, ceData, elementFlags, widget);
      if (!ceData.iconSet.isNull() &&
          (dx + ceData.iconSet
                    .pixmap(TQIconSet::Small, TQIconSet::Normal, TQIconSet::Off)
                    .width()) >= w) {
        cornArrow = true; // To little room. Draw the arrow in the corner, don't
                          // adjust the widget
      } else {
        p->setPen(cg.buttonText());
        drawPrimitive(PE_ArrowDown, p, ceData, elementFlags,
                      visualRect(TQRect(x + w - dx - 8, y + 2, dx, h - 4), r),
                      cg, flags, opt);
        w -= dx;
      }
    }

    // Draw the icon if there is one
    if (!ceData.iconSet.isNull()) {
      TQIconSet::Mode mode = TQIconSet::Disabled;
      TQIconSet::State state = TQIconSet::Off;

      if (elementFlags & CEF_IsEnabled)
        mode = (elementFlags & CEF_HasFocus) ? TQIconSet::Active
                                             : TQIconSet::Normal;
      if ((elementFlags & CEF_BiState) && (elementFlags & CEF_IsOn))
        state = TQIconSet::On;

      TQPixmap pixmap = ceData.iconSet.pixmap(TQIconSet::Small, mode, state);

      if (ceData.text.isEmpty() && ceData.fgPixmap.isNull())
        p->drawPixmap(x + w / 2 - pixmap.width() / 2,
                      y + h / 2 - pixmap.height() / 2, pixmap);
      else
        p->drawPixmap(x + 4, y + h / 2 - pixmap.height() / 2, pixmap);

      if (cornArrow) // Draw over the icon
        drawPrimitive(PE_ArrowDown, p, ceData, elementFlags,
                      visualRect(TQRect(x + w - 6, x + h - 6, 7, 7), r), cg,
                      flags, opt);

      int pw = pixmap.width();
      x += pw + 4;
      w -= pw + 4;
    }

    // Make the label indicate if the button is a default button or not
    drawItem(p, TQRect(x, y, w, h), AlignCenter | ShowPrefix, ceData.colorGroup,
             (elementFlags & CEF_IsEnabled),
             (ceData.fgPixmap.isNull()) ? NULL : &ceData.fgPixmap, ceData.text,
             -1, &ceData.colorGroup.buttonText());

    if (flags & Style_HasFocus)
      drawPrimitive(PE_FocusRect, p, ceData, elementFlags,
                    visualRect(subRect(SR_PushButtonFocusRect, ceData,
                                       elementFlags, widget),
                               ceData, elementFlags),
                    cg, flags);
    break;
  }

    // MENUBAR ITEM (sunken panel on mouse over)
    // -----------------------------------------
  case CE_MenuBarItem: {
    TQMenuItem *mi = opt.menuItem();
    bool active = flags & Style_Active;
    bool down = flags & Style_Down;
    const int text_flags =
        AlignVCenter | AlignHCenter | ShowPrefix | DontClip | SingleLine;
    // Explicitly fill with Base color (Standard Background/White) to clear previous state.
    // This matches standard Windows behavior (Paint Background -> Paint Highlight).
    p->fillRect(r, cg.base());

    // Context-Aware Strict Filter:
    // Solve "Persistence when unrolled": distinguish between "Mouse in Dropdown" (Valid) 
    // and "Mouse on Neighbor" (Invalid).
    
    bool shouldPaint = (active || down) && (flags & Style_Enabled);

    if (shouldPaint && widget) {
        // Check physical mouse position relative to the Menu Bar
        TQPoint localPos = widget->mapFromGlobal(TQCursor::pos());
        bool mouseInItem = r.contains(localPos);
        bool mouseInMenuBar = widget->rect().contains(localPos);

        if (mouseInMenuBar) {
            // Refined Check:
            // 1. If Menu Open ('down'): Be Strict on X (Neighbor) but Tolerant on Y (Gap to Popup).
            //    This fixes "Hover lost when descending".
            // 2. If Menu Closed (Hover): Be Strict on X and Y (Standard Hover).
            bool kill = false;
            
            if (active && down) { // Menu Open
                 bool xInRange = (localPos.x() >= r.left() && localPos.x() <= r.right());
                 if (!xInRange) kill = true;
            } else { // Menu Closed / Just Hovering
                 if (!mouseInItem) kill = true;
            }

            if (kill) {
                shouldPaint = false;
                // Strip flags to prevent base style ghosting
                flags &= ~Style_Active;
                flags &= ~Style_Down;
                flags &= ~Style_Sunken; 
            }
        }
        // If mouse is OUTSIDE the menu bar (e.g. in the dropdown below), 
        // we TRUST the 'down' flag to keep the parent item highlighted.
    }

    if (shouldPaint) {
      p->fillRect(r, getColor(cg, MouseOverHighlight));
    }

    if (!enabled && widget) {
      p->setPen(widget->palette().disabled().foreground());
    } else if (!enabled) {
      p->setPen(cg.mid());
    } else {
      p->setPen(cg.foreground());
    }
    p->drawText(r, text_flags, mi->text());
    break;
  }

    // POPUPMENU ITEM (highlighted on mouseover)
    // ------------------------------------------
  case CE_PopupMenuItem: {
    TQMenuItem *mi = opt.menuItem();

    if (!mi) {
      // Don't leave blank holes if we set NoBackground for the TQPopupMenu.
      // This only happens when the popupMenu spans more than one column.
      if (ceData.bgPixmap.isNull())
        p->fillRect(r, cg.background());

      break;
    }
    int tab = opt.tabWidth();
    int checkcol = opt.maxIconWidth();
    bool enabled = mi->isEnabled();
    bool checkable = (elementFlags & CEF_IsCheckable);
    bool active = flags & Style_Active;
    bool etchtext = styleHint(SH_EtchDisabledText, ceData, elementFlags);
    bool reverse = TQApplication::reverseLayout();
    if (checkable)
      checkcol = TQMAX(checkcol, 20);

    // Draw the menu item background
    if (active) {
      if (enabled) {
        renderSurface(p, r, cg.background(), cg.highlight(), cg.highlight(),
                      _contrast + 3, Draw_Top | Draw_Bottom | Is_Horizontal);
      } else {
        if (ceData.bgPixmap.isNull())
          p->drawPixmap(r.topLeft(), ceData.bgPixmap, r);
        else
          p->fillRect(r, cg.background());
        if (_drawFocusRect)
          p->drawWinFocusRect(r);
      }
    }
    // Draw the transparency pixmap
    else if (!ceData.bgPixmap.isNull())
      p->drawPixmap(r.topLeft(), ceData.bgPixmap, r);
    // Draw a solid background
    else
      p->fillRect(r, cg.background());
    // Are we a menu item separator?
    if (mi->isSeparator()) {
      p->setPen(cg.mid());
      p->drawLine(r.x() + 5, r.y() + 1, r.right() - 5, r.y() + 1);
      if (!m_darkMode) {
        // Only draw the "light" relief part if NOT in Dark Mode
        p->setPen(cg.light());
        p->drawLine(r.x() + 5, r.y() + 2, r.right() - 5, r.y() + 2);
      }
      break;
    }

    TQRect cr = visualRect(
        TQRect(r.x() + 2, r.y() + 2, checkcol - 1, r.height() - 4), r);
    // Do we have an icon?
    if (mi->iconSet()) {
      TQIconSet::Mode mode;

      // Select the correct icon from the iconset
      if (active)
        mode = enabled ? TQIconSet::Active : TQIconSet::Disabled;
      else
        mode = enabled ? TQIconSet::Normal : TQIconSet::Disabled;

      // Do we have an icon and are checked at the same time?
      // Then draw a "pressed" background behind the icon
      if (checkable && /*!active &&*/ mi->isChecked())
        drawPrimitive(PE_MenuItemIndicatorIconFrame, p, ceData, elementFlags, r,
                      cg, flags, opt);
      // Draw the icon
      TQPixmap pixmap = mi->iconSet()->pixmap(TQIconSet::Small, mode);
      TQRect pmr(0, 0, pixmap.width(), pixmap.height());
      pmr.moveCenter(cr.center());
      p->drawPixmap(pmr.topLeft(), pixmap);
    }

    // Are we checked? (This time without an icon)
    else if (checkable && mi->isChecked()) {
      // We only have to draw the background if the menu item is inactive -
      // if it's active the "pressed" background is already drawn
      // if ( ! active )
      drawPrimitive(PE_MenuItemIndicatorFrame, p, ceData, elementFlags, r, cg,
                    flags, opt);

      drawPrimitive(PE_MenuItemIndicatorCheck, p, ceData, elementFlags, r, cg,
                    flags, opt);
    }

    // Time to draw the menu item label...
    int xm = 2 + checkcol + 2; // X position margin

    int xp = reverse ? // X position
                 r.x() + tab + rightBorder + itemHMargin + itemFrame - 1
                     : r.x() + xm;

    int offset = reverse ? -1 : 1; // Shadow offset for etched text

    // Label width (minus the width of the accelerator portion)
    int tw =
        r.width() - xm - tab - arrowHMargin - itemHMargin * 3 - itemFrame + 1;

    // Set the color for enabled and disabled text
    // (used for both active and inactive menu items)
    TQColor draw;
    if (!enabled && widget) {
        draw = widget->palette().disabled().foreground();
    } else if (!enabled) {
        // Use the middle ground gray established for Dark Mode
        draw = m_darkMode ? TQColor(95, 95, 95) : cg.mid();
    } else {
        draw = (active) ? cg.highlightedText() : cg.foreground();
    }
    p->setPen(draw);

    TQColor discol = draw;

    // Does the menu item draw it's own label?
    if (mi->custom()) {
      int m = 2;
      // Save the painter state in case the custom
      // paint method changes it in some way
      p->save();

      // Draw etched text if we're inactive and the menu item is disabled
      if (etchtext && !enabled && !active) {
        p->setPen(cg.light());
        mi->custom()->paint(p, cg, active, enabled, xp + offset, r.y() + m + 1,
                            tw, r.height() - 2 * m);
        p->setPen(discol);
      }
      mi->custom()->paint(p, cg, active, enabled, xp, r.y() + m, tw,
                          r.height() - 2 * m);
      p->restore();
    } else {
      // The menu item doesn't draw it's own label
      TQString s = mi->text();
      // Does the menu item have a text label?
      if (!s.isNull()) {
        int t = s.find('\t');
        int m = 2;
        int text_flags = AlignVCenter | ShowPrefix | DontClip | SingleLine;
        text_flags |= reverse ? AlignRight : AlignLeft;

        // Pen already set to 'draw' above

        // Does the menu item have a tabstop? (for the accelerator text)
        if (t >= 0) {
          int tabx = reverse ? r.x() + rightBorder + itemHMargin + itemFrame
                             : r.x() + r.width() - tab - rightBorder -
                                   itemHMargin - itemFrame;

          // Draw the right part of the label (accelerator text)
          if (etchtext && !enabled && !m_darkMode) {
            // Draw etched text if we're inactive and the menu item is disabled
            // Disable etching in Dark Mode to avoid black offsets
            p->setPen(cg.light());
            p->drawText(tabx + offset, r.y() + m + 1, tab, r.height() - 2 * m,
                        text_flags, s.mid(t + 1));
            p->setPen(discol);
          }
          p->drawText(tabx, r.y() + m, tab, r.height() - 2 * m, text_flags,
                      s.mid(t + 1));
          s = s.left(t);
        }

        // Draw the left part of the label (or the whole label
        // if there's no accelerator)
        // segment segment
        p->drawText(xp, r.y() + m, tw, r.height() - 2 * m, text_flags, s, t);

      }

      // The menu item doesn't have a text label
      // Check if it has a pixmap instead
      else if (mi->pixmap()) {
        TQPixmap *pixmap = mi->pixmap();

        // Draw the pixmap
        if (pixmap->depth() == 1)
          p->setBackgroundMode(TQt::OpaqueMode);

        int diffw = ((r.width() - pixmap->width()) / 2) +
                    ((r.width() - pixmap->width()) % 2);
        p->drawPixmap(r.x() + diffw, r.y() + 1, *pixmap);

        if (pixmap->depth() == 1)
          p->setBackgroundMode(TQt::TransparentMode);
      }
    }

    // Does the menu item have a submenu?
    if (mi->popup()) {
      PrimitiveElement arrow = reverse ? PE_ArrowLeft : PE_ArrowRight;
      int dim = pixelMetric(PM_MenuButtonIndicator, ceData, elementFlags) - 1;
      // Moving 5px from edge + 1px for the double stroke offset = 6px
      TQRect vr = visualRect(TQRect(r.x() + r.width() - 6 - dim,
                                    r.y() + r.height() / 2 - dim / 2, dim, dim),
                             r);

      // Draw an arrow at the far end of the menu item
      if (active) {
        if (enabled)
          discol = cg.buttonText();

        TQColorGroup g2(discol, cg.highlight(), white, white,
                        enabled ? white : discol, discol, white);

        drawPrimitive(arrow, p, ceData, elementFlags, vr, g2, Style_Enabled);
      } else
        drawPrimitive(arrow, p, ceData, elementFlags, vr, cg,
                      enabled ? Style_Enabled : Style_Default);
    }

    break;
  }

    // Menu and dockwindow empty space
    //
  case CE_DockWindowEmptyArea:
    p->fillRect(r, cg.background());
    break;

  case CE_MenuBarEmptyArea:
    p->eraseRect(r);
    break;
    //             if ( _drawToolBarSeparator ) {
    //                 p->setPen( getColor(cg, PanelDark) );
    //                 p->drawLine( r.left(), r.bottom(), r.right(), r.bottom()
    //                 );
    //             }

    break;

  default:
    TDEStyle::drawControl(element, p, ceData, elementFlags, r, cg, flags, opt,
                          widget);
  }
}

void Q4Win10Style::drawControlMask(ControlElement element, TQPainter *p,
                                   const TQStyleControlElementData &ceData,
                                   ControlElementFlags elementFlags,
                                   const TQRect &r, const TQStyleOption &opt,
                                   const TQWidget *w) const {
  switch (element) {
  case CE_PushButton: {
    p->fillRect(r, color0);
    renderMask(p, r, color1,
               Round_UpperLeft | Round_UpperRight | Round_BottomLeft |
                   Round_BottomRight);
    break;
  }

  default: {
    TDEStyle::drawControlMask(element, p, ceData, elementFlags, r, opt, w);
  }
  }
}

void Q4Win10Style::drawComplexControlMask(
    ComplexControl c, TQPainter *p, const TQStyleControlElementData &ceData,
    const ControlElementFlags elementFlags, const TQRect &r,
    const TQStyleOption &o, const TQWidget *w) const {
  switch (c) {
  case CC_SpinWidget:
  case CC_ListView:
  case CC_ComboBox: {
    p->fillRect(r, color0);
    renderMask(p, r, color1,
               Round_UpperLeft | Round_UpperRight | Round_BottomLeft |
                   Round_BottomRight);
    break;
  }
  default: {
    TDEStyle::drawComplexControlMask(c, p, ceData, elementFlags, r, o, w);
  }
  }
}

void Q4Win10Style::drawItem(TQPainter *p, const TQRect &r, int flags,
                            const TQColorGroup &g, bool enabled,
                            const TQPixmap *pixmap, const TQString &text,
                            int len, const TQColor *penColor) const {
  if (m_darkMode && !enabled) {
    // Override disabled text color in Dark Mode for better contrast
    // User requested slightly more dark gray (95 instead of 115)
    TQColor disabledText(95, 95, 95);
    TDEStyle::drawItem(p, r, flags, g, enabled, pixmap, text, len,
                       &disabledText);
  } else {
    TDEStyle::drawItem(p, r, flags, g, enabled, pixmap, text, len, penColor);
  }
}

void Q4Win10Style::drawComplexControl(ComplexControl control, TQPainter *p,
                                      const TQStyleControlElementData &ceData,
                                      ControlElementFlags elementFlags,
                                      const TQRect &r, const TQColorGroup &cg,
                                      SFlags flags, SCFlags controls,
                                      SCFlags active, const TQStyleOption &opt,
                                      const TQWidget *widget) const {
  const bool reverseLayout = TQApplication::reverseLayout();

  const bool enabled = (flags & Style_Enabled);

  switch (control) {
    // LISTVIEW
    // --------
  case CC_ListView: {
    // Custom implementation to remove branch lines (dotted lines)
    // Based on TQWindowsStyle/QtCurve logic but without drawing the lines.

    // Paint the icon and text (background/selection).
    if ( controls & SC_ListView )
        TDEStyle::drawComplexControl( control, p, ceData, elementFlags, r, cg, flags, controls, active, opt, widget );

    // If we have a branch or are expanded...
    if ( controls & (SC_ListViewBranch | SC_ListViewExpand) )
    {
        // If no list view item was supplied, break
        if (opt.isDefault())
            break;

        TQListViewItem *item  = opt.listViewItem();
        TQListViewItem *child = item->firstChild();

        int y = r.y();
        
        // Skip the stuff above the exposed rectangle
        // Helper to find next visible sibling since we don't have the static helper here
        while ( child && y + child->height() <= 0 ) {
            y += child->totalHeight();
            child = child->nextSibling();
            while (child && !child->isVisible()) child = child->nextSibling();
        }

        int bx = r.width() / 2;

        TQListView* v = item->listView();
        // Calculate line height similar to QWindowsStyle
        int lh = 0;
        if ( !item->multiLinesEnabled() )
            lh = child ? child->height() : 0; // Use child height if available
        else
            lh = p->fontMetrics().height() + 2 * v->itemMargin();
        
        lh = TQMAX( lh, TQApplication::globalStrut().height() );
        if ( lh % 2 > 0 ) lh++;

        // Draw ONLY the expand/close boxes...
        while ( child && y < r.height() )
        {
            int linebot = y + lh/2;
            
            // Recalculate lh for this child if needed (variable height)
            if ( !item->multiLinesEnabled() ) {
                 lh = child->height();
                 lh = TQMAX( lh, TQApplication::globalStrut().height() );
                 if ( lh % 2 > 0 ) lh++;
                 linebot = y + lh/2;
            }

            if ( (child->isExpandable() || child->childCount()) && (child->height() > 0) )
            {
                // Draw the expand/collapse arrow (Right / Down)
         
                int cx = bx;         // Center X
                int cy = linebot;    // Center Y
                
                p->setPen( cg.text() ); // Use text color for arrows
                p->setBrush( Qt::NoBrush ); // Arrows are lines in this style (v-shape)

                if ( !child->isOpen() ) {
                    // Collapsed -> Right Arrow (Double Stroke)
                    int tipx = cx;
                    int tipy = cy;
                    int legx = cx - 4;
                    int legy1 = cy - 4;
                    int legy2 = cy + 4;
                    
                    // Stroke 1
                    p->drawLine(legx, legy1, tipx, tipy);
                    p->drawLine(tipx, tipy, legx, legy2);
                    
                    // Stroke 2 (Offset 1px Left)
                    p->drawLine(legx - 1, legy1, tipx - 1, tipy);
                    // Note: Tip for second stroke is also moved left.
                    p->drawLine(tipx - 1, tipy, legx - 1, legy2);
                    
                } else {
                    // Expanded -> Down Arrow (Rotated 90deg -> Checkmark style pointed down)
                    // Tip at (cx, cy). Legs at (cx-4, cy-4) and (cx+4, cy-4).
                    
                    int tipx = cx;
                    int tipy = cy;
                    int legx1 = cx - 4;
                    int legx2 = cx + 4;
                    int legy = cy - 4;
                    
                    // Stroke 1
                    p->drawLine(legx1, legy, tipx, tipy);
                    p->drawLine(tipx, tipy, legx2, legy);
                    
                    // Stroke 2 (Offset 1px Up)
                    p->drawLine(legx1, legy - 1, tipx, tipy - 1);
                    p->drawLine(tipx, tipy - 1, legx2, legy - 1);
                }
                
                // CRITICAL: WE DO NOT DRAW THE LINES HERE (dotlines)
            }
            
            y += child->totalHeight();
            child = child->nextSibling();
            while (child && !child->isVisible()) child = child->nextSibling();
        }
    }
    break;
  }

    // COMBOBOX
    // --------
  case CC_ComboBox: {
    static const unsigned int handleWidth = 15;

    const TQComboBox *cb = ::tqt_cast<const TQComboBox *>(widget);
    bool editable = (elementFlags & CEF_IsEditable);
    bool hasFocus = (elementFlags & CEF_HasFocus);

    const TQColor buttonColor = enabled ? cg.button() : cg.background();
    const TQColor inputColor =
        enabled ? (editable ? cg.base() : cg.button()) : cg.background();

    // 1. Unified Border
    uint contourFlags = Draw_Left | Draw_Right | Draw_Top | Draw_Bottom |
                        Round_UpperLeft | Round_UpperRight | Round_BottomLeft |
                        Round_BottomRight;
    if (tdehtmlWidgets.contains(cb))
      contourFlags |= Draw_AlphaBlend;

    TQColor borderColor;
    if (_inputFocusHighlight && hasFocus && editable && enabled) {
      borderColor = getColor(cg, FocusHighlight, enabled);
    } else {
      borderColor = getColor(cg, PanelContour);
    }

    if (m_win11Mode) {
      int radius = 9;
      int xRound = (r.width() > 0) ? (200 * radius / r.width()) : 0;
      int yRound = (r.height() > 0) ? (200 * radius / r.height()) : 0;

      // CRITICAL: Clear square corners
      p->fillRect(r, cg.background());

      // Fill background
      p->setPen(TQt::NoPen);
      p->setBrush(inputColor);
      p->drawRoundRect(r, xRound, yRound);
    } else {
      renderContour(p, r, cg.background(), borderColor, contourFlags);
    }

    // 2. Divide Interior (perfectly flush)
    TQRect inner = TQRect(r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2);
    TQRect RbuttonSurface, RcontentSurface;
    if (reverseLayout) {
      RbuttonSurface =
          TQRect(inner.x(), inner.y(), handleWidth, inner.height());
      RcontentSurface = TQRect(inner.x() + handleWidth, inner.y(),
                               inner.width() - handleWidth, inner.height());
    } else {
      RcontentSurface = TQRect(inner.x(), inner.y(),
                               inner.width() - handleWidth, inner.height());
      RbuttonSurface = TQRect(inner.x() + inner.width() - handleWidth,
                              inner.y(), handleWidth, inner.height());
    }

    // 3. Render Button Surface (no Draw_ flags to avoid shrinking)
    uint btnSurfaceFlags = Is_Horizontal;
    if (m_win11Mode) {
      if (reverseLayout) {
        btnSurfaceFlags |= Round_UpperLeft | Round_BottomLeft;
      } else {
        btnSurfaceFlags |= Round_UpperRight | Round_BottomRight;
      }
    } else {
      if (reverseLayout) {
        btnSurfaceFlags |= Round_UpperLeft | Round_BottomLeft;
      } else {
        btnSurfaceFlags |= Round_UpperRight | Round_BottomRight;
      }
    }
    if (flags & Style_MouseOver)
      btnSurfaceFlags |= Is_Highlight;
    // Note: sunken state handled by primitive calls if needed
    renderSurface(p, RbuttonSurface, cg.background(), buttonColor,
                  getColor(cg, MouseOverHighlight),
                  enabled ? _contrast + 3 : (_contrast / 2), btnSurfaceFlags);

    // 4. Render Content Surface

    if (editable) {
      if (m_win11Mode) {
        // In Win11 mode, we must respect the left rounding of the field
        uint contentSurfaceFlags = Is_Horizontal;
        if (reverseLayout) {
          contentSurfaceFlags |= Round_UpperRight | Round_BottomRight;
        } else {
          contentSurfaceFlags |= Round_UpperLeft | Round_BottomLeft;
        }
        renderSurface(p, RcontentSurface, cg.background(), inputColor,
                      getColor(cg, MouseOverHighlight),
                      enabled ? _contrast + 3 : (_contrast / 2),
                      contentSurfaceFlags);
      } else {
        p->fillRect(RcontentSurface, inputColor);
      }
    } else {
      uint contentSurfaceFlags = Is_Horizontal;
      if (reverseLayout) {
        contentSurfaceFlags |= Round_UpperRight | Round_BottomRight;
      } else {
        contentSurfaceFlags |= Round_UpperLeft | Round_BottomLeft;
      }
      if (flags & Style_MouseOver)
        contentSurfaceFlags |= Is_Highlight;
      renderSurface(p, RcontentSurface, cg.background(), buttonColor,
                    getColor(cg, MouseOverHighlight),
                    enabled ? _contrast + 3 : (_contrast / 2),
                    contentSurfaceFlags);
      if (hasFocus) {
        drawPrimitive(PE_FocusRect, p, ceData, elementFlags,
                      TQRect(RcontentSurface.x() + 2, RcontentSurface.y() + 2,
                             RcontentSurface.width() - 4,
                             RcontentSurface.height() - 4),
                      cg);
      }
    }

    // 5. Draw Arrow Icon
    p->setPen(cg.foreground());
    drawPrimitive(PE_SpinWidgetDown, p, ceData, elementFlags, RbuttonSurface,
                  cg, flags);

    // 6. Final Border (Win11 only, drawn last to be crisp)
    if (m_win11Mode) {
      int radius = 9;
      int xRound = (r.width() > 0) ? (200 * radius / r.width()) : 0;
      int yRound = (r.height() > 0) ? (200 * radius / r.height()) : 0;
      p->setPen(borderColor);
      p->setBrush(TQt::NoBrush);
      p->drawRoundRect(r, xRound, yRound);
    }

    p->setPen(cg.buttonText());
    p->setBackgroundColor(cg.button());
    break;
  }

    // TOOLBUTTON
    // ----------
  case CC_ToolButton: {
    TQRect button, menuarea;
    button = querySubControlMetrics(control, ceData, elementFlags,
                                    SC_ToolButton, opt, widget);
    menuarea = querySubControlMetrics(control, ceData, elementFlags,
                                      SC_ToolButtonMenu, opt, widget);

    SFlags bflags = flags, mflags = flags;

    if (kornMode) {
      drawPrimitive(PE_ButtonTool, p, ceData, elementFlags, button, cg, bflags,
                    opt);
      break;
    } else {
      // don't want to have the buttoncolor as the background...
      p->fillRect(r, cg.background());
      bflags &= ~Style_MouseOver;
    }

    if (active & SC_ToolButton)
      bflags |= Style_Down;

    if (active & SC_ToolButtonMenu)
      mflags |= Style_Down;

    if (controls & SC_ToolButton) {
      // If we're pressed, on, or raised...
      if (bflags & (Style_Down | Style_On | Style_Raised) ||
          (flags & Style_MouseOver)) {
        drawPrimitive(PE_ButtonTool, p, ceData, elementFlags, button, cg,
                      bflags, opt);
      } else if ((elementFlags & CEF_HasParentWidget) &&
                 !ceData.parentWidgetData.bgPixmap.isNull()) {
        TQPixmap pixmap = ceData.parentWidgetData.bgPixmap;
        p->drawTiledPixmap(r, pixmap, ceData.pos);
      }
    }

    // Draw a toolbutton menu indicator if required
    if (controls & SC_ToolButtonMenu) {
      if (mflags & (Style_Down | Style_On | Style_Raised)) {
        drawPrimitive(PE_ButtonDropDown, p, ceData, elementFlags, menuarea, cg,
                      mflags, opt);
      }
      drawPrimitive(PE_ArrowDown, p, ceData, elementFlags, menuarea, cg, mflags,
                    opt);
    }

    if ((elementFlags & CEF_HasFocus) && !(elementFlags & CEF_HasFocusProxy)) {
      TQRect fr = ceData.rect;
      fr.addCoords(2, 2, -2, -2);
      drawPrimitive(PE_FocusRect, p, ceData, elementFlags, fr, cg);
    }

    // Set the color for the ToolButton menu indicator
    p->setPen(cg.buttonText());

    break;
  }

    // SPINWIDGETS
    // -----------
  case CC_SpinWidget: {
    static const unsigned int handleWidth = 15;

    const TQSpinWidget *sw = ::tqt_cast<const TQSpinWidget *>(widget);
    bool hasFocus = sw ? sw->hasFocus() : false;

    const TQColor buttonColor = enabled ? cg.button() : cg.background();
    const TQColor inputColor = enabled ? cg.base() : cg.background();

    // 1. Unified Border
    uint contourFlags = Draw_Left | Draw_Right | Draw_Top | Draw_Bottom |
                        Round_UpperLeft | Round_UpperRight | Round_BottomLeft |
                        Round_BottomRight;
    TQColor borderColor;
    if (_inputFocusHighlight && hasFocus && enabled) {
      borderColor = getColor(cg, FocusHighlight, enabled);
    } else {
      borderColor = getColor(cg, PanelContour);
    }

    if (m_win11Mode) {
      int radius = 9;
      int xRound = (r.width() > 0) ? (200 * radius / r.width()) : 0;
      int yRound = (r.height() > 0) ? (200 * radius / r.height()) : 0;

      // CRITICAL: Clear square corners
      p->fillRect(r, cg.background());

      // Fill background
      p->setPen(TQt::NoPen);
      p->setBrush(inputColor);
      p->drawRoundRect(r, xRound, yRound);
    } else {
      renderContour(p, r, cg.background(), borderColor, contourFlags);
    }

    // 2. Divide Interior (perfectly flush)
    TQRect inner = TQRect(r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2);
    TQRect RbuttonSurface, RcontentSurface;
    if (reverseLayout) {
      RbuttonSurface =
          TQRect(inner.x(), inner.y(), handleWidth, inner.height());
      RcontentSurface = TQRect(inner.x() + handleWidth, inner.y(),
                               inner.width() - handleWidth, inner.height());
    } else {
      RcontentSurface = TQRect(inner.x(), inner.y(),
                               inner.width() - handleWidth, inner.height());
      RbuttonSurface = TQRect(inner.x() + inner.width() - handleWidth,
                              inner.y(), handleWidth, inner.height());
    }

    // 3. Render Buttons (Up/Down)
    TQRect upRect = TQRect(RbuttonSurface.left(), RbuttonSurface.top(),
                           RbuttonSurface.width(), RbuttonSurface.height() / 2);
    TQRect downRect = TQRect(
        RbuttonSurface.left(), upRect.bottom() + 1, RbuttonSurface.width(),
        RbuttonSurface.height() - upRect.height() - 1);

    // Up Button
    uint upFlags = Is_Horizontal;
    if (m_win11Mode) {
      if (reverseLayout)
        upFlags |= Round_UpperLeft;
      else
        upFlags |= Round_UpperRight;
    } else {
      if (reverseLayout)
        upFlags |= Round_UpperLeft;
      else
        upFlags |= Round_UpperRight;
    }
    if (active == SC_SpinWidgetUp)
      upFlags |= Is_Sunken;
    else if ((flags & Style_MouseOver) &&
             (active == SC_SpinWidgetUp || active == SC_None))
      upFlags |= Is_Highlight;

    renderSurface(p, upRect, cg.background(), buttonColor,
                  getColor(cg, MouseOverHighlight),
                  enabled ? _contrast + 3 : (_contrast / 2), upFlags);

    // Down Button
    uint downFlags = Is_Horizontal;
    if (m_win11Mode) {
      if (reverseLayout)
        downFlags |= Round_BottomLeft;
      else
        downFlags |= Round_BottomRight;
    } else {
      if (reverseLayout)
        downFlags |= Round_BottomLeft;
      else
        downFlags |= Round_BottomRight;
    }
    if (active == SC_SpinWidgetDown)
      downFlags |= Is_Sunken;
    else if ((flags & Style_MouseOver) &&
             (active == SC_SpinWidgetDown || active == SC_None))
      downFlags |= Is_Highlight;

    renderSurface(p, downRect, cg.background(), buttonColor,
                  getColor(cg, MouseOverHighlight),
                  enabled ? _contrast + 3 : (_contrast / 2), downFlags);

    // 4. Content
    if (m_win11Mode) {
      uint contentSurfaceFlags = Is_Horizontal;
      if (reverseLayout) {
        contentSurfaceFlags |= Round_UpperRight | Round_BottomRight;
      } else {
        contentSurfaceFlags |= Round_UpperLeft | Round_BottomLeft;
      }
      renderSurface(p, RcontentSurface, cg.background(), inputColor,
                    getColor(cg, MouseOverHighlight),
                    enabled ? _contrast + 3 : (_contrast / 2),
                    contentSurfaceFlags);
    } else {
      p->fillRect(RcontentSurface, inputColor);
    }

    // 5. Draw Arrows
    p->setPen(cg.foreground());
    drawPrimitive(PE_SpinWidgetUp, p, ceData, elementFlags, upRect, cg, flags);
    drawPrimitive(PE_SpinWidgetDown, p, ceData, elementFlags, downRect, cg,
                  flags);

    // 5. Final Border (Win11 only, drawn last to be crisp)
    if (m_win11Mode) {
      int radius = 9;
      int xRound = (r.width() > 0) ? (200 * radius / r.width()) : 0;
      int yRound = (r.height() > 0) ? (200 * radius / r.height()) : 0;
      p->setPen(borderColor);
      p->setBrush(TQt::NoBrush);
      p->drawRoundRect(r, xRound, yRound);
    }

    break;
  }

  case CC_ScrollBar: {
    if (m_win11Mode) {
      // Win11: No buttons, slider goes edge to edge.
      // We MUST clear the whole background to avoid "ghost" sliders
      // in the areas formerly occupied by buttons.
      p->fillRect(r, cg.background());
    }
    TDEStyle::drawComplexControl(control, p, ceData, elementFlags, r, cg, flags,
                                 controls, active, opt, widget);
    break;
  }

  default:
    TDEStyle::drawComplexControl(control, p, ceData, elementFlags, r, cg, flags,
                                 controls, active, opt, widget);
    break;
  }
}

TQRect Q4Win10Style::subRect(SubRect r, const TQStyleControlElementData &ceData,
                             const ControlElementFlags elementFlags,
                             const TQWidget *widget) const {
  switch (r) {
  case SR_ComboBoxFocusRect: {
    return querySubControlMetrics(CC_ComboBox, ceData, elementFlags,
                                  SC_ComboBoxEditField, TQStyleOption::Default,
                                  widget);
  }

  // Don't use TDEStyles progressbar subrect
  // TODO:
  case SR_ProgressBarGroove: {
    return TQRect(ceData.rect);
  }
  case SR_ProgressBarContents:
  case SR_ProgressBarLabel: {
    TQRect rw = ceData.rect;
    // +1/-2 to account for 1px border drawn by groove
    return TQRect(rw.left() + 1, rw.top() + 1, rw.width() - 2, rw.height() - 2);
  }

  default: {
    return TDEStyle::subRect(r, ceData, elementFlags, widget);
  }
  }
}

TQRect Q4Win10Style::querySubControlMetrics(
    ComplexControl control, const TQStyleControlElementData &ceData,
    ControlElementFlags elementFlags, SubControl subcontrol,
    const TQStyleOption &opt, const TQWidget *widget) const {
  TQRect r(ceData.rect);
  switch (control) {
  case CC_ScrollBar: {
    if (m_win11Mode) {
      const TQScrollBar *sb = ::tqt_cast<const TQScrollBar *>(widget);
      
      switch (subcontrol) {
      case SC_ScrollBarAddLine:
      case SC_ScrollBarSubLine:
        return TQRect(); // No buttons in Win11 mode
      case SC_ScrollBarGroove:
        return ceData.rect; // Extend track to full area (occupy button space)
      case SC_ScrollBarAddPage:
      case SC_ScrollBarSubPage:
      case SC_ScrollBarSlider:
        return TDEStyle::querySubControlMetrics(control, ceData, elementFlags,
                                                subcontrol, opt, widget);
      default:
        break;
      }
    }
    return TDEStyle::querySubControlMetrics(control, ceData, elementFlags,
                                            subcontrol, opt, widget);
  }
  case CC_ComboBox: {
    switch (subcontrol) {
    case SC_ComboBoxEditField: {
      // TODO: is the handler width in pixelmetric?
      return TQRect(r.left() + 2, r.top() + 2, r.width() - 4 - 15 - 1,
                    r.height() - 4);
    }
    default: {
      return TDEStyle::querySubControlMetrics(control, ceData, elementFlags,
                                              subcontrol, opt, widget);
    }
    }
    break;
  }
  case CC_SpinWidget: {
    const int fw = 2; // Frame width...

    const bool heightDividable = ((r.height() % 2) == 0);

    TQSize bs;
    if (heightDividable) {
      bs.setHeight(TQMAX(8, (r.height() - 2) / 2));
    } else {
      bs.setHeight(TQMAX(8, (r.height() - 2 - 1) / 2));
    }
    bs.setWidth(15);

    const int buttonsLeft =
        /*reverseLayout?r.left()+1:*/ r.right() - bs.width();

    switch (subcontrol) {
    case SC_SpinWidgetUp: {
      return TQRect(buttonsLeft, r.top() + 1, bs.width(), bs.height());
    }
    case SC_SpinWidgetDown: {
      if (heightDividable) {
        return TQRect(buttonsLeft, r.top() + 1 + bs.height(), bs.width(),
                      r.height() - (bs.height() + 2));
      } else {
        return TQRect(buttonsLeft, r.top() + 1 + bs.height() + 1, bs.width(),
                      r.height() - (bs.height() + 2 + 1));
      }
    }
    case SC_SpinWidgetFrame: {
      return TQRect(r.left(), r.top(), r.width(), r.height());
    }
    case SC_SpinWidgetEditField: {
      return TQRect(r.left() + fw, r.top() + fw,
                    r.width() - (bs.width() + 1 + 2 * fw), r.height() - 2 * fw);
    }
    case SC_SpinWidgetButtonField: {
      return TQRect(buttonsLeft, r.top() + 1, bs.width(), r.height() - 2);
    }
    default: {
      return TDEStyle::querySubControlMetrics(control, ceData, elementFlags,
                                              subcontrol, opt, widget);
    }
    }
    break;
  }
  default: {
    return TDEStyle::querySubControlMetrics(control, ceData, elementFlags,
                                            subcontrol, opt, widget);
  }
  }
}

int Q4Win10Style::pixelMetric(PixelMetric m,
                              const TQStyleControlElementData &ceData,
                              ControlElementFlags elementFlags,
                              const TQWidget *widget) const {
  switch (m) {
    // TABS
    // ----
  case PM_TabBarTabVSpace: {
    if (ceData.tabBarData.shape == TQTabBar::RoundedAbove ||
        ceData.tabBarData.shape == TQTabBar::RoundedBelow)
      return 12;
    else
      return 4;
  }

  case PM_TabBarTabOverlap: {
    if (m_win11Mode)
      return 0;
    return 1;
  }

    // extra space between menubar items
  case PM_MenuBarItemSpacing: {
    return 6;
  }

    //     // extra space between toolbar items
    //         case PM_ToolBarItemSpacing: {
    //             return 4;
    //         }

    // SCROLL BAR
  case PM_ScrollBarSliderMin: {
    return 21;
  }
  case PM_ScrollBarExtent: {
    return 16;
  }

  case PM_DockWindowSeparatorExtent:
    return 6;

    // SPLITTERS
    // ---------
  case PM_SplitterWidth: {
    return 6;
  }

    // PROGRESSBARS
    // ------------
  case PM_ProgressBarChunkWidth:
    return 10;

    // SLIDER
    // ------
  case PM_SliderLength:
    return m_win11Mode ? 15 : 8; // Win11: circular handle needs square rect; Win10: thin rectangle
  case PM_SliderControlThickness:
    return 18; // Height of handle for horizontal slider

    // MENU INDICATOR
    // --------------
  case PM_MenuButtonIndicator:
    return 12; // Adjusted to permit a 11px high chevron
    // CHECKBOXES / RADIO BUTTONS
    // --------------------------
  case PM_ExclusiveIndicatorWidth:  // Radiobutton size
  case PM_ExclusiveIndicatorHeight: // 13x13
  case PM_IndicatorWidth:           // Checkbox size
  case PM_IndicatorHeight:          // 13x13
    return 13;

    // FRAMES
    // ------
  case PM_SpinBoxFrameWidth:
    return 1;

  case PM_MenuBarFrameWidth:
    return 0; // Use 0 to match standard Windows/Motif and avoid invalidation offsets

  case PM_DefaultFrameWidth: {
    if (widget && ::tqt_cast<TQPopupMenu *>(widget))
      return 1;
    else
      return 2;
  }

  case PM_ButtonDefaultIndicator: {
    return 0;
  }

  case PM_ButtonMargin: {
    return 2;
  }

  case PM_ButtonShiftVertical:
  case PM_ButtonShiftHorizontal: {
    return 1;
  }

  case PM_MenuIndicatorFrameHBorder:
  case PM_MenuIndicatorFrameVBorder:
  case PM_MenuIconIndicatorFrameHBorder:
  case PM_MenuIconIndicatorFrameVBorder:
    return 2;

  default:
    return TDEStyle::pixelMetric(m, ceData, elementFlags, widget);
  }
}

TQSize Q4Win10Style::sizeFromContents(ContentsType t,
                                      const TQStyleControlElementData &ceData,
                                      ControlElementFlags elementFlags,
                                      const TQSize &s, const TQStyleOption &opt,
                                      const TQWidget *widget) const {
  switch (t) {
  case CT_PopupMenuItem: {
    if (!widget || opt.isDefault())
      return s;

    const TQPopupMenu *popup = ::tqt_cast<const TQPopupMenu *>(widget);
    if (!popup) return s;
    TQMenuItem *mi = opt.menuItem();
    int maxpmw = opt.maxIconWidth();
    int w = s.width(), h = s.height();
    bool checkable = popup->isCheckable();

    if (mi->custom()) {
      w = mi->custom()->sizeHint().width();
      h = mi->custom()->sizeHint().height();
      if (!mi->custom()->fullSpan())
        h += 4;
    } else if (mi->widget()) {
      // don't change the size in this case.
    } else if (mi->isSeparator()) {
      w = 20;
      h = 2;
    } else {
      if (mi->pixmap()) {
        h = TQMAX(h, mi->pixmap()->height() + 2);
      } else {
        h = TQMAX(h, 16 + 2);
        h = TQMAX(h, popup->fontMetrics().height() + 4);
      }

      if (mi->iconSet()) {
        h = TQMAX(h, mi->iconSet()
                             ->pixmap(TQIconSet::Small, TQIconSet::Normal)
                             .height() +
                         2);
      }
    }

    if (!mi->text().isNull() && (mi->text().find('\t') >= 0)) {
      w += itemHMargin + itemFrame * 2 + 7;
    } else if (mi->popup()) {
      w += 2 * arrowHMargin;
    }

    if (maxpmw) {
      w += maxpmw + 6;
    }
    if (checkable && maxpmw < 20) {
      w += 20 - maxpmw;
    }
    if (checkable || maxpmw > 0) {
      w += 12;
    }

    w += rightBorder;

    return TQSize(w, h);
  }

  case CT_PushButton: {
    const TQPushButton *btn = static_cast<const TQPushButton *>(widget);

    int w = s.width() +
            2 * pixelMetric(PM_ButtonMargin, ceData, elementFlags, widget);
    int h = s.height() +
            2 * pixelMetric(PM_ButtonMargin, ceData, elementFlags, widget);
    if (btn->text().isEmpty() && s.width() < 32)
      return TQSize(w, h);

    return TQSize(w + 25, h + 5);
  }

  case CT_ToolButton: {
    if (widget->parent() && ::tqt_cast<TQToolBar *>(widget->parent()))
      return TQSize(s.width() + 2 * 4, s.height() + 2 * 4);
    else
      return TDEStyle::sizeFromContents(t, ceData, elementFlags, s, opt,
                                        widget);
  }

  default:
    return TDEStyle::sizeFromContents(t, ceData, elementFlags, s, opt, widget);
  }

  return TDEStyle::sizeFromContents(t, ceData, elementFlags, s, opt, widget);
}

int Q4Win10Style::styleHint(StyleHint stylehint,
                            const TQStyleControlElementData &ceData,
                            ControlElementFlags elementFlags,
                            const TQStyleOption &option,
                            TQStyleHintReturn *returnData,
                            const TQWidget *widget) const {
  switch (stylehint) {
  // Menu popup delay - the only remaining configurable option
  case SH_PopupMenu_SubMenuPopupDelay:
    return 96; // Motif-like delay...

  case SH_MenuIndicatorColumnWidth: {
    int checkcol = option.maxIconWidth();
    bool checkable = (elementFlags & CEF_IsCheckable);

    if (checkable)
      checkcol = TQMAX(checkcol, 20);

    return checkcol;
  }

  // === HARDCODED OPTIONS ===
  // "Enable scrolling in popup menu" -> hardcoded OFF
  case SH_PopupMenu_Scrollable:
    return 0; // false - no scrolling in popup menus

  // "Pressing only the menu bar activator keys selects the menu bar" ->
  // hardcoded ON
  case SH_MenuBar_AltKeyNavigation:
    return 1; // true - Alt key activates menu

  case SH_MenuBar_MouseTracking:
    return 1; // true - Use standard TQt tracking (like Windows)

  // Windows 10 style: no underlined accelerators
  case SH_UnderlineAccelerator:
    return 0; // false - never show underlined accelerators
  case SH_HideUnderlineAcceleratorWhenAltUp:
    return 1; // true - always hide underlines

  default:
    return TDEStyle::styleHint(stylehint, ceData, elementFlags, option,
                               returnData, widget);
  }
}

bool Q4Win10Style::objectEventHandler(const TQStyleControlElementData &ceData,
                                      ControlElementFlags elementFlags,
                                      void *source, TQEvent *ev) {
  if (TDEStyle::objectEventHandler(ceData, elementFlags, source, ev))
    return true;

  if (ceData.widgetObjectTypes.contains("TQObject")) {
    TQObject *obj = reinterpret_cast<TQObject *>(source);

    // Track show events for progress bars
    // Animations disabled

    if (!qstrcmp(obj->name(), "tde toolbar widget")) {
      TQWidget *lb = static_cast<TQWidget *>(obj);
      if (lb->backgroundMode() == TQt::PaletteButton)
        lb->setBackgroundMode(TQt::PaletteBackground);
      removeObjectEventHandler(ceData, elementFlags, source, this);
    }

    if (TQTabBar *tabBar = ::tqt_cast<TQTabBar *>(obj)) {
      if (ev->type() == TQEvent::MouseButtonPress ||
          ev->type() == TQEvent::MouseButtonRelease ||
          ev->type() == TQEvent::Wheel) {
        tabBar->update();
      }
    }

    if (m_win11Mode) {
      if (TQPopupMenu *popup = ::tqt_cast<TQPopupMenu *>(obj)) {
        if (ev->type() == TQEvent::Resize || ev->type() == TQEvent::Show) {
          TQRect r = popup->rect();
          if (r.width() > 0 && r.height() > 0) {
            TQBitmap mask(r.size());
            mask.fill(TQt::color0);
            TQPainter mp(&mask);
            mp.setBrush(TQt::color1);
            mp.setPen(TQt::color1);
            int radius = 9;
            int xRound = (200 * radius / r.width());
            int yRound = (200 * radius / r.height());
            mp.drawRoundRect(r, xRound, yRound);
            popup->setMask(mask);
          }
        }
      }
    }
  }

  return false;
}

TQColor Q4Win10Style::getColor(const TQColorGroup &cg, const ColorType t,
                               const bool enabled) const {
  return getColor(cg, t, enabled ? IsEnabled : IsDisabled);
}

TQColor Q4Win10Style::getColor(const TQColorGroup &cg, const ColorType t,
                               const WidgetState s) const {
  const bool enabled =
      (s != IsDisabled) &&
      ((s == IsEnabled) || (s == IsPressed) || (s == IsHighlighted));
  const bool pressed = (s == IsPressed);
  const bool highlighted = (s == IsHighlighted);
  switch (t) {
  case ButtonContour:
    if (highlighted)
      return getColor(cg, FocusHighlight);
    if (m_darkMode)
      return cg.background().light(145); // Lighter gray border (User requested)
    return cg.background().dark(118);
  case DragButtonContour: {
    if (enabled) {
      if (pressed)
        return cg.button().dark(130 + _contrast * 6); // bright
      else if (highlighted)
        return cg.button().dark(130 + _contrast * 9); // dark
      else
        return cg.button().dark(130 + _contrast * 8); // normal
    } else {
      return cg.background().dark(120 + _contrast * 8);
    }
  }
  case DragButtonSurface: {
    if (enabled) {
      if (pressed)
        return cg.button().dark(100 - _contrast); // bright
      else if (highlighted)
        return cg.button().light(100 + _contrast); // dark
      else
        return cg.button(); // normal
    } else {
      return cg.background();
    }
  }
  case PanelContour:
    if (m_darkMode)
      return cg.background().light(145); // Lighter gray border (GroupBoxes, etc)
    return cg.background().dark(118);
  case PanelDark:
    return alphaBlendColors(cg.background(),
                            cg.background().dark(120 + _contrast * 5), 110);
  case PanelDark2:
    return alphaBlendColors(cg.background(),
                            cg.background().dark(110 + _contrast * 5), 110);
  case PanelLight:
    return alphaBlendColors(cg.background(),
                            cg.background().light(120 + _contrast * 5), 110);
  case PanelLight2:
    return alphaBlendColors(cg.background(),
                            cg.background().light(110 + _contrast * 5), 110);
  case MouseOverHighlight:
    if (m_darkMode)
      // Dark Mode: Darker background to contrast with white text (#2A4560 = 42 69 96)
      return TQColor(42, 69, 96);
    else
      // Light Mode: Windows 10 very light blue hover color
      return TQColor(0xE5, 0xF3, 0xFF); // #E5F3FF - very light blue
  case FocusHighlight:
    if (_customFocusHighlightColor)
      return _focusHighlightColor;
    else
      return cg.highlight();
  case CheckMark:
    if (_customCheckMarkColor)
      return _checkMarkColor;
    else
      return cg.foreground();
  default:
    return cg.background();
  }
}
