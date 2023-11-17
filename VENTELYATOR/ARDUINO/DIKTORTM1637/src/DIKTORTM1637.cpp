#include "DIKTORTM1637.h"
#include <Arduino.h>

/*const uint8_t digitHEX[] = {0x3f, 0x06, 0x5b, 0x4f,
	0x66, 0x6d, 0x7d, 0x07,
	0x7f, 0x6f, 0x00, 0x40
};//0~9, ,-

uint8_t digToHEX(uint8_t digit) {
	return digitHEX[digit];
}*/

DIKTORTM1637::DIKTORTM1637(uint8_t clk, uint8_t dio)
{
	Clkpin = clk;
	Datapin = dio;
	pinMode(Clkpin, OUTPUT);
	pinMode(Datapin, OUTPUT);
}

int DIKTORTM1637::writeByte(int8_t wr_data)
{
	uint8_t i, count1;
	for (i = 0; i < 8; i++) //sent 8bit data
	{
		digitalWrite(Clkpin, LOW);
		if (wr_data & 0x01)digitalWrite(Datapin, HIGH); //LSB first
		else digitalWrite(Datapin, LOW);
		wr_data >>= 1;
		digitalWrite(Clkpin, HIGH);

	}
	digitalWrite(Clkpin, LOW); //wait for the ACK
	digitalWrite(Datapin, HIGH);
	digitalWrite(Clkpin, HIGH);
	pinMode(Datapin, INPUT);

	delayMicroseconds(50);
	uint8_t ack = digitalRead(Datapin);
	if (ack == 0)
	{
		pinMode(Datapin, OUTPUT);
		digitalWrite(Datapin, LOW);
	}
	delayMicroseconds(50);
	pinMode(Datapin, OUTPUT);
	delayMicroseconds(50);

	return ack;
}
//send start signal to DIKTORTM1637
void DIKTORTM1637::start(void)
{
	digitalWrite(Clkpin, HIGH); //send start signal to DIKTORTM1637
	digitalWrite(Datapin, HIGH);
	digitalWrite(Datapin, LOW);
	digitalWrite(Clkpin, LOW);
}
//End of transmission
void DIKTORTM1637::stop(void)
{
	digitalWrite(Clkpin, LOW);
	digitalWrite(Datapin, LOW);
	digitalWrite(Clkpin, HIGH);
	digitalWrite(Datapin, HIGH);
}

// ************************** ФОРМИРУЕМ ДАННЫЕ *****************************
/*void DIKTORTM1637::display(uint8_t DispData[])
{
	uint8_t SegData[4];
	for (byte i = 0; i < 4; i ++) {
		//if (DispData[i] == 0x7f) SegData[i] = 0x00;
		//else
		//{
		lastData[i] = digitHEX[DispData[i]];
		SegData[i] = digitHEX[DispData[i]] + PointData;
		//}
	}
	sendArray(SegData);
}
void DIKTORTM1637::display(uint8_t BitAddr, uint8_t DispData)
{
	uint8_t SegData;
	//if (DispData == 0x7f) SegData = 0x00;
	//else
	//{
	lastData[BitAddr] = digitHEX[DispData];
	SegData = digitHEX[DispData] + PointData;
	//}
	sendByte(BitAddr, SegData);
}

void DIKTORTM1637::display(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3) {
	uint8_t dispArray[] = {bit0, bit1, bit2, bit3};
	display(dispArray);
}*/

void DIKTORTM1637::displayByte(uint8_t DispData[])
{
	uint8_t SegData[4];
	for (byte i = 0; i < 4; i++) {
		//if (DispData[i] == 0x7f) SegData[i] = 0x00;
		//else SegData[i] = DispData[i];
		//{
		lastData[i] = DispData[i];
		SegData[i] = DispData[i] + PointData;
		//}	
	}
	sendArray(SegData);
}

void DIKTORTM1637::displayByte(uint8_t BitAddr, uint8_t DispData)
{
	uint8_t SegData;
	//if (DispData == 0x7f) SegData = 0x00;
	//else
	//{  
	lastData[BitAddr] = DispData;
	SegData = DispData + PointData;
	//}  
	sendByte(BitAddr, SegData);
}

