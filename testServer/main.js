const express = require("express");
const mqtt = require("mqtt");
const app = express();
const broker = "mqtt://test.mosquitto.org:1883";
const receiveTopic = "zmalaPublish";
const client = mqtt.connect(broker); // Connect to a public broker for demo purposes
const publishTopic = "zamalSub";
const listenPort = 3030;
let startTime;
console.clear();
client.on("connect", function () {
  console.log("mqtt client connected");
});

// Endpoint to publish a message to the MQTT broker
app.post("/publish", (req, res) => {
  client.publish(publishTopic, req.params.payload);
  res.send("Published message to topic");
});

// Subscribe to a topic and log messages
client.subscribe(receiveTopic, function (err) {
  if (!err) {
    console.log("Successfullly subscribed !!in topic " + receiveTopic);
    startTime = new Date();
  } else console.log("the error is " + err);
  console.log();
});
client.on("message", function (topic, message) {
  console.log("The Message is: " + message.toString());
  const endTime = new Date();
  const elapsedTime = endTime - startTime;
  console.log("Time taken to receive the message: " + elapsedTime + "ms");
  console.log(
    "message recieved at : " +
      `${endTime.getHours()}:${endTime.getMinutes()}:${endTime.getSeconds()}`
  );
  startTime = new Date();
  // Your existing code here

  client.publish(publishTopic, "alarm, good");
  console.log("-----------------------------------");
  console.log();
});
client.on("error", function () {
  console.log("There is an error in MQTT");
});

app.listen(listenPort, () => {
  console.log("Express server started on port " + listenPort);
});
