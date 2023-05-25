
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Universitas Mulawarman"; //Nama SSID/Hotspot
const char* password = "";//Password WIFI
const char* mqtt_server = "broker.emqx.io";//Nama Broker yang digunakan

//MQTT
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];

#define buzzerPin D6  // Pemilihan Pin dan pendeklarasian variabel
#define ledPin D5 
int pirState;// Variabel status pir
int val; // variabel untuk membaca pin status

void setup_wifi() {
  delay(10);
  // menghubungkan ke wifi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA); //Mode Station
  WiFi.begin(ssid, password);// Mencocokan SSID dan Password

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

// untuk hubungkan ke broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("alarm/maling/status");// Topic yang Digunakan
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//untuk terima data publisher
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("] ");
  String data = ""; // variabel untuk menyimpan data yang berbentuk array char
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    data += (char)payload[i]; // menyimpan kumpulan char kedalam string
  }
  Serial.println("");
  int val = data.toInt();// Mengubah char ke integer
  if (val == 1){// Jika data yang diterima 1
    digitalWrite(ledPin, HIGH); //Status Lampu nyala
    tone(buzzerPin,1000,2000); // Status Buzzer nyala
  if (pirState == LOW)
    { 
    Serial.println("Ada Maling");//Mengirim Pesan Ada Maling
    pirState == HIGH;// Membuat state sesuai keadaan
    }
    }
   if (val == 0) {// Jika data yang diterima 0
    digitalWrite(ledPin, LOW); //status Lampu Mati
    noTone(buzzerPin); // Status Buzzer Mati
   if (pirState == HIGH){ 
      Serial.println("Rumah Aman");//Mengirim Pesan Rumah Aman
      pirState = LOW;// Membuat state sesuai keadaan
    }
    }
    
}

void setup(){
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);//Deklarasi Pin
  pinMode(ledPin, OUTPUT);// Deklarasi Pin 
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {//Perulangan Program 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
