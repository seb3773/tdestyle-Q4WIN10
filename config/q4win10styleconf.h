/*
  Q4WIN10 Style Configuration Dialog
  Simplified - only Menu Popup Delay option
*/

#ifndef Q4WIN10STYLE_CONF_H
#define Q4WIN10STYLE_CONF_H

#include <tqwidget.h>

class TQSlider;
class TQLabel;

class Q4Win10StyleConfig : public TQWidget {
  TQ_OBJECT
public:
  Q4Win10StyleConfig(TQWidget *parent);
  ~Q4Win10StyleConfig();

signals:
  void changed(bool);

public slots:
  void save();
  void defaults();

protected slots:
  void updateChanged();
  void sliderValueChanged(int value);

protected:
  TQSlider *popupDelaySlider;
  TQLabel *popupDelayLabel;

  int origPopupDelay;
};

#endif
