#ifndef _DIKTORTM1637_h
#define _DIKTORTM1637_h
#include <inttypes.h>
#include <Arduino.h>
class DIKTORTM1637 {
public:
    
    DIKTORTM1637(uint8_t clk, uint8_t dio);									// объявление и инициализация
    
    /*void display(uint8_t DispData[]);										// выводит цифры массивом по ячейкам. От 0 до 9 (byte values[] = {3, 5, 9, 0}; )
    void display(uint8_t BitAddr, uint8_t DispData);							// выводит цифру DispData в указанную ячейку дисплея BitAddr
    void display(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3);*/	// если лень создавать массив, выводит цифры в ячейки
    
    void displayByte(uint8_t DispData[]);									// выводит байт вида 0xe6 и буквы-константы вида _a , _b .... массивом
    void displayByte(uint8_t BitAddr, uint8_t DispData);						// выводит байт вида 0xe6 и буквы-константы вида _a , _b .... в ячейку
    void displayByte(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3);	// если лень создавать массив, выводит байты в ячейки
    
    /*void displayClock(uint8_t hrs, uint8_t mins);							// выводит часы и минуты
    void displayClockScroll(uint8_t hrs, uint8_t mins, int delayms);		// выводит часы и минуты с эффектом прокрутки
    void displayClockTwist(uint8_t hrs, uint8_t mins, int delayms);			// выводит часы и минуты с эффектом скрутки
    
    void displayInt(int value);*/												// выводит число от -999 до 9999 (да, со знаком минус)
    
    //void runningString(uint8_t DispData[], byte amount, int delayMs);  		// бегущая строка (array, sizeof(array), задержка в мс)
    
    void clear(void);														// очистить дисплей
    
    void point(boolean PointFlag, bool upd = true);											// вкл / выкл точку (POINT_ON / POINT_OFF)

    void brightness(uint8_t bright, uint8_t = 0x40, uint8_t = 0xc0);		// яркость 0 - 7	
    
    /*void scroll(uint8_t BitAddr, uint8_t DispData, int delayms);				// обновить значение прокруткой (адрес, ЦИФРА, задержка в мс)
    void scroll(uint8_t DispData[], int delayms);							// обновить значение прокруткой (массив ЦИФР, задержка в мс)
    void scroll(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3, int delayms);	// прокрутка посимвольно
    void scrollByte(uint8_t BitAddr, uint8_t DispData, int delayms);			// обновить значение прокруткой (адрес, БАЙТ, задержка в мс)
    void scrollByte(uint8_t DispData[], int delayms);						// обновить значение прокруткой (массив БАЙТ, задержка в мс)
    void scrollByte(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3, int delayms);	// прокрутка посимвольно
    
    void twist(uint8_t BitAddr, uint8_t DispData, int delayms);				// обновить значение скручиванием (адрес, ЦИФРА, задержка в мс)
    void twist(uint8_t DispData[], int delayms);								// обновить значение скручиванием (массив ЦИФР, задержка в мс)
    void twist(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3, int delayms);	// скрутка посимвольно
    void twistByte(uint8_t BitAddr, uint8_t DispData, int delayms);			// обновить значение скручиванием (адрес, БАЙТ, задержка в мс)
    void twistByte(uint8_t DispData[], int delayms);							// обновить значение скручиванием (массив БАЙТ, задержка в мс)
    void twistByte(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3, int delayms);*/	// скрутка посимвольно

    void OnAnimation(); // loop tick
    void StopAnimation();
    void PrintString(const String& str, bool stop_all_anim = false);

    //----------CUTOM--ANIM1 PROSHIVKA
    // 
    //  -- --
    // _  _
    void InitA1Animation(int _delay = 100);
    void OnA1Animation(); // loop tick
    bool GetA1AnimState();
    void StopA1Animation();

    //----------CUSTOM--ANIM2--ZAGRUZKA
    void InitA2Animation(int _delay, int _loops = -1);
    void OnA2Animation(); // loop tick
    bool GetA2AnimState();
    void StopA2Animation();

    //--------RUNSTR
    //void InitRunAnimation(const String& str, int _delay, int loops = -1);
    void DIKTORTM1637::InitRunAnimation(const String& str, int _delay);
    void OnRunAnimation(); // loop tick
    void StopRunAnimation();
    void ResetMessageRunAnimation();
    bool GetRunAnimState();
    bool shiftRunMessage();


    //----------DOTS
    void InitDotsAnimation(int _delay);
    void OnDotsAnimation(); // loop tick
    bool GetDotsAnimState();
    void StopDotsAnimation();

    //------------SEGMENTS
    void SetSegments(byte num_seg, const String& sevenseg_ag); // перезапишет ячейку
    void RemoveSegments(byte num_seg, const String& sevenseg_ag);
    void UpdateSegment(byte num_seg, char sevenseg_ag); // добавит сегмент к ячейке
    void ClearSegment(byte num_seg); // очистит сегмент
    //bool CheckSeg();
    


