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


JP::Timekeeping::RTC_DS3231 RTC;

//String TimeString;
DateTime ActualTime;
char* TimeChars = new char[6];
char* TimeFormat = "%2d:%02d";


const unsigned long TimeReadPeriod = 1000 * 60 * 10; // 10 min

const unsigned long TimeRefreshPeriod = 1000; // 
unsigned long LastTimeRefresh = 0;

const int TimeStringChangePeriod = 1000 * 60; // 1 sec
unsigned long NextTimeStringChange = 0;




void setup() {

	// Communication BUSes
	Serial.begin(9600);
	Wire.begin();
	
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

	LoadTimeFromRTC();

	// Print time.
	Serial.print(ActualTime.DateTimeToString());
	Serial.print(" ");
	Serial.println(ActualTime.DayName());
}

void loop() {

	// Read RTC once per given time
	if (millis() - RTC.GetLastRead().TimeRelatedMillis > TimeReadPeriod)
	{
		LoadTimeFromRTC();
	}

	// Increment Device Time - once per second
	auto diff = millis() - LastTimeRefresh;
	if (diff >= TimeRefreshPeriod)
	{
		auto secNumber = diff / 1000;
		ActualTime.AddSecond(secNumber);
		LastTimeRefresh += 1000;

		// Serial.print("Sec Incremented:  ");
		Serial.println(RTC.GetLastRead().TimeToString());
	}

	// Change time string
	if (millis() >= NextTimeStringChange)
	{
		// TimeString = RTC.GetLastRead().TimeToString();
		RTC.GetLastRead().TimeToChar(TimeChars, TimeFormat);

		NextTimeStringChange += (60 * 1000UL);
		// Serial.println("TimeStringChanged");

		DisplayRefreshRequired = true;
	}


	// print to output
	if (DisplayRefreshRequired)
	{
		// Serial.println(TimeString);
		Serial.println(TimeChars);

		DisplayRefreshRequired = false;
	}

}



void LoadTimeFromRTC()
{
	RTC.RefreshTime();
	ActualTime = RTC.GetLastRead();
	LastTimeRefresh = ActualTime.TimeRelatedMillis;

	// TimeString = dateTime.DateTimeToString();
	ActualTime.TimeToChar(TimeChars, TimeFormat);
	NextTimeStringChange = LastTimeRefresh + (60 * 1000UL) - (ActualTime.Seconds * 1000) + 200; //limit
}
