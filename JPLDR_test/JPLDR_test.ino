/*
 Name:		JPLDR_test.ino
 Created:	4/5/2020 10:10:20 PM
 Author:	JP
*/


enum PINS
{
	LDR = A0,
	DASHBOARD_BACKLIGHT = 2,
};


unsigned long LastSunRefresh = 0UL;
const unsigned int SunRefreshPeriod = 50;


const byte SunBufferCapacity = 10;
const byte SumLastReadWeigh = 2;
const byte SunMinChange = 5;

byte SunBuffer[SunBufferCapacity];
byte SunBuffer_Index = 0;
unsigned int SunBuffer_Sum = 0;
byte SunBuffer_Avg = 0;

const byte MAX_DASHBOARD_LIGHT = 240;

const byte DASHBOARD_DARK_LIGHT = 20;


byte blinker = 0;
unsigned long LastBlink = 0;
const int BlinkPeriod = 1000;

bool RefreshBacklight = true;


// the setup function runs once when you press reset or power the board
void setup() {

	Serial.begin(9600);

	pinMode(PINS::LDR, INPUT);

	// fill buffer with start values
	for (int thisReading = 0; thisReading < SunBufferCapacity; thisReading++) {
		SunBuffer[thisReading] = 0;
	}
}

// the loop function runs over and over again until power down or reset
void loop() {

	if (millis() - LastBlink >= BlinkPeriod)
	{
		LastBlink = millis();
		blinker = ~blinker;
		RefreshBacklight = true;

		Serial.print("Blinker: ");
		Serial.print(blinker);
		Serial.println();
	}


	if (millis() - LastSunRefresh >= SunRefreshPeriod)
	{
		LastSunRefresh = millis();

		// decrease AD convert resolution 10bit -> 8bit
		byte ldrRead = analogRead(PINS::LDR) >> 2;

		// fill last value to specific number of buffer places
		for (int i = 0; i < SumLastReadWeigh; i++)
		{
			SunBuffer_Sum -= SunBuffer[SunBuffer_Index];
			SunBuffer_Sum += ldrRead;

			SunBuffer[SunBuffer_Index] = ldrRead;

			// move index
			SunBuffer_Index++;
			if (SunBuffer_Index == SunBufferCapacity)
				SunBuffer_Index = 0;
		}

		// compute new average
		int newAvg = SunBuffer_Sum / SunBufferCapacity;


		if (abs(SunBuffer_Avg - newAvg) > SunMinChange)
		{
			SunBuffer_Avg = newAvg;
			RefreshBacklight = true;
		}

		
		Serial.print("LDR: ");
		Serial.print(ldrRead);
		Serial.print(" - new AVG: ");
		Serial.print(newAvg);
		Serial.println();
	}

	if (RefreshBacklight)
	{
		// general PWM settings - watch lower limit
		byte backlightPWM = max(DASHBOARD_DARK_LIGHT, SunBuffer_Avg);

		// dashboard PWM - watch upper limit
		byte dashboardBacklight = min(MAX_DASHBOARD_LIGHT, backlightPWM);
		// dashboardBacklight &= blinker; // blinking for test purposes
		analogWrite(PINS::DASHBOARD_BACKLIGHT, dashboardBacklight);

		RefreshBacklight = false;
	}


}