    uint8_t lastData[4];
private:
    void update();
    int  writeByte(int8_t wr_data);
    void start(void);
    void stop(void);
    
    void sendByte(uint8_t BitAddr, int8_t sendData);
    void sendArray(uint8_t sendData[]);

    uint8_t Cmd_SetData;
    uint8_t Cmd_SetAddr;
    uint8_t Cmd_DispCtrl;
    uint8_t PointData;

    uint8_t Clkpin;
    uint8_t Datapin;

    //---



    //---CUSTOM-ANIM1
    unsigned long last_millis_A1 = 0;
    bool flag_A1 = false;
    bool AnimA1Mode = false;
    int delay_time_A1 = 0; // default 100

    //---CUSTOM-ANIM2 загрузка нулей
    unsigned long last_millis_A2 = 0;
    byte anim_A2_ModeCounter = 0;
    byte anim_A2_ByteChar = 0; // last
    int anim_A2_loops_counter = 0;
    byte anim_A2_loops = -1; // крутить бесконечно анимку
    bool AnimA2Mode = false;
    int delay_time_A2 = 0;


    //---ANIM БЕГУЩАЯ СТРОКА
    unsigned long last_millis_Run = 0;
    byte* message_run = NULL;
    byte* message_run_orig = NULL;
    int MSGSZ_run = 0;
    //int Run_loops = 0;
    //int Run_loops_counter = 0;
    bool AnimRunMode = false; // бегущая строка
    int delay_time_run = 0;

    //----DOTS
    unsigned long last_millis_Dots = 0;
    bool flag_Dots = false;
    bool AnimDotsMode = false;
    int delay_time_dots = 0;

};

//void swapBytes(byte* newByte, byte oldByte, byte oldP, byte newP);
//uint8_t digToHEX(uint8_t digit);		// вернёт код цифры для displayByte
int getDisplayChar(char c);
String ModString(const String& str);
String GyverModString(const String& str);
byte* GetDisplayByteArray(String str);
int GetInt(char segmentChar);

int CombineSegments(const String& segmentString);
int CombineSegments(int combinedValue, const String& segmentString);
int CombineSegment(int combinedValue, char segment);
bool IsSegmentPresents(int combinedValue, const String& segmentString);
bool IsSegmentPresent(int combinedValue, char segmentChar);
int _RemoveSegments(int combinedValue, const String& segmentString);
int RemoveSegment(int combinedValue, char segmentChar);


//************definitions for TM1637*********************
#define ADDR_AUTO  0x40
#define ADDR_FIXED 0x44
#define STARTADDR  0xc0

/**** definitions for the clock point of the digit tube *******/
#define POINT_ON   1
#define POINT_OFF  0

/**************definitions for brightness***********************/
#define  BRIGHT_DARKEST 0
#define  BRIGHT_TYPICAL 2
#define  BRIGHTEST      7

/************** БУКВЫ И СИМВОЛЫ *****************/
//---ORIG
/*
#ifndef AVR
#define _A 0x77
#define _B 0x7f
#define _C 0x39
#define _D 0x3f
#define _E 0x79
#define _F 0x71
#define _G 0x3d
#define _H 0x76
#define _J 0x1e
#define _L 0x38
#define _N 0x37
#define _O 0x3f
#define _P 0x73
#define _S 0x6d
#define _U 0x3e
#define _Y 0x6e
#define _a 0x5f
#define _b 0x7c
#define _c 0x58
#define _d 0x5e
#define _e 0x7b
#define _f 0x71
#define _h 0x74
#define _i 0x10
#define _j 0x0e
#define _l 0x06
#define _n 0x54
#define _o 0x5c
#define _q 0x67
#define _r 0x50
#define _t 0x78
#define _u 0x1c
#define _y 0x6e
#define _dash 0x40
#define _under 0x08
#define _equal 0x48
#define _empty 0x00
#define _degree 0x63

#define _0 0x3f
#define _1 0x06
#define _2 0x5b
#define _3 0x4f
#define _4 0x66
#define _5 0x6d
#define _6 0x7d
#define _7 0x07
#define _8 0x7f
#define _9 0x6f

#else

enum TM1637_letters {
    _A = 0x77,
    _B = 0x7f,
    _C = 0x39,
    _D = 0x3f,
    _E = 0x79,
    _F = 0x71,
    _G = 0x3d,
    _H = 0x76,
    _J = 0x1e,
    _L = 0x38,
    _N = 0x37,
    _O = 0x3f,
    _P = 0x73,
    _S = 0x6d,
    _U = 0x3e,
    _Y = 0x6e,
    _a = 0x5f,
    _b = 0x7c,
    _c = 0x58,
    _d = 0x5e,
    _e = 0x7b,
    _f = 0x71,
    _h = 0x74,
    _i = 0x10,
    _j = 0x0e,
    _l = 0x06,
    _n = 0x54,
    _o = 0x5c,
    _q = 0x67,
    _r = 0x50,
    _t = 0x78,
    _u = 0x1c,
    _y = 0x6e,
    _dash = 0x40,
    _under = 0x08,
    _equal = 0x48,
    _empty = 0x00,
    _degree = 0x63,

    _0 = 0x3f,
    _1 = 0x06,
    _2 = 0x5b,
    _3 = 0x4f,
    _4 = 0x66,
    _5 = 0x6d,
    _6 = 0x7d,
    _7 = 0x07,
    _8 = 0x7f,
    _9 = 0x6f,
};
#endif

*/




