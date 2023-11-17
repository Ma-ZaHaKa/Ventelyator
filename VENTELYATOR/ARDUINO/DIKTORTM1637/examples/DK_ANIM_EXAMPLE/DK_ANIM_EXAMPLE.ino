
#define CLK 2
#define DIO 4

#include "DIKTORTM1637.h"
DIKTORTM1637 disp(CLK, DIO);
DIKTORTM1637 disp2(7, 8);

//#include "GyverTM1637.h"
//GyverTM1637 disp(CLK, DIO);



void setup()
{
  Serial.begin(9600);
  disp.clear();
  disp.brightness(0);  // яркость, 0 - 7 (минимум - максимум)

  disp.displayByte(_b, _b, _b, _b);
  delay(3000);
  //disp.ClearSegment(1);
  disp.UpdateSegment(0, 'a');
  delay(3000);
  //disp.ClearSegment(0);
  //delay(3000);
  disp.SetSegments(0, "bcef");
  delay(3000);
  //disp.RemoveSegments(0, "bf");
  //delay(3000);

  disp.clear();
  //disp.InitA2Animation(70, 3);
  //disp.InitA1Animation();
  //disp.InitDotsAnimation(500);
  //disp.InitRunAnimation("123", 400);
  //Serial.println(disp.TTA);
  disp.PrintString("abc");
}

void loop()
{
  disp.OnAnimation();
  //if(!disp.GetA2AnimState()) { delay(10); disp.displayByte(_1, _2, _3, _4); }
  //disp.displayByte(disp.message);
  //disp.displayByte(_empty, _empty, _h, _i);
  //byte a = 0b01110111;
  //disp.displayByte(a, _empty, _empty, _empty);
  //disp.displayByte(getDisplayChar('H'), getDisplayChar('E'), getDisplayChar('L'), getDisplayChar('O'));  // вывести текст
}


/*
#include "DIKTORTM1637.h"
DIKTORTM1637 disp(2, 4);
void setup() {
  disp.clear();        // очистить
  disp.brightness(0);  // яркость 0-7
  //disp.displayByte(_h, _e, _l, _l);  // вывести текст
  disp.displayByte(getDisplayChar('H'), getDisplayChar('E'), getDisplayChar('L'), getDisplayChar('O'));  // вывести текст
  delay(8000);
  disp.displayInt(1234);      // вывести чисел
  
  delay(2000);
  disp.displayClock(16, 37);  // вывести часы и минуты
}
void loop() {
  // мигаем двоеточием дисплея
  disp.point(0);
  delay(500);
  disp.point(1);
  delay(500);
}*/

