#include <ArduinoJson.h>
#include <EEPROM.h>
#include <Wire.h> 
//#include "GyverTM1637.h"
#include "DIKTORTM1637.h"
//#include <LiquidCrystal_I2C.h>
#include <LCD_1602_RUS.h> // 8 СИМВОЛОВ КИРРИЛИЦЫ МАСИМУМ

//------PINS----
//---PWM
#define ventLPin 9
#define ventRPin 10

#define redPin 11
#define greenPin 6
#define bluePin 5

#define levelPin 3


//---DIGITAL
// Define the pins for TM1637 (CLK, DIO)
#define Seven0CLK 7
#define Seven0DIO 8
#define Seven1CLK 2
#define Seven1DIO 4

#define levellightPin 12
//--------------



//------DATA-------
//#define JSON_BUFFER_SIZE 256
#define JSON_BUFFER_SIZE 128
#define PROJ_CODE "debug_station"
//----vent
byte L_perc = 10; // start val
byte R_perc = 10; // start val

#define L_perc_min 46 //47 from stop
#define R_perc_min 54 //69 from stop //min val to move






//---lcd
//LiquidCrystal_I2C lcd(0x27,16,2);  // Устанавливаем дисплей
LCD_1602_RUS lcd_ru(0x27,16,2); // присваиваем имя LCD для дисплея

//GyverTM1637 dispTM1637_0(Seven1CLK, Seven1DIO);
//GyverTM1637 dispTM1637_1(Seven2CLK, Seven2DIO);

DIKTORTM1637 dispTM1637_0(Seven0CLK, Seven0DIO);
DIKTORTM1637 dispTM1637_1(Seven1CLK, Seven1DIO);

String LCDR0 = "";
String LCDR1 = "";
//#define LCDROWCHARSSZ 16
//#define LCDCHARSZ 8
//byte LCDCHARSROW1[LCDROWCHARSSZ][LCDCHARSZ];
//byte LCDCHARSROW2[LCDROWCHARSSZ][LCDCHARSZ];

//byte** LCDCHARSROW1 = NULL;
//byte** LCDCHARSROW2 = NULL;

//---tm1637
/*int sensor_lvl = 0;
bool sensor_blacklight = false;*/
//-----------------
void(* resetFunc) (void) = 0;

bool CanIntParse(const String& str)
{
  //str.trim();
  return (str == String(str.toInt()));
}
bool IsPWMVal(int val) { return ((val >= 0) && (val <= 255)); }
bool IsPercentVal(int val) { return ((val >= 0) && (val <= 100)); }

int pwmToPercent(int pwmValue)
{  
  pwmValue = constrain(pwmValue, 0, 255);
  return map(pwmValue, 0, 255, 0, 100);
}

int percentToPwm(int percentValue)
{
  percentValue = constrain(percentValue, 0, 100);
  return map(percentValue, 0, 100, 0, 255);
}

int FixLevelSensor_(int val)
{
  double amplitude = 0.000294; // 0.0003066
  double izgib = 2.64; // 2.46
  return (amplitude * pow(val, izgib));
}

int LevelPercToPWM(int percent)
{
  int tochka_pereloma = 60;
  int pwmValue = 0;
  percent = constrain(percent, 0, 100);
  if (percent <= tochka_pereloma) { pwmValue = map(percent, 0, tochka_pereloma, 0, 120); }
  else { pwmValue = map(percent, tochka_pereloma, 100, 120, 255); }
  return pwmValue;
}

