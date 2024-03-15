const express = require("express");
const mqtt = require("mqtt");
const app = express();
const broker = "mqtt://test.mosquitto.org:1883";
const receiveTopic = "stuped11";
const client = mqtt.connect(broker); // Connect to a public broker for demo purposes
const publishTopic = "veryUltimateSecretTopics/pc/sendCommand";
const listenPort = 3030;
console.clear();
client.on("connect", function () {
  console.log("mqtt client connected");
});

// Endpoint to publish a message to the MQTT broker
app.post("/publish", (req, res) => {
  client.publish(req.params.topic, req.params.payload);
  res.send("Published message to topic");
});

// Subscribe to a topic and log messages
client.subscribe(receiveTopic, function (err) {
  if (!err) console.log("Successfullly subscribed !!in topic "+ receiveTopic);
  else console.log("the error is " + err);
  console.log();
});
client.on("message", function (topic, message) {
  console.log("The Message is: " + message.toString());
  console.log("-----------------------------------");
  console.log();
});
client.on("error", function () {
  console.log("There is an error in MQTT");
});

app.listen(listenPort, () => {
  console.log("Express server started on port " + listenPort);
});
