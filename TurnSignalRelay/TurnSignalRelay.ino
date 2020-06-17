/*
 Name:		TurnSignalRelay.ino
 Created:	6/9/2020 13:29:33
 Author:	JP
 */

constexpr bool ON = 0x1;
constexpr bool OFF = 0x0;
constexpr bool NEVER = 0x0;
constexpr bool RELEASED = 0x0;
constexpr bool PUSHED = 0x1;

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

bool R_Sw_Last = RELEASED;
bool L_Sw_LastState = RELEASED;

bool R_Sw_Stable = RELEASED;
bool L_Sw_Stable = RELEASED;

unsigned long R_Sw_LastReadChangeTime = NEVER;
unsigned long L_Sw_LastReadChangeTime = NEVER;



const int DEF_BLINK_ON_TIME = 250;
const int DEF_BLINK_OFF_TIME = 250;

const int ADD_DELAY_BLINKS = 2;
const int BLINK_ON_DELAY = 100;
const int BLINK_OFF_DELAY = 100;

int Actual_Blink_On_Time = DEF_BLINK_ON_TIME;
int Actual_Blink_Off_Time = DEF_BLINK_OFF_TIME;

int BlickCounter = 0;


bool Lights_State = OFF;
unsigned long LastTime_Toggle = NEVER;



// ************************************************************************************************************


void Set_R_Light(bool state)
{
	Lights_State = state;
	LastTime_Toggle = millis();
	
	digitalWrite(PINS::R_LIGHTS, state);
}
void Set_L_Light(bool state)
{
	Lights_State = state;
	LastTime_Toggle = millis();
	
	digitalWrite(PINS::L_LIGHTS, state);
}
void Set_Warn_Light(bool state)
{
	Lights_State = state;
	LastTime_Toggle = millis();

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

	bool act_R_Sw = digitalRead(R_TURN_SW);	// TODO: Read port instead of digitalRead()
	bool act_L_Sw = digitalRead(L_TURN_SW);
			
	if (act_R_Sw != R_Sw_Last && (millis() - R_Sw_LastReadChangeTime > DEBOUNCE_PROTECTION_TIME))
	{
		R_Sw_Last = act_R_Sw;
		R_Sw_LastReadChangeTime = millis();
	}
	if (act_L_Sw != L_Sw_LastState && (millis() - L_Sw_LastReadChangeTime > DEBOUNCE_PROTECTION_TIME))
	{
		L_Sw_LastState = act_L_Sw;
		L_Sw_LastReadChangeTime = millis();
	}


	// Set mode according INPUTS =========================

	DEVICE_MODES actMode = DEVICE_MODES::NO_FLASH;

	
	if (R_Sw_Stable != act_R_Sw && millis() - R_Sw_LastReadChangeTime > DEBOUNCE_PROTECTION_TIME)
	{
		R_Sw_Stable == act_R_Sw;
		
		actMode = act_R_Sw ? DEVICE_MODES::R_FLASH : DEVICE_MODES::NO_FLASH;
	}
	if (L_Sw_Stable != act_L_Sw && millis() - L_Sw_LastReadChangeTime > DEBOUNCE_PROTECTION_TIME)
	{
		L_Sw_Stable == act_L_Sw;

		actMode = act_L_Sw ? DEVICE_MODES::R_FLASH : DEVICE_MODES::NO_FLASH;
	}

	
	if (R_Sw_Stable && L_Sw_Stable)
	{
		actMode = DEVICE_MODES::WARN_FLASH;
	}
	if (!R_Sw_Stable && !L_Sw_Stable)
	{
		actMode = DEVICE_MODES::NO_FLASH;
	}


	// Handle mode change ================================

	if (actMode != Mode)
	{
		Lights_State = OFF;
		BlickCounter = 0;
		Actual_Blink_On_Time = DEF_BLINK_ON_TIME;
		Actual_Blink_Off_Time = DEF_BLINK_OFF_TIME;
				

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


	// Handle flashing mode ===============================
	{
		bool new_state = Lights_State;

		if (Mode != DEVICE_MODES::NO_FLASH)
		{
			if (Lights_State && (millis() - LastTime_Toggle > Actual_Blink_On_Time))
			{
				new_state = OFF;
			}
			else if (!Lights_State && (millis() - LastTime_Toggle > Actual_Blink_Off_Time))
			{
				new_state = ON;
				HnadleBlickCounter();
			}
		}


		// Set Lights =========================================
		// 


		if (new_state != Lights_State)
		{
			switch (Mode)
			{
				case DEVICE_MODES::R_FLASH:
				{
					Set_R_Light(new_state);
				}
				break;

				case DEVICE_MODES::L_FLASH:
				{
					Set_L_Light(new_state);
				}
				break;

				case DEVICE_MODES::WARN_FLASH:
				{
					Set_Warn_Light(new_state);
				}
				break;

				case DEVICE_MODES::NO_FLASH:
				default:
				{
					
				}
				break;
			}
		}

		
	}
}

// ************************************************************************************************************


