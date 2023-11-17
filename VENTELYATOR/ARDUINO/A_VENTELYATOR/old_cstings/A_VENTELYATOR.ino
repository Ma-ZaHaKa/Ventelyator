#include <ArduinoJson.h>
#include <EEPROM.h>
#include <Wire.h> 
#include "GyverTM1637.h"
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
#define Seven1CLK 2
#define Seven1DIO 4
#define Seven2CLK 7
#define Seven2DIO 8

#define levellightPin 12
//--------------



//------DATA-------
//#define JSON_BUFFER_SIZE 256
#define JSON_BUFFER_SIZE 128
//#define PROJ_CODE "debug_station"
const char PROJ_CODE[] PROGMEM = "debug_station";
//----vent
byte L_perc = 10; // start val
byte R_perc = 10; // start val

#define L_perc_min 46 //47 from stop
#define R_perc_min 54 //69 from stop //min val to move


/*
int GetInt(int segmentNumber);
int GetInt(char segmentChar);
int CombineSegments(const String& segmentString);
int CombineSegments(const int segments[], size_t seg_sz);
int getDisplayChar(char c);
bool IsModifyedStr(const String& str);
String ModString(const String& str);
byte* GetDisplayByteArray(String str);
//void runningTextDK(const String& str, int _delay);
void runningTextDK(const String& str, int _delay, GyverTM1637* disp);

class TM1637Animation
{
  public:
    TM1637Animation() {}

    void InitAnimation(const String& str, int _delay, GyverTM1637* disp)
    {
        if (message != NULL) { delete[] message; message = NULL; }
        IsAnimationActivated = true;
        String Mstr = ModString(str);
        last_millis = millis();
        message = GetDisplayByteArray(Mstr);
        MSGSZ = Mstr.length();
        delay = _delay;
        display = disp;
    }

    void OnAnimation()
    {
      unsigned long currentMillis = millis();
      if (currentMillis - last_millis >= delay) {
        last_millis = currentMillis;
        display->displayByte(message);
        shiftMessage();
      }
    }

    void StopAnimation()
    {
      IsAnimationActivated = false;
      display->clear();
    }

    void shiftMessage()
    {
      byte temp = message[0];
      for (int i = 1; i < MSGSZ; i++) {
        message[i - 1] = message[i];
      }
      message[MSGSZ - 1] = temp;
    }
    bool GetAnimState() { return IsAnimationActivated; }
  //private:
    unsigned long last_millis = 0;
    GyverTM1637* display = NULL;
    byte* message = NULL; 
    int MSGSZ = 0;
    int delay = 0;
    bool IsAnimationActivated = true;
};*/







//---lcd
//LiquidCrystal_I2C lcd(0x27,16,2);  // Устанавливаем дисплей
LCD_1602_RUS lcd_ru(0x27,16,2); // присваиваем имя LCD для дисплея
GyverTM1637 dispTM1637_0(Seven1CLK, Seven1DIO);
//TM1637Animation dispTM1637_0_Anim;
//bool dispTM1637_0_AFlag = false; // use anim ?
GyverTM1637 dispTM1637_1(Seven2CLK, Seven2DIO);
//TM1637Animation dispTM1637_1_Anim;
//bool dispTM1637_1_AFlag = false; // use anim ?

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
void PrintErrorJson(const String& error_msg)
{
   DynamicJsonDocument responseDoc(JSON_BUFFER_SIZE);
   responseDoc["error"] = error_msg;
   serializeJson(responseDoc, Serial);
   Serial.println();
   responseDoc.clear();
}

