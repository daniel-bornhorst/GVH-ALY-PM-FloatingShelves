#define DEBUG 1

#ifdef DEBUG
# define DEBUG_PRINTLN(x) Serial.println(x);
# define DEBUG_PRINT(x) Serial.print(x);
#else
# define DEBUG_PRINTLN(x)
# define DEBUG_PRINT(x)
#endif

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <Adafruit_NeoPixel.h>


// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (13)
#define PN532_MOSI (11)
#define PN532_SS   (10)
#define PN532_SS2   (9)
#define PN532_MISO (12)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

#define PIXEL_PIN1   5
#define PIXEL_PIN2   17
#define NUMPIXELS    1

Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN1, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel pixels2(NUMPIXELS, PIXEL_PIN2, NEO_RGB + NEO_KHZ800);

typedef struct {
  int cardRFID; 
  uint32_t hueVal;     // value of 1 - 15
} Card;

Card cardSet[6] = {
  { 1459236098, pixels.Color(255, 0, 0) },
  { 3140172802, pixels.Color(0, 255, 0) },
  { 2999925250, pixels.Color(0, 0, 255) },
  { 3279306516, pixels.Color(255, 0, 255) },
  { 22121023, pixels.Color(0, 255, 255) },
  { 3284421908, pixels.Color(255, 255, 0) }
};

// Uncomment just _one_ line below depending on how your breakout or shield
// is connected to the Arduino:

// Use this line for a breakout with a SPI connection:
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

Adafruit_PN532 nfc2(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS2);

// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
//Adafruit_PN532 nfc(PN532_SS);

// Or use this line for a breakout or shield with an I2C connection:
//Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10); // for Leonardo/Micro/Zero

  Serial.println();
  Serial.println("Hello!");
  delay(100);
  nfc.begin();

  nfc2.begin();

  delay(100);

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board 1");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5 - 1"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  delay(10);


  versiondata = nfc2.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board 2");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5 - 2"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  delay(10);

  pixels.begin();
  pixels2.begin();

  Serial.println("Waiting for an ISO14443A Card ...");
}

uint32_t oldCardid1;
uint32_t oldCardid2;

void loop(void) {
  nfc.begin();
  delay(10);
  nfc2.begin();
  delay(10);
  //Serial.println("Entering Loop 1");
  readerOneLoop();
  //Serial.println("Entering Loop 2");
  readerTwoLoop();
}


void readerOneLoop() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  //Serial.println("before NFC read loop 1");
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength,50);
  //Serial.println("After nfc read in loop 1");
  if (success) {
    // Display some basic information about the card
    // Serial.println("Found an ISO14443A card");
    // Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    // Serial.print("  UID Value: ");
    // nfc.PrintHex(uid, uidLength);

    if (uidLength == 4)
    {

      // We probably have a Mifare Classic card ...
      uint32_t cardid = uid[0];
      cardid <<= 8;
      cardid |= uid[1];
      cardid <<= 8;
      cardid |= uid[2];
      cardid <<= 8;
      cardid |= uid[3];

      //if (oldCardid1 != cardid) {

        pixels.clear();
        pixels.setPixelColor(0, matchIdToHue(cardid));
        pixels.show();

        Serial.println("----- 1 -----");

        Serial.print("Seems to be a Mifare Classic card #");
        Serial.println(cardid);
        Serial.println("");

        oldCardid1 = cardid;

        delay(100);
        pixels.clear();
        pixels.show();
      //}
    }
  }
}

void readerTwoLoop() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  //Serial.println("Before nfc read in loop 2");
  success = nfc2.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength,50);
  //Serial.println("After nfc read in loop 2");

  if (success) {
    // Display some basic information about the card
    // Serial.println("Found an ISO14443A card");
    // Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    // Serial.print("  UID Value: ");
    // nfc.PrintHex(uid, uidLength);

    if (uidLength == 4)
    {

      // We probably have a Mifare Classic card ...
      uint32_t cardid = uid[0];
      cardid <<= 8;
      cardid |= uid[1];
      cardid <<= 8;
      cardid |= uid[2];
      cardid <<= 8;
      cardid |= uid[3];

      //if (oldCardid2 != cardid) {

        pixels2.clear();
        pixels2.setPixelColor(0, matchIdToHue(cardid));
        pixels2.show();

        Serial.println("----- 2 -----");

        Serial.print("Seems to be a Mifare Classic card #");
        Serial.println(cardid);
        Serial.println("");

        oldCardid2 = cardid;

        delay(100);
        pixels2.clear();
        pixels2.show();
      //}
    }
  }
}

uint32_t matchIdToHue(int cardNum) {
  for(int i = 0; i < 6; ++i) {
    if(cardNum == cardSet[i].cardRFID) {
      return cardSet[i].hueVal;
    }
  }
}
