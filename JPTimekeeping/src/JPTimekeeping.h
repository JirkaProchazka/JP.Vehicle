/*
 Name:		JPTimekeeping.h
 Created:	4/1/2020 12:51:39 AM
 Author:	JP
 Editor:	http://www.visualmicro.com
*/

#ifndef _JPTimekeeping_h
#define _JPTimekeeping_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif


#include "Wire.h"


namespace JP
{
	namespace Timekeeping
	{
		struct DateTime
		{

		public:
			static char DateSeparator;
			static String DayNames[7];
			static String DayNamesShort[7];

			byte Year = 20;
			byte Month = 1;
			byte DayOfMonth = 1;
			byte DayOfWeek = 1;
			byte Hours = 0;
			byte Minutes = 0;
			byte Seconds = 0;

			unsigned long TimeRelatedMillis = 0;


			void TimeToChar(char*& buffer, char* format = "%2d:%02d:%02d")
			{
				sprintf(buffer, format, Hours, Minutes, Seconds);
			}

			String TimeToString()
			{
				return  GetLeadingZero(Hours, " ") + ":" + GetLeadingZero(Minutes) + ":" + GetLeadingZero(Seconds);
			}
			String TimeShortToString()
			{
				return  GetLeadingZero(Hours, " ") + ":" + GetLeadingZero(Minutes);
			}
			String DateToString()
			{
				return  "20" + String(Year, DEC) + DateSeparator + GetLeadingZero(Month) + DateSeparator + GetLeadingZero(DayOfMonth);
			}
			String DateTimeToString(bool mostSignificantFirst = true)
			{
				if (mostSignificantFirst)
					return DateToString() + " " + TimeToString();
				else return TimeToString() + " " + DateToString();
			}


			String DayName()
			{
				return DayNames[DayOfWeek-1];
			}
			String DayNameShort()
			{
				return DayNamesShort[DayOfWeek-1];
			}


			// formating 
			//
			String GetLeadingZero(byte timePart, char* leadingChar = "0")
			{
				return (timePart < 10 ? leadingChar : "") + String(timePart, DEC);
			}
			void GetLeadingZeroFast(byte timePart, char* buffer)
			{
				sprintf(buffer, "%02d", timePart);
			}

		};

		class Timekeeper
		{
		private:
			int I2C_ADDR;
			int NUMBER_OF_TRANSMISSION_BYTES = 7;
			DateTime Actual;

			byte shortMonths[7] = { 1,3,5,7,8,10,12 };
			byte longMonths[4] = { 4,6,9,11 };
			bool IsEndOfMonth();
			bool IsLeapYear();

			void SetInnerActual(byte seconds, byte minutes, byte hours, byte dayOfWeek, byte dayOfMonth, byte month, byte year, unsigned long relatedMillis);
			void SetDevice(byte seconds, byte minutes, byte hours, byte dayOfWeek, byte dayOfMonth, byte month, byte year);
			void ReadDevice(byte* seconds, byte* minutes, byte* hours, byte* dayOfWeek, byte* dayOfMonth, byte* month, byte* year);

		public:
			void Init(int i2c_addr = 0x68);

			// Get Time ---------------------------------
			void RefreshActual();
			DateTime GetActual();
			void GetActual(DateTime* dateTime);
			// Set Time ---------------------------------
			void SetDateTime(DateTime dateTime, unsigned long relatedTime = millis());

			void Timekeeper::AddSecond(byte seconds = 1);
			void Timekeeper::AddMinute();
			void Timekeeper::AddHour();
			void Timekeeper::AddDay();
			
			void Timekeeper::AddMonth();
			void Timekeeper::AddYear();

		};

		//extern Timekeeper Clock;


		// Converting
		byte decToBcd(byte val);
		byte bcdToDec(byte val);

	}
}

#endif

