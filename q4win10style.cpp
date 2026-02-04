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
#include <tqapplication.h>
#include <tqcheckbox.h>
#include <tqcleanuphandler.h>
#include <tqcombobox.h>
#include <tqdrawutil.h>
#include <tqheader.h>
#include <tqimage.h>
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

static TQColor alphaBlendColors(const TQColor &bgColor, const TQColor &fgColor,
                                const int a) {
  // normal button...
  TQRgb rgb = bgColor.rgb();
  TQRgb rgb_b = fgColor.rgb();
  int alpha = a;
  if (alpha > 255)
    alpha = 255;
  if (alpha < 0)
    alpha = 0;
  int inv_alpha = 255 - alpha;

  TQColor result = TQColor(
      tqRgb(tqRed(rgb_b) * inv_alpha / 255 + tqRed(rgb) * alpha / 255,
            tqGreen(rgb_b) * inv_alpha / 255 + tqGreen(rgb) * alpha / 255,
            tqBlue(rgb_b) * inv_alpha / 255 + tqBlue(rgb) * alpha / 255));

  return result;
}

#include "q4win10style.h"
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

  TQStringList keys() const { return TQStringList() << "Q4Win10v2"; }

  TQStyle *create(const TQString &key) {
    if (key.lower() == "q4win10v2")
      return new Q4Win10Style;
    return 0;
  }
};

TDE_EXPORT_PLUGIN(Q4Win10StylePlugin)
// -- end --

Q4Win10Style::Q4Win10Style()
    : TDEStyle(AllowMenuTransparency, WindowsStyleScrollBar), kickerMode(false),
      kornMode(false), flatMode(false) {
  horizontalLine = 0;
  verticalLine = 0;

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

  // setup pixmap cache...
  pixmapCache = new TQIntCache<CacheEntry>(150000, 499);
  pixmapCache->setAutoDelete(true);

  // Animation disabled - no timer needed
}

// Animations disabled - updateProgressPos removed

Q4Win10Style::~Q4Win10Style() {
  delete pixmapCache;
  delete horizontalLine;
  delete verticalLine;
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
      connect(widget, TQ_SIGNAL(destroyed(TQObject *)), this,
              TQ_SLOT(tdehtmlWidgetDestroyed(TQObject *)));
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
    } else if (::tqt_cast<TQPopupMenu *>(widget)) {
      widget->setBackgroundMode(NoBackground);
    } else if (!qstrcmp(widget->name(), "tde toolbar widget")) {
      installObjectEventHandler(ceData, elementFlags, ptr, this);
    }

    // Animations disabled
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
    contourColor = backgroundColor.dark(150);
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

  if (w > 0 && h > 0)
    p->fillRect(x, y, w, h, fillColor);
}

void Q4Win10Style::renderPixel(TQPainter *p, const TQPoint &pos,
                               const int alpha, const TQColor &color,
                               const TQColor &background,
                               bool fullAlphaBlend) const {
  if (fullAlphaBlend)
  // full alpha blend: paint into an image with alpha buffer and convert to a
  // pixmap ...
  {
    TQRgb rgb = color.rgb();
    // generate a quite unique key -- use the unused width field to store the
    // alpha value.
    CacheEntry search(cAlphaDot, alpha, 0, rgb);
    int key = search.key();

    CacheEntry *cacheEntry;
    if ((cacheEntry = pixmapCache->find(key))) {
      if (search == *cacheEntry) { // match! we can draw now...
        if (cacheEntry->pixmap)
          p->drawPixmap(pos, *(cacheEntry->pixmap));
        return;
      } else { // Remove old entry in case of a conflict!
        pixmapCache->remove(key);
      }
    }

    TQImage aImg(1, 1, 32); // 1x1
    aImg.setAlphaBuffer(true);
    aImg.setPixel(0, 0, tqRgba(tqRed(rgb), tqGreen(rgb), tqBlue(rgb), alpha));
    TQPixmap *result = new TQPixmap(aImg);

    p->drawPixmap(pos, *result);

    // add to the cache...
    CacheEntry *toAdd = new CacheEntry(search);
    toAdd->pixmap = result;
    bool insertOk = pixmapCache->insert(key, toAdd, result->depth() / 8);
    if (!insertOk)
      delete result;
  } else
  // don't use an alpha buffer: calculate the resulting color from the alpha
  // value, the fg- and the bg-color.
  {
    TQRgb rgb_a = color.rgb();
    TQRgb rgb_b = background.rgb();
    int a = alpha;
    if (a > 255)
      a = 255;
    if (a < 0)
      a = 0;
    int a_inv = 255 - a;

    TQColor res =
        TQColor(tqRgb(tqRed(rgb_b) * a_inv / 255 + tqRed(rgb_a) * a / 255,
                      tqGreen(rgb_b) * a_inv / 255 + tqGreen(rgb_a) * a / 255,
                      tqBlue(rgb_b) * a_inv / 255 + tqBlue(rgb_a) * a / 255));
    p->setPen(res);
    p->drawPoint(pos);
  }
}

void Q4Win10Style::renderButton(TQPainter *p, const TQRect &r,
                                const TQColorGroup &g, bool sunken,
                                bool mouseOver, bool horizontal, bool enabled,
                                bool tdehtmlMode) const {
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

  if (!flatMode) {
    contourFlags |= Round_UpperLeft | Round_UpperRight | Round_BottomLeft |
                    Round_BottomRight;
    surfaceFlags |= Round_UpperLeft | Round_UpperRight | Round_BottomLeft |
                    Round_BottomRight;

    renderContour(p, r, g.background(), getColor(g, ButtonContour),
                  contourFlags);
    renderSurface(
        p, TQRect(r.left() + 1, r.top() + 1, r.width() - 2, r.height() - 2),
        g.background(), g.button(), getColor(g, MouseOverHighlight), _contrast,
        surfaceFlags);
  } else {
    renderContour(p, r, g.background(), g.button().dark(105 + _contrast * 3),
                  contourFlags);
    renderSurface(
        p, TQRect(r.left() + 1, r.top() + 1, r.width() - 2, r.height() - 2),
        g.background(), g.button(), getColor(g, MouseOverHighlight),
        _contrast / 2, surfaceFlags);

    flatMode = false;
  }

  p->setPen(oldPen);
}

void Q4Win10Style::renderDot(TQPainter *p, const TQPoint &point,
                             const TQColor &baseColor, const bool thick,
                             const bool sunken) const {
  const TQColor topColor = alphaBlendColors(
      baseColor, sunken ? baseColor.dark(130) : baseColor.light(150), 70);
  const TQColor bottomColor = alphaBlendColors(
      baseColor, sunken ? baseColor.light(150) : baseColor.dark(130), 70);
  p->setPen(topColor);
  p->drawLine(point.x(), point.y(), point.x() + 1, point.y());
  p->drawPoint(point.x(), point.y() + 1);
  p->setPen(bottomColor);
  if (thick) {
    p->drawLine(point.x() + 1, point.y() + 2, point.x() + 2, point.y() + 2);
    p->drawPoint(point.x() + 2, point.y() + 1);
  } else {
    p->drawPoint(point.x() + 1, point.y() + 1);
  }
}

