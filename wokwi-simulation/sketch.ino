#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
// WiFi credentials
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
// Firebase configuration
#define API_KEY "your_firebase_api_key"
#define DATABASE_URL "https://your-project-id-default-rtdb.firebaseio.com"
// Hardware definitions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
// Initialize components
MPU6050 mpu;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
// Variables
bool crashSent = false;
unsigned long lastDataUpdate = 0;
const unsigned long DATA_UPDATE_INTERVAL = 2000; // 2 seconds
void setup() {
Serial.begin(115200);
Wire.begin();
// Initialize OLED
if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
Serial.println(F("SSD1306 allocation failed"));
for (;;);
}
// Display boot message
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(SSD1306_WHITE);
display.setCursor(0, 0);
display.print("Helmet Initializing...");
display.display();
delay(2000);
// Initialize MPU6050
mpu.initialize();
Serial.println("MPU6050 ready");
// Connect to WiFi
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
display.clearDisplay();
display.setCursor(0, 0);
display.print("Connecting WiFi...");
display.display();
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("\nWiFi connected");
// Initialize Firebase
config.api_key = API_KEY;
config.database_url = DATABASE_URL;
Firebase.begin(&config, &auth);
Firebase.reconnectWiFi(true);
// Display ready message
display.clearDisplay();
display.setCursor(0, 0);
display.print("Helmet Ready");
display.display();
delay(1000);
}
void loop() {
// Read sensor data
int16_t ax, ay, az, gx, gy, gz;
mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
// Convert to g-force
float fx = ax / 16384.0;
float fy = ay / 16384.0;
float fz = az / 16384.0;
float g_mag = sqrt(fx * fx + fy * fy + fz * fz);
Serial.print("g_mag: ");
Serial.println(g_mag);
// Update Firebase with sensor data periodically
if (millis() - lastDataUpdate > DATA_UPDATE_INTERVAL) {
updateFirebaseData(g_mag, fx, fy, fz);
lastDataUpdate = millis();
}
// Check for crash detection
if ((g_mag > 2.0) && !crashSent) {
handleCrashDetection(g_mag);
crashSent = true;
}
// Reset crash flag if acceleration normalizes
if (g_mag <= 1.5 && crashSent) {
crashSent = false;
Firebase.RTDB.setString(&fbdo, "/helmet/status", "Normal");
// Clear OLED display
display.clearDisplay();
display.setTextSize(1);
display.setCursor(0, 0);
display.print("Status: Normal");
display.display();
}
delay(100);
}
void handleCrashDetection(float g_mag) {
Serial.println("CRASH DETECTED!");
// Update Firebase status
Firebase.RTDB.setString(&fbdo, "/helmet/status", "Crash Detected");
// Add crash alert to alerts log
String alertId = String(millis());
String alertPath = "/helmet/alerts/" + alertId;
Firebase.RTDB.setString(&fbdo, alertPath + "/type", "Crash");
Firebase.RTDB.setInt(&fbdo, alertPath + "/timestamp", millis());
Firebase.RTDB.setFloat(&fbdo, alertPath + "/location/latitude", 0.0); // Add GPS if ava
Firebase.RTDB.setFloat(&fbdo, alertPath + "/location/longitude", 0.0);
// Display crash message on OLED
display.clearDisplay();
display.setTextSize(2);
display.setTextColor(SSD1306_WHITE);
display.setCursor(0, 20);
display.println("CRASH");
display.setCursor(0, 45);
display.println("DETECTED");
display.display();
}
void updateFirebaseData(float g_mag, float x, float y, float z) {
// Update acceleration data
Firebase.RTDB.setFloat(&fbdo, "/helmet/acceleration/magnitude", g_mag);
Firebase.RTDB.setFloat(&fbdo, "/helmet/acceleration/x", x);
Firebase.RTDB.setFloat(&fbdo, "/helmet/acceleration/y", y);
Firebase.RTDB.setFloat(&fbdo, "/helmet/acceleration/z", z);
// Update heart rate (simulated for demo)
int heartRate = random(60, 100);
Firebase.RTDB.setInt(&fbdo, "/helmet/heartRate/bpm", heartRate);
// Update location (simulated for demo)
Firebase.RTDB.setFloat(&fbdo, "/helmet/location/latitude", 12.9716);
Firebase.RTDB.setFloat(&fbdo, "/helmet/location/longitude", 77.5946);
// Set normal status if no crash
if (!crashSent) {
Firebase.RTDB.setString(&fbdo, "/helmet/status", "Normal");
}
}