void DIKTORTM1637::displayByte(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3) {
	uint8_t dispArray[] = { bit0, bit1, bit2, bit3 };
	displayByte(dispArray);
}

// ************************** ОТПРАВКА НА ДИСПЛЕЙ *****************************
void DIKTORTM1637::sendByte(uint8_t BitAddr, int8_t sendData) {
	start();          //start signal sent to DIKTORTM1637 from MCU
	writeByte(ADDR_FIXED);//
	stop();           //
	start();          //
	writeByte(BitAddr | 0xc0); //
	writeByte(sendData);//
	stop();            //
	start();          //
	writeByte(Cmd_DispCtrl);//
	stop();           //
}

void DIKTORTM1637::sendArray(uint8_t sendData[]) {
	start();          //start signal sent to DIKTORTM1637 from MCU
	writeByte(ADDR_AUTO);//
	stop();           //
	start();          //
	writeByte(Cmd_SetAddr);//
	for (byte i = 0; i < 4; i++) {
		writeByte(sendData[i]);        //
	}
	stop();           //
	start();          //
	writeByte(Cmd_DispCtrl);//
	stop();           //
}
// ******************************************




void DIKTORTM1637::clear(void)
{
	/*display(0x00, 0x7f);
	display(0x01, 0x7f);
	display(0x02, 0x7f);
	display(0x03, 0x7f);*/
	displayByte(0, 0, 0, 0);
	lastData[0] = 0x00;
	lastData[1] = 0x00;
	lastData[2] = 0x00;
	lastData[3] = 0x00;
}
void DIKTORTM1637::update(void)
{
	displayByte(lastData);
}

void DIKTORTM1637::brightness(uint8_t brightness, uint8_t SetData, uint8_t SetAddr)
{
	Cmd_SetData = SetData;
	Cmd_SetAddr = SetAddr;
	Cmd_DispCtrl = 0x88 + brightness; //Set the brightness and it takes effect the next time it displays.
	update();
}


void DIKTORTM1637::point(boolean PointFlag, bool upd)
{
	if (PointFlag) PointData = 0x80;
	else PointData = 0;
	if (upd) update();
}

// ************************** ВСЯКИЕ ФУНКЦИИ *****************************
/*void DIKTORTM1637::displayClock(uint8_t hrs, uint8_t mins) {
	if (hrs > 99 || mins > 99) return;
	uint8_t disp_time[4];
	if ((hrs / 10) == 0) disp_time[0] = 10;
	else disp_time[0] = (hrs / 10);
	disp_time[1] = hrs % 10;
	disp_time[2] = mins / 10;
	disp_time[3] = mins % 10;
	DIKTORTM1637::display(disp_time);
}
void DIKTORTM1637::displayClockScroll(uint8_t hrs, uint8_t mins, int delayms) {
	if (hrs > 99 || mins > 99) return;
	uint8_t disp_time[4];
	if ((hrs / 10) == 0) disp_time[0] = 10;
	else disp_time[0] = (hrs / 10);
	disp_time[1] = hrs % 10;
	disp_time[2] = mins / 10;
	disp_time[3] = mins % 10;
	scroll(disp_time, delayms);
}
void DIKTORTM1637::displayClockTwist(uint8_t hrs, uint8_t mins, int delayms) {
	if (hrs > 99 || mins > 99) return;
	uint8_t disp_time[4];
	if ((hrs / 10) == 0) disp_time[0] = 10;
	else disp_time[0] = (hrs / 10);
	disp_time[1] = hrs % 10;
	disp_time[2] = mins / 10;
	disp_time[3] = mins % 10;
	twist(disp_time, delayms);
}

void DIKTORTM1637::displayInt(int value) {
	if (value > 9999 || value < -999) return;
	boolean negative = false;
	boolean neg_flag = false;
	byte digits[4];
	if (value < 0) negative = true;
	value = abs(value);
	digits[0] = (int)value / 1000;      	// количесто тысяч в числе
	uint16_t b = (int)digits[0] * 1000; 	// вспомогательная переменная
	digits[1] = ((int)value - b) / 100; 	// получем количество сотен
	b += digits[1] * 100;               	// суммируем сотни и тысячи
	digits[2] = (int)(value - b) / 10;  	// получем десятки
	b += digits[2] * 10;                	// сумма тысяч, сотен и десятков
	digits[3] = value - b;              	// получаем количество единиц

	if (!negative) {
		for (byte i = 0; i < 3; i++) {
			if (digits[i] == 0) digits[i] = 10;
			else break;
		}
	} else {
		for (byte i = 0; i < 3; i++) {
			if (digits[i] == 0) {
				if (digits[i + 1] == 0){
					digits[i] = 10;
				} else {
					digits[i] = 11;
					break;
				}
			}
		}
	}
	DIKTORTM1637::display(digits);
}*/


