Các thư viện và phần mềm cần thiết:
- Phần mềm Docker - để tạo server ảo chạy Thingsboard Edge
- Phần mềm Arduino IDE (recommend v1.8.x) bảo gồm các thư viện: <ESP8266WiFi.h> <Wire.h> <Adafruit_Sensor.h> <DHT.h> <LiquidCrystal_I2C.h> <PubSubClient.h> <WiFiClient.h> <ArduinoJson.h> <Servo.h> <SPI.h> <MFRC522.h>, ngoài ra cần thêm cài đặt các board của UNO
Code bao gồm 3 file chính:
- File DHT_and_MQ6_to_LCD_and_edge.ino chứa code để gửi dữ liệu từ cảm biến nhiệt độ DHT11 và cảm biến khí gas MQ6 đến Thingsboard để hiển thị cũng như cảnh báo thông qua ESP8266.
- File GPIO.ino chứa code gửi tín hiệu từ ESP8266 đến Thingsboard để có thể điều khiển 3 đèn và rèm cửa thông qua bảng điều khiển GPIO của Thingsboard.
- File RFID-Lock-Door.ino chứa code điều khiển Arduino Uno R3 để điều khiển đóng mở tự động cửa chính của nhà thông qua sử dụng thẻ RFID. ( các bạn cần sử dụng file DumbInfo trong Example để tìm IP của thẻ RFID)

Bước 1: Tạo server ảo trên Docker ( đối với điều khiển trên Edge/ localhost)
- Cài đặt Docker cho Windows: https://docs.docker.com/toolbox/toolbox_install_windows/
- Cài đặt ThingsBoard Edge docker image bằng cách thực thi các lệnh:
```
docker pull thingsboard/tb-edge
```
- Khởi tạo Docker Volume
```commandline
docker volume create mytb-edge-data
docker volume create mytb-edge-logs
docker volume create mytb-edge-data-db
```
- Khởi tạo docker compose cho Thíngsboard Edge
```commandline
docker-compose.yml
```
- Thiết lập terminal trong thư mục chứa file docker-compose.yml và thực thi các lệnh sau để khởi tạo trực tiếp server Edge:
```commandline
docker compose up -d
docker compose logs -f mytbedge
```
Bước 2: Tạo Device trên Thingsboard platform 
- Bước 1: Chọn tên cảm biến mình muốn tạo 
- Bước 2: Lấy token để kết nối 
- Bước 3: Đưa chúng lên trên Dashboard để kiểm soát 
Bước 3: Thay đổi thông tin trên code
- Thay đổi tên và mật khẩu wifi tương ứng với các phần trong code của 2 file GPIO.ino và DHT_and_MQ6_to_LCD_and_edge.ino 
ví dụ trong code của chúng tôi cần thay đổi như sau :  WiFi.begin("YOUR_WIFI_ID", "YOUR_WIFi_PASSWORD") or #define WIFI_AP "YOUR_WIFI_ID" #define WIFI_PASSWORD "YOUR_WIFi_PASSWORD"
- Thay thế các thông tin host, token, MQTTport tương ứng với các server cũng như là cảm biến của bạn 
ví dụ với điều khiển local sẽ chọn IPv4: char thingsboardServer[] = "192.168.15.47" hoặc điều khiển cloud sẽ chọn char thingsboardServer[] = "demo.thingsboard.io" và token sẽ thay đổi với từng thiết bị trên thingsboard
Bước 4: Compile code và sửa lỗi (nếu có)
Bước 5: Chọn đúng loại VĐK ( trong code sử dụng NodeMCU ESP8266 và Arduino Uno R3) và đúng chân COM ( cài driver nếu chân COM không được phát hiện). Chọn sai có thể gây nên cháy VĐK.
Bước 6: Nạp code.
Bước 7: Test và điều khiển trên cloud của Thingsboard ( Edge với điều khiển local và demo.thingsboard.io với điều khiển qua cloud)



