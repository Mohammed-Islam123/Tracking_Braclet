#include <SoftwareSerial.h>
#define A9G_baudRate 115200
#define baudRate 115200
#define A9G_rstPin D4

SoftwareSerial sfw(D1, D2);
// define the MQTT settings
const String broker = "test.mosquitto.org";
String subTopic = "zamalSub";
const String PublishTopic = "zmalaPublish";
String lang = "", lat = "";
const String presionerID = "123";
String getPayload(String presioner_ID, String latitude, String langitude, String battery, String separator = "#//#");
void getLocation(String responce, String &lat, String &lang);
String getResponceFromBroker(int timeOut);
String sendCommand(String command, int timeOut, bool checkForErrors, bool debug = true);
void checkConnection();
void setup()
{

  Serial.begin(baudRate);
  // Serial1.begin(baudRate);
  sfw.begin(A9G_baudRate);
  Serial.println();
  Serial.println("I am starting\n\r");
  // setup the pins
  pinMode(A9G_rstPin, OUTPUT);

  Serial.println("A9G is resetting\n\r");
  // resetting the A9G

  digitalWrite(A9G_rstPin, LOW);
  digitalWrite(A9G_rstPin, HIGH);

  String res = "";
  while (res.indexOf("READY") < 0)
  {
    res = sfw.readString();
    Serial.println(res);
  };
  Serial.println("A9G resetted sucsessfully\n\r");

  // check the status
  sendCommand("AT", 3000, true);
  // //Attaching to the GPRS netwotk
  sendCommand("AT+CGATT=1", 3000, true);
  // //Setting the APN name
  sendCommand("AT+CGDCONT=1,\"IP\",\"internet\"", 3000, true);
  // //Active command is used to active the specified PDPcontext
  sendCommand("AT+CGACT=1,1", 3000, true);
  // //check the status of the connection
  sendCommand("AT+CIPSTATUS?", 3000, false);
  // //Turn on the GPS
  sendCommand("AT+GPS=1", 3000, true);
  // //Activate low power mode for the gps
  sendCommand("AT+GPSLP=2", 2000, true);
  // //Checking the signal strength
  sendCommand("AT+CSQ", 3000, false);
  // Connect to Mqtt broker
  sendCommand("AT+MQTTCONN=\"" + broker + "\",1883,\"XXXX\",120,0", 3000, true);
  //  Send MQTT subscribe packet
  sendCommand("AT+MQTTSUB=\"" + subTopic + "\",1,0", 3000, true);
}

void loop()
{

  // Get and send GPS Coordinations
  String location = sendCommand("AT+LOCATION=2", 2000, false, false);
  String battery = sendCommand("AT+CBC?", 1000, true, false).substring(20, 22);
  getLocation(location, lat, lang);
  sendCommand("AT+MQTTPUB=\"" + PublishTopic + "\",\"" + getPayload(presionerID, lat, lang, battery) + "\",0,0,0 ", 4000, false);

  Serial.println(getResponceFromBroker(6000));
  checkConnection();
  delay(1000);
}

void getLocation(String responce, String &lat, String &lang)
{
  lat = "";
  lang = "";
  if (responce.indexOf("ERROR") >= 0)
  {
    Serial.println("Error Getting GPS");
    lat = "none";
    lang = "none";
  }
  else
  {
    short index;
    for (index = 17; responce[index] != ','; index++)
    {
      lat += responce[index];
    }
    for (index = (index + 1); (responce[index] != '\r'); index++)
    {
      lang += responce[index];
    }
  }
}
// this function returns the String that will be sent containing all the infos
String getPayload(String presioner_ID, String latitude, String langitude, String battery, String separator)
{
  return presioner_ID + separator + latitude + separator + langitude + separator + battery;
}

String getResponceFromBroker(int timeOut)
{
  long startTime = millis();
  String responce = "";

  while ((millis() - startTime) < timeOut)
  {
    if (sfw.available())
      responce += (char)sfw.read();
  }

  return responce;
}

// fucntion to check if the module is still connected to the borker
void checkConnection()
{
  String responce;

  responce = sendCommand("AT+MQTTCONN?", 2000, false, false);

  while (responce.indexOf("0") >= 0)
  {
    Serial.println("Connection lost , tring to connect again ....");
    sendCommand("AT+MQTTCONN=\"" + broker + "\",1883,\"XXXX\",120,0", 3000, true);
    sendCommand("AT+MQTTSUB=\"" + subTopic + "\",1,0", 3000, true);
    responce = sendCommand("AT+MQTTCONN?", 2000, false);
  }
}

String sendCommand(String command, int timeOut, bool checkForErrors, bool debug)
{
  String responce;
  long startTime;
  do
  {
    responce = "";
    Serial.println("comm is: " + command);
    startTime = millis();
    sfw.println(command);

    while ((millis() - startTime < timeOut))
    {
      if (sfw.available() > 0)
        responce += (char)sfw.read();
    }

    if (debug)
      Serial.println("res is : " + responce);
  } while (checkForErrors && (responce.indexOf("ERR") >= 0));
  return responce;
}
