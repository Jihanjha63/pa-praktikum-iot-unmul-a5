
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Mengatur MQTT
const char* ssid = "Universitas Mulawarman"; // Nama SSID/Hotspot
const char* password = "";//Password WIFI
const char* mqtt_server = "broker.emqx.io";//Broker yang digunakan


WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];//Array Messages

// Pemilihan Pin dan pendeklarasian variabel
#define pirPin D1 
int pirState = LOW; //Asumsi Tidak Ada Gerakan Terdeteksi
int val = 0; //Variabel untuk membaca pin status

void setup_wifi() {
  delay(10);
  // menghubungkan dengan wifi 
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA); //Mode Station
  WiFi.begin(ssid, password); //Mencocokan SSID dan Password

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Menghubungkan ke broker
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("alarm/maling/status");//Topic yang digunakan
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup(){
  Serial.begin(9600);
  pinMode(pirPin, INPUT);//Deklarasi Pin
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop(){ //Perulangan Program
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  val = digitalRead(pirPin);// membaca nilai input
  delay(5000);
  snprintf (msg, MSG_BUFFER_SIZE, "%s", itoa(val,msg,10)); // itoa (konversi integer ke string)
  Serial.print("Publish message: ");
  Serial.println(msg);
  client.publish("alarm/maling/status", msg); // fungsi untuk publish ke broker
}
