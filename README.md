# Q4WIN10 Style for TDE

A flat, modern Windows 10 like inspired widget style for the Trinity Desktop Environment (TDE).
Version 2.0 - Optimized and Refined ;-)

## Features (v2.0)

*   **Windows 10 Aesthetics:**
    *   Flat buttons with hover effects (Blue border/highlight).
    *   Modern scrollbars with flat tracks and sliders using the window background color.
    *   Simple, flat progress bars (Green fill, no animations).
    *   Vector-based Radio Buttons and Checkboxes (replaced legacy bitmaps for scalability and sharp look).
    *   Cleaned up visual noise (removed gradients, shadows, and rounded corners on windows).

*   **Optimized Performance:**
    *   **Light Binary**: ~110KB (style) and ~37KB (config).
    *   Removed unused code: Animations and legacy bitmaps.
    *   Compiled with `-O2 -flto -ffast-math -fmerge-all-constants`.

## Context-Aware Window Borders (X11 Integration)

I implemented an X11 Atom-based communication to allow the window decoration to match the menu bar color (Standard Base Color).

### Implementation Details
The Style Plugin acts as the **Sender**.
- In `polish(TQMenuBar*)`, we calculate the menu height.
- We set an X11 property `_Q4WIN10_MENUBAR_HEIGHT` (CARDINAL 32) on the top-level window.
- In `unPolish`, we clear this property.

### Impact
** Negligible to Null.**

- **Memory**: The Style sets a single integer property (CARDINAL 32) on the X11 window. This consumes only 4 bytes of memory in the X server. It is infinitesimal.
- **CPU**:
    - **Style**: The `XChangeProperty` call happens ONLY during the creation (`polish`) of the menu bar. Cost: 1 X11 round-trip (a few microseconds), once per window.
    - **Decoration**: The `XGetWindowProperty` call happens at each border paint event (`paintEvent`). It is a very fast read-only call, optimized by the local X server (Shared Memory). Crucially, the decoration only repaints if you move, resize, or activate the window. If the window is stable, 0 CPU. Compared to the cost of drawing pixmaps and text, reading an integer is invisible.

1.  **Case 1: Q4WIN10 Style + Other Decoration (e.g., Plastik)**
    - The Style sets the `_Q4WIN10_MENUBAR_HEIGHT` property.
    - The other decoration ignores it.
    - **Result**: Standard behavior, no bugs.

2.  **Case 2: Other Style (e.g., Keramik) + Q4WIN10 Decoration**
    - The Style does not set the property.
    - The Q4WIN10 Decoration reads the property and finds nothing (`return 0`).
    - **Result**: The decoration sees `height = 0` and draws standard grey borders everywhere. This is the intended fallback behavior. The design is slightly less "integrated" but 100% functional and clean.

**Reliability**:
This method ("Custom Atom") is the standard mechanism used by EWMH/NetWM and KWin itself. It is reliable because:
- It is **synchronous** with X11 display.
- It is tied to the physical window (`WId`). No leaks.
- It handles dynamic changes (fonts/hide) via `polish`/`unPolish` cycles.

## Compilation

This style is a plugin for TDE and relies on **TDEBase** headers and build system.

**Requirements:**
*   Trinity Desktop Environment source tree (`tdebase`).
*   TQt3/TDE development headers.
*   CMake.

### Standalone Compilation (Recommended)

This style can be built independently of the full `tdebase` tree.

1.  Navigate to the `q4win10` directory.
2.  Build and install:
    ```bash
    make
    sudo make install
    tdebuildsyscoca
    ```

3.  Restart Trinity or change style in TDE Control Center.

### Integrated Compilation (TDE Core)
1.  Place the `q4win10` directory into `tdebase/kstyles/`.
2.  Edit `tdebase/kstyles/CMakeLists.txt` to include: `add_subdirectory( q4win10 )`.
3.  Build using the standard TDE build procedure.

