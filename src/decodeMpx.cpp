// Copyright 2009 Ken Shirriff
// Copyright 2017 David Conran
// based on Nikai , modified for MPx
#include <algorithm>
#include "IRrecv.h"
#include "IRsend.h"
// #include "IRutils.h"


//               MPX Decode
// use: http://asciiset.com/figletserver.html with the "letters" font.

// Constants
// Ref:
//   https://github.com/markszabo/IRremoteESP8266/issues/309
#define MPX_TICK             100U
#define MPX_HDR_MARK_TICKS     0U
#define MPX_HDR_MARK         (MPX_HDR_MARK_TICKS * MPX_TICK)
#define MPX_HDR_SPACE_TICKS    14U
#define MPX_HDR_SPACE        (MPX_HDR_SPACE_TICKS * MPX_TICK)
#define MPX_BIT_MARK_TICKS     28U
#define MPX_BIT_MARK         (MPX_BIT_MARK_TICKS * MPX_TICK)
#define MPX_ONE_SPACE_TICKS    14U
#define MPX_ONE_SPACE        (MPX_ONE_SPACE_TICKS * MPX_TICK)
#define MPX_ZERO_SPACE_TICKS   28U
#define MPX_ZERO_SPACE       (MPX_ZERO_SPACE_TICKS * MPX_TICK)
#define MPX_MIN_GAP_TICKS     100U
#define MPX_MIN_GAP          (MPX_MIN_GAP_TICKS * MPX_TICK)
#define MPX_BIT_onemark_TICKS 28U
#define MPX_BIT_zeromark_TICKS 14U

#if SEND_MPX
// Send a MPX TV formatted message.
//
// Args:
//   data:   The message to be sent.
//   nbits:  The bit size of the message being sent. typically MPX_BITS.
//   repeat: The number of times the message is to be repeated.
//
// Status: STABLE / Working.
//
// Ref: https://github.com/markszabo/IRremoteESP8266/issues/309
// NOT USED ON decode MPX
void IRsend::sendMPX(uint64_t data, uint16_t nbits, uint16_t repeat) {
  sendGeneric(MPX_HDR_MARK, MPX_HDR_SPACE,
              MPX_BIT_MARK, MPX_ONE_SPACE,
              MPX_BIT_MARK, MPX_ZERO_SPACE,
              MPX_BIT_MARK, MPX_MIN_GAP,
              data, nbits, 38, true, repeat, 33);
}
#endif

#if DECODE_MPX
// Decode the supplied MPX message.
//
// Args:
//   results: Ptr to the data to decode and where to store the decode result.
//   nbits:   Nr. of bits to expect in the data portion.
//            Typically MPX_BITS.
//   strict:  Flag to indicate if we strictly adhere to the specification.
// Returns:
//   boolean: True if it can decode it, false if it can't.
//
// Status: STABLE / Working.
//
bool IRrecv::decodeMPX(decode_results *results, uint16_t nbits, bool strict) {
  //if (results->rawlen < 2 * nbits + HEADER + FOOTER - 1)// original
  if (results->rawlen < 2 * nbits + 1 ) // el 1 es por el espacio del Header , inicio del frame
    return false;  // Can't possibly be a valid MPX message.
  if (strict && nbits != MPX_BITS)
    return false;  // We expect MPX to be a certain sized message.

  uint64_t data = 0;
  uint16_t offset = 1;

  // Header

//   if (!matchMark(results->rawbuf[offset], MPX_HDR_MARK)) return false;
  // Calculate how long the common tick time is based on the header mark.
//  uint32_t m_tick = results->rawbuf[offset++] * RAWTICK / MPX_HDR_MARK_TICKS; // sacado para debug

//  if (!matchSpace(results->rawbuf[offset], MPX_HDR_SPACE)) return false; // habilitar!
  // Calculate how long the common tick time is based on the header space.
  uint32_t s_tick = results->rawbuf[offset++] * RAWTICK /  MPX_HDR_SPACE_TICKS;

  uint32_t m_tick = s_tick; // gnk make both the same , no reason why not
  //Serial.print("m_tick: ") ;Serial.print(m_tick); // only for debug
  // Data

  // matchData(*data_ptr,  nbits,onemark, onespace,zeromark, zerospace,tolerance = TOLERANCE);
  match_result_t data_result = matchData(&(results->rawbuf[offset]), nbits,
                                         MPX_BIT_onemark_TICKS * MPX_TICK,
                                         MPX_ONE_SPACE_TICKS * MPX_TICK,
                                         MPX_BIT_zeromark_TICKS * MPX_TICK,
                                         MPX_ZERO_SPACE_TICKS * MPX_TICK);
  if (data_result.success == false) return false;
  data = data_result.data;
  offset += data_result.used;
  // Footer
// comentado pues no tiene footer
 // if (offset < results->rawlen &&
 //     !matchAtLeast(results->rawbuf[offset], MPX_MIN_GAP_TICKS * s_tick))
 //   return false;


  // Compliance

  // Success
  results->bits = nbits;
  results->value = data;
  results->decode_type = MPX;
  results->command = 0;
  results->address = 0;
  return true;
}

#endif
