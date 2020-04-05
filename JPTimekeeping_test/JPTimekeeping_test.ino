/*
 Name:		JPTimekeeping_test.ino
 Created:	4/1/2020 12:52:03 AM
 Author:	JP
*/


#include <Wire.h>
#include <JPTimekeeping.h>

using namespace JP::Timekeeping;


enum I2C_ADDRESSESS
{
	RTC_MODULE = 0x68,
};
enum PINS
{

};

JP::Timekeeping::Timekeeper RTC;

const unsigned long TimeReadPeriod = 1000 * 60 * 30; // 30 min

const unsigned long TimeRefreshPeriod = 1000; // 
unsigned long LastTimeRefresh = 0;

const int TimePrintPeriod = 1000; // 1 sec
unsigned long LastTimePrint = 0;

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
	Serial.print(dateTime.DateTimeToString());
	Serial.print(" ");
	Serial.print(dateTime.DayName());
	Serial.println();


}

// the loop function runs over and over again until power down or reset
void loop() {

	if (millis() - RTC.GetActual().TimeRelatedMillis > TimeReadPeriod)
	{
		RTC.RefreshActual();
	}
	else if (millis() - LastTimeRefresh > TimeRefreshPeriod)
	{
		RTC.AddSecond();
		LastTimeRefresh = millis();
	}
	if (millis() - LastTimePrint >= TimePrintPeriod)
	{
		Serial.println(RTC.GetActual().TimeToString());
		LastTimePrint = millis();
	}

}
