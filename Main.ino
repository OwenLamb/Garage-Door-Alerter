#include <WiFi.h>
#include <HTTPClient.h>

int SensorPin = 34;

// Wi-Fi credentials
const char* ssid = "";
const char* password = "";

// CallMeBot credentials
const char* phone_number = "";
const char* api_key = "";
const char* phone_number2 = "";
const char* api_key2 = "";

void setup() {
  Serial.begin(115200);
  pinMode(SensorPin, INPUT);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

int SignalVal;
int timer;
bool trigger;
bool messageSent;
bool closeMessage;
String message;

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    SignalVal = analogRead(SensorPin);

    if ((SignalVal <= 3000) && !trigger) {
      // Door is open, start the timer
      timer = millis();
      trigger = true;
      messageSent = false;
    } else if (SignalVal >= 3000) {
      // Door is closed, reset everything
      timer = 0;
      trigger = false;
      messageSent = false;
      if (closeMessage) {
        // Door got closed after sending the alert message so send one confirming it's closed
        message = "Door+is+now+closed!";
        sendSMS(message);
        closeMessage = false;
      }
    }

    // If the door remains open for 30 seconds and a message hasn't been sent
    if (trigger && !messageSent && millis() - timer >= 30000) {
      message = "Garage+door+is+still+open!";
      sendSMS(message);
      messageSent = true;  // Ensure only one message per open state
      closeMessage = true;
    }
  } else {
    Serial.print("Reconnecting to Wi-Fi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi");
  }
}

void sendSMS(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Construct API URL
    String url = "https://api.callmebot.com/whatsapp.php?phone=" + String(phone_number) + "&text=" + String(message) + "&apikey=" + String(api_key);

    // Make the HTTP GET request
    http.begin(url);
    int httpResponseCode = http.GET();

    // Check response
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("SMS sent successfully!");
      Serial.println(response);
      Serial.println(url);
    } else {
      Serial.print("Error sending SMS: ");
      Serial.println(httpResponseCode);
    }

    http.end();

    // Construct API URL
    url = "https://api.callmebot.com/whatsapp.php?phone=" + String(phone_number2) + "&text=" + String(message) + "&apikey=" + String(api_key2);

    // Make the HTTP GET request
    http.begin(url);
    httpResponseCode = http.GET();

    // Check response
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("SMS sent successfully!");
      Serial.println(response);
      Serial.println(url);
    } else {
      Serial.print("Error sending SMS: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Wi-Fi not connected!");
  }
}