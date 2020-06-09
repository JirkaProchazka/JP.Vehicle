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
	WARN_FLASH = 3
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

unsigned long ToggleTime_RTurnSw = 0;
unsigned long ToggleTime_LTurnSw = 0;
unsigned long ToggleTime_WarnSw = 0;

const int DEBOUNCE_PROTECTION = 150;



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

	Act_RTurnSw = digitalRead(R_TURN_SW);
	Act_LTurnSw = digitalRead(L_TURN_SW);
	Act_WarnSw = !digitalRead(WARN_SW);

	// Handle input changes

	FLASHES actMode = FLASHES::NO_FLASH;

	if (Act_RTurnSw != Last_RTurnSw && (millis() - ToggleTime_RTurnSw > DEBOUNCE_PROTECTION))
	{
		actMode = Act_RTurnSw ? FLASHES::R_FLASH : FLASHES::NO_FLASH;

		Last_RTurnSw = Act_RTurnSw;
		ToggleTime_RTurnSw = millis();
	}
	if (Act_LTurnSw != Last_LTurnSw && (millis() - ToggleTime_LTurnSw > DEBOUNCE_PROTECTION))
	{
		actMode = Act_LTurnSw ? FLASHES::L_FLASH : FLASHES::NO_FLASH;

		Last_LTurnSw = Act_LTurnSw;
		ToggleTime_LTurnSw = millis();
	}
	if (Act_WarnSw != Last_WarnSw && (millis() - ToggleTime_WarnSw > DEBOUNCE_PROTECTION))
	{
		actMode = Act_WarnSw ? FLASHES::WARN_FLASH : FLASHES::NO_FLASH;

		Last_WarnSw = Act_WarnSw;
		ToggleTime_WarnSw = millis();
	}


	// Handle mode change

	if (actMode != Mode)
	{
		switch (actMode)
		{
			case FLASHES::R_FLASH:
			{

			}
			break;

			case FLASHES::L_FLASH:
			{}
			break;

			case FLASHES::WARN_FLASH:
			{
			}
			break;

			case FLASHES::NO_FLASH:
			default:
			{
				// TURN OFF ANY FLAHING
				// TURN OFF LIGHTS ITSELF
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
		}
		break;
		case FLASHES::L_FLASH:
		{
		}
		break;
		case FLASHES::WARN_FLASH:
		{
		}
		break;

		case FLASHES::NO_FLASH:
		default:
		break;
	}
}


