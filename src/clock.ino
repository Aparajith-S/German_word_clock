/*
*   *****    *****       *******    *****   *******      *      *   *     *   ******
*   *       **              *      **          *         *      *   *     *   *    **
*   ***      *****          *       *****      *         *      *   *******   ****** 
*   *            **         *           **     *         *      *   *     *   *  **
*   *****    *****       *******    *****      *          ******    *     *   *   **  ........
*   @author : s.aparajith@live.com
*   @date   : 07.10.2022
*   @details : This application will configure a DS3231 RTC chip to store time during compilation. 
*              Then the RTC is supported by the battery to keep time even when the system is powered off. 
*              The output will be displayed on a custom LED German word display to tell the time in words. precise to +/- 3 mins.
*              The clock adapts itself for Daylight savings in Germany.
*/

//#############################################################################
// INCLUDES
//#############################################################################
#include <Wire.h>  // must be included here so that Arduino library object file references work
#include <RtcDS3231.h> // for rtc
#include <EepromAT24C32.h>
//#############################################################################
// MACROS
//#############################################################################

#define countof(a) (sizeof(a) / sizeof(a[0]))
#define MUX_S3 ((int)12)
#define MUX_S2 ((int)11)
#define MUX_S1 ((int)10)
#define MUX_S0 ((int)9)
#define M_VIERTEL ((int)2)
#define ES_IST ((int)3)
#define M_FUENF ((int)4)
#define M_ZEHN ((int)5)
#define M_ZWANZIG ((int)20)
#define VOR ((int)6)
#define NACH ((int)21)
#define HALB ((int)7)
#define S_EINS ((int)13)
#define UHR ((int)14)
#define N_EINS ((int)15)
#define EI_EINS_DREI ((int)16)
#define DR_DREI ((int)17)

//#############################################################################
// TYPEDEFS
//#############################################################################
typedef struct {
  int hour;
  int min;
  int sec;
  int year;
  int month;
  int day;}
   timedate;

enum DST{
  DST_CORRECTION_FORWARD=1,
  DST_CORRECTION_BACKWARD=0
};
//#############################################################################
// GLOBAL VARIABLES
//#############################################################################
RtcDS3231<TwoWire> Rtc(Wire);
EepromAt24c32<TwoWire> RtcEeprom(Wire);
bool stringComplete=false;
String inputString="";
bool low_batt_blink = false;
bool TOGGLE = LOW;
//#############################################################################
// FUNCTION DEFINITIONS
//#############################################################################
/*
* @brief Clears the Display
*/
void clearDisplay()
{
  digitalWrite(M_VIERTEL ,LOW);
  digitalWrite(ES_IST  ,LOW);
  digitalWrite(M_FUENF ,LOW);
  digitalWrite(M_ZEHN  ,LOW);
  digitalWrite(M_ZWANZIG ,LOW);
  digitalWrite(VOR ,LOW);
  digitalWrite(NACH ,LOW);
  digitalWrite(HALB ,LOW);
  digitalWrite(S_EINS ,LOW);
  digitalWrite(UHR ,LOW);
  digitalWrite(N_EINS ,LOW);
  digitalWrite(EI_EINS_DREI ,LOW);
  digitalWrite(DR_DREI ,LOW);
}