bool SetLevel(int percent)
{
  if(!IsPercentVal(percent)) { return false; }
  analogWrite(levelPin, LevelPercToPWM(percent));
  //analogWrite(levelPin, percent);
  return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~LCD~~
void DumpArray(byte* array, int sz)
{
  Serial.print("Array content: ");
  
  for (int i = 0; i < sz; i++)
  {
    //Serial.print(array[i], HEX);
    Serial.print(array[i]);
    Serial.print(" ");
  }
  Serial.println();
}
/*void PrintLCD(String text, int row = 0, bool cls = true, int col = 0)
{
  if(cls) { lcd.clear(); }
  lcd.clear();
  lcd.setCursor(col, row);
  lcd.print(text);
}*/

void PrintLCD_RU(const String& text, int row = 0,  bool cls = true, int col = 0)
{
  if(cls) { lcd_ru.clear(); }
  lcd_ru.setCursor(col, row);
  lcd_ru.print(text);
}
void PrintLCD_RUPrintCustomChar(byte _char[], byte pos, byte col, byte row)
{
  //if(cls) { lcd_ru.clear(); }
  lcd_ru.createChar(pos, _char); // Создание символа на ЖК-дисплее
  lcd_ru.setCursor(col, row);
  lcd_ru.write(pos); // Вывести символ на дисплей
}
/*void PrintLCD(const char* text, int row = 0, int col = 0)
{
  lcd.clear();
  lcd.setCursor(col, row);
  lcd.print(text);
}

void PrintLCD_RU(const char* text, int row = 0, int col = 0)
{
  lcd_ru.clear();
  lcd_ru.setCursor(col, row);
  lcd_ru.print(text);
}*/
/*void InitLCDCharsMemory() // максимум 8 символов 0-7 create char
{
  byte empty_char = B00000; // 12
  LCDCHARSROW1 = new byte*[LCDROWCHARSSZ]; // [16]
  for (int i = 0; i < LCDROWCHARSSZ; ++i) { LCDCHARSROW1[i] = new byte[LCDCHARSZ]; } // [8]
  for (int i = 0; i < LCDROWCHARSSZ; ++i) // init (each row char) [16]
  {
    for (int j = 0; j < LCDCHARSZ; ++j) // each byte char row [8]
    {
      LCDCHARSROW1[i][j] = empty_char;
      //LCDCHARSROW1[i][j] = 0;
    }
  }

  LCDCHARSROW2 = new byte*[LCDROWCHARSSZ];
  for (int i = 0; i < LCDROWCHARSSZ; ++i) { LCDCHARSROW2[i] = new byte[LCDCHARSZ]; }
  for (int i = 0; i < LCDROWCHARSSZ; ++i) // init (each row char)
  {
    for (int j = 0; j < LCDCHARSZ; ++j) // each byte char row
    {
      LCDCHARSROW2[i][j] = empty_char;
    }
  }
}

void PrintArrayCharsArray() // all chars
{
  //lcd_ru.clear();
  for (int i = 0; i < LCDROWCHARSSZ; ++i) { lcd_ru.createChar(i, LCDCHARSROW1[i]);  } //DumpArray(LCDCHARSROW1[i], LCDCHARSZ);
  for (int i = 0; i < LCDROWCHARSSZ; ++i) { lcd_ru.createChar(i+LCDROWCHARSSZ, LCDCHARSROW2[i]); } //DumpArray(LCDCHARSROW2[i], LCDCHARSZ);

  // PRINT 1,2 ROW
  for (int i = 0; i < LCDROWCHARSSZ; ++i) { lcd_ru.setCursor(i, 0); lcd_ru.write(i); }
  for (int i = 0; i < LCDROWCHARSSZ; ++i) { lcd_ru.setCursor(i, 1); lcd_ru.write(i+LCDROWCHARSSZ); }    
}

void FreeLCDCharsMemory()
{
  for (int i = 0; i < LCDROWCHARSSZ; ++i) { delete[] LCDCHARSROW1[i]; }
  delete[] LCDCHARSROW1;
  for (int i = 0; i < LCDROWCHARSSZ; ++i) { delete[] LCDCHARSROW2[i]; }
  delete[] LCDCHARSROW2;
}*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~









//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~VENT~~
void SetPerc()
{
  EEPROM.put(0, L_perc);
  EEPROM.put(sizeof(int), R_perc);
}

void GetPerc()
{
  EEPROM.get(0, L_perc);
  EEPROM.get(sizeof(int), R_perc);
}

int calculatePWM(int percentage, int minimumPWM = 0) // 0-15 реакции нет, для этого
{
  if(percentage == 0) { return 0; }
  int maximumPWM = 255;
  percentage = constrain(percentage, 0, 100);
  int pwmValue = map(percentage, 0, 100, minimumPWM, maximumPWM);
  return pwmValue;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~







//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~JSON~~~~~~
const char e_[] = "error";
const char d_[] = "data";
const char v_[] = "value";

void PrintErrorJson(const String& error_msg)
{
   DynamicJsonDocument responseDoc(JSON_BUFFER_SIZE);
   responseDoc[e_] = error_msg;
   serializeJson(responseDoc, Serial);
   Serial.println();
   responseDoc.clear();
}

void PrintOkJson()
{
   DynamicJsonDocument responseDoc(JSON_BUFFER_SIZE);
   responseDoc[d_] = "OK";
   serializeJson(responseDoc, Serial);
   Serial.println();
   responseDoc.clear();
}

void PrintDataJson(const String& message)
{
   DynamicJsonDocument responseDoc(JSON_BUFFER_SIZE);
   responseDoc[d_] = message;
   serializeJson(responseDoc, Serial);
   Serial.println();
   responseDoc.clear();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const char vent_[] = "vent";
const char m_[] = "mode";
//const char _1_[] = "1";
//const char true_[] = "true";
const char t_[] = "text";
const char r_[] = "row";
const char c_[] = "col";
const char bcklght_[] = "backlight";


bool IsValidAnalogVal(int value) { return (value >= 0 && value <= 255); }

int GetVent(String str)
{
   str.trim();
   //str.toLowerCase();
   str.toUpperCase();
   if(str == "L"){ return 1; }
   else if(str == "R"){ return 2; }
   else { return str.toInt(); }
}

void LCDPrecPrint()
{
  LCDR0 = "L:" + String(L_perc) + " R:" + String(R_perc);
  UpdateLCD();
}

void UpdateLCD()
{
  PrintLCD_RU(LCDR0, 0, true);
  PrintLCD_RU(LCDR1, 1, false);
}

void ClearTM(int num)
{
  if(num == 0){  dispTM1637_0.clear(); }
  else if(num == 1){  dispTM1637_1.clear(); }
}

/*void TestCustomChar()
{
  byte simvol1[8] = {  0b00001,  0b0,  0b0,  0b000,  0b00000,  0b00000,  0b00000,  0b00000  };
  byte simvol2[8] = {  0b00010,  0b0,  0b0,  0b000,  0b00000,  0b00000,  0b00000,  0b00000  };
  byte simvol3[8] = {  0b00100,  0b0,  0b0,  0b000,  0b00000,  0b00000,  0b00000,  0b00000  };
  byte simvol4[8] = {  0b01000,  0b0,  0b0,  0b000,  0b00000,  0b00000,  0b00000,  0b00000 };
  byte simvol5[8] = {  0b10000,  0b0,  0b0,  0b000,  0b00000,  0b00000,  0b00000,  0b00000  };
  byte simvol6[8] = {  0b00,  0b00001,  0b0,  0b000,  0b00000,  0b00000,  0b00000,  0b00000  };
  byte simvol7[8] = {  0b000,  0b00010,  0b0,  0b000,  0b00000,  0b00000,  0b00000,  0b00000  };
  byte simvol8[8] = {  0b000,  0b00100,  0b0,  0b000,  0b00000,  0b00000,  0b00000,  0b00000  };
  byte simvol9[8] = {  0b000,  0b01000,  0b0,  0b000,  0b00000,  0b00000,  0b00000,  0b00000  };

  PrintLCD_RUPrintCustomChar(simvol1, 0, 0);
  PrintLCD_RUPrintCustomChar(simvol2, 1, 0);
  PrintLCD_RUPrintCustomChar(simvol3, 2, 0);
  PrintLCD_RUPrintCustomChar(simvol4, 3, 0);
  PrintLCD_RUPrintCustomChar(simvol5, 4, 0);
  PrintLCD_RUPrintCustomChar(simvol6, 5, 0);
  PrintLCD_RUPrintCustomChar(simvol7, 6, 0);
  PrintLCD_RUPrintCustomChar(simvol8, 7, 0);
  PrintLCD_RUPrintCustomChar(simvol9, 8, 0);

  return;
  byte simvol[8] = {
  0b01100,
  0b10010,
  0b10010,
  0b01100,
  0b00000,
  0b00000,
  0b00000,
  0b00000
  };
    byte* myArray = new byte[8];
      myArray[0] = 0b01100;
      myArray[1] = 0b10010;
      myArray[2] = 0b10010;
      myArray[3] = 0b01100;
      myArray[4] = 0b00000;
      myArray[5] = 0b00000;
      myArray[6] = 0b00000;
      myArray[7] = 0b00000;
      
    lcd_ru.createChar(0, myArray); // Создание символа на ЖК-дисплее
    lcd_ru.setCursor(0, 0);
    lcd_ru.write(0); // Вывести символ на дисплей
    delete[] myArray;

    delay(3000);

    byte simvol1[8] = {
    0b11111,
    0b10010,
    0b10010,
    0b01100,
    0b00000,
    0b00000,
    0b00000,
    0b00000
    };

    lcd_ru.createChar(0, simvol1); // Создание символа на ЖК-дисплее
    lcd_ru.setCursor(1, 0);
    lcd_ru.write(0); // Вывести символ на дисплей
}*/

  //char buffer[1000];



//--------------------------------------------------------------------------------------
void setup()
{
 // strcpy_P(buffer, '3');

  Serial.begin(9600);
  //pinMode(LCDBACKLIGHTPIN, OUTPUT);
  //digitalWrite(LCDBACKLIGHTPIN, HIGH); // Turn on backlight
  lcd_ru.init();     
  //lcd_ru.setCursor(0, 0);
  //lcd_ru.clear(); // и так чистый           
  lcd_ru.noBacklight();
  //lcd_ru.backlight();
  //lcd.setBacklight(0); // Выключить подсветку
  
  ClearTM(0);
  ClearTM(1);

  dispTM1637_0.brightness(0);  // яркость 0-7 // 0 L
  dispTM1637_1.brightness(0);  // яркость 0-7 // 1 R

  pinMode(ventLPin, OUTPUT);
  pinMode(ventRPin, OUTPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  pinMode(levelPin, OUTPUT);
  pinMode(levellightPin, OUTPUT);


  GetPerc();
  analogWrite(ventLPin, calculatePWM(L_perc, L_perc_min));
  analogWrite(ventRPin, calculatePWM(R_perc, R_perc_min));
  //LCDPrecPrint();

  //TestCustomChar();
  //InitLCDCharsMemory();
  //PrintArrayCharsArray();
}


void loop() // 1175 54%
{
  if (Serial.available()) // || SA() > 0
  {
    String inputString = Serial.readStringUntil('\n'); //!!!! {}\n

    // Попытка разобрать JSON
    DynamicJsonDocument jsonDoc(JSON_BUFFER_SIZE);
    DeserializationError error = deserializeJson(jsonDoc, inputString);

    if (!error)
    {
      if (jsonDoc.containsKey(m_))
      {
        const String mode = jsonDoc[m_].as<String>();
        //mode.toLowerCase(); // !! const


        //-------------------------------------------------VENT---
        // {"mode":"set_val", "vent":"1", "value":"7"}
        // {"mode":"set_val", "vent":"2", "value":"15"}
        if(mode == "set_val" && jsonDoc.containsKey(vent_) && jsonDoc.containsKey(v_))
        {
          const String _vent = jsonDoc[vent_].as<String>();
          const String _value = jsonDoc[v_].as<String>();
          int value = _value.toInt();
          if (IsValidAnalogVal(value))
          {
            int vent = GetVent(_vent);
            if (vent == 1)
            { 
              L_perc = value;
              analogWrite(ventLPin, calculatePWM(value, L_perc_min));
              SetPerc();
              PrintOkJson();
            }
            else if (vent == 2)
            {
              R_perc = value;
              analogWrite(ventRPin, calculatePWM(value, R_perc_min));
              SetPerc();
              PrintOkJson();
            }
            else { PrintErrorJson("1"); }
            //LCDPrecPrint();
          }
          else { PrintErrorJson("2"); }
        }



        else if(mode == "get_val")
        {
          if(jsonDoc.containsKey(vent_))
          {
            int vent = GetVent(jsonDoc[vent_].as<String>());
            if (vent == 1) { PrintDataJson(String(L_perc)); } // {"data":"50"}
            else if (vent == 2) { PrintDataJson(String(R_perc)); }
            else { PrintErrorJson("3"); }
          }
          else
          {
            DynamicJsonDocument responseDoc(JSON_BUFFER_SIZE);
            responseDoc["1"] = String(L_perc); // {"1":"50", "2": "100"}
            responseDoc["2"] = String(R_perc);
            serializeJson(responseDoc, Serial);
            Serial.println();
            responseDoc.clear();
          }
        }
        //--------------------------------------------------------END---TEXT--


        //--------------------------------------------------------LCD---------
        // {"mode":"set_text", "text":"ABCDабвг", "row":"1"}
        else if(mode == "set_text" && jsonDoc.containsKey(t_))
        {
          const String _text = jsonDoc[t_].as<String>();
          PrintOkJson();
          int row = 1;
          if(jsonDoc.containsKey(r_)) { row = jsonDoc[r_].as<String>().toInt(); }
          if(!row) { LCDR0 = _text; } else { LCDR1 = _text; }
          UpdateLCD();
        }

        // {"mode":"set_lcd_bl", "value":"1"}
        else if(mode == "set_lcd_bl" && jsonDoc.containsKey(v_))
        {
          const String _text = jsonDoc[v_].as<String>();
          PrintOkJson();
          if((_text == "1") || (_text == "true")) { lcd_ru.backlight(); }
          else { lcd_ru.noBacklight(); }
          //lcd_ru.backlight();
          //lcd.noBacklight();
        }


        // {"mode":"get_text"}
        else if(mode == "get_text")
        {
          DynamicJsonDocument responseDoc(JSON_BUFFER_SIZE);
          responseDoc["LCDR0"] = LCDR0;
          responseDoc["LCDR1"] = LCDR1;
          serializeJson(responseDoc, Serial);
          Serial.println();
          responseDoc.clear();
        }


        // {"mode":"clear_lcd"}
        else if(mode == "clear_lcd") { lcd_ru.clear(); PrintOkJson(); }

        // {"mode":"set_customchar","characters":"[[255,129,129,129,129,129,255,0],[0,124,66,66,66,66,124,0],[0,62,97,73,69,63,0,0]]"}
        /*else if(mode == "set_customchar" && jsonDoc.containsKey("characters"))
        {
          JsonArray characters = jsonDoc["characters"];
          int numRows = 8;
          int row = 0;
          if(jsonDoc.containsKey("row")) { row = jsonDoc["row"].as<String>().toInt(); }
          int col = 0;
          if(jsonDoc.containsKey("col")) { row = jsonDoc["col"].as<String>().toInt(); }
          PrintDataJson("OK");

          for (int i = 0; i < characters.size(); i++)
          {
            JsonArray charData = characters[i];
            byte customChar[numRows];
        
            for (int j = 0; j < numRows; j++) {
              customChar[j] = charData[j];
            }
        
            lcd_ru.createChar(i, customChar); // Создание символа на ЖК-дисплее
          }
      
          lcd_ru.setCursor(row, col);
        
          for (int i = 0; i < characters.size(); i++) {
            lcd_ru.write((uint8_t)i); // Вывести символ на дисплей
          }
        }*/
        //------------------------------------------------------------------END---LCD---


        //-------------------------------------------------------------------TM1637--
        // {"mode":"set_run_anim", "str":"hello  enjoy 1637 display", "disp_num":"0", "delay":"200"}
        else if(mode == "set_run_anim" && jsonDoc.containsKey("str") && jsonDoc.containsKey("disp_num") && jsonDoc.containsKey("delay"))
        {
          const String str = jsonDoc["str"].as<String>();
          const String _disp_num = jsonDoc["disp_num"].as<String>();
          const String _delay = jsonDoc["delay"].as<String>();

          if(!CanIntParse(_disp_num)) { PrintErrorJson("4"); return; }
          if(!CanIntParse(_delay)) { PrintErrorJson("5"); return; }

          int disp_num = _disp_num.toInt();
          int delay = _delay.toInt();

          if(!((disp_num == 0) || (disp_num == 1))) { PrintErrorJson("6"); return; }

          // 0 -> L; 1 -> R
          PrintOkJson();
          if(disp_num == 0) { dispTM1637_0.InitRunAnimation(str, delay); }
          else if(disp_num == 1) { dispTM1637_1.InitRunAnimation(str, delay); }
        }

        // {"mode":"sync_run1337"}
        else if(mode == "sync_run1337")
        {
          PrintOkJson();
          if(dispTM1637_0.GetRunAnimState() && dispTM1637_1.GetRunAnimState()) { dispTM1637_0.ResetMessageRunAnimation(); dispTM1637_1.ResetMessageRunAnimation(); }
        }

        // {"mode":"set_1637text", "str":"hello  enjoy 1637 display", "disp_num":"0"}
        else if(mode == "set_1637text" && jsonDoc.containsKey("str") && jsonDoc.containsKey("disp_num"))
        {
          const String str = jsonDoc["str"].as<String>();
          const String _disp_num = jsonDoc["disp_num"].as<String>();

          if(!CanIntParse(_disp_num)) { PrintErrorJson("7"); return; }

          int disp_num = _disp_num.toInt();
          if(!((disp_num == 0) || (disp_num == 1))) { PrintErrorJson("8"); return; }

          // 0 -> L; 1 -> R
          PrintOkJson();
          if(disp_num == 0) { /*dispTM1637_0.StopAnimation();*/ dispTM1637_0.PrintString(str, true); }
          else if(disp_num == 1) { /*dispTM1637_1.StopAnimation();*/ dispTM1637_1.PrintString(str, true); }
        }

        // {"mode":"clear_1637", "disp_num":"0"}
        else if(mode == "clear_1637" && jsonDoc.containsKey("disp_num"))
        {
          const String _disp_num = jsonDoc["disp_num"].as<String>();
          if(!CanIntParse(_disp_num)) { PrintErrorJson("13"); return; }
          int disp_num = _disp_num.toInt();
          if(!((disp_num == 0) || (disp_num == 1))) { PrintErrorJson("14"); return; }

          // 0 -> L; 1 -> R
          PrintOkJson();
          if(disp_num == 0) { dispTM1637_0.StopAnimation(); dispTM1637_0.clear(); }
          else if(disp_num == 1) { dispTM1637_1.StopAnimation(); dispTM1637_1.clear(); }
        }

        // {"mode":"set_dots1637", "value":"1", "disp_num":"0"}
        else if(mode == "set_dots1637" && jsonDoc.containsKey(v_) && jsonDoc.containsKey("disp_num"))
        {
          const String _disp_num = jsonDoc["disp_num"].as<String>();
          const String _text = jsonDoc[v_].as<String>();
          bool flag = false;

          if(!CanIntParse(_disp_num)) { PrintErrorJson("15"); return; }
          int disp_num = _disp_num.toInt();
          if(!((disp_num == 0) || (disp_num == 1))) { PrintErrorJson("16"); return; }

          // 0 -> L; 1 -> R
          PrintOkJson();
          if((_text == "1") || (_text == "true")) { flag = true; }

          if(disp_num == 0) { dispTM1637_0.point(flag); }
          else if(disp_num == 1) { dispTM1637_1.point(flag); }
        }

        // {"mode":"set_anim1637", "value":"1", "disp_num":"-1", "delay":"100"}  // all
        // {"mode":"set_anim1637", "value":"1", "disp_num":"0", "delay":"100"}
        // {"mode":"set_anim1637", "value":"0", "disp_num":"0", "delay":"100", "loops":"3"}
        // 0->load 1->proshivka 2-> dots
        else if(mode == "set_anim1637" && jsonDoc.containsKey(v_) && jsonDoc.containsKey("disp_num") && jsonDoc.containsKey("delay"))
        {
          const String _disp_num = jsonDoc["disp_num"].as<String>();
          const String _value = jsonDoc[v_].as<String>();
          const String _delay = jsonDoc["delay"].as<String>();

          const String _loops = "";
          if(jsonDoc.containsKey("loops")) { _loops = jsonDoc["loops"].as<String>(); }

          if(!CanIntParse(_value)) { PrintErrorJson("15"); return; }
          if(!CanIntParse(_disp_num)) { PrintErrorJson("16"); return; }
          if(!CanIntParse(_delay)) { PrintErrorJson("17"); return; }
          if((_loops != "") && (!CanIntParse(_loops))) { PrintErrorJson("18"); return; }

          int value = _value.toInt();
          int disp_num = _disp_num.toInt();
          int delay = _delay.toInt();
          int loops = _loops.toInt();

          //if(!((disp_num == 0) || (disp_num == 1))) { PrintErrorJson("19"); return; }

          // 0 -> L; 1 -> R
          PrintOkJson();

          if(disp_num == 0)
          {
            dispTM1637_0.StopAnimation();
            if(value == 0) { dispTM1637_0.InitA2Animation(delay, ((_loops != "") ? loops : -1)); } // load (zero) // loops?
            else if(value == 1) { dispTM1637_0.InitA1Animation(delay); } // proshivka
            else if(value == 2) { dispTM1637_0.InitDotsAnimation(delay); } // dots
          }
          else if(disp_num == 1)
          {
            dispTM1637_1.StopAnimation();
            if(value == 0) { dispTM1637_1.InitA2Animation(delay, ((_loops != "") ? loops : -1)); } // load (zero) // loops?
            else if(value == 1) { dispTM1637_1.InitA1Animation(delay); } // proshivka
            else if(value == 2) { dispTM1637_1.InitDotsAnimation(delay); } // dots
          }
          else
          {
            dispTM1637_0.StopAnimation();
            dispTM1637_1.StopAnimation();
            if(value == 0) { dispTM1637_0.InitA2Animation(delay, ((_loops != "") ? loops : -1)); dispTM1637_1.InitA2Animation(delay, ((_loops != "") ? loops : -1)); } // load (zero) // loops?
            else if(value == 1) { dispTM1637_0.InitA1Animation(delay);  dispTM1637_1.InitA1Animation(delay); } // proshivka
            else if(value == 2) { dispTM1637_0.InitDotsAnimation(delay); dispTM1637_1.InitDotsAnimation(delay); } // dots

          }
        }

        // {"mode":"reset_1637anim"}   // all reset
        // {"mode":"reset_1637anim", "disp_num":"0"}
        else if(mode == "reset_1637anim")
        {
          const String _disp_num = "";
          if(jsonDoc.containsKey("disp_num")) { _disp_num = jsonDoc["disp_num"].as<String>(); }
          if((_disp_num != "") && (!CanIntParse(_disp_num))) { PrintErrorJson("13"); return; }
          int disp_num = _disp_num.toInt();
          if((_disp_num != "") && (!((disp_num == 0) || (disp_num == 1)))) { PrintErrorJson("14"); return; }

          // 0 -> L; 1 -> R
          PrintOkJson();

          if(_disp_num == "") { dispTM1637_0.StopAnimation(); dispTM1637_1.StopAnimation(); /*dispTM1637_0.clear();*/ }
          else
          {
            if(disp_num == 0) { dispTM1637_0.StopAnimation(); /*dispTM1637_0.clear();*/ }
            else if(disp_num == 1) { dispTM1637_1.StopAnimation(); /*dispTM1637_1.clear();*/ }
          }
        }

        // {"mode":"set_brig1637", "value":"0", "disp_num":"-1"}   // -1 all 0 1 val 0-7
        else if(mode == "set_brig1637" && jsonDoc.containsKey(v_) && jsonDoc.containsKey("disp_num"))
        {
          const String _disp_num = jsonDoc["disp_num"].as<String>();
          const String _value = jsonDoc[v_].as<String>();


          if(!CanIntParse(_value)) { PrintErrorJson("15"); return; }
          if(!CanIntParse(_disp_num)) { PrintErrorJson("16"); return; }

          int value = _value.toInt();
          int disp_num = _disp_num.toInt();

          if((value < 0) || (value > 7)) { PrintErrorJson("17"); return; }

          // 0 -> L; 1 -> R
          PrintOkJson();

          if(disp_num == 0) { dispTM1637_0.brightness(value); }
          else if(disp_num == 1) { dispTM1637_1.brightness(value); }
          else { dispTM1637_0.brightness(value); dispTM1637_1.brightness(value); }
        }

        // {"mode":"set_segs1637", "value":"abcdef", "disp_num":"0", "disp_seg":"0"}
        else if(mode == "set_segs1637" && jsonDoc.containsKey(v_) && jsonDoc.containsKey("disp_num") && jsonDoc.containsKey("disp_seg"))
        {
          const String _value = jsonDoc[v_].as<String>();
          const String _disp_num = jsonDoc["disp_num"].as<String>();
          const String _disp_seg = jsonDoc["disp_seg"].as<String>();


          if(!CanIntParse(_disp_num)) { PrintErrorJson("16"); return; }
          if(!CanIntParse(_disp_seg)) { PrintErrorJson("17"); return; }

          int disp_num = _disp_num.toInt();
          int disp_seg = _disp_seg.toInt();

          if(!((disp_num == 0) || (disp_num == 1))) { PrintErrorJson("19"); return; }
          if((disp_seg < 0) || (disp_seg > 3)) { PrintErrorJson("17"); return; }
          PrintOkJson();

          if(disp_num == 0) { dispTM1637_0.SetSegments(disp_seg, _value); }
          else if(disp_num == 1) { dispTM1637_1.SetSegments(disp_seg, _value); }
        }

        // {"mode":"set_segchar1637", "value":"a", "disp_num":"0", "disp_seg":"0"}
        else if(mode == "set_segchar1637" && jsonDoc.containsKey(v_) && jsonDoc.containsKey("disp_num") && jsonDoc.containsKey("disp_seg"))
        {
          const String _value = jsonDoc[v_].as<String>();
          const String _disp_num = jsonDoc["disp_num"].as<String>();
          const String _disp_seg = jsonDoc["disp_seg"].as<String>();


          if(!CanIntParse(_disp_num)) { PrintErrorJson("16"); return; }
          if(!CanIntParse(_disp_seg)) { PrintErrorJson("17"); return; }

          int disp_num = _disp_num.toInt();
          int disp_seg = _disp_seg.toInt();

          if((disp_seg < 0) || (disp_seg > 3)) { PrintErrorJson("18"); return; }
          if(_value.length() != 1) { PrintErrorJson("19"); return; }
          if(!((disp_num == 0) || (disp_num == 1))) { PrintErrorJson("20"); return; }
          PrintOkJson();

          if(disp_num == 0) { dispTM1637_0.displayByte(disp_seg, getDisplayChar(_value[0])); }
          else if(disp_num == 1) { dispTM1637_1.displayByte(disp_seg, getDisplayChar(_value[0])); }
        }
        // sync A2 (private flag ???)
        // sync runstr ??
        //-------------------------------------------------------------END--TM1637---







        //---------------------------------------------------------------LEVEL--DATA--
        // {"mode":"set_level", "value":"50", "backlight":"1"}
        else if(mode == "set_level" && jsonDoc.containsKey(v_))
        {
          const String _value = jsonDoc[v_].as<String>();
          const String _backlight_level_mode = "";
          if(!CanIntParse(_value)) { PrintErrorJson("9"); return; }
          int value = _value.toInt();
          if(!SetLevel(value)) { PrintErrorJson("10"); return; }

          if(jsonDoc.containsKey(bcklght_)) { _backlight_level_mode = jsonDoc[bcklght_].as<String>(); }
          if((_backlight_level_mode == "1") || (_backlight_level_mode == "true")) { digitalWrite(levellightPin, HIGH); }
          else if (_backlight_level_mode != "") { digitalWrite(levellightPin, LOW); }

          //if(!IsPercentVal(value)) { PrintErrorJson("ERROR! value not im diapasone percent 0-100!"); return; }
          PrintOkJson();
          //analogWrite(levelPin, FixLevelSensor(percentToPwm(value)));
        }

        // {"mode":"set_level_b", "value":"true"}
        else if(mode == "set_level_b" && jsonDoc.containsKey(v_))
        {
          const String _text = jsonDoc[v_].as<String>();
          PrintOkJson();
          if((_text == "1") || (_text == "true")) {digitalWrite(levellightPin, HIGH); }
          else { digitalWrite(levellightPin, LOW); }
        }
        //----------------------------------------------------------END--LEVEL--DATA--






        //-------------------------------------------------------------------OTHER---
        // {"mode":"set_glob_val", "value":"50", "disp_num":"0", "backlight": "1"}
        else if(mode == "set_glob_val" && jsonDoc.containsKey(v_) && jsonDoc.containsKey("disp_num"))
        {
          const String _value = jsonDoc[v_].as<String>();
          const String _disp_num = jsonDoc["disp_num"].as<String>();
          const String _backlight_level_mode = "";

          if(!CanIntParse(_value)) { PrintErrorJson("17"); return; }
          if(!CanIntParse(_disp_num)) { PrintErrorJson("18"); return; }
          if(jsonDoc.containsKey(bcklght_)) { _backlight_level_mode = jsonDoc[bcklght_].as<String>(); }
          if((_backlight_level_mode == "1") || (_backlight_level_mode == "true")) { digitalWrite(levellightPin, HIGH); }
          else if (_backlight_level_mode != "") { digitalWrite(levellightPin, LOW); }

          int disp_num = _disp_num.toInt();
          int value = _value.toInt();
          if(!((disp_num == 0) || (disp_num == 1))) { PrintErrorJson("19"); return; }
          PrintOkJson();

          //---1637
          if(disp_num == 0) { dispTM1637_0.PrintString(_value, true); }
          else if(disp_num == 1) { dispTM1637_1.PrintString(_value, true); }
          //---LEVEL-SENS
          if(!SetLevel(value)) { PrintErrorJson("20"); return; }
        }




        //else if (jsonDoc["mode"] == "hello") { PrintDataJson(PROJ_CODE); }
        else if (mode == "hello") { PrintDataJson(PROJ_CODE); }
        else if (mode == "reset") { resetFunc(); }
        else { PrintErrorJson("errmode"); }
      }

      //else if (true) { InfoA(); } // DEBUG
      else { PrintErrorJson("errprs"); }
      
    }
    
    else if (inputString == "hello") { PrintDataJson(PROJ_CODE); }
    //else { PrintErrorJson("Error Deserealization"); Serial.println(error.c_str()); }
    else { Serial.println(error.c_str()); }
    
  } // if serial available

  {
    // TICKS ANIMATIONS

    //~~TM1667 ANIMS
    dispTM1637_0.OnAnimation();
    dispTM1637_1.OnAnimation();
  }

}
