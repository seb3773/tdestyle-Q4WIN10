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
    *   **light binary:** ~113KB compiled shared object.
    *   Removed unused code: Animations, legacy bitmaps, configuration dialog logic (settings are now hardcoded for consistency).
    *   Merged source files to reduce compilation units.
    *   Compiled with `-O2`, stripped symbols, and hidden visibility (`-fvisibility=hidden`).

## Compilation

This style is a plugin for TDE and relies on **TDEBase** headers and build system.

**Requirements:**
*   Trinity Desktop Environment source tree (`tdebase`).
*   TQt3/TDE development headers.
*   CMake.

**How to build:**
1.  Download the **tdebase** source code.
2.  Place the `q4win10` directory into `tdebase/kstyles/`.
3.  Edit `tdebase/kstyles/CMakeLists.txt` to include the directory:
    ```cmake
    add_subdirectory( q4win10 )
    ```
4.  Build using the standard TDE build procedure:
    ```bash
    mkdir build && cd build
    cmake ..
    make
    sudo make install
    ```

