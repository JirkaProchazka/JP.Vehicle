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
			static char TimeSeparator;
			static String DayNames[7];
			static char* DayNamesShort[7];
			static byte LeapYears[];

#pragma region DateTime Components

			byte Year = 20;
			byte Month = 1;
			byte DayOfMonth = 1;
			byte DayOfWeek = 1;
			byte Hours = 0;
			byte Minutes = 0;
			byte Seconds = 0;

			unsigned long TimeRelatedMillis = 0;

			String DayName()
			{
				return DayNames[DayOfWeek - 1];
			}
			String DayNameShort()
			{
				return DayNamesShort[DayOfWeek - 1];
			}

#pragma endregion

#pragma region Adjust Time

			void AddSecond(byte seconds = 1)
			{
				for (size_t i = 0; i < seconds; i++)
				{
					if (Seconds >= 59)
					{
						Seconds = 0;
						AddMinute();
					}
					else Seconds++;
				}
			}
			void AddMinute()
			{
				if (Minutes >= 59)
				{
					Minutes = 0;
					AddHour();
				}
				else Minutes++;
			}
			void AddHour()
			{
				if (Hours >= 23)
				{
					Hours = 0;
					AddDay();
				}
				else Hours++;
			}
			void AddDay()
			{
				if (DayOfWeek >= 7)
				{
					DayOfWeek = 1;
				}
				else DayOfWeek++;

				if (IsEndOfMonth())
				{
					DayOfMonth = 1;
					AddMonth();
				}
				else DayOfMonth++;
			}
			void AddMonth()
			{
				if (Month == 12)
				{
					Month = 1;
					AddYear();
				}
				else Month++;
			}
			void AddYear()
			{
				Year++;
			}

			bool IsEndOfMonth()
			{
				switch (Month)
				{
					case 2:
					{
						if (DayOfMonth >= 29 || ((DayOfMonth >= 28) && IsInLeapYear()))
							return true;
						break;
					}
					case 4:
					case 6:
					case 9:
					case 11:
					{
						if (DayOfMonth >= 30)
							return true;
						break;
					}
					default:
						if (DayOfMonth >= 31)
							return true;
						break;
				}

				return false;
			}
			bool IsInLeapYear()
			{
				switch (Year)
				{
					case 24:
					case 28:
					case 32:
					case 36:
					case 40:
					case 44:
					case 48:
					case 52:
						return true;
				}

				return false;
			}

#pragma endregion

#pragma region DateTime Strings 

			void TimeToChar(char*& buffer, char* format = "%2d:%02d:%02d")
			{
				sprintf(buffer, format, Hours, Minutes, Seconds);
			}

			String TimeToString()
			{
				return  GetLeadingZero(Hours, " ") + TimeSeparator + GetLeadingZero(Minutes) + TimeSeparator + GetLeadingZero(Seconds);
			}
			String TimeShortToString()
			{
				return  GetLeadingZero(Hours, " ") + TimeSeparator + GetLeadingZero(Minutes);
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

#pragma endregion


			// formating 
			//
			String GetLeadingZero(byte timePart, char* leadingChar = "0")
			{
				return (timePart < 10 ? leadingChar : " ") + String(timePart, DEC);
			}
			void GetLeadingZeroFast(byte timePart, char* buffer)
			{
				sprintf(buffer, "%02d", timePart);
			}

		};




		class RTC_DS3231
		{
		private:
			int I2C_ADDR;
			int NUMBER_OF_TRANSMISSION_BYTES = 7;
			DateTime LastRead;

			void SetInnerActual(byte seconds, byte minutes, byte hours, byte dayOfWeek, byte dayOfMonth, byte month, byte year, unsigned long relatedMillis);
			void SetDevice(byte seconds, byte minutes, byte hours, byte dayOfWeek, byte dayOfMonth, byte month, byte year);
			void ReadDevice(byte* seconds, byte* minutes, byte* hours, byte* dayOfWeek, byte* dayOfMonth, byte* month, byte* year);

		public:
			void Init(int i2c_addr = 0x68);


			void RefreshTime();
			DateTime GetLastRead();
			void GetLastRead(DateTime* dateTime);

			void SetDateTime(DateTime dateTime, unsigned long relatedTime = millis());
		};

		//extern RTC_DS3231 Clock;


		// Converting
		byte decToBcd(byte val);
		byte bcdToDec(byte val);

	}
}

#endif

