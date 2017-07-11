#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "CDAC";
const char* password = "";
const char* mqtt_server = "10.208.37.147";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() 
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);           // We start by connecting to a WiFi network
  while (WiFi.status() != WL_CONNECTED) //Loop Until it Connects
  {
    delay(500);
    Serial.print(".");
  }
  
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) 
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the Red Light if an 1 is received as first character
  
  if ((char)payload[0] == '1') 
  {
    digitalWrite(D1, HIGH);   // D1 = REDLIGHT
  }
  
  // Switch on the Green Light if an 0 is received as first character
  
  if ((char)payload[0] == '0')
  {
    digitalWrite(D1, LOW);    // Turn-Off the Red Light First
    delay(500);
    digitalWrite(D2, HIGH);   // D2 = Green Light
    delay(30000);             // Keep the Green Light ON for 30 Sec.
    digitalWrite(D2, LOW);    // Turn-Off Greeen Light
  }
}

void reconnect() 
{
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
  
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("presence");
      
    } 
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() 
{
  // Initialize the Digital I/O pins as an output
 
  pinMode(D0, OUTPUT);  // D0 = Internal Relay to Enable the MQTT Controlled Circuit 
  pinMode(D1, OUTPUT);  // D1 = Relay For Red Light
  pinMode(D2, OUTPUT);  // D2 = Relay For Green Light
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  digitalWrite(D0, HIGH);  // Turn the System ON if the client connects to Wifi and subscribes to the topic successfully
  client.loop();

  /*long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }*/
}
