  case CC_ComboBox: {
    static const unsigned int handleWidth = 15;

    const TQComboBox *cb = dynamic_cast<const TQComboBox *>(widget);
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

    // 3. Render Button Surface (no Draw_ flags to avoid shrinking)
    uint btnSurfaceFlags = Is_Horizontal;
    if (reverseLayout) {
      btnSurfaceFlags |= Round_UpperLeft | Round_BottomLeft;
    } else {
      btnSurfaceFlags |= Round_UpperRight | Round_BottomRight;
    }
    if (flags & Style_MouseOver)
      btnSurfaceFlags |= Is_Highlight;
    // Note: sunken state handled by primitive calls if needed
    renderSurface(p, RbuttonSurface, cg.background(), buttonColor,
                  getColor(cg, MouseOverHighlight),
                  enabled ? _contrast + 3 : (_contrast / 2), btnSurfaceFlags);

    // 4. Render Content Surface
    if (editable) {
      p->fillRect(RcontentSurface, inputColor);
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
                  cg, Style_Default | Style_Enabled | Style_Raised);

    p->setPen(cg.buttonText());
    p->setBackgroundColor(cg.button());
    break;
  }
