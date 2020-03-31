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

const int TimeRefreshPeriod = 1000;

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

   if(millis() - lastRefresh)

   RTC.RefreshActual();
   auto dateTime = RTC.GetActual();
   Serial.println(dateTime.TimeToString());

   delay(1000);
   
}