void PrintDataJson(const String& message)
{
   DynamicJsonDocument responseDoc(JSON_BUFFER_SIZE);
   responseDoc["data"] = message;
   serializeJson(responseDoc, Serial);
   Serial.println();
   responseDoc.clear();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~7SEGMENT~~~~~~~~~BY~DIKTOR~~~~~~
// Функция, возвращающая значение для включения сегмента по указанному номеру
/*int GetInt(int segmentNumber)
{
    
 //   _a_
 //  f|   |b
 //   |_g_|
 //  e|   |c
 //   |_d_|
  
  // mask B ~ G F E D C B A  // 0 disabled 1 enabled

  // В зависимости от номера сегмента возвращаем соответствующее значение
  switch (segmentNumber) {
    case 1:
      return B00000001; // A
    case 2:
      return B00000010; // B
    case 3:
      return B00000100; // C
    case 4:
      return B00001000; // D
    case 5:
      return B00010000; // E
    case 6:
      return B00100000; // F
    case 7:
      return B01000000; // G
    default:
      return 0; // Возвращаем 0 для выключения всех сегментов
  }
}

int GetInt(char segmentChar)
{
  char lowercaseChar = tolower(segmentChar);
  switch (lowercaseChar) {
    case 'a':
      return B00000001; // A
    case 'b':
      return B00000010; // B
    case 'c':
      return B00000100; // C
    case 'd':
      return B00001000; // D
    case 'e':
      return B00010000; // E
    case 'f':
      return B00100000; // F
    case 'g':
      return B01000000; // G
    default:
      return 0;
  }
}

int CombineSegments(const String& segmentString) // "abcdefg"
{
  int result = 0;
  for (char segmentChar : segmentString) {
    result |= GetInt(segmentChar);
  }
  return result;
}

//size_t seg_sz = sizeof(segments) / sizeof(segments[0]);
int CombineSegments(const int segments[], size_t seg_sz)
{
  int result = 0;
  for (size_t i = 0; i < seg_sz; ++i) {
    result |= GetInt(segments[i]);
  }
  return result;
}

//uint8_t getDisplayChar(char c)
int getDisplayChar(char c)
{
  //   _a_
  // f|   |b
  //  |_g_|
  // e|   |c
  //  |_d_|
  
  // mask B ~ G F E D C B A  // 0 disabled 1 enabled

  // GetInt(char segmentChar)
  // int CombineSegments(String segmentString) // "abcdefg"
  c = tolower(c);
  switch (c)
  {
    case 'a': return CombineSegments("abcefg");
    case 'b': return CombineSegments("cdefg");
    case 'c': return CombineSegments("adef");
    case 'd': return CombineSegments("bcdeg");
    case 'e': return CombineSegments("adefg");
    case 'f': return CombineSegments("aefg");
    case 'g': return CombineSegments("abcdfg");
    case 'h': return CombineSegments("cefg");
    //case 'i': return CombineSegments("c");
    case 'i': return CombineSegments("ef");
    case 'j': return CombineSegments("bcde");
    case 'k': return CombineSegments("bcefg");
    case 'l': return CombineSegments("def");
    case 'm': return CombineSegments("ace");
    case 'n': return CombineSegments("ceg");
    case 'o': return CombineSegments("cdeg");
    case 'p': return CombineSegments("abefg");
    case 'q': return CombineSegments("abcfg");
    case 'r': return CombineSegments("eg");
    case 's': return CombineSegments("acdfg");
    case 't': return CombineSegments("defg");
    case 'u': return CombineSegments("bcdef");
    case 'v': return CombineSegments("cde");
    case 'w': return CombineSegments("bdf");
    case 'x': return CombineSegments("bcefg");
    case 'y': return CombineSegments("bcdfg");
    case 'z': return CombineSegments("abdeg");

    case '0': return CombineSegments("abcdef");
    case '1': return CombineSegments("bc");
    case '2': return CombineSegments("abdeg");
    case '3': return CombineSegments("abcdg");
    case '4': return CombineSegments("bcfg");
    case '5': return CombineSegments("acdfg");
    case '6': return CombineSegments("acdefg");
    case '7': return CombineSegments("abc");
    case '8': return CombineSegments("abcdefg");
    case '9': return CombineSegments("abcdfg");

    case '-': return CombineSegments("g");
    case '_': return CombineSegments("d");
    case '=': return CombineSegments("dg");
    case ' ': return CombineSegments("");
    default: return CombineSegments("");
  }
}

//bool IsModifyedStr(const String& str)
//{
//  if(str.length() > 8)
//  { return ((str[0]==' ') && (str[1]==' ') && (str[2]==' ') && (str[3]==' ') && (str[str.length()-4]==' ') && (str[str.length()-3]==' ') && (str[str.length()-2]==' ') && (str[str.length()-1]==' ')); }
//  return false;
//}
String ModString(const String& str) { return ("    " + str + " " ); }
String GyverModString(const String& str) { return ("    " + str + "   " ); }

byte* GetDisplayByteArray(String str)
{
  if (str == NULL || str.length() == 0) { return NULL; }
  //if(!IsModifyedStr(str)) { str = ModString(str); }
  //str = str.substring(0, min(str.length(), 4));
  //while (str.length() < 4) { str += " "; }

  byte* byteArray = new byte[str.length()];
  for (int i = 0; i < str.length(); i++) { byteArray[i] = getDisplayChar(str[i]); }
  //byte* byteArray = new byte[str.length()+8]; // ____TEXT____
  //for (int i = 0; i < 4; i++) { byteArray[i] = CombineSegments(""); }
  //for (int i = 4; i < str.length(); i++) { byteArray[i] = getDisplayChar(str[i]); }
  //for (int i = str.length(); i < 4; i++) { byteArray[i] = CombineSegments(""); }
  return byteArray; // !! delete[] byteArray;
}
void runningTextDK(const String& str, int _delay, GyverTM1637* disp)
{
  String Mstr = ModString(str);
  byte* bytes = GetDisplayByteArray(Mstr);
  int STRSZ = Mstr.length();
  runningString(bytes, STRSZ, _delay, disp);
  //disp.runningString(bytes, sizeof(bytes), 200);
  delete[] bytes;
}
void runningString(uint8_t DispData[], byte amount, int delayMs, GyverTM1637* disp)
{
    uint8_t segm_data[amount + 8];    // оставляем место для 4х пустых слотов в начале и в конце
    for (byte i = 0; i < 4; i++) {  // делаем первые 4 символа пустыми
        segm_data[i] = 0x00;
    }
    for (byte i = 0; i < amount; i++) {  // далее забиваем тем что на входе (сам текст строки)
        segm_data[i + 4] = DispData[i];
    }
    for (byte i = amount + 4; i < amount + 8; i++) {  // и последние 4 тоже забиваем пустыми символами
        segm_data[i] = 0x00;
    }
    for (byte i = 0; i <= amount + 4; i++) {   // выводим
        disp->displayByte(segm_data[i], segm_data[i + 1], segm_data[i + 2], segm_data[i + 3]);
        delay(delayMs);
    }
}*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~




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
      if (jsonDoc.containsKey("mode"))
      {
        const String mode = jsonDoc["mode"].as<String>();
        //mode.toLowerCase(); // !! const


        //-------------------------------------------------VENT---
        // {"mode":"set_val", "vent":"1", "value":"7"}
        // {"mode":"set_val", "vent":"2", "value":"15"}
        if(mode == "set_val" && jsonDoc.containsKey("vent") && jsonDoc.containsKey("value"))
        {
          const String _vent = jsonDoc["vent"].as<String>();
          const String _value = jsonDoc["value"].as<String>();
          int value = _value.toInt();
          if (IsValidAnalogVal(value))
          {
            int vent = GetVent(_vent);
            if (vent == 1)
            { 
              L_perc = value;
              analogWrite(ventLPin, calculatePWM(value, L_perc_min));
              SetPerc();
              PrintDataJson("OK");
            }
            else if (vent == 2)
            {
              R_perc = value;
              analogWrite(ventRPin, calculatePWM(value, R_perc_min));
              SetPerc();
              PrintDataJson("OK");
            }
            else { PrintErrorJson("ERROR! VENT NOT FOUND!"); }
            //LCDPrecPrint();
          }
          else { PrintErrorJson("ERROR! VALUE NOT VALID!"); }
        }



        else if(mode == "get_val")
        {
          if(jsonDoc.containsKey("vent"))
          {
            int vent = GetVent(jsonDoc["vent"].as<String>());
            if (vent == 1) { PrintDataJson(String(L_perc)); } // {"data":"50"}
            else if (vent == 2) { PrintDataJson(String(R_perc)); }
            else { PrintErrorJson("ERROR! VENT NOT FOUND!"); }
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
        else if(mode == "set_text" && jsonDoc.containsKey("text"))
        {
          const String _text = jsonDoc["text"].as<String>();
          PrintDataJson("OK");
          int row = 1;
          if(jsonDoc.containsKey("row")) { row = jsonDoc["row"].as<String>().toInt(); }
          if(!row) { LCDR0 = _text; } else { LCDR1 = _text; }
          UpdateLCD();
        }

        // {"mode":"set_lcd_bl", "value":"1"}
        else if(mode == "set_lcd_bl" && jsonDoc.containsKey("value"))
        {
          const String _text = jsonDoc["value"].as<String>();
          PrintDataJson("OK");
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
        else if(mode == "clear_lcd") { lcd_ru.clear(); PrintDataJson("OK"); }

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


        //-------------------------------------------------------------------TM1637--// set anim, set text, clear, set brightness, set :, set number[segment]
        // {"mode":"set_anim", "str":"hello  enjoy 1637 display", "disp_num":"0", "delay":"200"}
        else if(mode == "set_anim" && jsonDoc.containsKey("str") && jsonDoc.containsKey("disp_num") && jsonDoc.containsKey("delay"))
        {
          const String str = jsonDoc["str"].as<String>();
          const String _disp_num = jsonDoc["disp_num"].as<String>();
          const String _delay = jsonDoc["delay"].as<String>();

          if(!CanIntParse(_disp_num)) { PrintErrorJson("ERROR! _disp_num can not parse!"); return; }
          if(!CanIntParse(_delay)) { PrintErrorJson("ERROR! _delay can not parse!"); return; }

          int disp_num = _disp_num.toInt();
          int delay = _delay.toInt();

          if(!((disp_num == 0) || (disp_num == 1))) { PrintErrorJson("ERROR! disp_num over diapasone!"); return; }

          // 0 -> L; 1 -> R
          PrintDataJson("OK");
          //if(disp_num == 0) { dispTM1637_0_Anim.InitAnimation(str, delay, &dispTM1637_0); }
          //else if(disp_num == 1) { dispTM1637_1_Anim.InitAnimation(str, delay, &dispTM1637_1); }
        }

        // {"mode":"set_1637text", "str":"hello  enjoy 1637 display", "disp_num":"0"}
        else if(mode == "set_1637text" && jsonDoc.containsKey("str") && jsonDoc.containsKey("disp_num"))
        {
          const String str = jsonDoc["str"].as<String>();
          const String _disp_num = jsonDoc["disp_num"].as<String>();

          if(!CanIntParse(_disp_num)) { PrintErrorJson("ERROR! _disp_num can not parse!"); return; }

          int disp_num = _disp_num.toInt();
          if(!((disp_num == 0) || (disp_num == 1))) { PrintErrorJson("ERROR! disp_num over diapasone!"); return; }

          // 0 -> L; 1 -> R
          PrintDataJson("OK");
          //if(disp_num == 0) { dispTM1637_0_Anim.StopAnimation(); }
          //else if(disp_num == 1) { dispTM1637_1_Anim.StopAnimation(); }
        }
        //-------------------------------------------------------------END--TM1637---



        //---------------------------------------------------------------LEVEL--DATA--
        // {"mode":"set_level", "value":"50"}
        else if(mode == "set_level" && jsonDoc.containsKey("value"))
        {
          const String _value = jsonDoc["value"].as<String>();
          if(!CanIntParse(_value)) { PrintErrorJson("ERROR! _value can not parse!"); return; }
          int value = _value.toInt();
          if(!SetLevel(value)) { PrintErrorJson("ERROR! value diapasone error!"); return; }
          //if(!IsPercentVal(value)) { PrintErrorJson("ERROR! value not im diapasone percent 0-100!"); return; }
          PrintDataJson("OK");
          //analogWrite(levelPin, FixLevelSensor(percentToPwm(value)));
        }

        // {"mode":"set_level_backlight", "value":"true"}
        else if(mode == "set_level_backlight" && jsonDoc.containsKey("value"))
        {
          const String _text = jsonDoc["value"].as<String>();
          PrintDataJson("OK");
          if((_text == "1") || (_text == "true")) {digitalWrite(levellightPin, HIGH); }
          else { digitalWrite(levellightPin, LOW); }
        }
        //----------------------------------------------------------END--LEVEL--DATA--






        //-------------------------------------------------------------------OTHER---
        //else if (jsonDoc["mode"] == "hello") { PrintDataJson(PROJ_CODE); }
        else if (mode == "hello") { PrintDataJson(PROJ_CODE); }
        
        else { PrintErrorJson("ERROR! Cant find mode"); }
      }

      //else if (true) { InfoA(); } // DEBUG
      else { PrintErrorJson("ERROR! Cant find key mode"); }
      
    }
    
    else if (inputString == "hello") { PrintDataJson(PROJ_CODE); }
    else { PrintErrorJson("Error Deserealization");  Serial.println(error.c_str()); }
    
  } // if serial available

  {
    // TICKS ANIMATIONS

    //~~TM1667 ANIMS
    //if(dispTM1637_0_Anim.GetAnimState()) { dispTM1637_0_Anim.OnAnimation(); }
    //if(dispTM1637_1_Anim.GetAnimState()) { dispTM1637_1_Anim.OnAnimation(); }
  }

}
