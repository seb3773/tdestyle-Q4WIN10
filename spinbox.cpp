  case CC_SpinWidget: {
    static const unsigned int handleWidth = 15;

    const TQSpinWidget *sw = dynamic_cast<const TQSpinWidget *>(widget);
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
      borderColor = cg.background().dark(118);
    }
    renderContour(p, r, cg.background(), borderColor, contourFlags);

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
    if (reverseLayout)
      upFlags |= Round_UpperLeft;
    else
      upFlags |= Round_UpperRight;
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
    if (reverseLayout)
      downFlags |= Round_BottomLeft;
    else
      downFlags |= Round_BottomRight;
    if (active == SC_SpinWidgetDown)
      downFlags |= Is_Sunken;
    else if ((flags & Style_MouseOver) &&
             (active == SC_SpinWidgetDown || active == SC_None))
      downFlags |= Is_Highlight;

    renderSurface(p, downRect, cg.background(), buttonColor,
                  getColor(cg, MouseOverHighlight),
                  enabled ? _contrast + 3 : (_contrast / 2), downFlags);

    // 4. Content
    p->fillRect(RcontentSurface, inputColor);

    // 5. Draw Arrows
    p->setPen(cg.foreground());
    drawPrimitive(PE_SpinWidgetUp, p, ceData, elementFlags, upRect, cg, flags);
    drawPrimitive(PE_SpinWidgetDown, p, ceData, elementFlags, downRect, cg,
                  flags);

    break;
  }
