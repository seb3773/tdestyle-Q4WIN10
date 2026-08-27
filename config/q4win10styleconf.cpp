/*
  Q4WIN10 Style Configuration Dialog
*/

#include "q4win10styleconf.h"

#include <tdeglobal.h>
#include <tdelocale.h>
#include <tdeconfig.h>
#include <tqlabel.h>
#include <tqlayout.h>
#include <tqcheckbox.h>
#include <tqgroupbox.h>
#include <tqimage.h>
#include <tqpixmap.h>
#include <tqfont.h>
#include <tqwhatsthis.h>

#include "q4win10style_version.h"
#include "q4win10_logo.h"

extern "C" {
TQ_EXPORT TQWidget *allocate_tdestyle_config(TQWidget *parent) {
  TDEGlobal::locale()->insertCatalogue("q4win10style");
  return new Q4Win10StyleConfig(parent);
}

TQ_EXPORT TQWidget *allocate_kstyle_config(TQWidget *parent) {
  TDEGlobal::locale()->insertCatalogue("q4win10style");
  return new Q4Win10StyleConfig(parent);
}
}

Q4Win10StyleConfig::Q4Win10StyleConfig(TQWidget *parent) : TQWidget(parent) {
  TQVBoxLayout *layout = new TQVBoxLayout(this, 11, 6);

  // Title: Bold & larger
  TQLabel *labelTitle = new TQLabel("Q4WIN10", this);
  TQFont titleFont = labelTitle->font();
  titleFont.setBold(true);
  if (titleFont.pointSize() > 0) {
    titleFont.setPointSize(titleFont.pointSize() + 2);
  } else if (titleFont.pixelSize() > 0) {
    titleFont.setPixelSize(titleFont.pixelSize() + 3);
  }
  labelTitle->setFont(titleFont);
  labelTitle->setAlignment(TQt::AlignVCenter | TQt::AlignHCenter);
  layout->addWidget(labelTitle);

  // Logo (smooth-scaled to 96x56 at runtime)
  TQLabel *labelLogo = new TQLabel(this);
  labelLogo->setAlignment(TQt::AlignVCenter | TQt::AlignHCenter);
  TQPixmap logoPixmap;
  if (logoPixmap.loadFromData(q4win10_logo_png, q4win10_logo_png_len, "PNG")) {
    TQImage img = logoPixmap.convertToImage().smoothScale(96, 56);
    logoPixmap.convertFromImage(img);
    labelLogo->setPixmap(logoPixmap);
  }
  layout->addWidget(labelLogo);

  // Description & Credits
  TQLabel *labelDesc = new TQLabel(tr("A windows10 like widget style for Trinity Desktop"), this);
  labelDesc->setAlignment(TQt::AlignVCenter | TQt::AlignHCenter);
  layout->addWidget(labelDesc);

  TQLabel *labelAuthor = new TQLabel(tr("based on Plastik - by Seb3773"), this);
  labelAuthor->setAlignment(TQt::AlignVCenter | TQt::AlignHCenter);
  layout->addWidget(labelAuthor);

  TQLabel *labelVersion = new TQLabel(Q4WIN10STYLE_VERSION_STRING, this);
  labelVersion->setAlignment(TQt::AlignVCenter | TQt::AlignHCenter);
  layout->addWidget(labelVersion);

  // Spacer
  layout->addSpacing(10);

  // Options
  darkMode = new TQCheckBox(tr("Dark Mode"), this);
  TQWhatsThis::add(darkMode, tr("Check this option for dark desktop themes."));
  layout->addWidget(darkMode);
  
  win11Mode = new TQCheckBox(tr("Windows 11 Mode"), this);
  TQWhatsThis::add(win11Mode, tr("Check this option to enable Windows 11 rounded design aesthetics."));
  layout->addWidget(win11Mode);

  layout->addStretch(1);

  // Load settings via TDEConfig (standard user configuration in ~/.trinity/share/config/q4win10stylerc)
  TDEConfig config("q4win10stylerc");
  config.setGroup("Settings");
  origDarkMode = config.readBoolEntry("darkMode", false);
  origWin11Mode = config.readBoolEntry("win11Mode", false);

  darkMode->setChecked(origDarkMode);
  win11Mode->setChecked(origWin11Mode);

  // Connections
  connect(darkMode, TQT_SIGNAL(toggled(bool)), this, TQT_SLOT(updateChanged()));
  connect(win11Mode, TQT_SIGNAL(toggled(bool)), this, TQT_SLOT(updateChanged()));
}

Q4Win10StyleConfig::~Q4Win10StyleConfig() {}

void Q4Win10StyleConfig::sliderValueChanged(int) {
  // Unused
}

void Q4Win10StyleConfig::save() {
  TDEConfig config("q4win10stylerc");
  config.setGroup("Settings");
  config.writeEntry("darkMode", darkMode->isChecked());
  config.writeEntry("win11Mode", win11Mode->isChecked());
  config.sync();
}

void Q4Win10StyleConfig::defaults() {
  darkMode->setChecked(false);
  win11Mode->setChecked(false);
  updateChanged();
}

void Q4Win10StyleConfig::updateChanged() {
  bool modified = (darkMode->isChecked() != origDarkMode) || 
                  (win11Mode->isChecked() != origWin11Mode);
  emit changed(modified);
}

#include "q4win10styleconf.moc"
