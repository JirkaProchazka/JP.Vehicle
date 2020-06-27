/*
 Name:		TurnSignalRelay.ino
 Created:	6/9/2020 13:29:33
 Author:	JP
 */

#define INVERSE_OUTPUT true
#define OUTPUT_BY_PORT true

 // DEFINICITON OF EXPRESSIONS

constexpr bool ON = !INVERSE_OUTPUT;
constexpr bool OFF = !ON;

constexpr bool RELEASED = true;
constexpr bool PUSHED = !RELEASED;

constexpr unsigned long RESET_TIME = 0x0;

// PORT DEFINITION

// ARDUINO PINS
enum PINS
{
	L_TURN_SW = 8,
	R_TURN_SW = 9,

	L_LIGHTS = 4,
	R_LIGHTS = 5,
};

// Inputs

#define INPUT_PORT PINB
constexpr byte R_SWITCH_PORT_MASK = (1 << PINB0);
constexpr byte L_SWITCH_PORT_MASK = (1 << PINB1);

// Outputs

#if OUTPUT_BY_PORT

#if INVERSE_OUTPUT

#define OUTPUT_PORT PORTD
constexpr byte R_LIGHT_PIN_MASK = ~(1 << PIND4);
constexpr byte L_LIGHT_PIN_MASK = ~(1 << PIND5);
constexpr byte LIGHTS_PORT_MASK = L_LIGHT_PIN_MASK & R_LIGHT_PIN_MASK;

enum LIGHTS_OUTPUT
{
	NONE = B11111111,
	RIGHT = R_LIGHT_PIN_MASK,
	LEFT = L_LIGHT_PIN_MASK,
	BOTH = (RIGHT & LEFT),
};

#else

#define OUTPUT_PORT PORTD
constexpr byte R_LIGHT_PIN_MASK = (1 << PIND4);
constexpr byte L_LIGHT_PIN_MASK = (1 << PIND5);
constexpr byte LIGHTS_PORT_MASK = ~(L_LIGHT_PIN_MASK | R_LIGHT_PIN_MASK);

enum LIGHTS_OUTPUT
{
	NONE = 0x0,
	RIGHT = R_LIGHT_PIN_MASK,
	LEFT = L_LIGHT_PIN_MASK,
	BOTH = (RIGHT | LEFT),
};

#endif
#endif


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

unsigned long R_Sw_LastReadChangeTime = RESET_TIME;
unsigned long L_Sw_LastReadChangeTime = RESET_TIME;


// - flashing variables ----------------------------

const int START_BLINK_ON_TIME = 250;
const int START_BLINK_OFF_TIME = 250;

const int FLASH_COUNTER_INCREMENT_LIMIT = 3;

const int FLASH_ON_INCREMENT_TIME = 100;
const int FLASH_OFF_INCREMENT_TIME = 100;

int Flash_ON_Time = START_BLINK_ON_TIME;
int Flash_OFF_Time = START_BLINK_OFF_TIME;

int FlashCounter = 0;

bool Flash_State = OFF;
unsigned long Flash_LastTime = RESET_TIME;


// =============================================================================================================

// Setup output pins

#if OUTPUT_BY_PORT

void Set_Port_Out(LIGHTS_OUTPUT output)
{
	byte maskedPort = OUTPUT_PORT & LIGHTS_PORT_MASK;
	OUTPUT_PORT = maskedPort | output;
}

#else

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

#endif

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
// =============================================================================================================


void setup() {

	//Serial.begin(9600);

#if OUTPUT_BY_PORT
	Set_Port_Out(LIGHTS_OUTPUT::NONE);
#else
	Set_Both_Lights(OFF);
#endif 


	// INPUT pins
	pinMode(R_TURN_SW, INPUT_PULLUP);
	pinMode(L_TURN_SW, INPUT_PULLUP);

	// OUTPUT pins
	pinMode(R_LIGHTS, OUTPUT);
	pinMode(L_LIGHTS, OUTPUT);

}


// =============================================================================================================
// =============================================================================================================


void loop() {

	// - read inputs ---------------------------------------

#if OUTPUT_BY_PORT
	bool act_R_Sw = INPUT_PORT & R_SWITCH_PORT_MASK;
	bool act_L_Sw = INPUT_PORT & L_SWITCH_PORT_MASK;
#else
	bool act_R_Sw = digitalRead(R_TURN_SW);
	bool act_L_Sw = digitalRead(L_TURN_SW);
#endif

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

	// - handle stable changes ------------------------------

	bool inputChange = false;
	if ((R_Sw_Stable != R_Sw_LastState) && (millis() - R_Sw_LastReadChangeTime > DEBOUNCE_PROTECTION_TIME))
	{
		R_Sw_Stable = R_Sw_LastState;
		inputChange = true;
	}
	if ((L_Sw_Stable != L_Sw_LastState) && (millis() - L_Sw_LastReadChangeTime > DEBOUNCE_PROTECTION_TIME))
	{
		L_Sw_Stable = L_Sw_LastState;
		inputChange = true;
	}


	// Set mode if input changed ---------------------------
	//

	RELAY_MODES actMode = Mode;

	if (inputChange)
	{
		if (R_Sw_Stable == PUSHED)
		{
			actMode = (L_Sw_Stable == PUSHED) ? RELAY_MODES::WARN_FLASH : RELAY_MODES::R_FLASH;
		}
		else
		{
			actMode = (L_Sw_Stable == PUSHED) ? RELAY_MODES::L_FLASH : RELAY_MODES::IDLE;
		}
	}

	// Handle mode change ----------------------------------
	//

	if (actMode != Mode)
	{
		Flash_State = OFF;
		ResetFlashTime();

#if OUTPUT_BY_PORT

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

#else
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

#endif

		Mode = actMode;

		//Serial.print("Mode change: ");
		//Serial.print(Mode);
		//Serial.println();
	}



	// handle flashing -------------------------------------
	// 

	bool new_flash_state = Flash_State;

	if (Mode != RELAY_MODES::IDLE)
	{
		if ((Flash_State == ON) && (millis() - Flash_LastTime > Flash_ON_Time))
		{
			new_flash_state = OFF;
		}
		else if ((Flash_State == OFF) && (millis() - Flash_LastTime > Flash_OFF_Time))
		{
			new_flash_state = ON;
			HandleFlashTime();
		}
	}


	// Set lights if flash changed ------------------------------
	// 

	if (new_flash_state != Flash_State)
	{

#if OUTPUT_BY_PORT

		switch (Mode)
		{
		case RELAY_MODES::R_FLASH:
			Set_Port_Out((new_flash_state == ON) ? LIGHTS_OUTPUT::RIGHT : LIGHTS_OUTPUT::NONE);
			break;

		case RELAY_MODES::L_FLASH:
			Set_Port_Out((new_flash_state == ON) ? LIGHTS_OUTPUT::LEFT : LIGHTS_OUTPUT::NONE);
			break;

		case RELAY_MODES::WARN_FLASH:
			Set_Port_Out((new_flash_state == ON) ? LIGHTS_OUTPUT::BOTH : LIGHTS_OUTPUT::NONE);
			break;

		case RELAY_MODES::EMERG_BREAK:
			Set_Port_Out((new_flash_state == ON) ? LIGHTS_OUTPUT::BOTH : LIGHTS_OUTPUT::NONE);
			break;

		case RELAY_MODES::IDLE:
		default:
			// nothing to do here
			break;
		}

#else

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
#endif

		Flash_State = new_flash_state;
		Flash_LastTime = millis();
	}


}

// ************************************************************************************************************


