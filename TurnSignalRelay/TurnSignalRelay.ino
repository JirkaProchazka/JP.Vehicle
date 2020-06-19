/*
 Name:		TurnSignalRelay.ino
 Created:	6/9/2020 13:29:33
 Author:	JP
 */

 // DEFINICITON OF EXPRESSIONS

constexpr bool ON = 0x1;
constexpr bool OFF = 0x0;
constexpr bool NEVER = 0x0;
constexpr bool RELEASED = 0x0;
constexpr bool PUSHED = 0x1;

// PORT DEFINITION

// ARDUINO PINS
enum PINS
{
	L_TURN_SW = 12,
	R_TURN_SW = 11,

	L_LIGHTS = A5,
	R_LIGHTS = A4,
};


// Inputs

#define INPUT_PORT PINB
constexpr byte R_SWITCH_PORT_MASK = (1 << PINB0);
constexpr byte L_SWITCH_PORT_MASK = (1 << PINB1);

// Outputs

#define OUTPUT_PORT PORTD
constexpr byte R_LIGHT_PIN_MASK = (1 << PIND0);
constexpr byte L_LIGHT_PIN_MASK = (1 << PIND1);
constexpr byte LIGHTS_PORT_MASK = ~(L_LIGHT_PIN_MASK | R_LIGHT_PIN_MASK);


enum LIGHTS_OUTPUT
{
	NONE = 0x0,
	RIGHT = R_LIGHT_PIN_MASK,
	LEFT = L_LIGHT_PIN_MASK,
	BOTH = (RIGHT | LEFT),
};


// device modes
enum RELAY_MODES
{
	IDLE = 0,
	R_FLASH = 1,
	L_FLASH = 2,
	WARN_FLASH = 3,
	EMERG_BREAK = 4,
};



// =============================================================================================================

RELAY_MODES Mode = RELAY_MODES::IDLE;


// - debouncing inputs -----------------------------

const int DEBOUNCE_PROTECTION_TIME = 100;

bool R_Sw_LastState = RELEASED;
bool L_Sw_LastState = RELEASED;

bool R_Sw_Stable = RELEASED;
bool L_Sw_Stable = RELEASED;

unsigned long R_Sw_LastReadChangeTime = NEVER;
unsigned long L_Sw_LastReadChangeTime = NEVER;


// - flashing variables ----------------------------

const int START_BLINK_ON_TIME = 250;
const int START_BLINK_OFF_TIME = 250;

const int FLASH_COUNTER_INCREMENT_LIMIT = 2;

const int FLASH_ON_INCREMENT_TIME = 100;
const int FLASH_OFF_INCREMENT_TIME = 100;

int Flash_ON_Time = START_BLINK_ON_TIME;
int Flash_OFF_Time = START_BLINK_OFF_TIME;

int FlashCounter = 0;

bool Flash_State = OFF;
unsigned long LastTime_Toggle = NEVER;


// =============================================================================================================

// Setup output pins

void Set_Port_Out(LIGHTS_OUTPUT output)
{
	auto maskedPort = OUTPUT_PORT & LIGHTS_PORT_MASK;	// lights bits cleared;
	OUTPUT_PORT = maskedPort | output;					// lights bits set 
}


void Set_R_Light(bool state)
{
	digitalWrite(PINS::R_LIGHTS, state);
}
void Set_L_Light(bool state)
{
	digitalWrite(PINS::L_LIGHTS, state);
}
void Set_Both_Lights(bool state)
{
	digitalWrite(PINS::R_LIGHTS, state);
	digitalWrite(PINS::L_LIGHTS, state);
}

// -----------------------------------------------------

void HandleFlashTime()
{
	FlashCounter++;

	if (FlashCounter == FLASH_COUNTER_INCREMENT_LIMIT)
	{
		Flash_ON_Time += FLASH_ON_INCREMENT_TIME;
		Flash_OFF_Time += FLASH_OFF_INCREMENT_TIME;
	}
}

void ResetFlashTime()
{
	FlashCounter = 0;
	Flash_ON_Time = START_BLINK_ON_TIME;
	Flash_OFF_Time = START_BLINK_OFF_TIME;
}


// =============================================================================================================

void setup() {

	// INPUT pins
	pinMode(R_TURN_SW, INPUT);
	pinMode(L_TURN_SW, INPUT);

	// OUTPUT pins
	pinMode(R_LIGHTS, OUTPUT);
	pinMode(L_LIGHTS, OUTPUT);
}


// =============================================================================================================

