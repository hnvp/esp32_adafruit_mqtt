#include <WiFi.h>  // Thư viện WiFi để kết nối mạng
#include <DHT.h>   // Thư viện DHT để sử dụng cảm biến DHT22
#include "AdafruitIO_WiFi.h"  // Thư viện Adafruit IO để kết nối với Adafruit IO

// Thông tin kết nối WiFi
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASS ""

// Thông tin tài khoản Adafruit IO
#define IO_USERNAME  "flejzad"
#define IO_KEY       "aio_mVHt45Rk1QZhZ8VV5YkByb5cJIJT"

// Cấu hình chân kết nối cho cảm biến DHT
#define DHTPIN 17
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Cấu hình chân kết nối cho Potentiometer
#define POT_PIN 34        // Chân kết nối của Slide Potentiometer với ESP32 (sử dụng chân analog input 34)
#define MAX_MOISTURE 100  // Độ ẩm tối đa

// Cấu hình chân kết nối cho relay và nút nhấn
#define RELAY1_PIN 12
#define RELAY2_PIN 14
#define BUTTON1_PIN 4
#define BUTTON2_PIN 0

// Khởi tạo đối tượng Adafruit IO
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

// Khởi tạo các feed cho nhiệt độ, độ ẩm, độ ẩm đất và relay
AdafruitIO_Feed *tem_feed = io.feed("Tem");
AdafruitIO_Feed *hum_feed = io.feed("Hum");
AdafruitIO_Feed *moi_feed = io.feed("Moi");
AdafruitIO_Feed *relay1_feed = io.feed("Light");
AdafruitIO_Feed *relay2_feed = io.feed("Pump");

unsigned long previousTime = 0;  // Thời gian trước đó
unsigned long delayTime = 1000;  // Thời gian trễ giữa các lần đọc dữ liệu

void setup() {
  Serial.begin(115200);  // Khởi động cổng Serial

  io.connect();  // Kết nối tới Adafruit IO

  // Đợi kết nối thành công
  while (io.status() < AIO_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  Serial.println("WiFi connected!");  // In ra thông báo đã kết nối WiFi thành công

  dht.begin();  // Khởi động cảm biến DHT

  pinMode(RELAY1_PIN, OUTPUT);  // Thiết lập chân relay1 là đầu ra
  pinMode(RELAY2_PIN, OUTPUT);  // Thiết lập chân relay2 là đầu ra

  pinMode(BUTTON1_PIN, INPUT_PULLUP);  // Thiết lập chân nút nhấn 1 là đầu vào với chế độ pull-up
  pinMode(BUTTON2_PIN, INPUT_PULLUP);  // Thiết lập chân nút nhấn 2 là đầu vào với chế độ pull-up
}

void loop() {
  io.run();  // Chạy vòng lặp Adafruit IO
  
  float tem = dht.readTemperature();  // Đọc nhiệt độ từ cảm biến DHT22
  float hum = dht.readHumidity();     // Đọc độ ẩm từ cảm biến DHT22
  int moiRaw = analogRead(POT_PIN);   // Đọc giá trị từ Slide Potentiometer
  int moi = map(moiRaw, 0, 4095, 0, 100); // Chuyển đổi giá trị đọc từ cảm biến sang phạm vi từ 0 đến 100

  if (isnan(tem) || isnan(hum)) {  // Kiểm tra nếu giá trị đọc từ cảm biến không hợp lệ
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Gửi dữ liệu lên Adafruit IO và in ra Serial mỗi giây
  if (millis() - previousTime >= delayTime) {
    tem_feed->save(tem);  // Gửi nhiệt độ lên Adafruit IO
    hum_feed->save(hum);  // Gửi độ ẩm lên Adafruit IO
    moi_feed->save(moi);  // Gửi độ ẩm đất lên Adafruit IO

    Serial.print("\n");
    Serial.print("Temperature: " + String(tem) + "°C");  // In nhiệt độ ra Serial
    Serial.print("\t");
    Serial.print("Humidity: " + String(hum) + "%");     // In độ ẩm ra Serial
    Serial.print("\t");
    Serial.print("Moisture: " + String(moi) + "%");     // In độ ẩm đất ra Serial
    previousTime = millis();  // Cập nhật thời gian trước đó
  }

  // Điều khiển relay dựa trên nhiệt độ và độ ẩm
  if (tem < 10) {
    digitalWrite(RELAY1_PIN, HIGH); // Bật relay1 nếu nhiệt độ dưới 10°C
  } else {
    digitalWrite(RELAY1_PIN, LOW);  // Tắt relay1 nếu nhiệt độ >= 10°C
  }

  if ((moi < 20) || (tem > 50)) {
    digitalWrite(RELAY2_PIN, HIGH); // Bật relay2 nếu độ ẩm dưới 20% hoặc nhiệt độ trên 50°C
  } else {
    digitalWrite(RELAY2_PIN, LOW);  // Tắt relay2 nếu độ ẩm >= 20% hoặc nhiệt độ <= 50°C
  }

  // Điều khiển relay bằng nút nhấn
  if (digitalRead(BUTTON1_PIN) == LOW) {
    digitalWrite(RELAY1_PIN, HIGH);  // Bật relay1 khi nhấn nút 1
    Serial.println("\nRelay1 turned on by button");
  }

  if (digitalRead(BUTTON2_PIN) == LOW) {
    digitalWrite(RELAY2_PIN, HIGH);  // Bật relay2 khi nhấn nút 2
    Serial.println("\nRelay2 turned on by button");
  }

  delay(1000); // Thêm một chút thời gian trễ để tránh việc xử lý quá nhanh
}
