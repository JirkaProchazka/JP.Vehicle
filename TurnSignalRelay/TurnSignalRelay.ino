/*
 Name:		TurnSignalRelay.ino
 Created:	6/9/2020 13:29:33
 Author:	JP
 */

#define ON HIGH;
#define OFF LOW;

// chip pins

enum PINS
{
	L_TURN_SW = 12,
	R_TURN_SW = 11,
	WARN_SW = 13,

	L_LIGHTS = A5,
	R_LIGHTS = A4,
};

// device modes
enum DEVICE_MODES
{
	NO_FLASH = 0,
	R_FLASH = 1,
	L_FLASH = 2,
	WARN_FLASH = 3,
};


// ************************************************************************************************************

DEVICE_MODES Mode = DEVICE_MODES::NO_FLASH;


const int DEBOUNCE_PROTECTION_TIME = 150;

bool Act_RTurnSw = OFF;
bool Act_LTurnSw = OFF;

bool Last_RTurnSw = OFF;
bool Last_LTurnSw = OFF;

unsigned long LastTime_RTurnSw = 0;
unsigned long LastTime_LTurnSw = 0;


const int DEF_BLINK_ON_TIME = 250;
const int DEF_BLINK_OFF_TIME = 250;

const int ADD_DELAY_BLINKS = 2;
const int BLINK_ON_DELAY = 100;
const int BLINK_OFF_DELAY = 100;

int Actual_Blink_On_Time = DEF_BLINK_ON_TIME;
int Actual_Blink_Off_Time = DEF_BLINK_OFF_TIME;

int BlickCounter = 0;


unsigned long LastTime_R_Toggle = 0;
unsigned long LastTime_L_Toggle = 0;
unsigned long LastTime_Warn_Toggle = 0;

bool L_Lights_State = OFF;
bool R_Lights_State = OFF;
bool Warn_Lights_State = OFF;



// ************************************************************************************************************


void Set_R_Light(bool state)
{
	R_Lights_State = state;
	LastTime_R_Toggle = millis();
	
	digitalWrite(PINS::R_LIGHTS, state);
}
void Set_L_Light(bool state)
{
	L_Lights_State = state;
	LastTime_L_Toggle = millis();
	
	digitalWrite(PINS::L_LIGHTS, state);
}
void Set_Warn_Light(bool state)
{
	Warn_Lights_State = state;
	LastTime_Warn_Toggle = millis();

	digitalWrite(PINS::R_LIGHTS, state);
	digitalWrite(PINS::L_LIGHTS, state);
}
void TurnOffLights()
{
	bool offState = OFF;
	digitalWrite(PINS::R_LIGHTS, offState);
	digitalWrite(PINS::L_LIGHTS, offState);
}

void HnadleBlickCounter()
{
	BlickCounter++;

	if (BlickCounter == ADD_DELAY_BLINKS)
	{
		Actual_Blink_On_Time += BLINK_ON_DELAY;
		Actual_Blink_Off_Time += BLINK_OFF_DELAY;
	}
}


// ************************************************************************************************************

void setup() {

	// INPUT pins
	pinMode(R_TURN_SW, INPUT);
	pinMode(L_TURN_SW, INPUT);
	// OUTPUT pins
	pinMode(R_LIGHTS, OUTPUT);
	pinMode(L_LIGHTS, OUTPUT);
}


// ************************************************************************************************************

