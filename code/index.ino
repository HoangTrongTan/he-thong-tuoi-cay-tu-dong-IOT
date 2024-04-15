#define BLYNK_TEMPLATE_ID "TMPL6bVCAtfmx"
#define BLYNK_TEMPLATE_NAME "ESMARTDHT11"
#define BLYNK_AUTH_TOKEN "x8jfh5xMxVRXD1UhhkX03QrCRoGRvuZf"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <string.h>
#include <stdio.h>



#include "DHT.h"

#include <LiquidCrystal_I2C.h>


// DHT define
#define DHTTYPE DHT11      // DHT 11 Định nghĩa hằng DHTTYPE với giá trị DHT11, cho biết loại cảm biến DHT được sử dụng (DHT11 trong trường hợp này).
#define DHTPIN 13          //  Định nghĩa hằng DHTPIN với giá trị 13, chỉ ra chân GPIO được kết nối với dây dữ liệu của cảm biến DHT.
DHT dht(DHTPIN, DHTTYPE);  //Tạo một đối tượng DHT có tên dht và khởi tạo nó với loại cảm biến (DHTTYPE) và chân dữ liệu (DHTPIN).

// Define LED
// WidgetLED LED_ON_APP(V0);
int button;

char auth[] = BLYNK_AUTH_TOKEN;
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "OnlyOne";
char pass[] = "12345678";


int relay = 14;  //Chân relay số 5
int cb = 34;
int doc_cb, TBcb;

boolean flag = false;

int buttonCamBien;
boolean flagBtnCamBien = false;

void setup() {
  // pinMode(LED, OUTPUT);

  Serial.begin(115200);

  pinMode(cb, INPUT);      //Tín hiệu vào từ cảm biến
  pinMode(relay, OUTPUT);  //Tín hiệu xuất ra từ relay

  digitalWrite(relay, LOW);

  dht.begin();

  Blynk.begin(auth, ssid, pass);
}

BLYNK_WRITE(V3) {
  button = param.asInt();

  if (button == 1) {
    digitalWrite(relay, HIGH);
    // LED_ON_APP.on();
  } else {
    digitalWrite(relay, LOW);
    // LED_ON_APP.off();
  }
}
BLYNK_WRITE(V0) {
  buttonCamBien = param.asInt();

  if (buttonCamBien == 1) {
    flagBtnCamBien = true;
    // Blynk.virtualWrite(V0 , 1);
  } else {
    flagBtnCamBien = false;
    digitalWrite(relay, LOW);  //Thì relay ON, Bơm chạy
  }
}

void maybom() {
  for (int i = 0; i <= 9; i++) /*tạo một hàm for để đọc 10 lần giá trị cảm biến, sau đó lấy giá trị trung bình để được giá trị chính xác nhất.*/
  {
    doc_cb += analogRead(cb);
  }
  TBcb = doc_cb / 10;                           //Tính giá trị trung bình
  int phantramao = map(TBcb, 0, 1023, 0, 100);  //Chuyển giá trị Analog thành giá trị %
  int phantramthuc = 100 - phantramao;          //Tính giá trị phần trăm thực, chuyển điện thế khô thành ẩm

  Serial.print("\n\n >>>[analog]: ");
  Serial.print(TBcb);
  Serial.print(" ");
  Serial.print("~");
  Serial.print(" ");
  Serial.print(phantramthuc);
  Serial.println('% \n\n');
  delay(500);
  doc_cb = 0;

  if (phantramthuc >= 30)  //Nếu phần trăm thực lớn hơn hoặc = 30%
  {
      digitalWrite(relay, LOW);  //Thì relay OFF, không bơm
  }
  else  //Ngược lại, phần trăm thực nhỏ hơn 30%
  {
      digitalWrite(relay, HIGH);  //Thì relay ON, Bơm chạy
  }
}

void loop() {
  Serial.println("\nbutton");
  Serial.println(button);
  Serial.println("button cảm biến");
  Serial.println(buttonCamBien);
  Blynk.run();
  if (flagBtnCamBien) {
    maybom();
  }
  // Read Temp
  float t = dht.readTemperature();  // Đọc nhiệt độ từ cảm biến DHT và lưu trữ trong biến t.
  // Read Humi
  float h = dht.readHumidity();  //Đọc độ ẩm từ cảm biến DHT và lưu trữ trong biến h.
  // Check isRead ?
  if (isnan(h) || isnan(t)) {  //Kiểm tra xem giá trị h hoặc t có phải NaN (không phải số) hay không.
    delay(500);
    Serial.println("Failed to read from DHT sensor!\n");
    return;
  }

  Blynk.virtualWrite(V1, t);  //Gửi giá trị nhiệt độ t đến chân ảo V1 trong ứng dụng Blynk.
  Blynk.virtualWrite(V2, h);  //Gửi giá trị độ ẩm h đến chân ảo V2 trong ứng dụng Blynk.

  Serial.print("\n");
  Serial.print("Humidity: " + String(h) + "%");
  Serial.print("\t");
  Serial.print("Temperature:" + String(t) + " C");
  // digitalWrite(relay, LOW);
  delay(2000);
}