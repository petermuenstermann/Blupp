#include <Arduino.h>
#include <time.h>                       // time() ctime()
#include <sys/time.h>                   // struct timeval
#include <coredecls.h>                  // settimeofday_cb()

#include "biorb.h"
#include "network.h"

////////////////////////////////////////////////////////

#define TZ              1       // (utc+) TZ in hours
#define DST_MN          0

uint32_t color_codes[16] = {
  BIORB_RED,
  BIORB_GREEN ,
  BIORB_BLUE,
  BIORB_WHITE,
  BIORB_BROWN,
  BIORB_WOOD,
  BIORB_SKY,
  BIORB_PINK,
  BIORB_MEADOWGREEN,
  BIORB_DEEPVIOLETT,
  BIORB_ORANGE,
  BIORB_LIGHTGREEN,
  BIORB_VIOLETT,
  BIORB_YELLOW,
  BIORB_PETROL,
  BIORB_PURPLE
};

void SetBiorbColor(time_t SecondsSince1970)
{
  struct tm *timeinfo = localtime(&SecondsSince1970);
  static int8_t lastSec = 0;

  //  Serial.print(ctime(&SecondsSince1970));
  int8_t h = timeinfo->tm_hour;
  int8_t m = timeinfo->tm_min;
  int8_t s = timeinfo->tm_sec;

  if ( s == 0 && s != lastSec) {


    // night:
    if ( (m % 10) == 0 && (h <= 6 || h >= 19 ) ) SetColor(BIORB_BLUE, BIORB_BRIGHTNESS_40);

    // sun rise
    else if ( m == 20 && h == 6 ) SetColor(BIORB_ORANGE, BIORB_BRIGHTNESS_40);
    else if ( m == 40 && h == 6 ) SetColor(BIORB_WHITE, BIORB_BRIGHTNESS_40);
    else if ( m ==  0 && h == 7 ) SetColor(BIORB_WHITE, BIORB_BRIGHTNESS_60);
    else if ( m == 20 && h == 7 ) SetColor(BIORB_WHITE, BIORB_BRIGHTNESS_80);
    else if ( m == 40 && h == 7 ) SetColor(BIORB_WHITE, BIORB_BRIGHTNESS_100);

    // sun set
    else if ( m ==  0 && h == 18 ) SetColor(BIORB_WHITE, BIORB_BRIGHTNESS_80);
    else if ( m == 20 && h == 18 ) SetColor(BIORB_WHITE, BIORB_BRIGHTNESS_60);
    else if ( m == 40 && h == 18 ) SetColor(BIORB_WHITE, BIORB_BRIGHTNESS_40);

  }
  lastSec = s;
}


////////////////////////////////////////////////////////

#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)

timeval cbtime;			// time set in callback
bool cbtime_set = false;
void time_is_set (void)
{
  gettimeofday(&cbtime, NULL);
  cbtime_set = true;
}

void setup() {

  pinMode(IR_PIN, OUTPUT);
  digitalWrite(IR_PIN, LOW);

  Serial.begin(115200);
  delay(1000);

  Serial.println("\r\n");
  Serial.print("Chip ID: 0x");
  Serial.println(ESP.getChipId(), HEX);

  SendCode(BIORB_ON);
  SendCode(BIORB_ORANGE);

  settimeofday_cb(time_is_set);
  configTime(TZ_SEC, DST_SEC, "pool.ntp.org");

  networkInit();

}

// for testing purpose:
extern "C" int clock_gettime(clockid_t unused, struct timespec *tp);

timeval tv;
timespec tp;
time_t now;
boolean BiorbInitialized = 0;

void loop() {

  gettimeofday(&tv, nullptr);
  clock_gettime(0, &tp);

  if ( cbtime_set )  {
    now = time(nullptr);

    if (!BiorbInitialized)
    {
      struct tm *timeinfo = localtime(&now);

      Serial.print("setting Biorb status for ");
      Serial.print(ctime(&now));

      time_t past = now - ( timeinfo->tm_sec + 60 * (timeinfo->tm_min + 60 * timeinfo->tm_hour) );
      while ( past < now )  {
        SetBiorbColor(past);
        past++;
        if ( past % 100 == 0 ) yield();
      }

      BiorbInitialized = 1;
    }
    else SetBiorbColor( now );
  }

  HTTPHandleRequest();
  yield();
}
