#include <SoftwareSerial.h>

SoftwareSerial sfw(D1, D2);
#define A9G_baudRate 115200
#define baudRate 115200
#define A9G_rstPin D4

const String broker = "test.mosquitto.org";
const String subTopic;
const String PublishTopic = "veryUltimateSecretTopic/esp82/loc";

String sendCommand(String command, short waitTime, bool debug = true);
void setup()
{

  Serial.begin(baudRate);
  sfw.begin(A9G_baudRate);
  Serial.println();
  Serial.println("I am starting\n\r");
  // setup the pins
  pinMode(A9G_rstPin, OUTPUT);

  Serial.println("A9G is resetting\n\r");
  // resetting the A9G

  digitalWrite(A9G_rstPin, LOW);
  digitalWrite(A9G_rstPin, HIGH);
  delay(1000);

  String res = "";
  while (res.indexOf("READY") < 0)
  {
    res = sfw.readString();
    Serial.println(res);
  };
  Serial.println("A9G resetted sucsessfully\n\r");

  // check the status
  sendCommand("AT", 2000);
  // //Attaching to the GPRS netwotk
  sendCommand("AT+CGATT=1", 2000);
  // //Setting the APN name
  sendCommand("AT+CGDCONT=1,\"IP\",\"internet\"", 2000);
  // //Active command is used to active the specified PDPcontext
  sendCommand("AT+CGACT=1,1", 2000);
  // //check the status of the connection
  sendCommand("AT+CIPSTATUS?", 2000);
  // //Turn on the GPS
  sendCommand("AT+GPS=1", 2000);
  // //Activate low power mode for the gps
  sendCommand("AT+GPSLP=2", 2000);
  // //Checking the signal strength
  sendCommand("AT+CSQ", 2000);
  // Connect to Mqtt broker
  sendCommand("AT+MQTTCONN=\"" + broker + "\",1883,\"XXXX\",120,0", 2000);
  // Send MQTT subscribe packet
  // sendCommand("AT+MQTTSUB=\"" + subTopic + "\",1,0", 2000);
}

void loop()
{
  String responce = sendCommand("AT+LOCATION=2", 2000);
  // put your main code here, to run repeatedly:
  if (responce.indexOf("NOT FIX") >= 0)
  {
    Serial.println("I can not get GPS");
    sendCommand("AT+MQTTPUB=\"" + PublishTopic + "\",\"Error Getting GPS\",0,0,0 ", 1000, false);
  }
  else
  {
    Serial.println("I can get GPS");

    sendCommand("AT+MQTTPUB=\"" + PublishTopic + "\",\"" + responce + "\",0,0,0 ", 1000, false);
  }
  // while (sfw.available() > 0)
  // {

  //   Serial.println(sfw.readString());
  //   yield();
  // }
  // while (Serial.available() > 0)
  // {

  //   sfw.println(Serial.readString());
  //   yield();
  // }
  delay(1000);
}

// function to send a command and return the response
String sendCommand(String command, short waitTime, bool debug)
{
  Serial.println(command);

  sfw.println(command);
  delay(waitTime);
  String response = "";

  while (sfw.available() > 0)
  {

    response += (char)sfw.read();
  }

  if (debug)
    Serial.println(response);
  return response;
}
