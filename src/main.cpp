/*********
  Roger Steinbakk
  www.rogersteinbakk.no
*********/

#include <innstillinger.h>

void setup()
{

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // Klargjøre laser, setup()-del
  pinMode(pinDetector, INPUT); //
  pinMode(pinLaser, OUTPUT);
  // digitalWrite(pinLaser, HIGH); // starter laser

  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output

  // Koble til wifi med  SSID and password
  Serial.print("Kobler til WiFi ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  // Print lokal IP addresse og start web server
  Serial.println("");
  Serial.println("WiFi tilkoblet.");
  Serial.println("IP addresse: ");
  Serial.println(WiFi.localIP());

  server.begin();
  timeClient.begin();
  timeClient.update();

  // Firebase setup
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;         /* Assign the api key (required) */
  auth.user.email = "test@test.no"; /* Assign the user sign in credentials */
  auth.user.password = "testtest";  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback; /* Assign the callback function for the long running token generation task */ //see addons/TokenHelper.h
  //Or use legacy authenticate method  //config.database_url = DATABASE_URL;   //config.signer.tokens.legacy_token = "<database secret>";
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop()
{

  /*
  // Avstandsmåler start
  // Send ping, get distance in cm
  distance = sonar.ping_cm();

  // Send results to Serial Monitor
  Serial.print("Distance = ");

  if (distance < 10)
  {
    Serial.print("STARTER!");

    digitalWrite(LED_GREEN, HIGH);
  }
  else
  {
    digitalWrite(LED_GREEN, LOW);
  }

  if (distance >= 400 || distance <= 2)
  {
    Serial.println("Out of range");
  }
  else
  {
    Serial.print(distance);
    Serial.println(" cm");
  }
  delay(500); */
  // slutt avstandsmåler

  // runFirebase();
  // timeClient.update();
  /*
    Serial.print(daysOfTheWeek[timeClient.getDay()]);
    Serial.print(", ");
    Serial.print(timeClient.getHours());
    Serial.print(":");
    Serial.print(timeClient.getMinutes());
    Serial.print(":");
    Serial.println(timeClient.getSeconds());
    Serial.print(":");
    Serial.println(timeClient.getEpochTime();
  
    Serial.println(timeClient.getFormattedTime());
  
    delay(1000);
  */

  WiFiClient client = server.available(); // Lytt til om klient kobler til wifi-server.

  if (client)
  { // Dersom klient kobler til,
    Serial.println("Ny klient.");
    String currentLine = ""; // En String for å ta imot data fra klienten.
    currentTime = millis();
    previousTime = currentTime;

    // Loop så lenge klienten er koblet til
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    {
      currentTime = millis();
      if (client.available())
      {                         // Hvis det er bytes å lese fra klienten,
        char c = client.read(); // les byte, så
        Serial.write(c);        // print det ut i serial monitor
        header += c;
        if (c == '\n')
        { // hvis byte er ny-linje character. Dersom nåværende linje er blank, you got two newline characters in a row.
          // det er slutten av client HTTP request, så send respons:
          if (currentLine.length() == 0)
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Får info fra RoggyClock web-app om at adeltaker er klar og henter ned info om deltaker.
            if (header.indexOf("GET /ready") >= 0)
            {
              if (Firebase.ready() && currentCompetitor == "")
              {
                ;
                if (Firebase.RTDB.setString(&fbdo, "roggyclock/wemosReady", timeClient.getFormattedTime()) && Firebase.RTDB.getString(&fbdo, "roggyclock/currentCompetitor"))
                {
                  Serial.println(fbdo.to<String>());
                  currentCompetitor = fbdo.to<String>(); // Lagrer firebase-deltager på Wemos

                  digitalWrite(LED_RED, HIGH);

                  laserState = "on";
                }
                else
                {
                  Serial.println(fbdo.errorReason());
                }
              }
            }
            else if (header.indexOf("GET /start") >= 0 && currentCompetitor != "")
            {
              digitalWrite(LED_RED, LOW);
              digitalWrite(LED_GREEN, HIGH);
              while (currentCompetitor != "")
              {

                distance = sonar.ping_cm();
                Serial.print(distance);
                Serial.println(" cm");
                delay(100);
                if (distance < 10)
                {
                  startTid = timeClient.getFormattedTime();
                  Firebase.RTDB.setString(&fbdo, "roggyclock/start", startTid);
                  digitalWrite(LED_GREEN, LOW);
                  delay(100);
                  digitalWrite(LED_GREEN, HIGH);
                  delay(100);
                  digitalWrite(LED_GREEN, LOW);
                  delay(100);
                  digitalWrite(LED_GREEN, HIGH);
                  delay(5000);

                  while (currentCompetitor != "") // lytter på sluttresultat
                  {
                    distance = sonar.ping_cm();
                    Serial.print(distance);
                    Serial.println(" cm");
                    if (distance < 10)
                    {
                      stoppTid = timeClient.getFormattedTime();
                      Firebase.RTDB.setString(&fbdo, "roggyclock/stopp", stoppTid);
                      currentCompetitor = "";
                    }
                    if (currentCompetitor == "")
                      return;
                    delay(100);
                  }
                }
              }

              // if (stoppTid != "")
              // {
              //   taskCompleted = false;
              //   stoppTid = "";
              //   startTid = "";
              // }

              digitalWrite(pinLaser, HIGH); // Starter laser
              laserState = "on";
              startTid = timeClient.getFormattedTime();
              Serial.println("Laser er startet");

              if (Firebase.ready() && !taskCompleted)
              {
                taskCompleted = true;

                if (Firebase.RTDB.setString(&fbdo, "roggyclock/start", startTid))
                {
                  Serial.println("PASSED");
                  Serial.println("PATH: " + fbdo.dataPath());
                  Serial.println("TYPE: " + fbdo.dataType());
                }
                else
                {
                  Serial.println("FAILED");
                  Serial.println("REASON: " + fbdo.errorReason());
                }
              }
            }
            else if (header.indexOf("GET /stop") >= 0)
            {
              if (startTid != "")
              {
                taskCompleted = false;
                startTid = "";
              }
              digitalWrite(pinLaser, LOW); // Stopper laser
              stoppTid = timeClient.getFormattedTime();
              laserState = "off";
              Serial.println("Laser er stoppet");

              if (Firebase.ready() && !taskCompleted)
              {
                taskCompleted = true;

                if (Firebase.RTDB.setString(&fbdo, "roggyclock/stopp", stoppTid))
                {
                  Serial.println("PASSED");
                  if (Firebase.RTDB.getString(&fbdo, "roggyclock/readyCompetitor"))
                  {
                    Serial.println(fbdo.to<String>());
                  }
                  else
                  {
                    Serial.println(fbdo.errorReason());
                  }
                  Serial.println("PATH: " + fbdo.dataPath());
                  Serial.println("TYPE: " + fbdo.dataType());
                }
                else
                {
                  Serial.println("FAILED");
                  Serial.println("REASON: " + fbdo.errorReason());
                }
              }
            }

            // Vis HTML på wemos d1 mini-server
            client.println("<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS for å designe buttons (knappene)
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>Rogers Minicontroller Web Server</h1>");
            client.println("<small>Kontroller: Wemos D1 mini </small>");

            if (currentCompetitor != "")
            {
              client.println("<p>Deltager som er klar: " + currentCompetitor + "</p>");
              client.println("<p><a href=\"/stop\"><button class='button'>Stopp laser</button></a></p>");
            }
            else if (laserState == "off")
            {
              if (stoppTid != "")
              {
                client.println("<h2>Tid stoppet:</h2><h3> " + stoppTid);
              }
              client.println("<p><a href=\"/start\"><button class='button'>Start laser</button></a></p>");
            }
            else
            {
              client.println("<h2>Tid startet:</h2><h3> " + startTid + "</h3><p><a href=\"/stop\"><button class='button'>Stopp laser</button></a></p>");
            }

            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          }
          else
          { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}