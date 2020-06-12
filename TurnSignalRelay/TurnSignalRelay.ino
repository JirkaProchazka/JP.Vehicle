/*
 Name:		TurnSignalRelay.ino
 Created:	6/9/2020 13:29:33
 Author:	JP
 */

#define ON HIGH;
#define OFF LOW;



enum PINS
{
	L_TURN_SW = 12,
	R_TURN_SW = 11,
	WARN_SW = 13,

	L_LIGHTS = A5,
	R_LIGHTS = A4,

	WARN_FLAG = A3,
};

enum FLASHES
{
	NO_FLASH = 0,
	R_FLASH = 1,
	L_FLASH = 2,
	WARN_FLASH = 3,
};

enum LIGHTS
{
	NONE = 0,
	LEFT = 1,
	RIGHT = 2,
	BOTH = 3,
};

// Device Mode

FLASHES Mode = FLASHES::NO_FLASH;

// Inpus

bool Act_RTurnSw = OFF;
bool Act_LTurnSw = OFF;
bool Act_WarnSw = OFF;

bool Last_RTurnSw = OFF;
bool Last_LTurnSw = OFF;
bool Last_WarnSw = OFF;

unsigned long LastTime_RTurnSw = 0;
unsigned long LastTime_LTurnSw = 0;
unsigned long LastTime_WarnSw = 0;

const int DEBOUNCE_PROTECTION = 150;


const int FAST_BLINK_ON_TIME = 250;
const int FAST_BLINK_OFF_TIME = 250;

const int SLOW_BLINK_ON_TIME = 350;
const int SLOW_BLINK_OFF_TIME = 350;

int Actual_Blink_On_Time = FAST_BLINK_ON_TIME;
int Actual_Blink_Off_Time = FAST_BLINK_OFF_TIME;



unsigned long LastTime_R_Toggle = 0;
unsigned long LastTime_L_Toggle = 0;
unsigned long LastTime_Warn_Toggle = 0;



bool L_Lights_State = OFF;
bool R_Lights_State = OFF;
bool Warn_Lights_State = OFF;


void Set_R_Light(bool state)
{
	R_Lights_State = state;
	digitalWrite(PINS::R_LIGHTS, state);
}

void Set_L_Light(bool state)
{
	L_Lights_State = state;
	digitalWrite(PINS::L_LIGHTS, state);
}

void Set_Warn_Light(bool state)
{
	Set_R_Light(state);
	Set_L_Light(state);
}


void setup() {

	pinMode(R_TURN_SW, INPUT);
	pinMode(L_TURN_SW, INPUT);
	pinMode(WARN_SW, INPUT_PULLUP);

	pinMode(R_LIGHTS, OUTPUT);
	pinMode(L_LIGHTS, OUTPUT);
	pinMode(WARN_FLASH, OUTPUT);

}



void loop() {

	// Read inputs
	// TODO: ReadPort
	Act_RTurnSw = digitalRead(R_TURN_SW);
	Act_LTurnSw = digitalRead(L_TURN_SW);
	Act_WarnSw = !digitalRead(WARN_SW);

	// Handle input changes

	FLASHES actMode = FLASHES::NO_FLASH;

	if (Act_RTurnSw != Last_RTurnSw && (millis() - LastTime_RTurnSw > DEBOUNCE_PROTECTION))
	{
		actMode = Act_RTurnSw ? FLASHES::R_FLASH : FLASHES::NO_FLASH;

		Last_RTurnSw = Act_RTurnSw;
		LastTime_RTurnSw = millis();
	}
	if (Act_LTurnSw != Last_LTurnSw && (millis() - LastTime_LTurnSw > DEBOUNCE_PROTECTION))
	{
		actMode = Act_LTurnSw ? FLASHES::L_FLASH : FLASHES::NO_FLASH;

		Last_LTurnSw = Act_LTurnSw;
		LastTime_LTurnSw = millis();
	}
	if (Act_WarnSw != Last_WarnSw && (millis() - LastTime_WarnSw > DEBOUNCE_PROTECTION))
	{
		actMode = Act_WarnSw ? FLASHES::WARN_FLASH : FLASHES::NO_FLASH;

		Last_WarnSw = Act_WarnSw;
		LastTime_WarnSw = millis();
	}


	// Handle mode change

	if (actMode != Mode)
	{
		switch (actMode)
		{
			case FLASHES::R_FLASH:
			{
				Set_R_Light(HIGH);
				Set_L_Light(LOW);

				LastTime_L_Toggle = 0;
				LastTime_R_Toggle = millis();
				LastTime_Warn_Toggle = 0;
			}
			break;

			case FLASHES::L_FLASH:
			{
				Set_R_Light(LOW);
				Set_L_Light(HIGH);

				LastTime_L_Toggle = millis();
				LastTime_R_Toggle = 0;
				LastTime_Warn_Toggle = 0;
			}
			break;

			case FLASHES::WARN_FLASH:
			{
				Set_R_Light(HIGH);
				Set_L_Light(HIGH);

				LastTime_L_Toggle = 0;
				LastTime_R_Toggle = 0;
				LastTime_Warn_Toggle = millis();
			}
			break;

			case FLASHES::NO_FLASH:
			default:
			{
				Set_Warn_Light(HIGH);
			}
			break;
		}

		Mode = actMode;
	}

	// Handle actual mode
	switch (Mode)
	{
		case FLASHES::R_FLASH:
		{
			if (R_Lights_State && (millis() - LastTime_R_Toggle > Actual_Blink_On_Time))
			{
				Set_R_Light(LOW);
			}
			else if (!R_Lights_State && (millis() - LastTime_R_Toggle > Actual_Blink_Off_Time))
			{
				Set_R_Light(HIGH);
			}
		}
		break;
		case FLASHES::L_FLASH:
		{
			if (L_Lights_State && (millis() - LastTime_L_Toggle > Actual_Blink_On_Time))
			{
				Set_L_Light(LOW);
			}
			else if (!L_Lights_State && (millis() - LastTime_L_Toggle > Actual_Blink_Off_Time))
			{
				Set_L_Light(HIGH);
			}
		}
		break;
		case FLASHES::WARN_FLASH:
		{
			if (R_Lights_State && (millis() - LastTime_L_Toggle > Actual_Blink_On_Time))
			{
				Set_L_Light(LOW);
			}
			else if (!L_Lights_State && (millis() - LastTime_L_Toggle > Actual_Blink_Off_Time))
			{
				Set_L_Light(HIGH);
			}
		}
		break;

		case FLASHES::NO_FLASH:
		default:
			break;
	}
}