void loop() {

	// - read inputs ---------------------------------------

	bool act_R_Sw = INPUT_PORT & R_SWITCH_PORT_MASK;
	bool act_L_Sw = INPUT_PORT & L_SWITCH_PORT_MASK;

	/*
	bool act_R_Sw = digitalRead(R_TURN_SW);
	bool act_L_Sw = digitalRead(L_TURN_SW);
	*/


	// - handle read chanbge -------------------------------

	if (act_R_Sw != R_Sw_LastState)
	{
		R_Sw_LastState = act_R_Sw;
		R_Sw_LastReadChangeTime = millis();
	}
	if (act_L_Sw != L_Sw_LastState)
	{
		L_Sw_LastState = act_L_Sw;
		L_Sw_LastReadChangeTime = millis();
	}

	// -----------------------------------------------------

	RELAY_MODES actMode = Mode;


	// Set mode if input changed ---------------------------

	if (R_Sw_Stable != act_R_Sw && millis() - R_Sw_LastReadChangeTime > DEBOUNCE_PROTECTION_TIME)
	{
		R_Sw_Stable == act_R_Sw;

		actMode = act_R_Sw ? RELAY_MODES::R_FLASH : RELAY_MODES::IDLE;
	}
	if (L_Sw_Stable != act_L_Sw && millis() - L_Sw_LastReadChangeTime > DEBOUNCE_PROTECTION_TIME)
	{
		L_Sw_Stable == act_L_Sw;

		actMode = act_L_Sw ? RELAY_MODES::L_FLASH : RELAY_MODES::IDLE;
	}

	if (R_Sw_Stable && L_Sw_Stable)
	{
		actMode = RELAY_MODES::WARN_FLASH;
	}
	if (!R_Sw_Stable && !L_Sw_Stable)
	{
		actMode = RELAY_MODES::IDLE;
	}


	// Handle mode change ----------------------------------

	if (actMode != Mode)
	{
		Flash_State = OFF;
		ResetFlashTime();

		switch (actMode)
		{
			case RELAY_MODES::R_FLASH:
				Set_Port_Out(LIGHTS_OUTPUT::RIGHT);
				break;

			case RELAY_MODES::L_FLASH:
				Set_Port_Out(LIGHTS_OUTPUT::LEFT);
				break;

			case RELAY_MODES::WARN_FLASH:
				Set_Port_Out(LIGHTS_OUTPUT::BOTH);
				break;

			case RELAY_MODES::EMERG_BREAK:
				Set_Port_Out(LIGHTS_OUTPUT::NONE);
				break;

			case RELAY_MODES::IDLE:
			default:
				Set_Port_Out(LIGHTS_OUTPUT::NONE);
				break;
		}


		switch (actMode)
		{
			case RELAY_MODES::R_FLASH:
				Set_R_Light(ON);
				Set_L_Light(OFF);
				break;

			case RELAY_MODES::L_FLASH:
				Set_R_Light(OFF);
				Set_L_Light(ON);
				break;

			case RELAY_MODES::WARN_FLASH:
				Set_Both_Lights(ON);
				break;

			case RELAY_MODES::EMERG_BREAK:
				Set_Both_Lights(OFF);
				break;

			case RELAY_MODES::IDLE:
			default:
				Set_Both_Lights(OFF);
				break;
		}


		Mode = actMode;
	}


	// -----------------------------------------------------

	bool new_flash_state = Flash_State;


	// handle flashing -------------------------------------

	if (Mode != RELAY_MODES::IDLE)
	{
		if (Flash_State && (millis() - LastTime_Toggle > Flash_ON_Time))
		{
			new_flash_state = OFF;
		}
		else if (!Flash_State && (millis() - LastTime_Toggle > Flash_OFF_Time))
		{
			new_flash_state = ON;
			HandleFlashTime();
		}
	}


	// handle lights changes ------------------------------
	// 

	if (new_flash_state != Flash_State)
	{
		switch (Mode)
		{
			case RELAY_MODES::R_FLASH:
				Set_Port_Out(new_flash_state ? LIGHTS_OUTPUT::RIGHT : LIGHTS_OUTPUT::NONE);
				break;

			case RELAY_MODES::L_FLASH:
				Set_Port_Out(new_flash_state ? LIGHTS_OUTPUT::LEFT : LIGHTS_OUTPUT::NONE);
				break;

			case RELAY_MODES::WARN_FLASH:
				Set_Port_Out(new_flash_state ? LIGHTS_OUTPUT::BOTH : LIGHTS_OUTPUT::NONE);
				break;

			case RELAY_MODES::EMERG_BREAK:
				Set_Port_Out(new_flash_state ? LIGHTS_OUTPUT::BOTH : LIGHTS_OUTPUT::NONE);
				break;

			case RELAY_MODES::IDLE:
			default:
				// nothing to do here
				break;
		}


		switch (Mode)
		{
			case RELAY_MODES::R_FLASH:
				Set_R_Light(new_flash_state);
				break;

			case RELAY_MODES::L_FLASH:
				Set_L_Light(new_flash_state);
				break;

			case RELAY_MODES::WARN_FLASH:
				Set_Both_Lights(new_flash_state);
				break;

			case RELAY_MODES::EMERG_BREAK:
				Set_Both_Lights(new_flash_state);
				break;

			case RELAY_MODES::IDLE:
			default:
				// nothing to do here
				break;
		}


		Flash_State = new_flash_state;
		LastTime_Toggle = millis();
	}


}

// ************************************************************************************************************


