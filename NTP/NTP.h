#pragma once
#include "Arduino.h"
#include <Wire.h>
#include <Udp.h>

#define SEVENZYYEARS 2208988800UL
#define NTP_PACKET_SIZE 48
#define NTP_DEFAULT_LOCAL_PORT 1337
#define DS3231_REG 0x68
#define SECOND_RED 0x00

class NTPClient {
  private:
    UDP*          _udp;
    bool          _udpSetup       = false;

    const char*   _poolServerName = "pool.ntp.org"; // Default time server
    int           _port           = NTP_DEFAULT_LOCAL_PORT;
    long          _timeOffset     = 0;

    unsigned long _updateInterval = 60000;  // In ms

    unsigned long _currentEpoc    = 0;      // In s
    unsigned long _lastUpdate     = 0;      // In ms

    byte          _packetBuffer[NTP_PACKET_SIZE];

    void          sendNTPPacket();

  public:
    NTPClient(UDP& udp);
    NTPClient(UDP& udp, long timeOffset);
    NTPClient(UDP& udp, const char* poolServerName);
    NTPClient(UDP& udp, const char* poolServerName, long timeOffset);
    NTPClient(UDP& udp, const char* poolServerName, long timeOffset, unsigned long updateInterval);

    /**
     * Set time server name
     *
     * @param poolServerName
     */
    void setPoolServerName(const char* poolServerName);

    /**
     * Starts the underlying UDP client with the default local port
     */
    void begin();

    /**
     * Starts the underlying UDP client with the specified local port
     */
    void begin(int port);

    /**
     * This should be called in the main loop of your application. By default an update from the NTP Server is only
     * made every 60 seconds. This can be configured in the NTPClient constructor.
     *
     * @return true on success, false on failure
     */
    bool update();

    /**
     * This will force the update from the NTP Server.
     *
     * @return true on success, false on failure
     */
    bool forceUpdate();

    int getDay() const;
    int getHours() const;
    int getMinutes() const;
    int getSeconds() const;

    /**
     * Changes the time offset. Useful for changing timezones dynamically
     */
    void setTimeOffset(int timeOffset);

    /**
     * Set the update interval to another frequency. E.g. useful when the
     * timeOffset should not be set in the constructor
     */
    void setUpdateInterval(unsigned long updateInterval);

    /**
     * @return time formatted like `hh:mm:ss`
     */
    String getFormattedTime() const;

    /**
     * @return time in seconds since Jan. 1, 1970
     */
    unsigned long getEpochTime() const;

    /**
     * Stops the underlying UDP client
     */
    void end();
    uint8_t dec_to_bcd(int val) { //Convert decimal to BCD
    	return (uint8_t) val + 6 * ( val / 10 );
    }

    int bcd_to_dec(uint8_t val) { // Convert BCD to decimal
  	return val - 6 * ( val >> 4 );
    }

    String getFullFormattedTime() { // get formatted time from NTP
	String diw[] = {"Sun   ", "Mon   ", "Tue   ", "Wed   ", "Thurs ", "Fri   ", "Sat   "};
   	time_t rawtime = getEpochTime();
   	struct tm * ti;
   	ti = localtime (&rawtime);
   	return String(ti->tm_year + 1900) + "-" + String((ti->tm_mon + 1)/10) + String((ti->tm_mon + 1)%10) + "-" + String((ti->tm_mday)/10) + String((ti->tm_mday)%10) + " " +
          String(ti->tm_hour/10) + String(ti->tm_hour%10) + ":" + String(ti->tm_min/10) + String(ti->tm_min%10) + ":" + String(ti->tm_sec/10) + String(ti->tm_sec%10) +
		diw[ti->tm_wday];
    } 

    void setRTCTime() { // set NTP time for RTC
   	time_t rawtime = getEpochTime();
   	struct tm * ti;
   	ti = localtime (&rawtime);
  	Wire.beginTransmission(DS3231_REG);
  	Wire.write(SECOND_RED);
  	Wire.write(dec_to_bcd(ti->tm_sec));
  	Wire.write(dec_to_bcd(ti->tm_min));
  	Wire.write(dec_to_bcd(ti->tm_hour));
  	Wire.write(dec_to_bcd(ti->tm_wday + 1));
  	Wire.write(dec_to_bcd(ti->tm_mday));
  	Wire.write(dec_to_bcd(ti->tm_mon+1));
  	Wire.write(dec_to_bcd(ti->tm_year%100));
  	Wire.endTransmission(DS3231_REG);
    }

