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
		// STATIC DECLARATION
		char DateTime::DateSeparator = '-';

		String DateTime::DayNames[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
		String DateTime::DayNamesShort[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };




		void Timekeeper::Init(int i2c_addr = 0x68)
		{
			I2C_ADDR = i2c_addr;

			Actual.Year = 20;
			Actual.Month = 1;
			Actual.DayOfMonth = 1;
			Actual.DayOfWeek = 3;
			Actual.Hours = 0;
			Actual.Minutes = 0;
			Actual.Seconds = 0;
		}


		void Timekeeper::SetInnerActual(byte seconds, byte minutes, byte hours, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
		{
			Actual.Seconds = seconds;
			Actual.Minutes = minutes;
			Actual.Hours = hours;
			Actual.DayOfWeek = dayOfWeek;
			Actual.DayOfMonth = dayOfMonth;
			Actual.Month = month;
			Actual.Year = year;
		}



		void Timekeeper::ReadDevice(byte* second, byte* minute, byte* hour, byte* dayOfWeek, byte* dayOfMonth, byte* month, byte* year) {
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


		void Timekeeper::RefreshActual()
		{
			ReadDevice(&Actual.Seconds, &Actual.Minutes, &Actual.Hours, &Actual.DayOfWeek, &Actual.DayOfMonth, &Actual.Month, &Actual.Year);
		}

		DateTime Timekeeper::GetActual()
		{
			return Actual;
		}
		void Timekeeper::GetActual(DateTime* dateTime)
		{
			dateTime->Seconds = Actual.Seconds;
			dateTime->Minutes = Actual.Minutes;
			dateTime->Hours = Actual.Hours;
			dateTime->DayOfWeek = Actual.DayOfWeek;
			dateTime->DayOfMonth = Actual.DayOfMonth;
			dateTime->Month = Actual.Month;
			dateTime->Year = Actual.Year;
		}


		void Timekeeper::SetDateTime(DateTime dateTime)
		{
			SetInnerActual(dateTime.Seconds, dateTime.Minutes, dateTime.Hours, dateTime.DayOfWeek, dateTime.DayOfMonth, dateTime.Month, dateTime.Year);
			SetDevice(dateTime.Seconds, dateTime.Minutes, dateTime.Hours, dateTime.DayOfWeek, dateTime.DayOfMonth, dateTime.Month, dateTime.Year);
		}

		void Timekeeper::SetDevice(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year) {

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


		// GENERAL FUNCTION

		// Converting
		//
		byte decToBcd(byte val) {
			return((val / 10 * 16) + (val % 10));
		}
		byte bcdToDec(byte val) {
			return((val / 16 * 10) + (val % 16));
		}


	}
}