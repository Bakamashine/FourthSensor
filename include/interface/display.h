#pragma once
#include <stdint.h>

class MenuUI;
class Sensor;

#define DEFAULT_SIZE 24

enum UiFont
{
  FONT_UI,
  FONT_START,
};

class IDisplay
{
private:
  virtual void main () = 0;
  virtual void permanent () = 0;

protected:
  virtual ~IDisplay () = default;

public:
  virtual void clearDisplay () = 0;
  virtual void setFont (UiFont font) = 0;
  virtual void drawStr (int x, int y, const char *str) = 0;
  virtual int getStrWidth (const char *str) = 0;
  virtual int getFontHeight () = 0;
  virtual void initDisplay (int sda = -1, int scl = -1) = 0;
  virtual void initUI () = 0;
  virtual void draw () = 0;
  virtual MenuUI &getMenuUI () = 0;
  virtual void startWindow () = 0;

  virtual IDisplay &setMainTemp (float) = 0;
  virtual IDisplay &setFirstResTemp (float) = 0;
  virtual IDisplay &setSecondResTemp (float) = 0;
  virtual IDisplay &setStreetTemp (float) = 0;
  virtual float getMainTemp () = 0;
  virtual float getFirstResTemp () = 0;
  virtual float getSecondResTemp () = 0;
  virtual float getStreetTemp () = 0;

  virtual IDisplay &setMainAcp (int) = 0;
  virtual IDisplay &setFirstResAcp (int) = 0;
  virtual IDisplay &setSecondResAcp (int) = 0;
  virtual IDisplay &setStreetAcp (int) = 0;
  virtual int getMainAcp () = 0;
  virtual int getFirstResAcp () = 0;
  virtual int getSecondResAcp () = 0;
  virtual int getStreetAcp () = 0;

  virtual IDisplay &setMainSensor (Sensor &) = 0;
  virtual IDisplay &setFirstReserveSensor (Sensor &) = 0;
  virtual IDisplay &setStreetSensor (Sensor &) = 0;
  virtual Sensor &getMainSensor () = 0;
  virtual Sensor &getFirstReserveSensor () = 0;
  virtual IDisplay &setSecondReserveSensor (Sensor &) = 0;
  virtual Sensor &getSecondReserveSensor () = 0;
  virtual Sensor &getStreetSensor () = 0;

  virtual void error () = 0;

  IDisplay &operator= (const IDisplay &) = delete;
};