/*
void DIKTORTM1637::runningString(uint8_t DispData[], byte amount, int delayMs) {
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
		displayByte(segm_data[i], segm_data[i + 1], segm_data[i + 2], segm_data[i + 3]);
		delay(delayMs);
	}
}*/


/*void DIKTORTM1637::scroll(uint8_t DispData[], int delayms) {
	byte DispDataByte[4];
	for (byte i = 0; i < 4; i++) {
		DispDataByte[i] = digitHEX[DispData[i]];
	}
	scrollByte(DispDataByte, delayms);
}

void DIKTORTM1637::scroll(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3, int delayms) {
	byte DispData[] = {digitHEX[bit0], digitHEX[bit1], digitHEX[bit2], digitHEX[bit3]};
	DIKTORTM1637::scrollByte(DispData, delayms);
}

void DIKTORTM1637::scroll(uint8_t BitAddr, uint8_t DispData, int delayms) {
	byte DispDataByte = digitHEX[DispData];
	scrollByte(BitAddr, DispDataByte, delayms);
}

void DIKTORTM1637::scrollByte(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3, int delayms) {
	byte DispData[] = {bit0, bit1, bit2, bit3};
	DIKTORTM1637::scrollByte(DispData, delayms);
}

void DIKTORTM1637::scrollByte(uint8_t DispData[], int delayms) {
	byte lastBytes[4];
	byte step;
	byte stepArray[4];
	boolean changeByte[4] = {0, 0, 0, 0};

	for (byte i = 0; i < 4; i++) {
		if (DispData[i] != lastData[i]) changeByte[i] = 1;
		lastBytes[i] = 	lastData[i];
	}

	for (byte i = 0; i < 4; i++) {
		byte lastByte = lastData[i];
		stepArray[i] = lastByte;

		if (changeByte[i]) {
			step = 0;
			swapBytes(&step, lastByte, 6, 0);
			swapBytes(&step, lastByte, 2, 1);
			swapBytes(&step, lastByte, 4, 5);
			swapBytes(&step, lastByte, 3, 6);
			stepArray[i] = step;
		}
	}
	displayByte(stepArray);
	delay(delayms);

	for (byte i = 0; i < 4; i++) {
		byte lastByte = lastBytes[i];
		stepArray[i] = lastByte;

		if (changeByte[i]) {
			step = 0;
			swapBytes(&step, lastByte, 3, 0);
			stepArray[i] = step;
		}
	}
	displayByte(stepArray);
	delay(delayms);

	for (byte i = 0; i < 4; i++) {
		if (changeByte[i]) {
			stepArray[i] = 0;
		}
	}
	displayByte(stepArray);
	delay(delayms);

	for (byte i = 0; i < 4; i++) {
		byte lastByte = lastBytes[i];
		byte newByte = DispData[i];
		stepArray[i] = lastByte;

		if (changeByte[i]) {
			step = 0;
			swapBytes(&step, newByte, 0, 3);
			stepArray[i] = step;
		}
	}
	displayByte(stepArray);
	delay(delayms);

	for (byte i = 0; i < 4; i++) {
		byte newByte = DispData[i];
		stepArray[i] = lastBytes[i];

		if (changeByte[i]) {
			step = 0;
			swapBytes(&step, newByte, 0, 6);
			swapBytes(&step, newByte, 1, 2);
			swapBytes(&step, newByte, 5, 4);
			swapBytes(&step, newByte, 6, 3);
			stepArray[i] = step;
		}
	}
	displayByte(stepArray);
	delay(delayms);

	for (byte i = 0; i < 4; i++) {
		displayByte(DispData);
	}
}

void DIKTORTM1637::scrollByte(uint8_t BitAddr, uint8_t DispData, int delayms) {
	byte oldByte = lastData[BitAddr];
	byte newByte = DispData;
	byte step;

	step = 0;
	swapBytes(&step, oldByte, 6, 0);
	swapBytes(&step, oldByte, 2, 1);
	swapBytes(&step, oldByte, 4, 5);
	swapBytes(&step, oldByte, 3, 6);
	displayByte(BitAddr, step);
	delay(delayms);

	step = 0;
	swapBytes(&step, oldByte, 3, 0);
	swapBytes(&step, newByte, 0, 3);
	displayByte(BitAddr, step);
	delay(delayms);

	step = 0;
	swapBytes(&step, newByte, 0, 6);
	swapBytes(&step, newByte, 1, 2);
	swapBytes(&step, newByte, 5, 4);
	swapBytes(&step, newByte, 6, 3);
	displayByte(BitAddr, step);
	delay(delayms);

	displayByte(BitAddr, newByte);
}

void swapBytes(byte* newByte, byte oldByte, byte newP, byte oldP) {
	byte newBit = 0;
	if (oldByte & (1 << oldP)) newBit = 1;
	else newBit = 0;
	*newByte = *newByte | (newBit << newP);
}

void DIKTORTM1637::twist(uint8_t BitAddr, uint8_t DispData, int delayms) {
	twistByte(BitAddr, digitHEX[DispData], delayms);
}

void DIKTORTM1637::twist(uint8_t DispData[], int delayms) {
	byte newData[4];
	for (byte i = 0; i < 4; i++) {
		newData[i] = digitHEX[DispData[i]];
	}
	twistByte(newData, delayms);
}

void DIKTORTM1637::twist(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3, int delayms) {
	byte DispData[] = {digitHEX[bit0], digitHEX[bit1], digitHEX[bit2], digitHEX[bit3]};
	DIKTORTM1637::twistByte(DispData, delayms);
}

void DIKTORTM1637::twistByte(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3, int delayms) {
	byte DispData[] = {bit0, bit1, bit2, bit3};
	DIKTORTM1637::twistByte(DispData, delayms);
}

void DIKTORTM1637::twistByte(uint8_t DispData[], int delayms) {
	byte step;
	byte stepArray[4];
	boolean changeByte[4] = {0, 0, 0, 0};

	for (byte i = 0; i < 4; i++) {
		if (DispData[i] != lastData[i]) changeByte[i] = 1;
		stepArray[i] = DispData[i];
	}

	step = step & 0b00111111;			// выкл центральную
	for (byte i = 0; i < 5; i++) {
		step |= (1 << i);				// зажигаем по одной
		for (byte k = 0; k < 4; k++) {
			if (changeByte[k])
			stepArray[k] = step;
		}
		displayByte(stepArray);
		delay(delayms);
	}
	//for (byte r = 0; r < 1; r++) {
	for (byte i = 0; i < 6; i++) {
		step = 0b11000000;
		step = ~(step | (1 << i) | (1 << i + 1));	// бегает дырка
		for (byte k = 0; k < 4; k++) {
			if (changeByte[k]) stepArray[k] = step;
		}
		displayByte(stepArray);
		delay(delayms);
	}
	//}
	step = 0b11000000;
	for (byte i = 0; i < 6; i++) {
		step |= (1 << i);
		for (byte k = 0; k < 4; k++) {
			if (changeByte[k])
			stepArray[k] = ~step;
		}
		displayByte(stepArray);
		delay(delayms);
	}
	for (byte k = 0; k < 4; k++) {
		if (changeByte[k])
		stepArray[k] = 0b0000000;
	}
	for (byte i = 0; i < 7; i++) {
		for (byte k = 0; k < 4; k++) {
			if (changeByte[k]) {
				byte newBit = 0;
				if (DispData[k] & (1 << i)) newBit = 1;
				else newBit = 0;
				stepArray[k] |= (newBit << i);
			}
		}
		displayByte(stepArray);
		delay(delayms);
	}
}

void DIKTORTM1637::twistByte(uint8_t BitAddr, uint8_t DispData, int delayms) {
	byte oldByte = lastData[BitAddr];
	byte newByte = DispData;
	byte step = oldByte;

	step = step & 0b0111111;		// выкл центральную

	for (byte i = 0; i < 5; i++) {
		step |= (1 << i);				// зажигаем по одной
		displayByte(BitAddr, step);
		delay(delayms);
	}
	//for (byte r = 0; r < 1; r++) {
	for (byte i = 0; i < 6; i++) {
		step = 0b1000000;
		step = ~(step | (1 << i) | (1 << i + 1));	// бегает дырка

		displayByte(BitAddr, step);
		delay(delayms);
	}
	//}
	step = 0b1000000;
	for (byte i = 0; i < 6; i++) {
		step |= (1 << i);
		displayByte(BitAddr, ~step);
		delay(delayms);
	}
	step = 0;
	for (byte i = 0; i < 7; i++) {
		byte newBit = 0;
		if (newByte & (1 << i)) newBit = 1;
		else newBit = 0;
		step |= (newBit << i);
		displayByte(BitAddr, step);
		delay(delayms);
	}
	displayByte(BitAddr, newByte);
}*/



