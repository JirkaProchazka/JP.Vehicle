/*
 Name:		JPTimekeeping_test.ino
 Created:	4/1/2020 12:52:03 AM
 Author:	JP
*/




// the setup function runs once when you press reset or power the board
#include <Wire.h>
#include <JPTimekeeping.h>

using namespace JP::Timekeeping;


enum I2C_ADDRESSESS
{
	RTC_MODULE = 0x68,
};

JP::Timekeeping::Timekeeper RTC;

String TimeString;

// const unsigned long TimeReadPeriod = 1000 * 60 * 30; // 30 min

const unsigned long TimeRefreshPeriod = 1000; // 
unsigned long LastTimeRefresh = 0;

const int TimeStringChangePeriod = 1000 * 60; // 1 sec
unsigned long NextTimeStringChange = 0;


const int TestPrintPeriod = 1000 * 30;
unsigned long LastTestPrint = 0;

void setup() {

	Wire.begin();
	Serial.begin(9600);

	DateTime::DateSeparator = '-';

	RTC.Init(I2C_ADDRESSESS::RTC_MODULE);

	/*
   JP::Timekeeping::DateTime d;
   d.Hours = 9;
   d.Minutes = 57;
   d.Seconds = 00;
   d.Month = 3;
   d.DayOfMonth = 31;
   d.DayOfWeek = 3;
   d.Year = 20;
   */
   // Clock.SetDateTime(d);
   // Clock.SetDateTime(30,42,21,4,26,11,14);


	RTC.RefreshActual();
	auto dateTime = RTC.GetActual();
	TimeString = dateTime.DateTimeToString();
	LastTimeRefresh = dateTime.TimeRelatedMillis;

	NextTimeStringChange = millis() + (60 * 1000) - (dateTime.Seconds * 1000);

	Serial.print(dateTime.DateTimeToString());
	Serial.print(" ");
	Serial.print(dateTime.DayName());
	Serial.println();
	

}

// the loop function runs over and over again until power down or reset
void loop() {

	// Device time - incremented by second
	auto diff = millis() - LastTimeRefresh;
	if (millis() - LastTimeRefresh >= TimeRefreshPeriod)
	{
		auto secNumber = diff / 1000;
		RTC.AddSecond(secNumber);
		LastTimeRefresh += 1000;
	}

	// Change time string with one minute period
	if (millis() >= NextTimeStringChange)
	{
		TimeString = RTC.GetActual().TimeToString();
		NextTimeStringChange += (60 * 1000);
	}


	// Obecne prepisovani displaye
	if (millis() - LastTestPrint >= TestPrintPeriod)
	{
		Serial.println(TimeString);
		LastTestPrint = millis();
	}

}
