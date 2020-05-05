/*
 Name:		BulbWarmUp_test.ino
 Created:	4/13/2020 2:54:30 PM
 Author:	Jirka


 https://www.arduinoslovakia.eu/blog/2017/1/casovac-v-ctc-rezime?lang=cs
*/

enum PINS
{
	LED_TEST = 2,
	PARKING_LIGHTS,
	LOW_BEAM,
	HI_BEAM,

	HEAD_LIGHT_SWITCH_MEASURE,
	HEAD_LIGHT_SWITCH,

	TEST_SWITCH = 31,
};

enum ANALOGUE_PINS
{
	HEADLIGHT_LEVEL = A0,
};

bool ParkingLights = false;
bool HiBeam = false;
bool LowBeam = false;

byte HeadlightLevel = 25;


bool LED_State = false;

int lightLevel = 0;
int diff = -8;


unsigned long ToggleTime = 0;
bool LastRead = HIGH;

// the setup function runs once when you press reset or power the board
void setup() {

	Serial.begin(9600);
	pinMode(PINS::TEST_SWITCH, INPUT_PULLUP);
	pinMode(PINS::LED_TEST, OUTPUT);



	noInterrupts();           // disable all interrupts
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	OCR1A = 624;				// compare match register 16MHz/1024/1Hz
	TCCR1B |= (1 << WGM12);		// CTC mode
	TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10); // 1024 prescaler 
	//TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
	interrupts();             // enable all interrupts
}


// the loop function runs over and over again until power down or reset
void loop() {

	auto actualRead = digitalRead(PINS::TEST_SWITCH);
	if ((actualRead != LastRead) && ((millis() - ToggleTime) > 150))
	{
		LastRead = actualRead;
		ToggleTime = millis();

		if (actualRead)
		{
			Serial.println("Do Job");
			diff *= -1;
			if (!TIMSK1)
			{
				Serial.println("start timer");
				TIMSK1 |= (1 << OCIE1A);
			}
		}
	}



	/*
	if ((millis() - ToggleTime) >= ToggleInterval)
	{
		//noInterrupts();

		if (state)
		{
			Serial.println("turn off");
			TIMSK1 &= (B11111110 << OCIE1A);
			TCNT1 = 0;
			analogWrite(PINS::LED_TEST, 0);
			lightLevel = 0;
		}
		else
		{
			Serial.println("turn on");
			TIMSK1 |= (1 << OCIE1A);
		}

		state = !state;
		ToggleTime = millis();

		//interrupts();
	}

	*/

}

void CheckLight()
{

}



ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
	//LED_State = LED_State ^ 1;
	//digitalWrite(PINS::LED_TEST, LED_State);   // toggle LED pin

	lightLevel += diff;
	lightLevel = constrain(lightLevel, 0, 255);

	if (lightLevel < 255)
	{
		analogWrite(PINS::LED_TEST, lightLevel);
	}

	if (lightLevel == 255 || lightLevel == 0)
	{
		noInterrupts();
		TIMSK1 &= (B11111110 << OCIE1A);
		TCNT1 = 0;
		
		Serial.println("Job DONE");
		interrupts();
	}

	/*
	if (lightLevel == 255 || lightLevel == 0)
		diff *= -1;

		*/
}



void StartUp()
{
	for (byte i = 2; i < 256; i += 2)
	{
		SetHeadLight(i);
	}
}

void SetHeadLight(byte level)
{
	analogWrite(PINS::HEAD_LIGHT_SWITCH, level);
}
