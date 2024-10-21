# esp32_adafruit_mqtt
Yêu cầu thiết kế
- Hệ thống giám sát và điều khiển môi trường cho cây dựa trên các cảm biến nhiệt độ và độ ẩm, cùng
với việc kết nối với Adafruit IO để gửi và nhận dữ liệu qua MQTT.
- Giám sát nhiệt độ và độ ẩm: Sử dụng cảm biến DHT22 để đo nhiệt độ và độ ẩm không khí và sử dụng
thanh Slide Potentiometer như một cảm biến độ ẩm đất.
- Điều khiển relay dựa trên các giá trị nhận được từ Adafruit IO, các relay này có thể dùng để bật/tắt
các thiết bị như đèn, máy bơm.
- Tự động điều khiển relay dựa trên các cảm biến: Khi nhiệt độ giảm xuống dưới 100C thì relay1 được
kích hoạt để bật đèn nhằm sưởi ấm, khi độ ẩm đất giảm xuống dưới 20% hoặc nhiệt độ vượt quá 500C thì
relay2 được kích hoạt để bật máy bơm nước.
- Relay1 và relay2 cũng có thể được điều khiển trực tiếp thông qua các nút nhấn BUTTON1 và
BUTTON2.
