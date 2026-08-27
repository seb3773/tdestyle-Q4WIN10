#!/usr/bin/env bash
set -euo pipefail

# ==============================================================================
# Script de mise à jour du Dépôt APT Q4WIN10 Style & GitHub Pages
# ==============================================================================

REPO_DIR="$(cd "$(dirname "$0")" && pwd)"
PAGES_BRANCH="gh-pages"

need_cmd() {
    command -v "$1" >/dev/null 2>&1 || {
        echo "Error: missing required command: $1" >&2
        exit 1
    }
}

need_cmd dpkg-scanpackages
need_cmd apt-ftparchive
need_cmd git

echo "=================================================="
echo " Q4WIN10 Style APT Repository & GitHub Pages Sync"
echo "=================================================="

# Ensure we have deb and qsi packages
DEB_FILES=($(find "$REPO_DIR" -maxdepth 1 -name "tde-win-style-q4win10_*_amd64.deb" | sort -V -r))
if [ ${#DEB_FILES[@]} -eq 0 ]; then
    echo "Error: No tde-win-style-q4win10_*_amd64.deb package found in $REPO_DIR." >&2
    echo "Please run ./create_deb.sh first." >&2
    exit 1
fi

QSI_FILES=($(find "$REPO_DIR" -maxdepth 1 -name "setup_tde-win-style-q4win10_*.qsi" | sort -V -r))

PAGES_DIR=$(mktemp -d -t q4win10-style-gh-pages-XXXXXX)
echo "Staging in temporary directory: $PAGES_DIR"

REMOTE_URL="$(git -C "$REPO_DIR" remote get-url origin)"

# Clone or checkout gh-pages into temp directory
git clone --branch "$PAGES_BRANCH" --single-branch "$REMOTE_URL" "$PAGES_DIR" 2>/dev/null || {
    echo "Creating new orphan gh-pages branch in temp directory..."
    git init "$PAGES_DIR"
    (
        cd "$PAGES_DIR"
        git checkout --orphan "$PAGES_BRANCH"
        git remote add origin "$REMOTE_URL"
    )
}

# Structure pool and dists directories for standard APT repo
POOL_DIR="$PAGES_DIR/pool/main/t/tde-win-style-q4win10"
DISTS_DIR="$PAGES_DIR/dists/stable/main/binary-amd64"
mkdir -p "$POOL_DIR"
mkdir -p "$DISTS_DIR"

# Copy all deb packages into pool
for deb in "${DEB_FILES[@]}"; do
    echo "  -> Added DEB: $(basename "$deb")"
    cp -a "$deb" "$POOL_DIR/"
done

# Copy latest QSI to root of pages
for qsi in "${QSI_FILES[@]}"; do
    echo "  -> Added QSI: $(basename "$qsi")"
    cp -a "$qsi" "$PAGES_DIR/"
done

# Generate Packages & Packages.gz index files
echo "Generating Packages index..."
(
    cd "$PAGES_DIR"
    dpkg-scanpackages --multiversion --arch amd64 pool/main > "$DISTS_DIR/Packages"
    gzip -9 -c "$DISTS_DIR/Packages" > "$DISTS_DIR/Packages.gz"
)

# Generate Release file
echo "Generating Release manifest..."
apt-ftparchive \
  -o APT::FTPArchive::Release::Origin="Q4WIN10" \
  -o APT::FTPArchive::Release::Label="Q4WIN10 Widget Style APT Repository" \
  -o APT::FTPArchive::Release::Suite="stable" \
  -o APT::FTPArchive::Release::Codename="stable" \
  -o APT::FTPArchive::Release::Architectures="amd64" \
  -o APT::FTPArchive::Release::Components="main" \
  -o APT::FTPArchive::Release::Description="APT Repository for Q4WIN10 Widget Style (Trinity Desktop)" \
  release "$PAGES_DIR/dists/stable" > "$PAGES_DIR/dists/stable/Release"

# Copy assets (logo, favicon, screenshots, etc.)
if [ -f "$REPO_DIR/tdewin_big.png" ]; then
    cp -a "$REPO_DIR/tdewin_big.png" "$PAGES_DIR/"
elif [ -f "$REPO_DIR/_ressources/tdewin_big.png" ]; then
    cp -a "$REPO_DIR/_ressources/tdewin_big.png" "$PAGES_DIR/tdewin_big.png"
fi

if [ -f "$REPO_DIR/favicon.png" ]; then
    cp -a "$REPO_DIR/favicon.png" "$PAGES_DIR/"
elif [ -f "$REPO_DIR/tdewin_128_16.png" ]; then
    cp -a "$REPO_DIR/tdewin_128_16.png" "$PAGES_DIR/favicon.png"
fi

if [ -d "$REPO_DIR/screenshots" ]; then
    mkdir -p "$PAGES_DIR/screenshots"
    cp -a "$REPO_DIR/screenshots/"* "$PAGES_DIR/screenshots/" 2>/dev/null || true
fi

# Fetch metadata for web page
LATEST_DEB_NAME="$(basename "${DEB_FILES[0]}")"
LATEST_QSI_NAME="$(basename "${QSI_FILES[0]:-setup_tde-win-style-q4win10_2.0.1.qsi}")"
PKG_VERSION="$(dpkg-deb -f "${DEB_FILES[0]}" Version 2>/dev/null || echo "2.0.1")"
TIMESTAMP="$(date -u +'%Y-%m-%d %H:%M UTC')"

echo "Generating modern index.html web portal..."
cat << 'HTML_EOF' > "$PAGES_DIR/index.html"
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Q4WIN10 Widget Style — APT Repository & Downloads</title>
  <link rel="icon" type="image/png" href="favicon.png">
  <meta name="description" content="Official APT Repository and Download Portal for Q4WIN10 Widget Style for Trinity Desktop Environment (TDE).">
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=Plus+Jakarta+Sans:wght@400;500;600;700;800&family=JetBrains+Mono:wght@400;500&display=swap" rel="stylesheet">
  <style>
    :root {
      --bg: #090d16;
      --card-bg: rgba(20, 27, 45, 0.7);
      --card-border: rgba(255, 255, 255, 0.08);
      --accent: #0078d7;
      --accent-glow: rgba(0, 120, 215, 0.35);
      --accent-light: #38bdf8;
      --text: #f8fafc;
      --text-muted: #94a3b8;
      --code-bg: #0d121f;
      --success: #10b981;
    }

    * { box-sizing: border-box; margin: 0; padding: 0; }

    body {
      font-family: 'Plus Jakarta Sans', -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
      background-color: var(--bg);
      color: var(--text);
      line-height: 1.6;
      padding: 0;
      margin: 0;
      background-image: 
        radial-gradient(circle at 15% 50%, rgba(0, 120, 215, 0.15), transparent 40%),
        radial-gradient(circle at 85% 30%, rgba(56, 189, 248, 0.12), transparent 40%);
      min-height: 100vh;
    }

    .container {
      max-width: 960px;
      margin: 0 auto;
      padding: 40px 20px 80px;
    }

    /* Header */
    header {
      text-align: center;
      margin-bottom: 48px;
    }

    .logo-container {
      display: inline-flex;
      align-items: center;
      justify-content: center;
      margin-bottom: 20px;
      padding: 16px;
      background: rgba(255, 255, 255, 0.03);
      border: 1px solid var(--card-border);
      border-radius: 24px;
      backdrop-filter: blur(10px);
    }

    .logo {
      height: 72px;
      width: auto;
      filter: drop-shadow(0 8px 16px var(--accent-glow));
    }

    h1 {
      font-size: 2.8rem;
      font-weight: 800;
      letter-spacing: -0.03em;
      background: linear-gradient(135deg, #ffffff 30%, #38bdf8 100%);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
      margin-bottom: 12px;
    }

    .tagline {
      font-size: 1.15rem;
      color: var(--text-muted);
      max-width: 650px;
      margin: 0 auto 20px;
    }

    .badges {
      display: flex;
      gap: 10px;
      justify-content: center;
      flex-wrap: wrap;
      margin-bottom: 8px;
    }

    .badge {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      padding: 6px 14px;
      border-radius: 100px;
      font-size: 0.82rem;
      font-weight: 600;
      background: rgba(255, 255, 255, 0.05);
      border: 1px solid var(--card-border);
      color: var(--text-muted);
    }

    .badge-accent {
      background: rgba(0, 120, 215, 0.15);
      border-color: rgba(0, 120, 215, 0.4);
      color: #38bdf8;
    }

    /* Cards */
    .card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      border-radius: 20px;
      padding: 32px;
      margin-bottom: 28px;
      backdrop-filter: blur(16px);
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.2);
    }

    .card h2 {
      font-size: 1.45rem;
      font-weight: 700;
      margin-bottom: 16px;
      display: flex;
      align-items: center;
      gap: 12px;
      color: #fff;
    }

    /* Code Block */
    .code-container {
      position: relative;
      margin: 16px 0 8px;
    }

    pre {
      background: var(--code-bg);
      border: 1px solid var(--card-border);
      padding: 18px 20px;
      border-radius: 12px;
      overflow-x: auto;
      font-family: 'JetBrains Mono', monospace;
      font-size: 0.92rem;
      color: #e2e8f0;
      line-height: 1.5;
    }

    .copy-btn {
      position: absolute;
      top: 10px;
      right: 10px;
      background: rgba(255, 255, 255, 0.1);
      border: 1px solid var(--card-border);
      color: #fff;
      padding: 6px 12px;
      border-radius: 8px;
      font-size: 0.8rem;
      font-weight: 600;
      cursor: pointer;
      transition: all 0.2s;
    }

    .copy-btn:hover {
      background: var(--accent);
      border-color: var(--accent);
    }

    /* Downloads Grid */
    .downloads-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
      gap: 18px;
      margin-top: 20px;
    }

    .download-card {
      background: rgba(255, 255, 255, 0.02);
      border: 1px solid var(--card-border);
      border-radius: 16px;
      padding: 22px;
      display: flex;
      flex-direction: column;
      justify-content: space-between;
      transition: all 0.2s ease;
    }

    .download-card:hover {
      border-color: rgba(56, 189, 248, 0.4);
      transform: translateY(-2px);
    }

    .download-header {
      display: flex;
      align-items: center;
      justify-content: space-between;
      margin-bottom: 12px;
    }

    .download-title {
      font-weight: 700;
      font-size: 1.1rem;
    }

    .download-tag {
      font-size: 0.75rem;
      padding: 3px 8px;
      border-radius: 6px;
      background: rgba(56, 189, 248, 0.15);
      color: #38bdf8;
      font-weight: 600;
    }

    .download-desc {
      font-size: 0.88rem;
      color: var(--text-muted);
      margin-bottom: 20px;
      flex-grow: 1;
    }

    .btn-download {
      display: inline-flex;
      align-items: center;
      justify-content: center;
      gap: 8px;
      background: linear-gradient(135deg, var(--accent) 0%, #005a9e 100%);
      color: #fff;
      text-decoration: none;
      padding: 12px 18px;
      border-radius: 10px;
      font-weight: 600;
      font-size: 0.95rem;
      transition: all 0.2s ease;
      box-shadow: 0 4px 14px var(--accent-glow);
    }

    .btn-download:hover {
      background: linear-gradient(135deg, #1084e3 0%, #006ec4 100%);
      transform: scale(1.02);
    }

    /* Features */
    .features-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(260px, 1fr));
      gap: 20px;
      margin-top: 16px;
    }

    .feature-item {
      padding: 18px;
      background: rgba(255, 255, 255, 0.02);
      border: 1px solid var(--card-border);
      border-radius: 14px;
    }

    .feature-icon {
      font-size: 1.5rem;
      margin-bottom: 8px;
      display: block;
    }

    .feature-title {
      font-weight: 700;
      margin-bottom: 6px;
      color: #fff;
    }

    .feature-text {
      font-size: 0.86rem;
      color: var(--text-muted);
    }

    /* Screenshots */
    .screenshots-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
      gap: 16px;
      margin-top: 16px;
    }

    .screenshot-thumb {
      border-radius: 12px;
      overflow: hidden;
      border: 1px solid var(--card-border);
      background: #000;
      cursor: pointer;
      transition: all 0.2s;
    }

    .screenshot-thumb:hover {
      border-color: #38bdf8;
      transform: scale(1.02);
    }

    .screenshot-thumb img {
      width: 100%;
      height: auto;
      display: block;
    }

    /* Lightbox Modal */
    .modal {
      display: none;
      position: fixed;
      z-index: 1000;
      left: 0;
      top: 0;
      width: 100%;
      height: 100%;
      background: rgba(0, 0, 0, 0.85);
      backdrop-filter: blur(8px);
      align-items: center;
      justify-content: center;
      padding: 20px;
    }

    .modal.active {
      display: flex;
    }

    .modal img {
      max-width: 90%;
      max-height: 85vh;
      border-radius: 12px;
      box-shadow: 0 10px 40px rgba(0, 0, 0, 0.6);
      border: 1px solid rgba(255, 255, 255, 0.1);
    }

    .modal-close {
      position: absolute;
      top: 20px;
      right: 30px;
      color: #fff;
      font-size: 36px;
      font-weight: bold;
      cursor: pointer;
    }

    /* Footer */
    footer {
      text-align: center;
      margin-top: 48px;
      padding-top: 24px;
      border-top: 1px solid var(--card-border);
      color: var(--text-muted);
      font-size: 0.86rem;
    }

    footer a {
      color: #38bdf8;
      text-decoration: none;
    }

    footer a:hover {
      text-decoration: underline;
    }

    .footer-links {
      margin-top: 12px;
      display: flex;
      justify-content: center;
      gap: 16px;
      flex-wrap: wrap;
    }
  </style>