//------------SEGMENTS
void DIKTORTM1637::SetSegments(byte num_seg, const String& sevenseg_ag)
{
	if ((num_seg < 0) || (num_seg > 3)) { return; }
	//clear();
	//displayByte(num_seg, CombineSegment(lastData[num_seg], sevenseg_ag));
	//Serial.println(CombineSegments(sevenseg_ag));
	displayByte(num_seg, CombineSegments(sevenseg_ag));
}
void DIKTORTM1637::RemoveSegments(byte num_seg, const String& sevenseg_ag)
{
	if ((num_seg < 0) || (num_seg > 3)) { return; }
	//clear();
	//displayByte(num_seg, CombineSegment(lastData[num_seg], sevenseg_ag));
	//Serial.println(CombineSegments(sevenseg_ag));
	displayByte(num_seg, _RemoveSegments(lastData[num_seg], sevenseg_ag));
}

void DIKTORTM1637::UpdateSegment(byte num_seg, char sevenseg_ag)
{
	if ((num_seg < 0) || (num_seg > 3)) { return; }
	displayByte(num_seg, CombineSegment(lastData[num_seg], sevenseg_ag));
}

void DIKTORTM1637::ClearSegment(byte num_seg)
{
	if ((num_seg < 0) || (num_seg > 3)) { return; }
	displayByte(num_seg, getDisplayChar(' '));
}






