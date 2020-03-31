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


			void AddSecond()
			{
				if (Seconds >= 59)
				{
					Seconds = 0;
					AddMinute();
				}
				else Seconds++;
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
				}
				else Hours++;
			}


			String TimeToString()
			{
				return  GetLeadingZero(Hours, " ") + ":" + GetLeadingZero(Minutes) + ":" + GetLeadingZero(Seconds);
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
				return DayNames[DayOfWeek];
			}
			String DayNameShort()
			{
				return DayNamesShort[DayOfWeek];
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

			void SetInnerActual(byte seconds, byte minutes, byte hours, byte dayOfWeek, byte dayOfMonth, byte month, byte year);

		public:
			void Init(int i2c_addr = 0x68);

			// Get Time ---------------------------------
			void RefreshActual();
			DateTime GetActual();
			void GetActual(DateTime* dateTime);

			void ReadDevice(byte* seconds, byte* minutes, byte* hours, byte* dayOfWeek, byte* dayOfMonth, byte* month, byte* year);

			// Set Time ---------------------------------
			void SetDateTime(DateTime dateTime);
			void SetDevice(byte seconds, byte minutes, byte hours, byte dayOfWeek, byte dayOfMonth, byte month, byte year);
		};

		//extern Timekeeper Clock;


		// Converting
		byte decToBcd(byte val);
		byte bcdToDec(byte val);

	}
}

#endif