</head>
<body>
  <div class="container">
    
    <!-- Header -->
    <header>
      <div class="logo-container">
        <img src="tdewin_big.png" alt="Q4WIN10 Widget Style Logo" class="logo">
      </div>
      <h1>Q4WIN10 Widget Style</h1>
      <p class="tagline">
        Modern flat Windows 10 &amp; 11 inspired widget style plugin for the Trinity Desktop Environment (TDE / TQt3).
      </p>

      <div class="badges">
        <span class="badge badge-accent">Version __PKG_VERSION__</span>
        <span class="badge">Trinity R14.1.x Universal</span>
        <span class="badge">Debian / Ubuntu / Q4OS</span>
        <span class="badge">x86_64</span>
      </div>
    </header>

    <!-- Method 1: APT Repository -->
    <div class="card">
      <h2>
        <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="#38bdf8" stroke-width="2"><path d="M21 16V8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73l7 4a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16z"/></svg>
        Method 1: Automatic APT Repository Installation
      </h2>
      <p style="color: var(--text-muted); font-size: 0.92rem;">
        Add the official repository to your system to receive automatic updates alongside your system packages:
      </p>

      <div class="code-container">
        <button class="copy-btn" onclick="copyCode('apt-cmd', this)">Copy</button>
        <pre id="apt-cmd">echo "deb [trusted=yes] https://seb3773.github.io/tdestyle-Q4WIN10/ stable main" | sudo tee /etc/apt/sources.list.d/tde-win-style-q4win10.list
