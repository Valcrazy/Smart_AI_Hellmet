#include <Wire.h>
#define GSM_TX 27 // ESP32 TX pin to SIM800L RX
#define GSM_RX 26 // ESP32 RX pin to SIM800L TX

#include <MPU6050.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

MPU6050 mpu;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

HardwareSerial GSM(1);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for most displays
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
}
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(SSD1306_WHITE);
display.setCursor(0,0);
display.print("Helmet Ready");
display.display();
delay(1000); // Optional: show boot message
display.clearDisplay();
display.display();

  mpu.initialize();
  Serial.println("MPU6050 ready");
  GSM.begin(9600, SERIAL_8N1, GSM_RX, GSM_TX);
}

bool crashSent = false;

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  float fx = ax / 16384.0;
  float fy = ay / 16384.0;
  float fz = az / 16384.0;
  float g_mag = sqrt(fx*fx + fy*fy + fz*fz);
  g_mag=4;

  Serial.print("g_mag: "); Serial.println(g_mag);

  if ((g_mag > 2.0) && !crashSent) {
    Serial.println("CRASH DETECTED!");
    sendCrashSMS("+919080794837", "Crash Detected! Accel: " + String(g_mag,2));
    display.clearDisplay();
display.setTextSize(2);  // Large text
display.setCursor(0, 20);
display.setTextColor(SSD1306_WHITE);
display.println("CRASH");
display.setCursor(0, 45);
display.println("DETECTED");
display.display();

    crashSent = true; // Only send once unless code reset

  }
  delay(100);
}

void sendCrashSMS(String contact, String message) {
  GSM.println("AT+CMGF=1");
  delay(1000);
  GSM.print("AT+CMGS=\"");
  GSM.print(contact);
  GSM.println("\"");
  delay(1000);
  GSM.print(message);
  GSM.write(26); // CTRL+Z to send
  delay(5000); // Wait for message to send
}
