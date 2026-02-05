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