/*
* @brief  Displays the correct hour on the LED display. 
* @param [in] hour: hour to be displayed
* @param [in] isUhr: flag to turn the "Uhr" LED display on.
*/
void hourDisplay(int hour, bool isUhr = false)
{

  
    digitalWrite(N_EINS, LOW);
    digitalWrite(EI_EINS_DREI, LOW);
    digitalWrite(N_EINS, LOW);
    digitalWrite(DR_DREI, LOW);
    switch (hour)
    {
    case 1:
        digitalWrite(N_EINS, HIGH);
        digitalWrite(EI_EINS_DREI, HIGH);
        if (isUhr)
        {
            digitalWrite(S_EINS, LOW);
        }
        else
        {
            digitalWrite(S_EINS, HIGH);
        }
        digitalWrite(MUX_S0, LOW);
        digitalWrite(MUX_S1, HIGH);
        digitalWrite(MUX_S2, HIGH);
        digitalWrite(MUX_S3, HIGH);

        break;
    case 2:
    digitalWrite(MUX_S0, LOW);
    digitalWrite(MUX_S1, LOW);
        digitalWrite(MUX_S2, HIGH);
        digitalWrite(MUX_S3, HIGH);
        break;
    case 3:
        digitalWrite(DR_DREI, HIGH);
        digitalWrite(EI_EINS_DREI, HIGH);
        digitalWrite(MUX_S0, LOW);
        digitalWrite(MUX_S1, HIGH);
        digitalWrite(MUX_S2, HIGH);
        digitalWrite(MUX_S3, HIGH);
        break;
    case 4:
        digitalWrite(MUX_S0, HIGH);
        digitalWrite(MUX_S1, HIGH);
        digitalWrite(MUX_S2, LOW);
        digitalWrite(MUX_S3, LOW);
        break;
    case 5:
    digitalWrite(MUX_S0, LOW);
    digitalWrite(MUX_S1, LOW);
    digitalWrite(MUX_S2, LOW);
    digitalWrite(MUX_S3, LOW);
        break;
    case 6:
    digitalWrite(MUX_S0, LOW);
    digitalWrite(MUX_S1, LOW);
        digitalWrite(MUX_S2, HIGH);
        digitalWrite(MUX_S3, LOW);
        break;
    case 7:
        digitalWrite(MUX_S0, HIGH);
        digitalWrite(MUX_S1, HIGH);
        digitalWrite(MUX_S2, HIGH);
        digitalWrite(MUX_S3, LOW);
        break;
    case 8:
    digitalWrite(MUX_S0, LOW);
    digitalWrite(MUX_S1, LOW);
    digitalWrite(MUX_S2, LOW);
        digitalWrite(MUX_S3, HIGH);
        break;
    case 9:

        digitalWrite(MUX_S1, HIGH);
        digitalWrite(MUX_S2, LOW);
        digitalWrite(MUX_S3, LOW);
            digitalWrite(MUX_S0, LOW);
        break;
    case 10:
        digitalWrite(MUX_S0, HIGH);
        digitalWrite(MUX_S1, HIGH);
        digitalWrite(MUX_S2, LOW);
        digitalWrite(MUX_S3, HIGH);

        break;
    case 11:
        digitalWrite(MUX_S0, HIGH);
        digitalWrite(MUX_S1, HIGH);
        digitalWrite(MUX_S2, HIGH);
        digitalWrite(MUX_S3, HIGH);
        break;
    case 0:
    case 12:
        digitalWrite(MUX_S0, HIGH);
        digitalWrite(MUX_S1, LOW);
        digitalWrite(MUX_S2, HIGH);
        digitalWrite(MUX_S3, HIGH);
        break;
    default:
        digitalWrite(MUX_S1, HIGH);
        digitalWrite(MUX_S0, LOW);
        digitalWrite(MUX_S2, HIGH);
        digitalWrite(MUX_S3, HIGH);
        break;
    }
    if (isUhr)
    {
        digitalWrite(UHR, HIGH);
    }
    else
    {
        digitalWrite(UHR, LOW);
    }
}

/*
* @brief runs a power on self test of the display to show the user all the LEDs are working
*/
void POSTDisplay()
{
    digitalWrite(M_VIERTEL, HIGH);
    digitalWrite(ES_IST, HIGH);
    digitalWrite(M_FUENF, HIGH);
    digitalWrite(M_ZEHN, HIGH);
    digitalWrite(M_ZWANZIG, HIGH);
    digitalWrite(VOR, HIGH);
    digitalWrite(NACH, HIGH);
    digitalWrite(HALB, HIGH);
    digitalWrite(UHR, HIGH);
    for (int i = 0; i <= 12; i++)
    {
        hourDisplay(i, i % 2);
        delay(250);
    }
}