void loop() {

	// Read inputs =======================================

	Act_RTurnSw = digitalRead(R_TURN_SW);	// TODO: Read port instead of digitalRead()
	Act_LTurnSw = digitalRead(L_TURN_SW);
		

	DEVICE_MODES actMode = DEVICE_MODES::NO_FLASH;

	// Set mode according INPUTS =========================

	if (Act_RTurnSw != Last_RTurnSw && (millis() - LastTime_RTurnSw > DEBOUNCE_PROTECTION_TIME))
	{
		Last_RTurnSw = Act_RTurnSw;
		LastTime_RTurnSw = millis();

		actMode = Act_RTurnSw ? DEVICE_MODES::R_FLASH : DEVICE_MODES::NO_FLASH;
	}
	if (Act_LTurnSw != Last_LTurnSw && (millis() - LastTime_LTurnSw > DEBOUNCE_PROTECTION_TIME))
	{
		Last_LTurnSw = Act_LTurnSw;
		LastTime_LTurnSw = millis();

		actMode = Act_LTurnSw ? DEVICE_MODES::L_FLASH : DEVICE_MODES::NO_FLASH;
	}

	if (Last_RTurnSw && Last_LTurnSw)
	{
		actMode = DEVICE_MODES::WARN_FLASH;
	}
	if (!Last_RTurnSw && !Last_LTurnSw)
	{
		actMode = DEVICE_MODES::NO_FLASH;
	}


	// Handle mode change ================================

	if (actMode != Mode)
	{
		BlickCounter = 0;
		Actual_Blink_On_Time = DEF_BLINK_ON_TIME;
		Actual_Blink_Off_Time = DEF_BLINK_OFF_TIME;

		Warn_Lights_State = OFF;
		L_Lights_State = OFF;
		R_Lights_State = OFF;

		switch (actMode)
		{
			case DEVICE_MODES::R_FLASH:
			{
				Set_R_Light(HIGH);
				Set_L_Light(LOW);
			}
			break;

			case DEVICE_MODES::L_FLASH:
			{
				Set_R_Light(LOW);
				Set_L_Light(HIGH);
			}
			break;

			case DEVICE_MODES::WARN_FLASH:
			{
				Set_Warn_Light(HIGH);
			}
			break;

			case DEVICE_MODES::NO_FLASH:
			default:
			{
				TurnOffLights();
			}
			break;
		}

		Mode = actMode;
	}


	// Handle actual mode ===============================
	{
		bool new_r_state = R_Lights_State;
		bool new_l_state = L_Lights_State;
		bool new_warn_state = Warn_Lights_State;

		switch (Mode)
		{
			case DEVICE_MODES::R_FLASH:
			{
				if (R_Lights_State && (millis() - LastTime_R_Toggle > Actual_Blink_On_Time))
				{
					new_r_state = OFF;
				}
				else if (!R_Lights_State && (millis() - LastTime_R_Toggle > Actual_Blink_Off_Time))
				{
					new_r_state = ON;
					HnadleBlickCounter();
				}
			}
			break;
			case DEVICE_MODES::L_FLASH:
			{
				if (L_Lights_State && (millis() - LastTime_L_Toggle > Actual_Blink_On_Time))
				{
					new_l_state = OFF;
				}
				else if (!L_Lights_State && (millis() - LastTime_L_Toggle > Actual_Blink_Off_Time))
				{
					new_l_state = ON;
					HnadleBlickCounter();
				}
			}
			break;
			case DEVICE_MODES::WARN_FLASH:
			{
				if (Warn_Lights_State && (millis() - LastTime_Warn_Toggle > Actual_Blink_On_Time))
				{
					new_warn_state = OFF;
				}
				else if (!Warn_Lights_State && (millis() - LastTime_Warn_Toggle > Actual_Blink_Off_Time))
				{
					new_warn_state = ON;
					HnadleBlickCounter();
				}
			}
			break;
			case DEVICE_MODES::NO_FLASH:
			default:
			{ /* Nothing to do here. */ }
			break;
		}


		// Set outputs =========================================
		// 

		if (new_warn_state != Warn_Lights_State)
		{
			Set_Warn_Light(new_l_state);
		}
		else if (new_l_state != L_Lights_State)
		{
			Set_L_Light(new_l_state);
		}
		else if (new_r_state != R_Lights_State)
		{
			Set_R_Light(new_r_state);
		}
	}
}

// ************************************************************************************************************


