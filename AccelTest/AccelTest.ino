/*
 Name:		AccelTest.ino
 Created:	6/28/2020 10:17:55 PM
 Author:	Jirka
*/


constexpr int LIMIT_HI = 450;
constexpr int LIMIT_LO = 270;		
constexpr int IN_WARN_TIME = 200;	// [ms]
constexpr int LED_ON_TIME = 3000;	// [ms]

unsigned long ChangeModeTime, onTime;
bool LED_ON = false;

enum Mode
{
	normal,
	warn,
	err,
};

Mode AMode = Mode::normal;

const byte SunBufferCapacity = 6;
const byte SumLastReadWeigh = 2;
int SunBuffer[SunBufferCapacity];
byte SunBuffer_Index = 0;
unsigned int SunBuffer_Sum = 0;
int SunBuffer_Avg = 0;


// the setup function runs once when you press reset or power the board
void setup() {

	Serial.begin(9600);

	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);

	pinMode(LED_BUILTIN, OUTPUT);


	// fill buffer with start values
	int initValue = analogRead(A0);
	for (int index = 0; index < SunBufferCapacity; index++) {
		SunBuffer[index] = initValue;
		SunBuffer_Sum += initValue;
	}
	SunBuffer_Avg = initValue;
}


// the loop function runs over and over again until power down or reset
void loop() {

	int ldrRead = analogRead(A0);
	delayMicroseconds(50);

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
	SunBuffer_Avg = SunBuffer_Sum / SunBufferCapacity;


	/*
	y = analogRead(A1);
	delayMicroseconds(100);

	z = analogRead(A2);
	delayMicroseconds(100);
	*/

	if (SunBuffer_Avg > LIMIT_LO && SunBuffer_Avg < LIMIT_HI)
	{
		AMode = Mode::normal;
		ChangeModeTime = millis();
	}

	if (AMode == Mode::normal && (SunBuffer_Avg >= LIMIT_HI || SunBuffer_Avg <= LIMIT_LO))
	{
		AMode = Mode::warn;
		ChangeModeTime = millis();
	}

	if (AMode == Mode::warn && (millis() - ChangeModeTime) > IN_WARN_TIME)
	{
		AMode = Mode::err;
		ChangeModeTime = millis();

		digitalWrite(LED_BUILTIN, HIGH);
		LED_ON = true;
		onTime = millis();
	}
	if (LED_ON && (millis() - onTime > LED_ON_TIME))
	{
		digitalWrite(LED_BUILTIN, LOW);
		LED_ON = false;
	}


}
