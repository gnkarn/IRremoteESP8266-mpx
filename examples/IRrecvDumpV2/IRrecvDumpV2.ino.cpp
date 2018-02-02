# 1 "/var/folders/s5/4rjcd2z95pq6k_lc2cyrynhm0000gp/T/tmp3mgN70"
#include <Arduino.h>
# 1 "/Volumes/D5-INFO1/IRremoteESP8266-mpx/examples/IRrecvDumpV2/IRrecvDumpV2.ino"
# 20 "/Volumes/D5-INFO1/IRremoteESP8266-mpx/examples/IRrecvDumpV2/IRrecvDumpV2.ino"
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#if DECODE_AC
#include <ir_Daikin.h>
#include <ir_Fujitsu.h>
#include <ir_Kelvinator.h>
#include <ir_Midea.h>
#include <ir_Toshiba.h>
#endif




#define RECV_PIN D2






#define BAUD_RATE 115200



#define CAPTURE_BUFFER_SIZE 128
# 65 "/Volumes/D5-INFO1/IRremoteESP8266-mpx/examples/IRrecvDumpV2/IRrecvDumpV2.ino"
#if DECODE_AC
#define TIMEOUT 50U


#else
#define TIMEOUT 15U
#endif
# 97 "/Volumes/D5-INFO1/IRremoteESP8266-mpx/examples/IRrecvDumpV2/IRrecvDumpV2.ino"
#define MIN_UNKNOWN_SIZE 12




IRrecv irrecv(RECV_PIN, CAPTURE_BUFFER_SIZE, TIMEOUT, true);

decode_results results;
void dumpACInfo(decode_results *results);
void setup();
void loop();
#line 107 "/Volumes/D5-INFO1/IRremoteESP8266-mpx/examples/IRrecvDumpV2/IRrecvDumpV2.ino"
void dumpACInfo(decode_results *results) {
  String description = "";
#if DECODE_DAIKIN
  if (results->decode_type == DAIKIN) {
    IRDaikinESP ac(0);
    ac.setRaw(results->state);
    description = ac.toString();
  }
#endif
#if DECODE_FUJITSU_AC
  if (results->decode_type == FUJITSU_AC) {
    IRFujitsuAC ac(0);
    ac.setRaw(results->state, results->bits / 8);
    description = ac.toString();
  }
#endif
#if DECODE_KELVINATOR
  if (results->decode_type == KELVINATOR) {
    IRKelvinatorAC ac(0);
    ac.setRaw(results->state);
    description = ac.toString();
  }
#endif
#if DECODE_TOSHIBA_AC
  if (results->decode_type == TOSHIBA_AC) {
    IRToshibaAC ac(0);
    ac.setRaw(results->state);
    description = ac.toString();
  }
#endif
#if DECODE_MIDEA
  if (results->decode_type == MIDEA) {
    IRMideaAC ac(0);
    ac.setRaw(results->value);
    description = ac.toString();
  }
#endif

  if (description != "") Serial.println("Mesg Desc.: " + description);
}


void setup() {
  Serial.begin(BAUD_RATE, SERIAL_8N1, SERIAL_TX_ONLY);
  delay(500);

#if DECODE_HASH

  irrecv.setUnknownThreshold(MIN_UNKNOWN_SIZE);
#endif
  irrecv.enableIRIn();
}



void loop() {

  if (irrecv.decode(&results)) {

    uint32_t now = millis();
    Serial.printf("Timestamp : %06u.%03u\n", now / 1000, now % 1000);
    if (results.overflow)
      Serial.printf("WARNING: IR code is too big for buffer (>= %d). "
                    "This result shouldn't be trusted until this is resolved. "
                    "Edit & increase CAPTURE_BUFFER_SIZE.\n",
                    CAPTURE_BUFFER_SIZE);

    Serial.print(resultToHumanReadableBasic(&results));
    dumpACInfo(&results);
    yield();


    Serial.print("Library   : v");
    Serial.println(_IRREMOTEESP8266_VERSION_);
    Serial.println();


    Serial.println(resultToTimingInfo(&results));
    yield();


    Serial.println(resultToSourceCode(&results));
    Serial.println("");
    yield();
  }
}