//-----------CUSTOM
#ifndef AVR
#define _A 0x77
#define _B 0x7f
#define _C 0x39
#define _D 0x3f
#define _E 0x79
#define _F 0x71
#define _G 0x3d
#define _H 0x76
#define _J 0x1e
#define _L 0x38
#define _N 0x37
#define _O 0x3f
#define _P 0x73
#define _S 0x6d
#define _U 0x3e
#define _Y 0x6e
#define _a 0x5f
#define _b 0x7c
#define _c 0x58
#define _d 0x5e
#define _e 0x7b
#define _f 0x71
#define _h 0x74
#define _i 0x10
#define _j 0x0e
#define _l 0x06
#define _n 0x54
#define _o 0x5c
#define _q 0x67
#define _r 0x50
#define _t 0x78
#define _u 0x1c
#define _y 0x6e
#define _dash 0x40
#define _under 0x08
#define _equal 0x48
#define _empty 0x00
#define _degree 0x63

#define _0 0x3f
#define _1 0x06
#define _2 0x5b
#define _3 0x4f
#define _4 0x66
#define _5 0x6d
#define _6 0x7d
#define _7 0x07
#define _8 0x7f
#define _9 0x6f

#else

#ifndef GYVVER
enum TM1637_letters {
    //    _a_
    //  f|      |b
    //   |_g_|
    // e|      |c
    //   |_d_|

    // mask B ~ G F E D C B A  // 0 disabled 1 enabled



    _a = 0b01110111,
    _b = 0b01111100,
    _c = 0b00111001,
    _d = 0b01011110,
    _e = 0b01111001,
    _f = 0b01110001,
    _g = 0b01101111,
    _h = 0b01110100,
    //case 'i': return CombineSegments("c");
    //_i = 0b00000100,
    _i = 0b00110000,
    _j = 0b00011110,
    _k = 0b01110110,
    _l = 0b00111000,
    _m = 0b00010101,
    _n = 0b01010100,
    _o = 0b01011100,
    _p = 0b01110011,
    _q = 0b01100111,
    _r = 0b01010000,
    _s = 0b01101101,
    _t = 0b01111000,
    _u = 0b00111110,
    _v = 0b00011100,
    _w = 0b00101010,
    _x = 0b01110110,
    _y = 0b01101110,
    _z = 0b01011011,
    _0 = 0b00111111,
    _1 = 0b00000110,
    _2 = 0b01011011,
    _3 = 0b01001111,
    _4 = 0b01100110,
    _5 = 0b01101101,
    _6 = 0b01111101,
    _7 = 0b00000111,
    _8 = 0b01111111,
    _9 = 0b01101111,
    _dash = 0b01000000,
    _under = 0b00001000,
    _equal = 0b01001000,
    _empty = 0b00000000,
    _degree = 0b01100011,
};
#else

enum TM1637_letters {
    _A = 0x77,
    _B = 0x7f,
    _C = 0x39,
    _D = 0x3f,
    _E = 0x79,
    _F = 0x71,
    _G = 0x3d,
    _H = 0x76,
    _J = 0x1e,
    _L = 0x38,
    _N = 0x37,
    _O = 0x3f,
    _P = 0x73,
    _S = 0x6d,
    _U = 0x3e,
    _Y = 0x6e,
    _a = 0x5f,
    _b = 0x7c,
    _c = 0x58,
    _d = 0x5e,
    _e = 0x7b,
    _f = 0x71,
    _h = 0x74,
    _i = 0x10,
    _j = 0x0e,
    _l = 0x06,
    _n = 0x54,
    _o = 0x5c,
    _q = 0x67,
    _r = 0x50,
    _t = 0x78,
    _u = 0x1c,
    _y = 0x6e,
    _dash = 0x40,
    _under = 0x08,
    _equal = 0x48,
    _empty = 0x00,
    _degree = 0x63,

    _0 = 0x3f,
    _1 = 0x06,
    _2 = 0x5b,
    _3 = 0x4f,
    _4 = 0x66,
    _5 = 0x6d,
    _6 = 0x7d,
    _7 = 0x07,
    _8 = 0x7f,
    _9 = 0x6f,
};
#endif

#endif













#endif










/*

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

*/