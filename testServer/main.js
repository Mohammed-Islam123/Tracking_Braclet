const express = require("express");
const mqtt = require("mqtt");
const app = express();
const broker = "https://test.mosquitto.org:1883";
const receiveTopic = "veryUltimateSecretTopic/esp82/loc";
const client = mqtt.connect(broker); // Connect to a public broker for demo purposes
const publishTopic = "veryUltimateSecretTopic/pc/sendCommand";
client.on("connect", function () {
  console.log("mqtt client connected");
});

// Endpoint to publish a message to the MQTT broker
app.post("/publish", (req, res) => {
  client.publish(req.params.topic, req.params.payload);
  res.send("Published message to topic");
});

// Subscribe to a topic and log messages
client.subscribe(receiveTopic, function (err) {});
client.on("message", function (topic, message) {
  console.log("The Message is: " + message.toString());
  console.log("-----------------------------------");
  console.log();
});
client.on("error", function () {
  console.log("There is an error in MQTT");
});

app.listen(3333, () => {
  console.log("Express server started on port 3002");
});