sudo apt update
sudo apt install tde-win-style-q4win10</pre>
      </div>

      <p style="color: var(--text-muted); font-size: 0.85rem; margin-top: 10px;">
        * Supports Debian (Bullseye, Bookworm, Trixie), Ubuntu (Jammy, Noble) and Q4OS running Trinity Desktop R14.1.x.
      </p>
    </div>

    <!-- Method 2: Direct Downloads -->
    <div class="card">
      <h2>
        <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="#38bdf8" stroke-width="2"><path d="M4 16v1a3 3 0 003 3h10a3 3 0 003-3v-1m-4-4l-4 4m0 0l-4-4m4 4V4"/></svg>
        Method 2: Direct Package Downloads
      </h2>
      <p style="color: var(--text-muted); font-size: 0.92rem;">
        Prefer offline installation? Download pre-built standalone binaries directly:
      </p>

      <div class="downloads-grid">
        <div class="download-card">
          <div class="download-header">
            <span class="download-title">Q4OS Installer (.qsi)</span>
            <span class="download-tag">Q4OS 1-Click</span>
          </div>
          <p class="download-desc">Graphical one-click installer designed specifically for Q4OS Trinity desktop environment.</p>
          <a href="__LATEST_QSI_NAME__" class="btn-download">
            Download .qsi
          </a>
        </div>

        <div class="download-card">
          <div class="download-header">
            <span class="download-title">Debian Package (.deb)</span>
            <span class="download-tag">Universal x86_64</span>
          </div>
          <p class="download-desc">Standard standalone package for Trinity Desktop / Debian-based systems.</p>
          <a href="pool/main/t/tde-win-style-q4win10/__LATEST_DEB_NAME__" class="btn-download">
            Download .deb
          </a>
        </div>
      </div>
      <p style="color: var(--text-muted); font-size: 0.85rem; margin-top: 16px;">
        * Note: Once installed, open <strong>Trinity Control Center &rarr; Appearance &amp; Themes &rarr; Style</strong> and select <strong>Q4WIN10</strong>.
      </p>
    </div>

    <!-- Key Features -->
    <div class="card">
      <h2>
        <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="#38bdf8" stroke-width="2"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>
        Key Capabilities &amp; Architecture
      </h2>

      <div class="features-grid">
        <div class="feature-item">
          <span class="feature-icon">🪟</span>
          <div class="feature-title">Windows 10 &amp; 11 Modes</div>
          <div class="feature-text">Crisp vector flat design with flat buttons and checkmarks (Win10) or curved tabs and rounded widgets (Win11).</div>
        </div>

        <div class="feature-item">
          <span class="feature-icon">🌙</span>
          <div class="feature-title">Native Dark Mode</div>
          <div class="feature-text">High-contrast text and indicator adjustments tailored for dark desktop themes.</div>
        </div>

        <div class="feature-item">
          <span class="feature-icon">🎨</span>
          <div class="feature-title">Seamless Menubar Matching</div>
          <div class="feature-text">Seamless X11 Atom integration (<code>_Q4WIN10_MENUBAR_HEIGHT</code>) with static caching for window decorations.</div>
        </div>

        <div class="feature-item">
          <span class="feature-icon">⚙️</span>
          <div class="feature-title">Control Center Module</div>
          <div class="feature-text">Interactive style configuration in Trinity Control Center with embedded logo and live setting toggles.</div>
        </div>

        <div class="feature-item">
          <span class="feature-icon">⚡</span>
          <div class="feature-title">High-Performance Engine</div>
          <div class="feature-text">Fast bitwise arithmetic (<code>div255</code>), zero RTTI in hot paths, zero heap caches: ~136 KB plugin, ~43 KB deb.</div>
        </div>

        <div class="feature-item">
          <span class="feature-icon">🚀</span>
          <div class="feature-title">Universal Compatibility</div>
          <div class="feature-text">Single standalone binary fully compatible across all Trinity Desktop R14.1.x releases.</div>
        </div>
      </div>
    </div>

    <!-- Screenshots -->
    <div class="card">
      <h2>
        <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="#38bdf8" stroke-width="2"><rect x="3" y="3" width="18" height="18" rx="2" ry="2"/><circle cx="8.5" cy="8.5" r="1.5"/><polyline points="21 15 16 10 5 21"/></svg>
        Preview
      </h2>
      <div class="screenshots-grid">
        <div class="screenshot-thumb" onclick="openModal('screenshots/ALL.png')">
          <img src="screenshots/ALL.png" alt="Q4WIN10 Widget Style Overview">
        </div>
      </div>
    </div>

    <!-- Footer -->
    <footer>
      <p>Source Code &amp; Releases: <a href="https://github.com/seb3773/tdestyle-Q4WIN10" target="_blank" rel="noopener">github.com/seb3773/tdestyle-Q4WIN10</a></p>
      <p style="margin-top: 6px;">Developed with ❤️ for the Trinity Desktop Environment &amp; Q4OS community.</p>
      <p class="footer-links">
        <a href="http://trinitydesktop.org/" target="_blank" rel="noopener">http://trinitydesktop.org/</a> &bull; 
        <a href="https://www.q4os.org/" target="_blank" rel="noopener">https://www.q4os.org/</a> &bull; 
        <a href="https://www.q4os.org/forum/index.php" target="_blank" rel="noopener">https://www.q4os.org/forum/index.php</a>
      </p>
    </footer>

  </div>

  <!-- Lightbox Modal -->
  <div id="imageModal" class="modal" onclick="closeModal()">
    <span class="modal-close">&times;</span>
    <img id="modalImg" src="" alt="Enlarged screenshot" onclick="event.stopPropagation()">
  </div>

  <script>
    function copyCode(id, btn) {
      const text = document.getElementById(id).innerText;
      navigator.clipboard.writeText(text).then(() => {
        const orig = btn.innerText;
        btn.innerText = "Copied!";
        setTimeout(() => btn.innerText = orig, 2000);
      });
    }

    function openModal(src) {
      document.getElementById('modalImg').src = src;
      document.getElementById('imageModal').classList.add('active');
    }

    function closeModal() {
      document.getElementById('imageModal').classList.remove('active');
    }

    document.addEventListener('keydown', (e) => {
      if (e.key === 'Escape') closeModal();
    });
  </script>
</body>
</html>
HTML_EOF

# Replace placeholders
sed -i "s/__PKG_VERSION__/${PKG_VERSION}/g" "$PAGES_DIR/index.html"
sed -i "s/__LATEST_DEB_NAME__/${LATEST_DEB_NAME}/g" "$PAGES_DIR/index.html"
sed -i "s/__LATEST_QSI_NAME__/${LATEST_QSI_NAME}/g" "$PAGES_DIR/index.html"

# Also save a copy of index.html in main branch repo
cp -a "$PAGES_DIR/index.html" "$REPO_DIR/index.html"

# Git commit and push to gh-pages
echo "Committing and pushing to gh-pages branch..."
(
    cd "$PAGES_DIR"
    git add -A
    git commit -m "Update APT repository and download portal: $(date +'%Y-%m-%d %H:%M:%S')" || echo "No changes to commit."
    git push origin "$PAGES_BRANCH"
)

echo "Cleaning up temporary directory..."
rm -rf "$PAGES_DIR"

echo "=================================================="
echo " SUCCESS: APT repository updated on gh-pages!"
echo " URL: https://seb3773.github.io/tdestyle-Q4WIN10/"
echo "=================================================="
