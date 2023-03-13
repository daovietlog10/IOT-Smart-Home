#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

#define DHTTYPE DHT11
#define DHTPIN 2                                     //chân Data của DHT11 sensor là GPIO2
#define FAN_PIN D6                                   //chân tín hiệu của quạt là D6
#define GAS_SENSOR_PIN A0                            //chân Data MQ-6 sensor là A0
#define BUZZER_PIN D5                                //chân của Buzzer là D5
#define SDA_PIN 4                                    //cấu hình chân cho LED 
#define SCL_PIN 5
#define THINGSBOARD_HOST "192.168.15.47"              // Host  : 192.168.43.47
#define DHT_TOKEN "XNekmwjdFJOQ1wN7x0Pm"
#define MQ6_TOKEN "uOsEd8XIwH6FLpY4pIpy"

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, SDA_PIN, SCL_PIN);          //khai báo LCD

WiFiClient wifiClient;
PubSubClient client1(wifiClient);
PubSubClient client2(wifiClient);

void setup() {
  Serial.begin(115200);
  dht.begin();                                        //khởi động các sensor, LCD, còi, quạt
  pinMode(FAN_PIN, OUTPUT);                   
  pinMode(BUZZER_PIN, OUTPUT);
  lcd.begin();
  lcd.setBacklight(HIGH);
  lcd.home();
  lcd.print("Connecting WiFi");
  WiFi.begin("DVL", "dvl23102001");                 //kết nối Wifi qua ESP
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi network");
  //Connect to Edge for DHT11
  client1.setServer(THINGSBOARD_HOST, 11883);
  while (!client1.connected()) {
    if (client1.connect("DHT11",DHT_TOKEN , "")) {
      Serial.println("Connected to edge for DHT11");
    } else {
      Serial.print("DHT11 connection failed, rc=");
      Serial.print(client1.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
  // Connect to MQTT broker for second device
  client2.setServer(THINGSBOARD_HOST, 11883);
  while (!client2.connected()) {
    if (client2.connect("MQ6", MQ6_TOKEN, "")) {
      Serial.println("Connected to edge for MQ6");
    } else {
      Serial.print("MQ6 connection failed, rc=");
      Serial.print(client2.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  float hum = dht.readHumidity();                     //đọc dữ liệu từ các cảm biến     
  float temp = dht.readTemperature();
  int gasLevel = analogRead(GAS_SENSOR_PIN);

  if (!isnan(hum) && !isnan(temp)) {                   //hiển thị lên LCD
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Nhiet do: ");
    lcd.print(temp);
    lcd.print((char)223);
    lcd.print("C");
    lcd.setCursor(0,1);
    lcd.print("Do am: ");
    lcd.print(hum);
    lcd.print("%");
  }
  if (temp > 30) {
    // Bật quạt nếu nhiệt độ lớn hơn ngưỡng
    digitalWrite(FAN_PIN, HIGH);
  } else {
    // Tắt quạt nếu nhiệt độ nhỏ hơn ngưỡng
    digitalWrite(FAN_PIN, LOW);
  }
  String data1 = "{\"temp\":" + String(temp) + ",\"hum\":" + String(hum)+ "}";       //gửi nhiệt độ độ ẩm lên TB edge
  client1.publish("v1/devices/me/telemetry", data1.c_str(), data1.length());
  Serial.println("Published temperature and humidity data to Thingsboard Edge");

  
// Gửi nồng độ khí gas lên TB edge
  String gasData = "{\"gas_level\":";
  gasData += String(gasLevel);
  gasData += "}";
   if (gasLevel > 250) {
    // Nếu khí gas vượt ngưỡng cho phép thì còi kêu 2s
    digitalWrite(BUZZER_PIN, HIGH);
    delay(2000);
    digitalWrite(BUZZER_PIN, LOW);
    delay(2000);
   }
  client2.publish("v1/devices/me/telemetry", gasData.c_str(), gasData.length());
  Serial.println("Published gas level data to Thingsboard Edge");
}
