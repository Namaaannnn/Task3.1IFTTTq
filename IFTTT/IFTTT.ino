#include <WiFiNINA.h>
#include <Wire.h>
#include <BH1750.h>

// WiFi credentials
char wifiSSID[] = "realme";
char wifiPassword[] = "realme221";

WiFiClient wifiClient; // Create a WiFi client object

BH1750 lightSensor; // Create an instance of the BH1750 light sensor

// IFTTT Webhook details
char iftttHostName[] = "maker.ifttt.com"; // Hostname for IFTTT server
String iftttPathName = "https://maker.ifttt.com/trigger/LIGHT_DETECTED/with/key/peKGXMpeOgB8nqY9GWJCFB3JDrvZtuZrlhWmZ91zHVFS"; // Webhook URL

void setup() 
{
  WiFi.begin(wifiSSID, wifiPassword); // Start WiFi connection
  Serial.begin(9600); // Initialize Serial communication at 9600 baud rate

  Wire.begin(); // Start I2C communication
  lightSensor.begin(); // Initialize the light sensor

  Serial.println(F("BH1750 Test begin"));

  // Attempt to connect to WiFi
  connectToWiFi();
}

void loop() 
{
  float lightLevel = lightSensor.readLightLevel(); // Read light intensity from the sensor
  Serial.print("lightLevel: ");
  Serial.println(lightLevel);
  
  // Trigger webhook based on light intensity thresholds
  if (lightLevel >= 360) 
  {
    triggerIFTTTWebhook("Daylight"); // Trigger webhook for daylight
  }
   else if (lightLevel < 80) 
  {
    triggerIFTTTWebhook("Dawnlight"); // Trigger webhook for dawnlight
  }
  
  delay(30000); // Wait for 30 seconds before the next light reading
}

void connectToWiFi() 
{
  Serial.println("Attempting to connect to WiFi...");
  WiFi.begin(wifiSSID, wifiPassword);  // Start WiFi connection
  
  // Wait until the device is connected to WiFi
  do 
  {
    delay(500);  // Check every 0.5 seconds for connection status
    Serial.print(".");
  } 
  while (WiFi.status() != WL_CONNECTED);
  
  Serial.println("WiFi connected!"); // Notify successful connection
}

void triggerIFTTTWebhook(String eventName) 
{
  // Ensure the device is still connected to WiFi
  if (WiFi.status() != WL_CONNECTED) 
  {
    connectToWiFi(); // Reconnect if not connected
  }
  
  // Send HTTP GET request to the IFTTT webhook
  if (wifiClient.connect("maker.ifttt.com", 80)) 
  {
    wifiClient.println("GET " + iftttPathName + " HTTP/1.1");
    wifiClient.println("Host: " + String(iftttHostName));
    wifiClient.println("Connection: close");
    wifiClient.println();
    delay(500);
    wifiClient.stop(); // Close the connection
    Serial.println("Data sent successfully");
  } 
  else 
  {
    Serial.println("Unable to connect to webhook server"); // Error message if unable to connect
  }
}