    int getRTCTime(String & Date, String & Time, int & diw) { // get formatted time from RTC
  	Wire.beginTransmission(DS3231_REG);
  	Wire.write(SECOND_RED);
  	Wire.requestFrom(DS3231_REG, 7, 1);
  	int s = bcd_to_dec (Wire.read());
  	int m = bcd_to_dec (Wire.read());
  	int h = bcd_to_dec (Wire.read());
  	diw = bcd_to_dec (Wire.read()) - 1;
  	int d = bcd_to_dec (Wire.read());
  	int mo = bcd_to_dec (Wire.read());
  	int y = bcd_to_dec (Wire.read());
  	Wire.endTransmission(DS3231_REG);
	Date = "Date: " + String(2000 + y) + "-" + String(mo/10) + String(mo%10) + "-" + String(d/10) + String(d%10) + "  ";
	Time = "Time: " + String(h/10) + String(h%10) + ":" + String(m/10) + String(m%10) + ":" + String(s/10) + String(s%10) + "    ";
  	return  s;
    }

     void setRTCTime_offline(String Date, String Time) { // set NTP time for RTC
	int diw = changeToDay(Date);
  	Wire.beginTransmission(DS3231_REG);
	int sec = (Time[12] - '0') * 10 + Time[13] - '0';
	int min = (Time[9] - '0') * 10 + Time[10] - '0';
	int hour = (Time[6] - '0') * 10 + Time[7] - '0';
	int day = (Date[14] - '0') * 10 + Date[15] - '0';
	int mon = (Date[11] - '0') * 10 + Date[12] - '0';
	int year = (Date[8] - '0') * 10 + Date[9] - '0';
  	Wire.write(SECOND_RED);
  	Wire.write(dec_to_bcd(sec));
  	Wire.write(dec_to_bcd(min));
  	Wire.write(dec_to_bcd(hour));
  	Wire.write(dec_to_bcd(diw + 1));
  	Wire.write(dec_to_bcd(day));
  	Wire.write(dec_to_bcd(mon));
  	Wire.write(dec_to_bcd(year));
  	Wire.endTransmission(DS3231_REG);
    }
//-------------------------------------------------
int changeToDay(String date)
{
	int totalDays = 0;
	int currentDay;
	int currentMonth;
	int currentYear;

	// get currentYear
	currentYear = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + (date[8] - '0') * 10 + date[9] - '0';
	
	// get currentMonth
	currentMonth = (date[11] - '0') * 10 + date[12] - '0';
	// get currentDay
	currentDay = (date[14] -'0') * 10 + date[15] - '0';
	
	switch(currentMonth)
	{
		case 1: totalDays+= (currentYear-2000)*365; break;
		case 2: totalDays+= (currentYear-2000)*365 +31; break;
		case 3: totalDays+= (currentYear-2000)*365 +59; break;
		case 4: totalDays+= (currentYear-2000)*365 +90; break;
		case 5: totalDays+= (currentYear-2000)*365 +120; break;
		case 6: totalDays+= (currentYear-2000)*365 +151; break;
		case 7: totalDays+= (currentYear-2000)*365 +181; break;
		case 8: totalDays+= (currentYear-2000)*365 +212; break;
		case 9: totalDays+= (currentYear-2000)*365 +243; break;
		case 10: totalDays+=(currentYear-2000)*365 +273; break;
		case 11: totalDays+=(currentYear-2000)*365 +304; break;
		case 12: totalDays+=(currentYear-2000)*365 +334; break;
	}

	totalDays += (currentYear - 2000) / 4 + 1;
	if (currentMonth <= 2 && currentYear % 4 == 0) totalDays--;
	totalDays += currentDay;	
	return((totalDays + 5) % 7);
}
//--------------------------------------------------
};
