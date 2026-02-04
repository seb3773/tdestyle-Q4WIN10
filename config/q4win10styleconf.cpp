/*
  Q4WIN10 Style Configuration Dialog
  Simplified - only Menu Popup Delay option
*/

#include "q4win10styleconf.h"

#include <tdeglobal.h>
#include <tdelocale.h>
#include <tqlabel.h>
#include <tqlayout.h>
#include <tqsettings.h>
#include <tqslider.h>

extern "C" {
TDE_EXPORT TQWidget *allocate_tdestyle_config(TQWidget *parent) {
  TDEGlobal::locale()->insertCatalogue("q4win10style");
  return new Q4Win10StyleConfig(parent);
}
}

Q4Win10StyleConfig::Q4Win10StyleConfig(TQWidget *parent) : TQWidget(parent) {
  TQVBoxLayout *layout = new TQVBoxLayout(this, 0, 6);

  // Popup delay slider
  TQHBoxLayout *delayLayout = new TQHBoxLayout(0, 0, 6);
  TQLabel *titleLabel = new TQLabel(tr("Menu popup delay:"), this);
  delayLayout->addWidget(titleLabel);

  popupDelaySlider = new TQSlider(0, 500, 10, 96, TQt::Horizontal, this);
  delayLayout->addWidget(popupDelaySlider);

  popupDelayLabel = new TQLabel("96 ms", this);
  popupDelayLabel->setMinimumWidth(50);
  delayLayout->addWidget(popupDelayLabel);

  layout->addLayout(delayLayout);

  // Spacer
  layout->addStretch(1);

  // Credits
  TQLabel *credits = new TQLabel("<center><b>Q4WIN10 Widget Style</b><br>"
                                 "Based on Plastik by Sandro Giessl</center>",
                                 this);
  layout->addWidget(credits);

  // Load settings
  TQSettings settings;
  settings.beginGroup("/q4win10style/Settings");
  origPopupDelay = settings.readNumEntry("/popupDelay", 96);
  settings.endGroup();

  popupDelaySlider->setValue(origPopupDelay);
  popupDelayLabel->setText(TQString::number(origPopupDelay) + " ms");

  // Connections
  connect(popupDelaySlider, TQ_SIGNAL(valueChanged(int)), this,
          TQ_SLOT(sliderValueChanged(int)));
  connect(popupDelaySlider, TQ_SIGNAL(valueChanged(int)), this,
          TQ_SLOT(updateChanged()));
}

Q4Win10StyleConfig::~Q4Win10StyleConfig() {}

void Q4Win10StyleConfig::sliderValueChanged(int value) {
  popupDelayLabel->setText(TQString::number(value) + " ms");
}

void Q4Win10StyleConfig::save() {
  TQSettings settings;
  settings.beginGroup("/q4win10style/Settings");
  settings.writeEntry("/popupDelay", popupDelaySlider->value());
  settings.endGroup();
}

void Q4Win10StyleConfig::defaults() {
  popupDelaySlider->setValue(96);
  updateChanged();
}

void Q4Win10StyleConfig::updateChanged() {
  bool modified = (popupDelaySlider->value() != origPopupDelay);
  emit changed(modified);
}

#include "q4win10styleconf.moc"