void Q4Win10Style::renderGradient(TQPainter *painter, const TQRect &rect,
                                  const TQColor &c1, const TQColor &c2,
                                  bool horizontal) const {
  if ((rect.width() <= 0) || (rect.height() <= 0))
    return;

  // generate a quite unique key for this surface.
  CacheEntry search(cGradientTile, horizontal ? 0 : rect.width(),
                    horizontal ? rect.height() : 0, c1.rgb(), c2.rgb(),
                    horizontal);
  int key = search.key();

  CacheEntry *cacheEntry;
  if ((cacheEntry = pixmapCache->find(key))) {
    if (search == *cacheEntry) { // match! we can draw now...
      if (cacheEntry->pixmap) {
        painter->drawTiledPixmap(rect, *(cacheEntry->pixmap));
      }
      return;
    } else {
      // Remove old entry in case of a conflict!
      // This shouldn't happen very often, see comment in CacheEntry.
      pixmapCache->remove(key);
    }
  }

  // there wasn't anything matching in the cache, create the pixmap now...
  TQPixmap *result = new TQPixmap(horizontal ? 10 : rect.width(),
                                  horizontal ? rect.height() : 10);
  TQPainter p(result);

  int r_w = result->rect().width();
  int r_h = result->rect().height();
  int r_x, r_y, r_x2, r_y2;
  result->rect().coords(&r_x, &r_y, &r_x2, &r_y2);

  int rDiff, gDiff, bDiff;
  int rc, gc, bc;

  int x, y;

  rDiff = (c2.red()) - (rc = c1.red());
  gDiff = (c2.green()) - (gc = c1.green());
  bDiff = (c2.blue()) - (bc = c1.blue());

  int rl = rc << 16;
  int gl = gc << 16;
  int bl = bc << 16;

  int rdelta = ((1 << 16) / (horizontal ? r_h : r_w)) * rDiff;
  int gdelta = ((1 << 16) / (horizontal ? r_h : r_w)) * gDiff;
  int bdelta = ((1 << 16) / (horizontal ? r_h : r_w)) * bDiff;

  // these for-loops could be merged, but the if's in the inner loop
  // would make it slow
  if (horizontal) {
    for (y = 0; y < r_h; y++) {
      rl += rdelta;
      gl += gdelta;
      bl += bdelta;

      p.setPen(TQColor(rl >> 16, gl >> 16, bl >> 16));
      p.drawLine(r_x, r_y + y, r_x2, r_y + y);
    }
  } else {
    for (x = 0; x < r_w; x++) {
      rl += rdelta;
      gl += gdelta;
      bl += bdelta;

      p.setPen(TQColor(rl >> 16, gl >> 16, bl >> 16));
      p.drawLine(r_x + x, r_y, r_x + x, r_y2);
    }
  }

  p.end();

  // draw the result...
  painter->drawTiledPixmap(rect, *result);

  // insert into cache using the previously created key.
  CacheEntry *toAdd = new CacheEntry(search);
  toAdd->pixmap = result;
  bool insertOk = pixmapCache->insert(
      key, toAdd, result->width() * result->height() * result->depth() / 8);

  if (!insertOk)
    delete result;
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
    //  Stolen wholesale from Keramik. I don't like it, but oh well.
    if (sunken) {
      const TQCOORD corners[] = {x2, y, x2, y2, x, y2, x, y};
      p->setPen(g.background().dark());
      p->drawConvexPolygon(TQPointArray(4, corners));
      p->setPen(g.background().light());
      p->drawPolyline(TQPointArray(4, corners), 0, 3);
    } else {
      const TQCOORD corners[] = {x, y2, x, y, x2, y, x2, y2};
      p->setPen(g.background().dark());
      p->drawPolygon(TQPointArray(4, corners));
      p->setPen(g.background().light());
      p->drawPolyline(TQPointArray(4, corners), 0, 3);
    }
  } else {
    renderContour(p, r, g.background(), getColor(g, PanelContour));

    if (pseudo3d) {
      if (sunken) {
        p->setPen(getColor(g, PanelDark));
      } else {
        p->setPen(getColor(g, PanelLight));
      }
      p->drawLine(r.left() + 2, r.top() + 1, r.right() - 2, r.top() + 1);
      p->drawLine(r.left() + 1, r.top() + 2, r.left() + 1, r.bottom() - 2);
      if (sunken) {
        p->setPen(getColor(g, PanelLight));
      } else {
        p->setPen(getColor(g, PanelDark));
      }
      p->drawLine(r.left() + 2, r.bottom() - 1, r.right() - 2, r.bottom() - 1);
      p->drawLine(r.right() - 1, r.top() + 2, r.right() - 1, r.bottom() - 2);
    }
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

  if (selected) {
    // is selected

    // the top part of the tab which is nearly the same for all positions
    TQRect Rc; // contour
    if (!bottom) {
      if (isFirst && !cornerWidget && !reverseLayout) {
        Rc = TQRect(r.x(), r.y(), r.width() - 1, r.height() - 3);
      } else if (isFirst && !cornerWidget && reverseLayout) {
        Rc = TQRect(r.x() + 1, r.y(), r.width() - 1, r.height() - 3);
      } else {
        Rc = TQRect(r.x() + 1, r.y(), r.width() - 2, r.height() - 3);
      }
    } else {
      if (isFirst && !cornerWidget && !reverseLayout) {
        Rc = TQRect(r.x(), r.y() + 3, r.width() - 1, r.height() - 3);
      } else if (isFirst && !cornerWidget && reverseLayout) {
        Rc = TQRect(r.x() + 1, r.y() + 3, r.width() - 1, r.height() - 3);
      } else {
        Rc = TQRect(r.x() + 1, r.y() + 3, r.width() - 2, r.height() - 3);
      }
    }
    const TQRect Rs(Rc.x() + 1, bottom ? Rc.y() : Rc.y() + 1, Rc.width() - 2,
                    Rc.height() - 1); // the resulting surface
    // the area where the fake border shoudl appear
    const TQRect Rb(r.x(), bottom ? r.top() : Rc.bottom() + 1, r.width(),
                    r.height() - Rc.height());

    uint contourFlags = Draw_Left | Draw_Right;
    if (!bottom) {
      contourFlags |= Draw_Top | Round_UpperLeft | Round_UpperRight;
    } else {
      contourFlags |= Draw_Bottom | Round_BottomLeft | Round_BottomRight;
    }
    renderContour(p, Rc, g.background(), getColor(g, PanelContour),
                  contourFlags);

    // surface
    if (!bottom) {
      p->setPen(getColor(g, PanelLight));
      p->drawLine(Rs.x() + 1, Rs.y(), Rs.right() - 1, Rs.y());
      renderGradient(p, TQRect(Rs.x(), Rs.y() + 1, 1, Rs.height() - 1),
                     getColor(g, PanelLight), getColor(g, PanelLight2));
      renderGradient(p, TQRect(Rs.right(), Rs.y() + 1, 1, Rs.height() - 1),
                     getColor(g, PanelDark), getColor(g, PanelDark2));
    } else {
      p->setPen(
          alphaBlendColors(g.background(), g.background().dark(160), 100));
      p->drawLine(Rs.x() + 1, Rs.bottom(), Rs.right() - 1, Rs.bottom());
      renderGradient(p, TQRect(Rs.x(), Rs.y(), 1, Rs.height() - 1),
                     getColor(g, PanelLight), getColor(g, PanelLight2));
      renderGradient(p, TQRect(Rs.right(), Rs.y(), 1, Rs.height() - 1),
                     getColor(g, PanelDark), getColor(g, PanelDark2));
    }

    // some "position specific" paintings...
    // draw parts of the inactive tabs around...
    if (!isSingle) {
      p->setPen(
          alphaBlendColors(g.background(), getColor(g, ButtonContour), 50));
      if ((!isFirst && !reverseLayout) || (!isLast && reverseLayout)) {
        p->drawPoint(r.left(),
                     bottom ? (triangular ? r.bottom() - 2 : r.bottom() - 3)
                            : (triangular ? r.top() + 2 : r.top() + 3));
        renderSurface(p,
                      TQRect(r.left(),
                             bottom ? r.top() + 3
                                    : (triangular ? r.top() + 3 : r.top() + 4),
                             1, (triangular ? r.height() - 6 : r.height() - 7)),
                      g.background(), g.button(),
                      getColor(g, MouseOverHighlight), _contrast,
                      Draw_Top | Draw_Bottom | Is_Horizontal);
      }
      if ((!isLast && !reverseLayout) || (!isFirst && reverseLayout)) {
        p->drawPoint(r.right(),
                     bottom ? (triangular ? r.bottom() - 2 : r.bottom() - 3)
                            : (triangular ? r.top() + 2 : r.top() + 3));
        renderSurface(p,
                      TQRect(r.right(),
                             bottom ? r.top() + 3
                                    : (triangular ? r.top() + 3 : r.top() + 4),
                             1, (triangular ? r.height() - 6 : r.height() - 7)),
                      g.background(), g.button(),
                      getColor(g, MouseOverHighlight), _contrast,
                      Draw_Top | Draw_Bottom | Is_Horizontal);
      }
    }
    // left connection from the panel border to the tab. :)
    if (isFirst && !reverseLayout && !cornerWidget) {
      p->setPen(
          alphaBlendColors(g.background(), getColor(g, PanelContour), 50));
      p->drawLine(Rb.x(), Rb.y(), Rb.x(), Rb.bottom());
      p->setPen(getColor(g, PanelLight));
      p->drawLine(Rb.x() + 1, Rb.y(), Rb.x() + 1, Rb.bottom());
    } else if (isFirst && reverseLayout && !cornerWidget) {
      p->setPen(
          alphaBlendColors(g.background(), getColor(g, PanelContour), 50));
      p->drawLine(Rb.right(), Rb.y(), Rb.right(), Rb.bottom());
      p->setPen(getColor(g, PanelDark));
      p->drawLine(Rb.right() - 1, Rb.y(), Rb.right() - 1, Rb.bottom());
    }
    // rounded connections to the panel...
    if (!bottom) {
      // left
      if ((!isFirst && !reverseLayout) || (reverseLayout) ||
          (isFirst && !reverseLayout && cornerWidget)) {
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 50));
        p->drawPoint(Rb.x(), Rb.y());
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 150));
        p->drawPoint(Rb.x(), Rb.y() + 1);
        p->drawPoint(Rb.x() + 1, Rb.y());
      }
      // right
      if ((!reverseLayout) || (!isFirst && reverseLayout) ||
          (isFirst && reverseLayout && cornerWidget)) {
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 50));
        p->drawPoint(Rb.right(), Rb.y());
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 150));
        p->drawPoint(Rb.right(), Rb.y() + 1);
        p->drawPoint(Rb.right() - 1, Rb.y());
      }
    } else {
      // left
      if ((!isFirst && !reverseLayout) || (reverseLayout) ||
          (isFirst && !reverseLayout && cornerWidget)) {
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 50));
        p->drawPoint(Rb.x(), Rb.bottom());
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 150));
        p->drawPoint(Rb.x(), Rb.bottom() - 1);
        p->drawPoint(Rb.x() + 1, Rb.bottom());
      }
      // right
      if ((!reverseLayout) || (!isFirst && reverseLayout) ||
          (isFirst && reverseLayout && cornerWidget)) {
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 50));
        p->drawPoint(Rb.right(), Rb.bottom());
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 150));
        p->drawPoint(Rb.right(), Rb.bottom() - 1);
        p->drawPoint(Rb.right() - 1, Rb.bottom());
      }
    }

  } else {
    // inactive tabs

    // the top part of the tab which is nearly the same for all positions
    TQRect Rc; // contour
    if (isFirst && reverseLayout) {
      Rc =
          TQRect(r.x() + 1,
                 (bottom ? r.y() + 2 : (triangular ? r.y() + 2 : r.y() + 3)),
                 r.width() - 2, (triangular ? r.height() - 4 : r.height() - 5));
    } else {
      Rc =
          TQRect(r.x() + 1,
                 (bottom ? r.y() + 2 : (triangular ? r.y() + 2 : r.y() + 3)),
                 r.width() - 1, (triangular ? r.height() - 4 : r.height() - 5));
    }
    TQRect Rs; // the resulting surface
    if ((isFirst && !reverseLayout) || (isLast && reverseLayout)) {
      Rs = TQRect(Rc.x() + 1, bottom ? Rc.y() : Rc.y() + 1, Rc.width() - 2,
                  Rc.height() - 1);
    } else {
      Rs = TQRect(Rc.x(), bottom ? Rc.y() : Rc.y() + 1, Rc.width() - 1,
                  Rc.height() - 1);
    }
    // the area where the fake border shoudl appear
    const TQRect Rb(r.x(), bottom ? r.y() : Rc.bottom() + 1, r.width(), 2);

    uint contourFlags;
    if (!bottom) {
      if ((isFirst && !reverseLayout) || (isLast && reverseLayout)) {
        contourFlags = Draw_Left | Draw_Right | Draw_Top | Round_UpperLeft;
      } else if ((isLast && !reverseLayout) || (isFirst && reverseLayout)) {
        contourFlags = Draw_Right | Draw_Top | Round_UpperRight;
      } else {
        contourFlags = Draw_Right | Draw_Top;
      }
    } else {
      if ((isFirst && !reverseLayout) || (isLast && reverseLayout)) {
        contourFlags = Draw_Left | Draw_Right | Draw_Bottom | Round_BottomLeft;
      } else if ((isLast && !reverseLayout) || (isFirst && reverseLayout)) {
        contourFlags = Draw_Right | Draw_Bottom | Round_BottomRight;
      } else {
        contourFlags = Draw_Right | Draw_Bottom;
      }
    }
    renderContour(p, Rc, g.background(), getColor(g, ButtonContour),
                  contourFlags);

    uint surfaceFlags = Is_Horizontal;
    if (mouseOver) {
      surfaceFlags |= (bottom ? Highlight_Bottom : Highlight_Top);
      surfaceFlags |= Is_Highlight;
    }
    if ((isFirst && !reverseLayout) || (isLast && reverseLayout)) {
      if (!bottom)
        surfaceFlags |= Draw_Left | Draw_Top | Draw_Bottom | Round_UpperLeft;
      else
        surfaceFlags |= Draw_Left | Draw_Top | Draw_Bottom | Round_BottomLeft;
    } else if ((isLast && !reverseLayout) || (isFirst && reverseLayout)) {
      if (!bottom)
        surfaceFlags |= Draw_Right | Draw_Top | Draw_Bottom | Round_UpperRight;
      else
        surfaceFlags |= Draw_Right | Draw_Top | Draw_Bottom | Round_BottomRight;
    } else {
      surfaceFlags |= Draw_Top | Draw_Bottom;
    }
    renderSurface(p, Rs, g.background(), g.button(),
                  getColor(g, MouseOverHighlight), _contrast, surfaceFlags);

    // some "position specific" paintings...
    // fake parts of the panel border
    if (!bottom) {
      p->setPen(
          alphaBlendColors(g.background(), getColor(g, PanelContour), 50));
      p->drawLine(Rb.x(), Rb.y(),
                  ((isLast && !reverseLayout) ||
                   (isFirst && reverseLayout && cornerWidget))
                      ? Rb.right()
                      : Rb.right() - 1,
                  Rb.y());
      p->setPen(getColor(g, PanelLight));
      p->drawLine(Rb.x(), Rb.y() + 1,
                  ((isLast && !reverseLayout) ||
                   (isFirst && reverseLayout && cornerWidget))
                      ? Rb.right()
                      : Rb.right() - 1,
                  Rb.y() + 1);
    } else {
      p->setPen(
          alphaBlendColors(g.background(), getColor(g, PanelContour), 50));
      p->drawLine(Rb.x(), Rb.bottom(),
                  ((isLast && !reverseLayout) ||
                   (isFirst && reverseLayout && cornerWidget))
                      ? Rb.right()
                      : Rb.right() - 1,
                  Rb.bottom());
      p->setPen(getColor(g, PanelDark));
      p->drawLine(Rb.x(), Rb.bottom() - 1,
                  ((isLast && !reverseLayout) ||
                   (isFirst && reverseLayout && cornerWidget))
                      ? Rb.right()
                      : Rb.right() - 1,
                  Rb.bottom() - 1);
    }
    // fake the panel border edge for tabs which are aligned left-most
    // (i.e. only if there is no widget in the corner of the tabwidget!)
    if (isFirst && !reverseLayout && !cornerWidget)
    // normal layout
    {
      if (!bottom) {
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 50));
        p->drawPoint(Rb.x() + 1, Rb.y() + 1);
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 150));
        p->drawPoint(Rb.x(), Rb.y() + 1);
        p->setPen(g.background());
        p->drawPoint(Rb.x(), Rb.y());
        p->setPen(alphaBlendColors(
            alphaBlendColors(g.background(), getColor(g, ButtonContour), 50),
            getColor(g, PanelContour), 150));
        p->drawPoint(Rb.x() + 1, Rb.y());
      } else {
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 50));
        p->drawPoint(Rb.x() + 1, Rb.bottom() - 1);
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 150));
        p->drawPoint(Rb.x(), Rb.bottom() - 1);
        p->setPen(g.background());
        p->drawPoint(Rb.x(), Rb.bottom());
        p->setPen(alphaBlendColors(
            alphaBlendColors(g.background(), getColor(g, ButtonContour), 50),
            getColor(g, PanelContour), 150));
        p->drawPoint(Rb.x() + 1, Rb.bottom());
      }
    } else if (isFirst && reverseLayout && !cornerWidget)
    // reverse layout
    {
      if (!bottom) {
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 50));
        p->drawPoint(Rb.right() - 1, Rb.y() + 1);
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 150));
        p->drawPoint(Rb.right(), Rb.y() + 1);
        p->setPen(g.background());
        p->drawPoint(Rb.right(), Rb.y());
        p->setPen(alphaBlendColors(
            alphaBlendColors(g.background(), getColor(g, ButtonContour), 50),
            getColor(g, PanelContour), 150));
        p->drawPoint(Rb.right() - 1, Rb.y());
      } else {
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 50));
        p->drawPoint(Rb.right() - 1, Rb.bottom() - 1);
        p->setPen(
            alphaBlendColors(g.background(), getColor(g, PanelContour), 150));
        p->drawPoint(Rb.right(), Rb.bottom() - 1);
        p->setPen(g.background());
        p->drawPoint(Rb.right(), Rb.bottom());
        p->setPen(alphaBlendColors(
            alphaBlendColors(g.background(), getColor(g, ButtonContour), 50),
            getColor(g, PanelContour), 150));
        p->drawPoint(Rb.right() - 1, Rb.bottom());
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

    if (horizontal) {
      int center = r.y() + r.height() / 2;
      renderContour(p, TQRect(r.left(), center - 2, r.width(), 4),
                    cg.background(), cg.background().dark(enabled ? 150 : 130),
                    Draw_Left | Draw_Right | Draw_Top | Draw_Bottom);
    } else {
      int center = r.x() + r.width() / 2;
      renderContour(p, TQRect(center - 2, r.top(), 4, r.height()),
                    cg.background(), cg.background().dark(enabled ? 150 : 130),
                    Draw_Left | Draw_Right | Draw_Top | Draw_Bottom);
    }
    break;
  }

  case KPE_SliderHandle: {
    bool horizontal = ceData.orientation == TQt::Horizontal;
    const bool mouseOver = flags & Style_MouseOver;
    const bool enabled = flags & Style_Enabled;
    const bool pressed = (flags & Style_Active) || (flags & Style_Down);

    // Windows 10 style: Simple rectangle
    // Vertical rectangle for horizontal slider, Horizontal for vertical slider

    TQColor handleColor;
    if (!enabled) {
      handleColor = cg.background().dark(110);
    } else if (mouseOver || pressed) {
      handleColor = getColor(cg, MouseOverHighlight); // Blueish on hover
    } else {
      handleColor = cg.background(); // Window background color as requested
    }

    // Add a border for visibility since it's same color as background
    p->setPen(cg.background().dark(140));
    p->setBrush(handleColor);

    int cx = r.center().x();
    int cy = r.center().y();

    if (horizontal) {
      // Vertical rectangle - wider (12px)
      int w = 12;
      int h = 18;
      if (h > r.height())
        h = r.height();

      p->drawRect(cx - w / 2, cy - h / 2, w, h);
    } else {
      // Horizontal rectangle - wider (12px)
      int w = 18;
      int h = 12;
      if (w > r.width())
        w = r.width();

      p->drawRect(cx - w / 2, cy - h / 2, w, h);
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
  case KPE_ListViewBranch: {
    // Typical Windows style listview branch element (dotted line).

    // Create the dotline pixmaps if not already created
    if (!verticalLine) {
      // make 128*1 and 1*128 bitmaps that can be used for
      // drawing the right sort of lines.
      verticalLine = new TQBitmap(1, 129, true);
      horizontalLine = new TQBitmap(128, 1, true);
      TQPointArray a(64);
      TQPainter p2;
      p2.begin(verticalLine);

      int i;
      for (i = 0; i < 64; i++)
        a.setPoint(i, 0, i * 2 + 1);
      p2.setPen(color1);
      p2.drawPoints(a);
      p2.end();
      TQApplication::flushX();
      verticalLine->setMask(*verticalLine);

      p2.begin(horizontalLine);
      for (i = 0; i < 64; i++)
        a.setPoint(i, i * 2 + 1, 0);
      p2.setPen(color1);
      p2.drawPoints(a);
      p2.end();
      TQApplication::flushX();
      horizontalLine->setMask(*horizontalLine);
    }

    p->setPen(cg.mid());

    if (flags & Style_Horizontal) {
      int point = r.x();
      int other = r.y();
      int end = r.x() + r.width();
      int thickness = r.height();

      while (point < end) {
        int i = 128;
        if (i + point > end)
          i = end - point;
        p->drawPixmap(point, other, *horizontalLine, 0, 0, i, thickness);
        point += i;
      }

    } else {
      int point = r.y();
      int other = r.x();
      int end = r.y() + r.height();
      int thickness = r.width();
      int pixmapoffset = (flags & Style_NoChange) ? 0 : 1; // ### Hackish

      while (point < end) {
        int i = 128;
        if (i + point > end)
          i = end - point;
        p->drawPixmap(other, point, *verticalLine, 0, pixmapoffset, thickness,
                      i);
        point += i;
      }
    }

    break;
  }

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
      TQHeader *header = dynamic_cast<TQHeader *>(p->device());
      if (header) {
        isFirst = header->mapToIndex(header->sectionAt(r.x())) == 0;
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
    renderButton(p, r, cg, (on || down), mouseOver, true, enabled, tdehtmlMode);
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
    // Windows 10 style: light gray slider (slightly darkened)
    // Windows 10 style: slider uses window background color
    const TQColor sliderColor = cg.background();
    const TQColor borderColor =
        cg.background().dark(130); // darkened border for visibility

    // Draw border
    p->setPen(borderColor);
    p->drawRect(r.x(), r.y(), r.width(), r.height());

    // Fill with white - no gradients, no complex surfaces
    if (r.width() > 2 && r.height() > 2) {
      p->fillRect(r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2,
                  sliderColor);
    }

    break;
  }

  case PE_ScrollBarAddPage:
  case PE_ScrollBarSubPage: {
    // Windows 10 style: darker gray track for contrast with white slider
    TQColor trackColor = cg.background().dark(115);
    if (on || down) {
      trackColor = trackColor.dark(105);
    }
    p->fillRect(r, trackColor);
    break;
  }

    // SCROLLBAR BUTTONS
    // -----------------
  case PE_ScrollBarSubLine: {
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
    renderContour(p, r, cg.background(), getColor(cg, ButtonContour),
                  contourFlags);
    renderSurface(
        p, TQRect(r.left() + 1, r.top() + 1, r.width() - 2, r.height() - 2),
        cg.background(), cg.button(), getColor(cg, MouseOverHighlight),
        _contrast + 3, surfaceFlags);

    p->setPen(cg.foreground());
    drawPrimitive((horiz ? PE_ArrowLeft : PE_ArrowUp), p, ceData, elementFlags,
                  r, cg, flags);
    break;
  }

  case PE_ScrollBarAddLine: {
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
    renderContour(p, r, cg.background(), getColor(cg, ButtonContour),
                  contourFlags);
    renderSurface(
        p, TQRect(r.left() + 1, r.top() + 1, r.width() - 2, r.height() - 2),
        cg.background(), cg.button(), getColor(cg, MouseOverHighlight),
        _contrast + 3, surfaceFlags);

    p->setPen(cg.foreground());
    drawPrimitive((horiz ? PE_ArrowRight : PE_ArrowDown), p, ceData,
                  elementFlags, r, cg, flags);
    break;
  }

    // CHECKBOXES
    // ----------
  case PE_Indicator: {
    TQColor contentColor = enabled ? cg.base() : cg.background();

    uint contourFlags = Draw_Left | Draw_Right | Draw_Top | Draw_Bottom;
    if (!enabled) {
      contourFlags |= Is_Disabled;
    }
    renderContour(p, r, cg.background(), getColor(cg, ButtonContour),
                  contourFlags);

    // surface
    uint surfaceFlags =
        Draw_Left | Draw_Right | Draw_Top | Draw_Bottom | Is_Horizontal;
    if (!enabled) {
      surfaceFlags |= Is_Disabled;
    } else if (mouseOver) {
      contentColor =
          alphaBlendColors(contentColor, getColor(cg, MouseOverHighlight), 240);
      surfaceFlags |= Is_Highlight;
      surfaceFlags |=
          Highlight_Left | Highlight_Right | Highlight_Top | Highlight_Bottom;
    }
    renderSurface(
        p, TQRect(r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2),
        cg.background(), contentColor, getColor(cg, MouseOverHighlight),
        enabled ? _contrast + 3 : (_contrast / 2), surfaceFlags);

    drawPrimitive(PE_CheckMark, p, ceData, elementFlags, r, cg, flags);

    break;
  }

  case PE_IndicatorMask: {
    p->fillRect(r, color1);
    break;
  }

    // RADIOBUTTONS
    // ------------
  case PE_ExclusiveIndicator: {
    // Windows 10 flat style radio button
    const int diam = 13;
    int cx = r.x() + (r.width() - diam) / 2;
    int cy = r.y() + (r.height() - diam) / 2;
    if (cx < r.x())
      cx = r.x();
    if (cy < r.y())
      cy = r.y();

    // Background
    p->setPen(TQt::NoPen);
    if (!enabled) {
      p->setBrush(cg.background());
    } else if (mouseOver) {
      p->setBrush(TQt::white); // Hover background
    } else {
      p->setBrush(TQt::white); // Normal background
    }
    p->drawEllipse(cx, cy, diam, diam);

    // Border
    TQColor borderColor;
    if (!enabled) {
      borderColor = cg.background().dark(120);
    } else if (mouseOver) {
      borderColor = TQColor(0x3C, 0x7F, 0xB1); // Win10 Blue-ish border on hover
    } else {
      borderColor = TQColor(51, 51, 51); // Dark gray border
    }
    p->setPen(borderColor);
    p->setBrush(TQt::NoBrush);
    p->drawEllipse(cx, cy, diam, diam);

    // Dot (if checked)
    if (flags & Style_On || flags & Style_Down) {
      int dotSize = 7;
      int dotX = cx + (diam - dotSize) / 2;
      int dotY = cy + (diam - dotSize) / 2;

      TQColor dotColor;
      if (!enabled) {
        dotColor = cg.shadow();
      } else {
        dotColor = TQColor(0, 0, 0); // Black dot
      }
      p->setPen(TQt::NoPen);
      p->setBrush(dotColor);
      p->drawEllipse(dotX, dotY, dotSize, dotSize);
    }

    break;
  }

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
    if (w > h) {
      if (h > 4) {
        int ycenter = r.height() / 2;
        for (int k = 2 * r.width() / 10; k < 8 * r.width() / 10; k += 5) {
          renderDot(p, TQPoint(k, ycenter - 1), color, false, true);
        }
      }
    } else {
      if (w > 4) {
        int xcenter = r.width() / 2;
        for (int k = 2 * r.height() / 10; k < 8 * r.height() / 10; k += 5) {
          renderDot(p, TQPoint(xcenter - 1, k), color, false, true);
        }
      }
    }

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
    if (p->device() && dynamic_cast<TQLineEdit *>(p->device())) {
      TQLineEdit *lineEdit = dynamic_cast<TQLineEdit *>(p->device());
      isReadOnly = lineEdit->isReadOnly();
      isEnabled = lineEdit->isEnabled();
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
    if (p->device() && dynamic_cast<TQPixmap *>(p->device())) {
      contourFlags += Draw_AlphaBlend;
    }

    if (_inputFocusHighlight && hasFocus && !isReadOnly && isEnabled) {
      renderContour(p, r, cg.background(),
                    getColor(cg, FocusHighlight, enabled), contourFlags);
    } else {
      renderContour(p, r, cg.background(), getColor(cg, ButtonContour, enabled),
                    contourFlags);
    }
    const TQColor contentColor = enabled ? cg.base() : cg.background();
    if (_inputFocusHighlight && hasFocus && !isReadOnly && isEnabled) {
      p->setPen(getColor(cg, FocusHighlight).dark(130));
    } else {
      p->setPen(contentColor.dark(130));
    }
    p->drawLine(r.left() + 1, r.top() + 2, r.left() + 1, r.bottom() - 2);
    p->drawLine(r.left() + 2, r.top() + 1, r.right() - 2, r.top() + 1);
    if (_inputFocusHighlight && hasFocus && !isReadOnly && isEnabled) {
      p->setPen(getColor(cg, FocusHighlight).light(130));
    } else {
      p->setPen(contentColor.light(130));
    }
    p->drawLine(r.left() + 2, r.bottom() - 1, r.right() - 2, r.bottom() - 1);
    p->drawLine(r.right() - 1, r.top() + 2, r.right() - 1, r.bottom() - 2);
    break;
  }

  case PE_StatusBarSection: {
    // Windows 10 style: no border/separator on status bar sections
    break;
  }

  case PE_TabBarBase: // Still not sure what this one does
  case PE_PanelTabWidget: {
    renderPanel(p, r, cg, true, sunken);
    break;
  }

  case PE_PanelPopup: {
    renderContour(p, r, cg.background(), cg.background().dark(200),
                  Draw_Left | Draw_Right | Draw_Top | Draw_Bottom);
    break;
  }

    // MENU / TOOLBAR PANEL
    // --------------------
  case PE_PanelMenuBar:
  case PE_PanelDockWindow: {
    // fix for toolbar lag (from Mosfet Liquid)
    TQWidget *w = dynamic_cast<TQWidget *>(p->device());
    if (w && w->backgroundMode() == PaletteButton)
      w->setBackgroundMode(PaletteBackground);
    p->fillRect(r, cg.brush(TQColorGroup::Background));

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

    int counter = 1;

    if (horiz) {
      int center = r.left() + r.width() / 2;
      for (int j = r.top() + 2; j <= r.bottom() - 3; j += 3) {
        if (counter % 2 == 0) {
          renderDot(p, TQPoint(center + 1, j), cg.background(), true, true);
        } else {
          renderDot(p, TQPoint(center - 2, j), cg.background(), true, true);
        }
        counter++;
      }
    } else {
      int center = r.top() + r.height() / 2;
      for (int j = r.left() + 2; j <= r.right() - 3; j += 3) {
        if (counter % 2 == 0) {
          renderDot(p, TQPoint(j, center + 1), cg.background(), true, true);
        } else {
          renderDot(p, TQPoint(j, center - 2), cg.background(), true, true);
        }
        counter++;
      }
    }

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
        p->setPen(getColor(cg, PanelLight));
        p->drawLine(center, r.top() + 3, center, r.bottom() - 3);
      } else {
        int center = r.top() + r.height() / 2;
        p->setPen(getColor(cg, PanelDark));
        p->drawLine(r.x() + 3, center - 1, r.right() - 3, center - 1);
        p->setPen(getColor(cg, PanelLight));
        p->drawLine(r.x() + 3, center, r.right() - 3, center);
      }
    }
    break;
  }

  case PE_CheckMark: {
    // Windows 10 style checkmark using Unicode character
    TQColor checkmarkColor =
        enabled ? getColor(cg, CheckMark) : cg.background().dark(120);
    if (flags & Style_Down) {
      checkmarkColor = checkmarkColor.dark(120);
    }

    if (flags & Style_On) {
      // Draw Unicode checkmark ✓
      p->setPen(checkmarkColor);
      TQFont f = p->font();
      f.setPointSize(r.height() - 2);
      f.setBold(true);
      p->setFont(f);
      p->drawText(r, TQt::AlignCenter, TQString::fromUtf8("✓"));
    } else if (flags & Style_Off) {
      // empty - nothing to draw
    } else {
      // tristate - draw a minus sign
      p->setPen(checkmarkColor);
      TQFont f = p->font();
      f.setPointSize(r.height() - 2);
      f.setBold(true);
      p->setFont(f);
      p->drawText(r, TQt::AlignCenter, TQString::fromUtf8("−"));
    }

    break;
  }

  case PE_MenuItemIndicatorFrame: {
    int x, y, w, h;
    r.rect(&x, &y, &w, &h);
    int checkcol = styleHint(SH_MenuIndicatorColumnWidth, ceData, elementFlags,
                             opt, NULL, NULL);
    TQRect cr = visualRect(TQRect(x + 2, y + 2, checkcol - 1, h - 4), r);
    qDrawShadePanel(p, cr.x(), cr.y(), cr.width(), cr.height(), cg, true, 1,
                    &cg.brush(TQColorGroup::Midlight));
    break;
  }
  case PE_MenuItemIndicatorIconFrame: {
    int x, y, w, h;
    r.rect(&x, &y, &w, &h);
    int checkcol = styleHint(SH_MenuIndicatorColumnWidth, ceData, elementFlags,
                             opt, NULL, NULL);
    TQRect cr = visualRect(TQRect(x + 2, y + 2, checkcol - 1, h - 4), r);
    qDrawShadePanel(p, cr.x(), cr.y(), cr.width(), cr.height(), cg, true, 1,
                    &cg.brush(TQColorGroup::Midlight));
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
    TQPointArray a;

    switch (pe) {
    case PE_SpinWidgetUp:
    case PE_ArrowUp: {
      a.setPoints(7, u_arrow);
      break;
    }
    case PE_SpinWidgetDown:
    case PE_ArrowDown: {
      a.setPoints(7, d_arrow);
      break;
    }
    case PE_ArrowLeft: {
      a.setPoints(7, l_arrow);
      break;
    }
    case PE_ArrowRight: {
      a.setPoints(7, r_arrow);
      break;
    }
    default: {
      if (flags & Style_Up) {
        a.setPoints(7, u_arrow);
      } else {
        a.setPoints(7, d_arrow);
      }
    }
    }

    const TQWMatrix oldMatrix(p->worldMatrix());

    if (flags & Style_Down) {
      p->translate(pixelMetric(PM_ButtonShiftHorizontal, ceData, elementFlags),
                   pixelMetric(PM_ButtonShiftVertical, ceData, elementFlags));
    }

    a.translate((r.x() + r.width() / 2), (r.y() + r.height() / 2));
    // extra-pixel-shift, correcting some visual tics...
    switch (pe) {
    case PE_ArrowLeft:
    case PE_ArrowRight:
      a.translate(0, -1);
      break;
    case PE_SpinWidgetUp:
    case PE_SpinWidgetDown:
      a.translate(+1, 0);
      break;
    default:
      a.translate(0, 0);
    }

    if (p->pen() == TQt::NoPen) {
      if (flags & Style_Enabled) {
        p->setPen(cg.buttonText());
      } else {
        p->setPen(cg.highlightedText());
      }
    }
    p->drawLineSegments(a, 0, 3);
    p->drawPoint(a[6]);

    p->setWorldMatrix(oldMatrix);

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
    default:
      TDEStyle::drawControl(element, p, ceData, elementFlags, r, cg, flags, opt,
                            widget);
    }

    break;
  }

  case CE_PushButton: {
    TQPushButton *button = (TQPushButton *)widget;

    const bool isDefault = enabled && (elementFlags & CEF_IsDefault);

    if (elementFlags & CEF_IsFlat)
      flatMode = true;

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
    bool focused = flags & Style_HasFocus;
    bool down = flags & Style_Down;
    const int text_flags =
        AlignVCenter | AlignHCenter | ShowPrefix | DontClip | SingleLine;

    p->fillRect(r, cg.background());

    if (active && focused) {
      if (down) {
        drawPrimitive(PE_ButtonTool, p, ceData, elementFlags, r, cg,
                      flags | Style_Down, opt);
      } else {
        drawPrimitive(PE_ButtonTool, p, ceData, elementFlags, r, cg, flags,
                      opt);
      }
    }

    p->setPen(cg.foreground());
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
        p->fillRect(r, cg.background().light(105));

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
          p->fillRect(r, cg.background().light(105));
        if (_drawFocusRect)
          p->drawWinFocusRect(r);
      }
    }
    // Draw the transparency pixmap
    else if (!ceData.bgPixmap.isNull())
      p->drawPixmap(r.topLeft(), ceData.bgPixmap, r);
    // Draw a solid background
    else
      p->fillRect(r, cg.background().light(105));
    // Are we a menu item separator?
    if (mi->isSeparator()) {
      p->setPen(cg.mid());
      p->drawLine(r.x() + 5, r.y() + 1, r.right() - 5, r.y() + 1);
      p->setPen(cg.light());
      p->drawLine(r.x() + 5, r.y() + 2, r.right() - 5, r.y() + 2);
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
    p->setPen(enabled ? cg.buttonText() : cg.mid());

    // This color will be used instead of the above if the menu item
    // is active and disabled at the same time. (etched text)
    TQColor discol = cg.mid();

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

        // TQColor draw = cg.text();
        TQColor draw =
            (active && enabled) ? cg.highlightedText() : cg.foreground();
        p->setPen(draw);

        // Does the menu item have a tabstop? (for the accelerator text)
        if (t >= 0) {
          int tabx = reverse ? r.x() + rightBorder + itemHMargin + itemFrame
                             : r.x() + r.width() - tab - rightBorder -
                                   itemHMargin - itemFrame;

          // Draw the right part of the label (accelerator text)
          if (etchtext && !enabled) {
            // Draw etched text if we're inactive and the menu item is disabled
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
        if (etchtext && !enabled) {
          // Etched text again for inactive disabled menu items...
          p->setPen(cg.light());
          p->drawText(xp + offset, r.y() + m + 1, tw, r.height() - 2 * m,
                      text_flags, s, t);
          p->setPen(discol);
        }

        p->drawText(xp, r.y() + m, tw, r.height() - 2 * m, text_flags, s, t);

        p->setPen(cg.text());

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
      TQRect vr = visualRect(TQRect(r.x() + r.width() - 5 - 1 - dim,
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
    p->fillRect(r, cg.background());

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
    // COMBOBOX
    // --------
  case CC_ComboBox: {
    static const unsigned int handleWidth = 15;

    const TQComboBox *cb = dynamic_cast<const TQComboBox *>(widget);
    bool editable = false;
    bool hasFocus = false;
    editable = (elementFlags & CEF_IsEditable);
    hasFocus = (elementFlags & CEF_HasFocus);

    const TQColor buttonColor = enabled ? cg.button() : cg.background();
    const TQColor inputColor =
        enabled ? (editable ? cg.base() : cg.button()) : cg.background();

    uint contourFlags = 0;
    if (tdehtmlWidgets.contains(cb))
      contourFlags |= Draw_AlphaBlend;

    if (_inputFocusHighlight && hasFocus && editable && enabled) {
      TQRect editField = querySubControlMetrics(control, ceData, elementFlags,
                                                SC_ComboBoxEditField,
                                                TQStyleOption::Default, widget);
      TQRect editFrame = r;
      TQRect buttonFrame = r;

      uint editFlags = contourFlags;
      uint buttonFlags = contourFlags;

      // Hightlight only the part of the contour next to the control button
      if (reverseLayout) {
        // querySubControlMetrics doesn't work right for reverse Layout
        int dx = r.right() - editField.right();
        editFrame.setLeft(editFrame.left() + dx);
        buttonFrame.setRight(editFrame.left() - 1);
        editFlags |= Draw_Right | Draw_Top | Draw_Bottom | Round_UpperRight |
                     Round_BottomRight;
        buttonFlags |= Draw_Left | Draw_Top | Draw_Bottom | Round_UpperLeft |
                       Round_BottomLeft;
      } else {
        editFrame.setRight(editField.right());
        buttonFrame.setLeft(editField.right() + 1);

        editFlags |= Draw_Left | Draw_Top | Draw_Bottom | Round_UpperLeft |
                     Round_BottomLeft;
        buttonFlags |= Draw_Right | Draw_Top | Draw_Bottom | Round_UpperRight |
                       Round_BottomRight;
      }
      renderContour(p, editFrame, cg.background(),
                    getColor(cg, FocusHighlight, enabled), editFlags);
      renderContour(p, buttonFrame, cg.background(),
                    getColor(cg, ButtonContour, enabled), buttonFlags);
    } else {
      contourFlags |= Draw_Left | Draw_Right | Draw_Top | Draw_Bottom |
                      Round_UpperLeft | Round_UpperRight | Round_BottomLeft |
                      Round_BottomRight;
      renderContour(p, r, cg.background(), getColor(cg, ButtonContour, enabled),
                    contourFlags);
    }
    // extend the contour: between input and handler...
    p->setPen(alphaBlendColors(cg.background(),
                               getColor(cg, ButtonContour, enabled), 50));
    if (reverseLayout) {
      p->drawLine(r.left() + 1 + handleWidth, r.top() + 1,
                  r.left() + 1 + handleWidth, r.bottom() - 1);
    } else {
      p->drawLine(r.right() - handleWidth - 1, r.top() + 1,
                  r.right() - handleWidth - 1, r.bottom() - 1);
    }

    const TQRect RbuttonSurface(reverseLayout ? r.left() + 1
                                              : r.right() - handleWidth,
                                r.top() + 1, handleWidth, r.height() - 2);
    const TQRect RcontentSurface(
        reverseLayout ? r.left() + 1 + handleWidth + 1 : r.left() + 1,
        r.top() + 1, r.width() - handleWidth - 3, r.height() - 2);

    // handler

    uint surfaceFlags =
        Draw_Left | Draw_Right | Draw_Top | Draw_Bottom | Is_Horizontal;
    if (reverseLayout) {
      surfaceFlags |= Round_UpperLeft | Round_BottomLeft;
    } else {
      surfaceFlags |= Round_UpperRight | Round_BottomRight;
    }

    if (flags & Style_MouseOver) {
      surfaceFlags |= Is_Highlight;
      if (editable)
        surfaceFlags |= Highlight_Left | Highlight_Right;
      surfaceFlags |= Highlight_Top | Highlight_Bottom;
    }
    renderSurface(p, RbuttonSurface, cg.background(), buttonColor,
                  getColor(cg, MouseOverHighlight),
                  enabled ? _contrast + 3 : (_contrast / 2), surfaceFlags);

    if (!editable) {
      surfaceFlags =
          Draw_Left | Draw_Right | Draw_Top | Draw_Bottom | Is_Horizontal;
      if (reverseLayout) {
        surfaceFlags |= Round_UpperRight | Round_BottomRight;
      } else {
        surfaceFlags |= Round_UpperLeft | Round_BottomLeft;
      }

      if (flags & Style_MouseOver) {
        surfaceFlags |= Is_Highlight;
        surfaceFlags |= Highlight_Top | Highlight_Bottom;
      }
      renderSurface(p, RcontentSurface, cg.background(), buttonColor,
                    getColor(cg, MouseOverHighlight),
                    enabled ? _contrast + 3 : (_contrast / 2), surfaceFlags);
      if (hasFocus) {
        drawPrimitive(PE_FocusRect, p, ceData, elementFlags,
                      TQRect(RcontentSurface.x() + 2, RcontentSurface.y() + 2,
                             RcontentSurface.width() - 4,
                             RcontentSurface.height() - 4),
                      cg);
      }
    } else {
      // thin frame around the input area
      if (_inputFocusHighlight && hasFocus && editable && enabled) {
        p->setPen(getColor(cg, FocusHighlight).dark(130));
      } else {
        p->setPen(inputColor.dark(130));
      }
      p->drawLine(RcontentSurface.x(),
                  reverseLayout ? RcontentSurface.y() : RcontentSurface.y() + 1,
                  RcontentSurface.x(),
                  reverseLayout ? RcontentSurface.bottom()
                                : RcontentSurface.bottom() - 1);
      p->drawLine(RcontentSurface.x() + 1, RcontentSurface.y(),
                  reverseLayout ? RcontentSurface.right() - 1
                                : RcontentSurface.right(),
                  RcontentSurface.y());
      if (_inputFocusHighlight && hasFocus && editable && enabled) {
        p->setPen(getColor(cg, FocusHighlight).light(130));
      } else {
        p->setPen(inputColor.light(130));
      }
      p->drawLine(reverseLayout ? RcontentSurface.x() : RcontentSurface.x() + 1,
                  RcontentSurface.bottom(),
                  reverseLayout ? RcontentSurface.right() - 1
                                : RcontentSurface.right(),
                  RcontentSurface.bottom());
      p->drawLine(RcontentSurface.right(), RcontentSurface.top() + 1,
                  RcontentSurface.right(), RcontentSurface.bottom() - 1);

      // input area
      p->fillRect(RcontentSurface.x() + 1, RcontentSurface.y() + 1,
                  RcontentSurface.width() - 2, RcontentSurface.height() - 2,
                  inputColor);
    }

    p->setPen(cg.foreground());
    drawPrimitive(PE_SpinWidgetDown, p, ceData, elementFlags, RbuttonSurface,
                  cg, Style_Default | Style_Enabled | Style_Raised);

    // TQComboBox draws the text using cg.text(), we can override this
    // from here
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

    const TQSpinWidget *sw = dynamic_cast<const TQSpinWidget *>(widget);
    SFlags sflags = flags;
    PrimitiveElement pe;

    bool hasFocus = false;
    if (sw)
      hasFocus = sw->hasFocus();

    const TQColor buttonColor = enabled ? cg.button() : cg.background();
    const TQColor inputColor = enabled ? cg.base() : cg.background();

    // contour
    const bool heightDividable = ((r.height() % 2) == 0);
    if (_inputFocusHighlight && hasFocus && enabled) {
      TQRect editField = querySubControlMetrics(control, ceData, elementFlags,
                                                SC_SpinWidgetEditField,
                                                TQStyleOption::Default, widget);
      TQRect editFrame = r;
      TQRect buttonFrame = r;

      uint editFlags = 0;
      uint buttonFlags = 0;

      // Hightlight only the part of the contour next to the control buttons
      if (reverseLayout) {
        // querySubControlMetrics doesn't work right for reverse Layout
        int dx = r.right() - editField.right();
        editFrame.setLeft(editFrame.left() + dx);
        buttonFrame.setRight(editFrame.left() - 1);
        editFlags |= Draw_Right | Draw_Top | Draw_Bottom | Round_UpperRight |
                     Round_BottomRight;
        buttonFlags |= Draw_Left | Draw_Top | Draw_Bottom | Round_UpperLeft |
                       Round_BottomLeft;
      } else {
        editFrame.setRight(editField.right());
        buttonFrame.setLeft(editField.right() + 1);

        editFlags |= Draw_Left | Draw_Top | Draw_Bottom | Round_UpperLeft |
                     Round_BottomLeft;
        buttonFlags |= Draw_Right | Draw_Top | Draw_Bottom | Round_UpperRight |
                       Round_BottomRight;
      }
      renderContour(p, editFrame, cg.background(), cg.highlight(), editFlags);
      renderContour(p, buttonFrame, cg.background(),
                    getColor(cg, ButtonContour, enabled), buttonFlags);
    } else {
      renderContour(p,
                    querySubControlMetrics(control, ceData, elementFlags,
                                           SC_SpinWidgetFrame,
                                           TQStyleOption::Default, widget),
                    cg.background(), getColor(cg, ButtonContour, enabled));
    }
    p->setPen(alphaBlendColors(cg.background(),
                               getColor(cg, ButtonContour, enabled), 50));
    p->drawLine(reverseLayout ? r.left() + 1 + handleWidth
                              : r.right() - handleWidth - 1,
                r.top() + 1,
                reverseLayout ? r.left() + 1 + handleWidth
                              : r.right() - handleWidth - 1,
                r.bottom() - 1);
    p->drawLine(reverseLayout ? r.left() + 1 : r.right() - handleWidth,
                r.top() + 1 + (r.height() - 2) / 2,
                reverseLayout ? r.left() + handleWidth : r.right() - 1,
                r.top() + 1 + (r.height() - 2) / 2);
    if (heightDividable)
      p->drawLine(reverseLayout ? r.left() + 1 : r.right() - handleWidth,
                  r.top() + 1 + (r.height() - 2) / 2 - 1,
                  reverseLayout ? r.left() + handleWidth : r.right() - 1,
                  r.top() + 1 + (r.height() - 2) / 2 - 1);

    // surface
    TQRect upRect =
        TQRect(reverseLayout ? r.left() + 1 : r.right() - handleWidth,
               r.top() + 1, handleWidth, (r.height() - 2) / 2);
    TQRect downRect =
        TQRect(reverseLayout ? r.left() + 1 : r.right() - handleWidth,
               heightDividable ? r.top() + 1 + ((r.height() - 2) / 2)
                               : r.top() + 1 + ((r.height() - 2) / 2) + 1,
               handleWidth, ((r.height() - 2) / 2));
    if (heightDividable) {
      upRect = TQRect(upRect.left(), upRect.top(), upRect.width(),
                      upRect.height() - 1);
      downRect = TQRect(downRect.left(), downRect.top() + 1, downRect.width(),
                        downRect.height() - 1);
    }

    uint surfaceFlags =
        Draw_Left | Draw_Right | Draw_Top | Draw_Bottom | Is_Horizontal;
    if (reverseLayout) {
      surfaceFlags |= Round_UpperLeft;
    } else {
      surfaceFlags |= Round_UpperRight;
    }
    if (sflags & Style_MouseOver) {
      surfaceFlags |= Is_Highlight;
      surfaceFlags |= Highlight_Top | Highlight_Left | Highlight_Right;
    }
    if (active == SC_SpinWidgetUp)
      surfaceFlags |= Is_Sunken;
    if (!enabled)
      surfaceFlags |= Is_Disabled;
    renderSurface(p, upRect, cg.background(), buttonColor,
                  getColor(cg, MouseOverHighlight), _contrast, surfaceFlags);
    surfaceFlags =
        Draw_Left | Draw_Right | Draw_Top | Draw_Bottom | Is_Horizontal;
    if (reverseLayout) {
      surfaceFlags |= Round_BottomLeft;
    } else {
      surfaceFlags |= Round_BottomRight;
    }
    if (sflags & Style_MouseOver) {
      surfaceFlags |= Is_Highlight;
      surfaceFlags |= Highlight_Bottom | Highlight_Left | Highlight_Right;
    }
    if (active == SC_SpinWidgetDown)
      surfaceFlags |= Is_Sunken;
    if (!enabled)
      surfaceFlags |= Is_Disabled;
    renderSurface(p, downRect, cg.background(), buttonColor,
                  getColor(cg, MouseOverHighlight), _contrast, surfaceFlags);

    // icons...
    sflags = Style_Default | Style_Enabled;
    if (active == SC_SpinWidgetUp) {
      sflags |= Style_On;
      sflags |= Style_Sunken;
    } else
      sflags |= Style_Raised;
    if (sw->buttonSymbols() == TQSpinWidget::PlusMinus)
      pe = PE_SpinWidgetPlus;
    else
      pe = PE_SpinWidgetUp;
    p->setPen(cg.foreground());
    drawPrimitive(pe, p, ceData, elementFlags, upRect, cg, sflags);

    sflags = Style_Default | Style_Enabled;
    if (active == SC_SpinWidgetDown) {
      sflags |= Style_On;
      sflags |= Style_Sunken;
    } else
      sflags |= Style_Raised;
    if (sw->buttonSymbols() == TQSpinWidget::PlusMinus)
      pe = PE_SpinWidgetMinus;
    else
      pe = PE_SpinWidgetDown;
    p->setPen(cg.foreground());
    drawPrimitive(pe, p, ceData, elementFlags, downRect, cg, sflags);

    // thin frame around the input area
    const TQRect Rcontent =
        TQRect(reverseLayout ? r.left() + 1 + handleWidth + 1 : r.left() + 1,
               r.top() + 1, r.width() - 1 - 2 - handleWidth, r.height() - 2);
    if (_inputFocusHighlight && hasFocus && enabled) {
      p->setPen(getColor(cg, FocusHighlight).dark(130));
    } else {
      p->setPen(inputColor.dark(130));
    }
    p->drawLine(Rcontent.left(),
                reverseLayout ? Rcontent.top() : Rcontent.top() + 1,
                Rcontent.left(),
                reverseLayout ? Rcontent.bottom() : Rcontent.bottom() - 1);
    p->drawLine(Rcontent.left() + 1, Rcontent.top(),
                reverseLayout ? Rcontent.right() - 1 : Rcontent.right(),
                Rcontent.top());
    if (_inputFocusHighlight && hasFocus && enabled) {
      p->setPen(getColor(cg, FocusHighlight).light(130));
    } else {
      p->setPen(inputColor.light(130));
    }
    p->drawLine(Rcontent.left() + 1, Rcontent.bottom(), Rcontent.right() - 1,
                Rcontent.bottom());
    p->drawLine(Rcontent.right(), Rcontent.top() + 1, Rcontent.right(),
                reverseLayout ? Rcontent.bottom() - 1 : Rcontent.bottom());

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
    return 11;

    // MENU INDICATOR
    // --------------
  case PM_MenuButtonIndicator:
    return 8;

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
    return 1;

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

    const TQPopupMenu *popup = dynamic_cast<const TQPopupMenu *>(widget);
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

    if (!obj->isWidgetType())
      return false;

    // Track show events for progress bars
    // Animations disabled

    if (!qstrcmp(obj->name(), "tde toolbar widget")) {
      TQWidget *lb = static_cast<TQWidget *>(obj);
      if (lb->backgroundMode() == TQt::PaletteButton)
        lb->setBackgroundMode(TQt::PaletteBackground);
      removeObjectEventHandler(ceData, elementFlags, source, this);
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
    return enabled ? cg.button().dark(130 + _contrast * 8)
                   : cg.background().dark(120 + _contrast * 8);
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
    return cg.background().dark(160 + _contrast * 8);
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
    // Windows 10 very light blue hover color
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
