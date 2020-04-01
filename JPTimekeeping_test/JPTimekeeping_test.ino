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

bool DisplayRefreshRequired = true;

JP::Timekeeping::Timekeeper RTC;

//String TimeString;
char* TimeChars = new char[6];
char* TimeFormat = "%2d:%02d";

const unsigned long TimeReadPeriod = 1000 * 60 * 10; // 10 min


const unsigned long TimeRefreshPeriod = 1000; // 
unsigned long LastTimeRefresh = 0;

const int TimeStringChangePeriod = 1000 * 60; // 1 sec
unsigned long NextTimeStringChange = 0;


void setup() {

	Wire.begin();
	Serial.begin(9600);



	RTC.Init(I2C_ADDRESSESS::RTC_MODULE);
	DateTime::DateSeparator = '-';

	JP::Timekeeping::DateTime d;
	d.Hours = 13;
	d.Minutes = 25;
	d.Seconds = 00;
	d.Month = 4;
	d.DayOfMonth = 1;
	d.DayOfWeek = 4;
	d.Year = 20;

	//RTC.SetDateTime(30,42,21,4,26,11,14);
	//RTC.SetDateTime(d);

	RTC.RefreshActual();
	auto dateTime = RTC.GetActual();
	LastTimeRefresh = dateTime.TimeRelatedMillis;

	// TimeString = dateTime.DateTimeToString();
	dateTime.TimeToChar(TimeChars, TimeFormat);
	NextTimeStringChange = LastTimeRefresh + (60 * 1000UL) - (dateTime.Seconds * 1000) + 200; //limit

	// Print time.
	Serial.print(dateTime.DateTimeToString());
	Serial.print(" ");
	Serial.println(dateTime.DayName());
}

void loop() {

	// Read RTC once per given time
	if (millis() - RTC.GetActual().TimeRelatedMillis > TimeReadPeriod)
	{
		RTC.RefreshActual();
		auto dateTime = RTC.GetActual();
		LastTimeRefresh = dateTime.TimeRelatedMillis;

		// TimeString = dateTime.DateTimeToString();
		dateTime.TimeToChar(TimeChars, TimeFormat);
		NextTimeStringChange = LastTimeRefresh + (60 * 1000UL) - (dateTime.Seconds * 1000) + 200; //limit
	}

	// Increment Device Time - once per second
	auto diff = millis() - LastTimeRefresh;
	if (diff >= TimeRefreshPeriod)
	{
		auto secNumber = diff / 1000;
		RTC.AddSecond(secNumber);
		LastTimeRefresh += 1000;

		// Serial.print("Sec Incremented:  ");
		Serial.println(RTC.GetActual().TimeToString());
	}

	// Change time string
	if (millis() >= NextTimeStringChange)
	{
		// TimeString = RTC.GetActual().TimeToString();
		RTC.GetActual().TimeToChar(TimeChars, TimeFormat);

		NextTimeStringChange += (60 * 1000UL);
		// Serial.println("TimeStringChanged");

		DisplayRefreshRequired = true;
	}


	// Vypis displaye
	if (DisplayRefreshRequired)
	{
		// Serial.println(TimeString);
		Serial.println(TimeChars);

		DisplayRefreshRequired = false;
	}

}
