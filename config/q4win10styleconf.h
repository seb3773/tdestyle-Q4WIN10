/*
  Q4WIN10 Style Configuration Dialog
  Simplified - only Menu Popup Delay option
*/

#ifndef Q4WIN10STYLE_CONF_H
#define Q4WIN10STYLE_CONF_H

#include <tqwidget.h>

class TQCheckBox;

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
protected:
  TQCheckBox *darkMode;
  TQCheckBox *win11Mode;

  bool origDarkMode;
  bool origWin11Mode;
};

#endif
