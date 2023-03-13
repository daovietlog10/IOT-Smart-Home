#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Servo.h>

#define WIFI_AP "DVL"                          //thay thế với ID và pass wifi của bạn 
#define WIFI_PASSWORD "dvl23102001"

#define TOKEN "95K8R2bgtGfh9QF1BjfM"           //token của thiết bị trên Thingsboard   

#define GPIO0 D3                       //khai báo chân GPIO
#define GPIO2 D2
#define GPIO4 D4
#define GPIO5 D1

#define GPIO0_PIN 3                    //đánh số PIN
#define GPIO2_PIN 5
#define GPIO4_PIN 1
#define GPIO5_PIN 7



char thingsboardServer[] = "192.168.15.47";      // địa chỉ IPv4 của máy chủ

WiFiClient wifiClient;                           // thiết lập wifi và servo
Servo myservo;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;

// Chỉ định tất cả GPIO là LOW
boolean gpioState[] = {false, false,false,false};

void setup() {
  Serial.begin(115200);
  // Đặt chế độ OUTPUT cho các chân GPIO
  pinMode(GPIO0, OUTPUT);
  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO4, OUTPUT);
  pinMode(GPIO5, OUTPUT);
   myservo.attach(D5);       // chân servo
  delay(10);
  InitWiFi();
  client.setServer(thingsboardServer,11883);          //tạo client
  client.setCallback(on_message);
}

void loop() {
  if ( !client.connected() ) {
    reconnect();
  }

  client.loop();
}

// Trả về khi nhận được tín hiệu từ server.
void on_message(const char* topic, byte* payload, unsigned int length) {

  Serial.println("On message");

  char json[length + 1];
  strncpy (json, (char*)payload, length);
  json[length] = '\0';

  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(json);

  // Decode JSON request
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject((char*)json);

  if (!data.success())
  {
    Serial.println("parseObject() failed");
    return;
  }

  // Check request method
  String methodName = String((const char*)data["method"]);

  if (methodName.equals("getGpioStatus")) {
    // Reply with GPIO status
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_gpio_status().c_str());
  } else if (methodName.equals("setGpioStatus")) {
    // Update GPIO status and reply
    set_gpio_status(data["params"]["pin"], data["params"]["enabled"]);
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_gpio_status().c_str());
    client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
  }
}

String get_gpio_status() {
  // Prepare gpios JSON payload string
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();
  data[String(GPIO0_PIN)] = gpioState[0] ? true : false;
  data[String(GPIO2_PIN)] = gpioState[1] ? true : false;
  data[String(GPIO4_PIN)] = gpioState[2] ? true : false;
  data[String(GPIO5_PIN)] = gpioState[3] ? true : false;
  char payload[256];
  data.printTo(payload, sizeof(payload));
  String strPayload = String(payload);
  Serial.print("Get gpio status: ");
  Serial.println(strPayload);
  return strPayload;
}

void set_gpio_status(int pin, boolean enabled) {
  if (pin == GPIO0_PIN) {
    // Output GPIOs state
    digitalWrite(GPIO0, enabled ? HIGH : LOW);
    // Update GPIOs state
    gpioState[0] = enabled;
  } else if (pin == GPIO2_PIN) {
    // Output GPIOs state
    digitalWrite(GPIO2, enabled ? HIGH : LOW);
    // Update GPIOs state
    gpioState[1] = enabled;
  }
  else if (pin == GPIO4_PIN) {
    // Output GPIOs state
    digitalWrite(GPIO4, enabled ? HIGH : LOW);
    // Update GPIOs state
    gpioState[2] = enabled;
  }
  else if (pin == GPIO5_PIN) {
    // Output GPIOs state
    digitalWrite(GPIO5, enabled ? HIGH : LOW);
    // Update GPIOs state
    gpioState[3] = enabled;
     if (gpioState[3]==1){
      myservo.write(-10);
    }
     if (gpioState[3]==0){
      myservo.write(120);
    }
  }
}


void InitWiFi() {
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP8266 Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
      // Subscribing to receive RPC requests
      client.subscribe("v1/devices/me/rpc/request/+");
      // Sending current GPIO status
      Serial.println("Sending current GPIO status ...");
      client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
