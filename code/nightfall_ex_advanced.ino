//======== NIGHTFALL-EX COMPLETE CODE======
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <sneham7568-project-1_inferencing.h>

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ================= PINS =================
#define ECG_PIN    34
#define LO_PLUS    32
#define LO_MINUS   33

// GPS (UART1)
#define GPS_RX     16
#define GPS_TX     17

// GSM (UART2)
#define GSM_RX     26
#define GSM_TX     25

#define BUTTON_PIN 4

// SD Card
#define SD_CS   5
#define SD_SCK  18
#define SD_MOSI 23
#define SD_MISO 19

File sdFile;

// ================= ECG BUFFER =================
#define ECG_BUFFER_SIZE SCREEN_WIDTH
float ecgBuffer[ECG_BUFFER_SIZE];
int ecgIndex = 0;

// ================= FILTER =================
float prevInput = 0, prevOutput = 0;
float alpha = 0.2;
float amplitudeScale = 1.8;

// ================= BPM =================
unsigned long lastPeak = 0;
float bpm = 0;
bool peakDetected = false;
String ecgCondition = "Normal";

// ================= GPS =================
HardwareSerial gpsSerial(1);
TinyGPSPlus gps;

String gpsText = "GPS: NO FIX";
String gpsLat = "0.0";
String gpsLng = "0.0";

unsigned long lastGPSUpdate = 0;

// ================= GSM =================
HardwareSerial GSM(2);
const long GSM_BAUD = 115200;

// ================= BUTTON =================
unsigned long lastPress = 0;
int pressCount = 0;
bool buttonLast = HIGH;
bool longHandled = false;

const unsigned long DOUBLE_CLICK_GAP = 500;
const unsigned long LONG_PRESS_TIME  = 2000;

// ================= SD LOGGING =================
bool loggingToSD = false;

// ================= OLED MESSAGE =================
String oledFlashMsg = "";
unsigned long oledFlashUntil = 0;

// ================= TIMING =================
unsigned long lastEcgTime = 0;
unsigned long ecgInterval = 8;

// ================= ECG HISTORY =================
struct EcgRecord {
  unsigned long time;
  int bpm;
  String condition;
};

#define HISTORY_SIZE 10
EcgRecord history[HISTORY_SIZE];
int historyIndex = 0;
unsigned long lastHistoryUpdate = 0;

// ================= EDGE IMPULSE =================
static float ei_buffer[EI_CLASSIFIER_RAW_SAMPLE_COUNT];
static uint16_t ei_buf_index = 0;
unsigned long lastEiSampleTime = 0;


// ================= PRINT FUNCTION =================
void ei_printf(const char *format, ...) {
  static char print_buf[256];
  va_list args;
  va_start(args, format);
  vsnprintf(print_buf, sizeof(print_buf), format, args);
  va_end(args);
  Serial.print(print_buf);
}

// ================= FUNCTION DECLARATIONS =================
void ei_collect_sample(float value);
void ei_run_inference();
void startSdLogging();
void stopSdLogging();
void sendTacticalAmbulanceSMS(bool includeHistory10s);


// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\n=== Nightfall-EX Boot ===");

  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  GSM.begin(GSM_BAUD, SERIAL_8N1, GSM_RX, GSM_TX);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LO_PLUS, INPUT);
  pinMode(LO_MINUS, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true);
  }

  display.clearDisplay();
  display.display();

  for (int i = 0; i < ECG_BUFFER_SIZE; i++) {
    ecgBuffer[i] = SCREEN_HEIGHT / 2;
  }

  // SD Init
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI);

  if (!SD.begin(SD_CS)) {
    Serial.println("SD failed");
  } else {
    Serial.println("SD OK");
  }

  Serial.println("System Ready");
}
