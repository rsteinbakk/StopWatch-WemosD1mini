//#include <Arduino.h> //nødvendig?
#include <NTPClient.h> // Henter tid fra internett
#include "NewPing.h" // Til avstandsmåler
#include <WiFiUdp.h>
#include <Firebase_ESP_Client.h>

// Avstandsmåler
#define TRIGGER_PIN D1
#define ECHO_PIN D2
// LED-lys
#define LED_GREEN D7
#define LED_RED D0
// Maximum distance we want to ping for (in centimeters).
#define MAX_DISTANCE 400

// NewPing setup of pins and maximum distance.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
float duration, distance;

// FIREBASE initier
#include <addons/TokenHelper.h>                           //Provide the token generation process info.
#include <addons/RTDBHelper.h>                            //Provide the RTDB payload printing info and other helper functions.
#define API_KEY "AIzaSyBroihBK9boV7M2uUwXQqHWYP_nQynB1KA" /* Firebase api-nøkkel */
#define DATABASE_URL "rs-prototyping-default-rtdb.firebaseio.com"

// Definer Firebase dataobjekt
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool taskCompleted = false; // Forhindrer i å starte firebaseoperasjon dersom oppgave ikke er fullført.

// Logg inn på wifi og start server.
const char *ssid = "Best i test";
const char *password = "Margareth";
WiFiServer server(80); // Set web server port number to 80

// Variabler for å lagre HTTP forespørsel
String header;
unsigned long currentTime = millis(); // Current time
unsigned long previousTime = 0;       // Previous time
const long timeoutTime = 2000;        // Define timeout time in milliseconds (example: 2000ms = 2s)

// Klargjør laser
const int pinLaser = D1;    // pin som kontrollerer laser
const int pinDetector = D8; // pin som kontrollerer laser-avleser
String laserState = "off";

String startTid = "";
String stoppTid = "";

// Info om deltager
String currentCompetitor = "";
bool competitorReady = false;

// Definer NTP klient for å kunne få tid fra internett.
const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);





















/* 
void setupFirebase()
{
}

void runFirebase()
{
  //Flash string (PROGMEM and  (FPSTR), String, String C/C++ string, const char, char array, string literal are supported
  //in all Firebase and FirebaseJson functions, unless F() macro is not supported.

  if (!Firebase.ready() && !taskCompleted)
  {
    taskCompleted = true;

    Serial.printf("Set timestamp... %s\n", Firebase.RTDB.setTimestamp(&fbdo, "/test/timestamp") ? "ok" : fbdo.errorReason().c_str());

    if (fbdo.httpCode() == FIREBASE_ERROR_HTTP_CODE_OK)
    {

      //In setTimestampAsync, the following timestamp will be 0 because the response payload was ignored for all async functions.

      //Timestamp saved in millisecond, get its seconds from int value
      Serial.print("TIMESTAMP (Seconds): ");
      Serial.println(fbdo.to<int>());

      //Or print the total milliseconds from double value
      //Due to bugs in Serial.print in Arduino library, use printf to print double instead.
      printf("TIMESTAMP (milliSeconds): %lld\n", fbdo.to<uint64_t>());
    }

       
    
    
    
    
    Serial.printf("Get timestamp... %s\n", Firebase.RTDB.getDouble(&fbdo, "/test/timestamp") ? "ok" : fbdo.errorReason().c_str());
    if (fbdo.httpCode() == FIREBASE_ERROR_HTTP_CODE_OK)
      printf("TIMESTAMP: %lld\n", fbdo.to<uint64_t>());

    //To set and push data with timestamp, requires the JSON data with .sv placeholder
    FirebaseJson json;

    json.set("Data", "Hello");
    //now we will set the timestamp value at Ts
    json.set("Ts/.sv", "timestamp"); // .sv is the required place holder for sever value which currently supports only string "timestamp" as a value

    //Set data with timestamp
    Serial.printf("Set data with timestamp... %s\n", Firebase.RTDB.setJSON(&fbdo, "/test/set/data", &json) ? fbdo.to<FirebaseJson>().raw() : fbdo.errorReason().c_str());

    //Push data with timestamp
    Serial.printf("Push data with timestamp... %s\n", Firebase.RTDB.pushJSON(&fbdo, "/test/push/data", &json) ? "ok" : fbdo.errorReason().c_str());

    //Get previous pushed data
    Serial.printf("Get previous pushed data... %s\n", Firebase.RTDB.getJSON(&fbdo, "/test/push/data/" + fbdo.pushName()) ? fbdo.to<FirebaseJson>().raw() : fbdo.errorReason().c_str());
  }
}
*/