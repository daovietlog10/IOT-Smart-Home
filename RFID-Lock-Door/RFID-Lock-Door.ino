#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
 
#define SS_PIN 10
#define RST_PIN 9
#define LED_G 5                                       //cấu hình chân LED xanh
#define LED_R 4                                       //cấu hình chân LED đỏ
#define BUZZER 2                                      //chân còi
MFRC522 mfrc522(SS_PIN, RST_PIN);                     //Khởi tạo MFRC522.
Servo myServo;                                        //đăng ký tên đcơ Servo
 
void setup() 
{
  Serial.begin(9600);                                 
  SPI.begin();                                        // Khởi tạo SPI bus
  mfrc522.PCD_Init();                                 // Khởi tạo MFRC522
  myServo.attach(3);                                  // Chân servo 
  myServo.write(0);                                   // Điểm bắt đầu Servo
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  Serial.println("Put your card to the reader...");
  Serial.println();

}
void loop() 
{
  // Đăng ký Card mới
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Chọn 1 trong các card
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Hiện UID tag trên Serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "92 FF CF 21") // UID của thẻ / những thẻ cho phép truy cập hệ thống
  {
    Serial.println("Truy cập được cho phép");
    Serial.println();
    delay(500);
    digitalWrite(LED_G, HIGH);
    tone(BUZZER, 500);
    delay(300);
    noTone(BUZZER);
    myServo.write(90);
    delay(5000);
    myServo.write(0);
    digitalWrite(LED_G, LOW);
  }
 
 else   {
    Serial.println("Truy cập bị từ chối");
    digitalWrite(LED_R, HIGH);
    tone(BUZZER, 300);
    delay(1000);
    digitalWrite(LED_R, LOW);
    noTone(BUZZER);
  }
}
