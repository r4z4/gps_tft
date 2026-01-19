#include <TinyGPSPlus.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char errMsg[25];

// void sleepDisplay(Adafruit_SSD1306* display) {
//   display->ssd1306_command(SSD1306_DISPLAYOFF);
// }
//
// void wakeDisplay(Adafruit_SSD1306* display) {
//   display->ssd1306_command(SSD1306_DISPLAYON);
// }
//
//
// void goToSleep() {
//   Serial.println("Going to sleep now");
//   Serial.flush();
//   sleepDisplay(&display);
//   esp_deep_sleep_start();
// }

#define GPS_BAUD 9600
int RXPin = 21;
int TXPin = 20;

// Create a TinyGPSPlus object
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
HardwareSerial gpsSerial(2);
int attempt = 0;

void setup() {
  // Start the Arduino hardware serial port at 9600 baud
  Serial.begin(9600);

  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXPin, TXPin);
  Serial.println("Serial 2 started at 9600 baud rate");
}

void loop() {
  // This sketch displays information every time a new sentence is correctly encoded.
  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      displayInfo();

  // If 5000 milliseconds pass and there are no characters coming in
  // over the software serial port, show a "No GPS detected" error
  if (millis() > 10000 && gps.charsProcessed() < 10) {
    attempt++;
    sprintf(errMsg, sizeof(errMsg), "No GPS detected. %d", attempt);
    Serial.println(errMsg);
    printToScreen(errMsg, "");
    while (true)
      ;
  }
}

void printToScreen(char* printStr, char* printStr2) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println(printStr);
  display.println(printStr2);
  display.display();
  delay(3000);
  display.clearDisplay();
  display.display(); // Display the cleared buffer
}

void displayInfo() {
  if (gps.location.isValid()) {
    float lat = gps.location.lat();
    float lon = gps.location.lng();
    // float alt = gps.altitude.meters();
    char lat_message[20];
    char lon_message[20];
    snprintf(lat_message, sizeof(lat_message), "Lat: %.1f", lat);
    snprintf(lon_message, sizeof(lon_message), "Lon: %.1f", lon);
    // Publish the string
    Serial.println(lat_message);
    Serial.println(lon_message);
    printToScreen(lat_message, lon_message);
    // Serial.print("Latitude: ");
    // Serial.println(gps.location.lat(), 6);
    // Serial.print("Longitude: ");
    // Serial.println(gps.location.lng(), 6);
    // Serial.print("Altitude: ");
    // Serial.println(gps.altitude.meters());
  } else {
    Serial.println("Location: Not Available");
  }

  Serial.print("Date: ");
  if (gps.date.isValid()) {
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.println(gps.date.year());
  } else {
    Serial.println("Not Available");
  }

  Serial.print("Time: ");
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(":");
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(":");
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(".");
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.println(gps.time.centisecond());
  } else {
    Serial.println("Not Available");
  }

  Serial.println();
  Serial.println();
  delay(1000);
}
