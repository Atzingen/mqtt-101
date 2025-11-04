import paho.mqtt.client as mqtt

BROKER = "localhost"
PORT = 1883
TOPIC = "test/topic"

client = mqtt.Client()
client.connect(BROKER, PORT, 60)
client.loop_start()

msg = "Hello MQTT"
client.publish(TOPIC, msg)
print(f"Sent: {msg}")

client.loop_stop()
client.disconnect()