/*
* @brief      checks for a leap year
* @param [in] year : in the format YYYY
* @return True : if year is leap year
* @return True : if year is not leap year
*/
bool isLeap(int year) 
{
    /*
    Leap year check:
    The year is multiple of 400.
    or
    The year is multiple of 4 and not multiple of 100.
    */
    if (((year % 400) == 0) || ((year % 4 == 0) && (year % 100 != 0)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
@brief : correction for Day light savings in Germany
@details : In Germany the DST is done by advancing one hour during end of Winter 
           i.e. when clock ticks during the last Sunday of march at 2 AM.
           And reducing one hour before Winter begins during last sunday of October at 3 AM.
@param [in] now : read time and date values from RTC.
@param [in] startup_check_flag : flag to differentiate call in setup() and call in loop()
@return : DST : whether to advance or reverse the clock hour hand.
*/
DST CorrectForDST(RtcDateTime const & now,
                  bool startup_check_flag=false)
{
  DST old_state = RtcEeprom.GetMemory(0);
  //DST begin
    if(now.Month()==3 && (31-now.Day())<8 && now.DayOfWeek()==0 &&
       now.Hour()==2 && now.Minute()==0)
  {
      if(startup_check_flag || DST_CORRECTION_FORWARD!=old_state)
      {
        //during startup donot correct the time but just set the eeprom with a valid value.
      if(!startup_check_flag)
      {RtcDateTime newdatetime = RtcDateTime(now.Year(),
      now.Month(),
      now.Day(),
      now.Hour()+1,
      now.Minute(),
      now.Second());
      Rtc.SetDateTime(newdatetime);
      }
      RtcEeprom.SetMemory(0, DST_CORRECTION_FORWARD); 
  }
  }
  else
  //DST end
  if(now.Month()==10 && (31-now.Day())<8 && now.DayOfWeek()==0
     &&now.Hour()==3 && now.Minute()==0)
  {
      if(startup_check_flag || DST_CORRECTION_BACKWARD!=old_state)
      {
      //during startup donot correct the time but just set the eeprom with a valid value.
      if(!startup_check_flag) 
      { RtcDateTime newdatetime = RtcDateTime(now.Year(),
      now.Month(),
      now.Day(),
      now.Hour()-1,
      now.Minute(),
      now.Second());
      Rtc.SetDateTime(newdatetime);
      }
      RtcEeprom.SetMemory(0, DST_CORRECTION_BACKWARD);
      }
  }
  else
  {
    //initialize the eeprom
    if(startup_check_flag)
    {
      if(now.Month()>10 || now.Month()<3 || (now.Month()==3 && (31-now.Day())>8))
      {
        RtcEeprom.SetMemory(0, DST_CORRECTION_BACKWARD);
      }
      if((now.Month()<10 && now.Month()>3 ) || (now.Month()==10 && (31-now.Day())>8))
      {
        RtcEeprom.SetMemory(0, DST_CORRECTION_FORWARD);
      }
    }
  }

  return RtcEeprom.GetMemory(0);
}

void setup() {
   Serial.begin(57600);

    pinMode(MUX_S0, OUTPUT);
    pinMode(MUX_S1, OUTPUT);
    pinMode(MUX_S2, OUTPUT);
    pinMode(MUX_S3, OUTPUT);
    pinMode(M_VIERTEL, OUTPUT);
    pinMode(ES_IST, OUTPUT);
    pinMode(M_FUENF, OUTPUT);
    pinMode(M_ZEHN, OUTPUT);
    pinMode(M_ZWANZIG, OUTPUT);
    pinMode(VOR, OUTPUT);
    pinMode(NACH, OUTPUT);
    pinMode(HALB, OUTPUT);
    pinMode(S_EINS, OUTPUT);
    pinMode(UHR, OUTPUT);
    pinMode(N_EINS, OUTPUT);
    pinMode(EI_EINS_DREI, OUTPUT);
    pinMode(DR_DREI, OUTPUT);
    POSTDisplay();
    clearDisplay();
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

    if (!Rtc.IsDateTimeValid()) {
        if (Rtc.LastError() != 0) {
            // we have a communications error
            // see https://www.arduino.cc/en/Reference/WireEndTransmission for
            // what the number means
            ;
        }
        else {

            Rtc.SetDateTime(compiled);
            low_batt_blink=true;
        }
    }

    if (!Rtc.GetIsRunning()) {

        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) {

        Rtc.SetDateTime(compiled);
    }
    now = Rtc.GetDateTime();
    CorrectForDST(now,true);
    
    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
}

void loop() {
  if (!Rtc.IsDateTimeValid()) {
    if (Rtc.LastError() != 0) {
      // we have a communications error
      // see https://www.arduino.cc/en/Reference/WireEndTransmission for
      // what the number means

    } else {
      // Common Causes:
      //    1) the battery on the device is low or even missing and the power line was 
      //       disconnected

      low_batt_blink=true;
    }
  }

  RtcDateTime now = Rtc.GetDateTime(); 
  CorrectForDST(now);
  printDateTime(now);

  // blinks the UHR display when the coin cell battery is low/missing. 
  if(low_batt_blink)
  {
    TOGGLE^=HIGH;
    digitalWrite(UHR, TOGGLE);
  }
  delay(1500);
  
  //Setting the date is important to track DST in germany.
if (Serial.available()) 
{
  inputString = Serial.readString();
  Serial.println(inputString.length());
  timedate newDT;
  bool error = false;
  //Handle Date:
  if (inputString.substring(0, 4) == "DATE") 
  {
    if (inputString.length() != 14) 
    {
      error = true;
    } 
    else {
        if (isdigit(inputString[4]) && (isdigit(inputString[5])) && isdigit(inputString[7]) &&
            (isdigit(inputString[8])) && isdigit(inputString[10]) && (isdigit(inputString[11])) &&
            isdigit(inputString[12]) && (isdigit(inputString[13])))
        {
            newDT.day = (inputString[4] - '0') * 10 + (inputString[5] - '0');
            newDT.month = (inputString[7] - '0') * 10 + (inputString[8] - '0');
            newDT.year = (inputString[10] - '0') * 1000 + (inputString[11] - '0') * 100 + 
                         (inputString[12] - '0') * 10 + (inputString[13] - '0');
            //implausible ranges.
            if ((newDT.day < 1 && newDT.day>31) || (newDT.month < 1 && newDT.month>12))
            {
                error = true;
            }
            else
                //handle february
                if (newDT.month == 2)
                {
                    if (isLeap(newDT.year))
                    {
                        if (newDT.day > 29)
                        {
                            error = true;
                        }
                    }
                    else
                    {
                        if (newDT.day > 28)
                        {
                            error = true;
                        }
                    }
                }
                else if (newDT.month <= 7)
                {//barring february, from january to july all the even months have 30 days
                    if (newDT.month % 2 == 0)
                    {
                        if (newDT.day > 30)
                        {
                            error = true;
                        }
                    }
                }
                else
                { //august to december all the odd months have 30 days
                    if ((newDT.month) % 2)
                    {
                        if (newDT.day > 30)
                        {
                            error = true;
                        }
                    }
                }
        }
        else
        {
            // input string is bad
            error = true;
        }
    }

    if (error != true)
    {
      
      RtcDateTime newdatetime = RtcDateTime(newDT.year,
      newDT.month,
      newDT.day,
      now.Hour(),
      now.Minute(),
      now.Second());
      Rtc.SetDateTime(newdatetime);
    }
    else 
    {
    Serial.println("inputString error please use DATEDD:MM:YYYY format with valid date fields");
    }
  }
  // Handle Time:
  else if (inputString.substring(0, 4) == "TIME") 
  {
    
    if (inputString.length() != 12) 
    {
      error = true;
    } 
    else 
    {
      if (isdigit(inputString[4]) && (isdigit(inputString[5])) && isdigit(inputString[7]) && 
          (isdigit(inputString[8])) && isdigit(inputString[10]) && (isdigit(inputString[11]))) 
      {

        newDT.hour = (inputString[4]-'0')*10+(inputString[5]-'0');
        newDT.min =  (inputString[7]-'0')*10+(inputString[8]-'0');
        newDT.sec =  (inputString[10]-'0')*10+(inputString[11]-'0');
        //check for implausible time
        if(newDT.hour>23 || newDT.min>59||newDT.sec>59)
        {
          error =true;
        }

      }
    }
  if (error != true)
    {
      RtcDateTime newdatetime = RtcDateTime(now.Year(),
      now.Month(),
      now.Day(),
      newDT.hour,
      newDT.min,
      newDT.sec);
      Rtc.SetDateTime(newdatetime);
    }
    else 
    {
    Serial.println("inputString error please use TIMEHH:MM:SS format with valid 24h time fields");
    }
  }
  else
  {
    Serial.println("inputString error please use TIMEHH:MM:SS format with valid 24h time fields or DATEDD:MM:YYYY format with valid date fields");
  }
}
}


/*
* @brief: decision tree for displaying the correct time in words.
* @details: expressing time in words has a minor caveat. accuracy is limited to +/- 3 mins.
* @param [in] : dt : date-time object.
*/
void printDateTime(const RtcDateTime& dt) {
    char datestring[40];
    int hour = dt.Hour();
    int min = dt.Minute();
    clearDisplay();
    digitalWrite(ES_IST, HIGH);
    if (min <= 2) {
        if ((hour % 12) == 1)
        {
            hourDisplay(1, true);
        }
        else
        {
            hourDisplay(hour % 12, true);
        }
    }
    else if (min <= 7)
    {
        digitalWrite(M_FUENF, HIGH);
        digitalWrite(NACH, HIGH);
        hourDisplay(hour % 12);

    }
    else if (min <= 12) {
        digitalWrite(M_ZEHN, HIGH);
        digitalWrite(NACH, HIGH);
        hourDisplay(hour % 12);
    }
    else if (min <= 17) {
        digitalWrite(M_VIERTEL, HIGH);
        digitalWrite(NACH, HIGH);
        hourDisplay(hour % 12);
    }
    else if (min <= 22) {
        digitalWrite(M_ZWANZIG, HIGH);
        digitalWrite(NACH, HIGH);
        hourDisplay(hour % 12);
    }
    else if (min <= 27) {
        digitalWrite(M_FUENF, HIGH);
        digitalWrite(VOR, HIGH);
        digitalWrite(HALB, HIGH);
        hourDisplay((hour + 1) % 12);
    }
    else if (min <= 32) {
        digitalWrite(HALB, HIGH);
        hourDisplay((hour + 1) % 12);
    }
    else if (min <= 37)
    {
        digitalWrite(M_FUENF, HIGH);
        digitalWrite(NACH, HIGH);
        digitalWrite(HALB, HIGH);
        hourDisplay((hour + 1) % 12);
    }
    else if (min <= 42)
    {
        digitalWrite(M_ZWANZIG, HIGH);
        digitalWrite(VOR, HIGH);
        hourDisplay((hour + 1) % 12);
    }
    else if (min <= 47) {
        digitalWrite(M_VIERTEL, HIGH);
        digitalWrite(VOR, HIGH);
        hourDisplay((hour + 1) % 12);
    }
    else if (min <= 52) {
        digitalWrite(M_ZEHN, HIGH);
        digitalWrite(VOR, HIGH);
        hourDisplay((hour + 1) % 12);
    }
    else if (min <= 57)
    {
        digitalWrite(M_FUENF, HIGH);
        digitalWrite(VOR, HIGH);
        hourDisplay((hour + 1) % 12);
    }
    else {
        if (hour == 12 || hour == 0) {
            hourDisplay(1, true);
        }
        else {
            hourDisplay((hour + 1) % 12, true);
        }
    }
}