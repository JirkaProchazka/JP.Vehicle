/*
 Name:		JPTimekeeping.cpp
 Created:	4/1/2020 12:51:39 AM
 Author:	JP
 Editor:	http://www.visualmicro.com
*/

#include "JPTimekeeping.h"

namespace JP
{
	namespace Timekeeping
	{

#pragma region DateTime struct

		// STATIC DECLARATION
		char DateTime::DateSeparator = '-';
		char DateTime::TimeSeparator = ':';

		String DateTime::DayNames[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
		char* DateTime::DayNamesShort[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
		byte DateTime::LeapYears[8] = { 24, 28, 32, 36, 40, 44, 48, 52 };

#pragma endregion


#pragma region Timekeeper RTC

		void RTC_DS3231::Init(int i2c_addr = 0x68)
		{
			I2C_ADDR = i2c_addr;

			LastRead.Year = 20;
			LastRead.Month = 1;
			LastRead.DayOfMonth = 1;
			LastRead.DayOfWeek = 3;
			LastRead.Hours = 0;
			LastRead.Minutes = 0;
			LastRead.Seconds = 0;

			LastRead.TimeRelatedMillis = 0;
		}

		void RTC_DS3231::SetInnerActual(byte seconds, byte minutes, byte hours, byte dayOfWeek, byte dayOfMonth, byte month, byte year, unsigned long relatedMillis = millis())
		{
			LastRead.Seconds = seconds;
			LastRead.Minutes = minutes;
			LastRead.Hours = hours;
			LastRead.DayOfWeek = dayOfWeek;
			LastRead.DayOfMonth = dayOfMonth;
			LastRead.Month = month;
			LastRead.Year = year;

			LastRead.TimeRelatedMillis = relatedMillis;
		}




		void RTC_DS3231::RefreshTime()
		{
			ReadDevice(&LastRead.Seconds, &LastRead.Minutes, &LastRead.Hours, &LastRead.DayOfWeek, &LastRead.DayOfMonth, &LastRead.Month, &LastRead.Year);
			LastRead.TimeRelatedMillis = millis();
		}

		DateTime RTC_DS3231::GetLastRead()
		{
			return LastRead;
		}
		void RTC_DS3231::GetLastRead(DateTime* dateTime)
		{
			dateTime->Seconds = LastRead.Seconds;
			dateTime->Minutes = LastRead.Minutes;
			dateTime->Hours = LastRead.Hours;
			dateTime->DayOfWeek = LastRead.DayOfWeek;
			dateTime->DayOfMonth = LastRead.DayOfMonth;
			dateTime->Month = LastRead.Month;
			dateTime->Year = LastRead.Year;
			dateTime->TimeRelatedMillis = LastRead.TimeRelatedMillis;
		}
		

		void RTC_DS3231::SetDateTime(DateTime dateTime, unsigned long relatedTime = millis())
		{
			SetDevice(dateTime.Seconds, dateTime.Minutes, dateTime.Hours, dateTime.DayOfWeek, dateTime.DayOfMonth, dateTime.Month, dateTime.Year);
			SetInnerActual(dateTime.Seconds, dateTime.Minutes, dateTime.Hours, dateTime.DayOfWeek, dateTime.DayOfMonth, dateTime.Month, dateTime.Year, relatedTime);
		}


		void RTC_DS3231::ReadDevice(byte* second, byte* minute, byte* hour, byte* dayOfWeek, byte* dayOfMonth, byte* month, byte* year) {
			Wire.beginTransmission(I2C_ADDR);
			Wire.write(0);							// set DS3231 register pointer to 00h
			Wire.endTransmission();
			Wire.requestFrom(I2C_ADDR, NUMBER_OF_TRANSMISSION_BYTES);

			// request seven bytes of data from DS3231 starting from register 00h
			*second = bcdToDec(Wire.read() & 0x7f);
			*minute = bcdToDec(Wire.read());
			*hour = bcdToDec(Wire.read() & 0x3f);
			*dayOfWeek = bcdToDec(Wire.read());
			*dayOfMonth = bcdToDec(Wire.read());
			*month = bcdToDec(Wire.read());
			*year = bcdToDec(Wire.read());
		}
		void RTC_DS3231::SetDevice(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year) {

			// sets time and date data to DS3231
			Wire.beginTransmission(I2C_ADDR);
			Wire.write(0); // set next input to start at the seconds register
			Wire.write(decToBcd(second));		// set seconds
			Wire.write(decToBcd(minute));		// set minutes
			Wire.write(decToBcd(hour));			// set hours
			Wire.write(decToBcd(dayOfWeek));	// set day of week (1=Sunday, 7=Saturday)
			Wire.write(decToBcd(dayOfMonth));	// set date (1 to 31)
			Wire.write(decToBcd(month));		// set month (1-12)
			Wire.write(decToBcd(year));			// set year (0 to 99)
			Wire.endTransmission();
		}


#pragma endregion	


		// Converting bytes 
		//
		byte decToBcd(byte val) {
			return((val / 10 * 16) + (val % 10));
		}
		byte bcdToDec(byte val) {
			return((val / 16 * 10) + (val % 16));
		}


	}
}