//***********************************************************
void DIKTORTM1637::OnAnimation()
{
	if (AnimRunMode) { OnRunAnimation(); }
	if (AnimDotsMode) { OnDotsAnimation(); }
	if (AnimA1Mode) { OnA1Animation(); }
	if (AnimA2Mode) { OnA2Animation(); }
}

void DIKTORTM1637::StopAnimation()
{
	if (AnimRunMode) { StopRunAnimation(); }
	if (AnimDotsMode) { StopDotsAnimation(); }
	if (AnimA1Mode) { StopA1Animation(); }
	if (AnimA2Mode) { StopA2Animation(); }
}

void DIKTORTM1637::PrintString(const String& str, bool stop_all_anim)
{
	if (stop_all_anim) { StopAnimation(); }
	if (str.length() == 1) { displayByte(getDisplayChar(str[0]), _empty, _empty, _empty); }
	else if (str.length() == 1) { displayByte(getDisplayChar(str[0]), getDisplayChar(str[1]), _empty, _empty); }
	else if (str.length() == 2) { displayByte(getDisplayChar(str[0]), getDisplayChar(str[1]), getDisplayChar(str[2]), _empty); }
	else { displayByte(getDisplayChar(str[0]), getDisplayChar(str[1]), getDisplayChar(str[2]), getDisplayChar(str[3])); }
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~PROSHIVKA
void DIKTORTM1637::InitA1Animation(int _delay) // def 100
{
	StopA1Animation();
	last_millis_A1 = millis();
	AnimA1Mode = true;
	delay_time_A1 = _delay;
}

void DIKTORTM1637::OnA1Animation()
{
	unsigned long currentMillis = millis();
	if (currentMillis - last_millis_A1 >= delay_time_A1) {
		last_millis_A1 = currentMillis;
		int g = GetInt('g');
		int d = GetInt('d');
		if (flag_A1) { displayByte(g, d, g, d); }
		else { displayByte(d, g, d, g); }
		flag_A1 = !flag_A1;
	}
}
bool DIKTORTM1637::GetA1AnimState() { return AnimA1Mode; }

void DIKTORTM1637::StopA1Animation()
{
	AnimA1Mode = false;
	flag_A1 = false;
	clear();
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ZAGRUZKA
void DIKTORTM1637::InitA2Animation(int _delay, int _loops) // def 100
{
	StopA2Animation();
	last_millis_A2 = millis();
	anim_A2_loops = _loops;
	AnimA2Mode = true;
	delay_time_A2 = _delay;
}

void DIKTORTM1637::OnA2Animation()
{
	unsigned long currentMillis = millis();
	if (currentMillis - last_millis_A2 >= delay_time_A2) {
		last_millis_A2 = currentMillis;
		switch (anim_A2_ModeCounter)
		{
		case 0:
			break;
		case 1:
			anim_A2_ByteChar = CombineSegment(anim_A2_ByteChar, 'a');
			break;
		case 2:
			anim_A2_ByteChar = CombineSegment(anim_A2_ByteChar, 'b');
			break;
		case 3:
			anim_A2_ByteChar = CombineSegment(anim_A2_ByteChar, 'c');
			break;
		case 4:
			anim_A2_ByteChar = CombineSegment(anim_A2_ByteChar, 'd');
			break;
		case 5:
			anim_A2_ByteChar = CombineSegment(anim_A2_ByteChar, 'e');
			break;
		case 6:
			anim_A2_ByteChar = CombineSegment(anim_A2_ByteChar, 'f');
			break;
		}
		displayByte(anim_A2_ByteChar, anim_A2_ByteChar, anim_A2_ByteChar, anim_A2_ByteChar);
		++anim_A2_ModeCounter;

		if (anim_A2_ModeCounter > 6) { ++anim_A2_loops_counter; anim_A2_ByteChar = 0; anim_A2_ModeCounter = 0; } // scroll?
		if (anim_A2_loops_counter >= anim_A2_loops) { StopA2Animation(); }
	}
}

bool DIKTORTM1637::GetA2AnimState() { return AnimA2Mode; }

void DIKTORTM1637::StopA2Animation()
{
	AnimA2Mode = false;
	anim_A2_loops = -1;
	anim_A2_ByteChar = 0;
	anim_A2_ModeCounter = 0;
	anim_A2_loops_counter = 0;
	clear();
}







//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~RUN
//void DIKTORTM1637::InitRunAnimation(const String& str, int _delay, int loops)
void DIKTORTM1637::InitRunAnimation(const String& str, int _delay)
{
	StopRunAnimation();
	if (message_run != NULL) { delete[] message_run; message_run = NULL; }
	String Mstr = ModString(str);
	last_millis_Run = millis();
	AnimRunMode = true;
	message_run = GetDisplayByteArray(Mstr);
	message_run_orig = GetDisplayByteArray(Mstr);
	MSGSZ_run = Mstr.length();
	delay_time_run = _delay;
	//Run_loops = loops;
}

void DIKTORTM1637::OnRunAnimation()
{
	if (!AnimRunMode) { return; }
	unsigned long currentMillis = millis();
	if (currentMillis - last_millis_Run >= delay_time_run) {
		last_millis_Run = currentMillis;
		displayByte(message_run);
		shiftRunMessage();
		//if (shiftRunMessage()) { ++Run_loops_counter; }
		//if (Run_loops_counter >= Run_loops) { StopRunAnimation(); }
	}
}

void DIKTORTM1637::ResetMessageRunAnimation()
{
	for (int i = 0; i < MSGSZ_run; i++) { message_run[i] = message_run_orig[i]; }
}

bool DIKTORTM1637::GetRunAnimState() { return AnimRunMode; }


void DIKTORTM1637::StopRunAnimation()
{
	AnimRunMode = false;
	delete[] message_run;
	delete[] message_run_orig;
	message_run = NULL;
	message_run_orig = NULL;
	MSGSZ_run = 0;
	//Run_loops = -1;
	//Run_loops_counter = 0;
	clear();
}

bool DIKTORTM1637::shiftRunMessage()
{
	//if ((message_run[MSGSZ_run] == '\0') && (Run_loops_counter != 0)) { return true; }
	byte temp = message_run[0];
	for (int i = 1; i < MSGSZ_run; i++) {
		message_run[i - 1] = message_run[i];
	}
	message_run[MSGSZ_run - 1] = temp;
	return false;
}



//-------------------------------------------------DOST ANIM
void DIKTORTM1637::InitDotsAnimation(int _delay)
{
	StopDotsAnimation();
	last_millis_Dots = millis();
	AnimDotsMode = true;
	delay_time_dots = _delay;
}

void DIKTORTM1637::OnDotsAnimation()
{
	unsigned long currentMillis = millis();
	if (currentMillis - last_millis_Dots >= delay_time_dots) {
		last_millis_Dots = currentMillis;
		point(flag_Dots);
		flag_Dots = !flag_Dots;
	}
}
bool DIKTORTM1637::GetDotsAnimState() { return AnimDotsMode; }

void DIKTORTM1637::StopDotsAnimation()
{
	AnimDotsMode = false;
	flag_Dots = false;
	//clear(); // NOT USE SEGS
}






//---------------------------------------------------------------------------
String ModString(const String& str) { return ("    " + str + " "); }
String GyverModString(const String& str) { return ("    " + str + "   "); }
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

//uint8_t getDisplayChar(char c)
int getDisplayChar(char c)
{
	//
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
	case 'a': return _a;
	case 'b': return _b;
	case 'c': return _c;
	case 'd': return _d;
	case 'e': return _e;
	case 'f': return _f;
	case 'g': return _g;
	case 'h': return _h;
	case 'i': return _i;
	case 'j': return _j;
	case 'k': return _k;
	case 'l': return _l;
	case 'm': return _m;
	case 'n': return _n;
	case 'o': return _o;
	case 'p': return _p;
	case 'q': return _q;
	case 'r': return _r;
	case 's': return _s;
	case 't': return _t;
	case 'u': return _u;
	case 'v': return _v;
	case 'w': return _w;
	case 'x': return _x;
	case 'y': return _y;
	case 'z': return _z;
		;
	case '0': return _0;
	case '1': return _1;
	case '2': return _2;
	case '3': return _3;
	case '4': return _4;
	case '5': return _5;
	case '6': return _6;
	case '7': return _7;
	case '8': return _8;
	case '9': return _9;

	case '-': return _dash;
	case '_': return _under;
	case '=': return _equal;
	case ' ': return _empty;
	case '.': return _degree;

	default: return _empty;
	}
	return 0;
}

int GetInt(char segmentChar)
{
	//
	//   _a_
	// f|   |b
	//  |_g_|
	// e|   |c
	//  |_d_|

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

int CombineSegments(int combinedValue, const String& segmentString) // "abcdefg"
{
	int result = combinedValue;
	for (char segmentChar : segmentString) {
		result |= GetInt(segmentChar);
	}
	return result;
}

int CombineSegment(int combinedValue, char segment) // "abcdefg"
{
	int result = combinedValue;
	result |= GetInt(segment);
	return result;
}

bool IsSegmentPresents(int combinedValue, const String& segmentString)
{
	for (char segmentChar : segmentString)
	{
		if (!IsSegmentPresent(combinedValue, segmentChar)) { return false; }
	}
	return true;
}

bool IsSegmentPresent(int combinedValue, char segmentChar)
{
	int segmentValue = GetInt(segmentChar);
	return (combinedValue & segmentValue) == segmentValue;
}

int _RemoveSegments(int combinedValue, const String& segmentString)
{
	for (char segmentChar : segmentString) {
		combinedValue &= ~GetInt(segmentChar);
	}
	return combinedValue;
}

int RemoveSegment(int combinedValue, char segmentChar)
{
	int segmentToRemove = GetInt(segmentChar);
	return combinedValue & ~segmentToRemove;
}