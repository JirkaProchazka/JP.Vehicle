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
const unsigned int SunRefreshPeriod = 300;
const byte SunMinChange = 10;
bool SunChanged = true;

const byte SunReadNumber = 3;
const byte SumLastReadWeigh = 2;
byte SunBuffer[SunReadNumber];
byte SunBuffer_Index = 0;
byte SunBuffer_Sum = 0;
byte SunBuffer_Avg = 0;


const int DARK_LIMIT = 20;
const int BRIGHT_LIMIT = 200;
const byte MAX_DASHBOARD_LIGHT = 240;

const byte DARK_DASHBOARD_LIGHT = 50;
const byte BRIGHT_DASHBOARD_LIGHT = 0;




// the setup function runs once when you press reset or power the board
void setup() {

	Serial.begin(9600);
	pinMode(PINS::LDR, INPUT);

	for (int thisReading = 0; thisReading < SunReadNumber; thisReading++) {
		SunBuffer[thisReading] = 0;
	}
}

// the loop function runs over and over again until power down or reset
void loop() {

	if (millis() - LastSunRefresh >= SunRefreshPeriod)
	{
		LastSunRefresh = millis();

		byte ldrRead =  analogRead(PINS::LDR)>>2;

		for (int  i = 0; i < SumLastReadWeigh; i++)
		{
			SunBuffer_Sum -= SunBuffer[SunBuffer_Index];
			SunBuffer_Sum += ldrRead;

			SunBuffer[SunBuffer_Index] = ldrRead;

			SunBuffer_Index++;
			if (SunBuffer_Index >= SunReadNumber)
				SunBuffer_Index = 0;
		}

		int newAvg = SunBuffer_Sum / SunReadNumber;

		SunChanged = abs(SunBuffer_Avg - newAvg) > SunMinChange;
		if (SunChanged)
			SunBuffer_Avg = newAvg;


		Serial.print("LDR: ");
		Serial.print(ldrRead);
		Serial.print(" - new AVG: ");
		Serial.print(newAvg);
		Serial.println();
	}

	if (SunChanged)
	{
		// DashBoard
		/*
		byte dashBoardLight = MAX_DASHBOARD_LIGHT;
		if (LastSun <= DARK_LIMIT)
			dashBoardLight = DARK_DASHBOARD_LIGHT;
		else if (LastSun >= BRIGHT_LIMIT)
			dashBoardLight = BRIGHT_DASHBOARD_LIGHT;
		else
		{
			dashBoardLight = map(LastSun, 0, 1023, 0, 255);
		}
		*/

		analogWrite(PINS::DASHBOARD_BACKLIGHT, SunBuffer_Avg);

		SunChanged = false;
	}


}
