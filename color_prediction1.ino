#define RED    6
#define GREEN  4
#define BLUE   5
#define SENSOR 7

int pins[3] = {RED, GREEN, BLUE};

unsigned long previousMillis = 0;
const long interval = 80;
int currentColor = 0;
bool whiteMeasured = false;

int raw_r = 0;
int raw_g = 0;
int raw_b = 0;
int raw_w = 0;

// PYTHON'DAKİ le.classes_ SIRASINA %100 UYAN RENK LİSTESİ!!!
const char* colors[] = {
  "Beige", "Black", "Blue", "Brown", "Coral", "Cyan", "Gold", "Gray",
  "Green", "Indigo", "Lavender", "Lime", "Magenta", "Maroon", "Navy",
  "Olive", "Orange", "Pink", "Purple", "Red", "Silver", "Teal",
  "Turquoise", "Violet", "White", "Yellow"
};
const int num_colors = 26;

// TFLITE
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "model.h"

namespace {
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;

  constexpr int kTensorArenaSize = 300 * 1024;
  uint8_t* tensor_arena = nullptr;
}

void predictColor() {
  const float max_val = 3373.0f;  // Senin gerçek veri seti max değeri

  float r = (max_val - raw_r) / max_val;
  float g = (max_val - raw_g) / max_val;
  float b = (max_val - raw_b) / max_val;
  float w = (max_val - raw_w) / max_val;

  float total = r + g + b + 1e-8f;
  float silver_fix = w - (r + g + b) / 3.0f;

  float features[13] = {
    r, g, b, w,
    r/total, g/total, b/total,
    r/(g+b+1e-8f), g/(r+b+1e-8f), b/(r+g+1e-8f),
    w/total, total,
    silver_fix
  };

  for (int i = 0; i < 13; i++) {
    input->data.f[i] = features[i];
  }

  if (interpreter->Invoke() != kTfLiteOk) {
    Serial.println("Invoke hatası!");
    return;
  }

  int best_index = 0;
  float best_score = output->data.f[0];
  for (int i = 1; i < num_colors; i++) {
    if (output->data.f[i] > best_score) {
      best_score = output->data.f[i];
      best_index = i;
    }
  }

  // GÜZEL ÇIKTI + İNDEKS GÖSTERİMİ
  Serial.printf("Değerler → %4d %4d %4d %4d  |  ", raw_r, raw_g, raw_b, raw_w);
  Serial.printf("İNDEKS: %2d  →  ", best_index);
  Serial.print(colors[best_index]);
  Serial.print("  (%");
  Serial.print(best_score * 100.0f, 1);
  Serial.println(")");
  Serial.println("─────────────────────────────────────────────");
}

void setup() {
  Serial.begin(115200);

  tensor_arena = (uint8_t*)heap_caps_malloc(kTensorArenaSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!tensor_arena) {
    Serial.println("PSRAM ayrılamadı!");
    while (1);
  }

  for (int i = 0; i < 3; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }

  model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model versiyon hatası!");
    while (1);
  }

  static tflite::MicroMutableOpResolver<3> resolver;
  resolver.AddFullyConnected();
  resolver.AddRelu6();
  resolver.AddSoftmax();

  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("Bellek hatası!");
    while (1);
  }

  input = interpreter->input(0);
  output = interpreter->output(0);

  Serial.println("LDR RENK TANIYICI BAŞLATILDI!");
  Serial.println("Gerçek zamanlı ölçüm başlıyor...\n");
}

void loop() {
  unsigned long now = millis();

  if (now - previousMillis >= interval) {
    previousMillis = now;

    // Önceki LED'i kapat
    digitalWrite(pins[(currentColor + 2) % 3], LOW);
    // Yeni LED'i yak
    digitalWrite(pins[currentColor], HIGH);

    delay(30);  // LDR kararlı olsun
    int val = analogRead(SENSOR);

    // Değeri kaydet
    if (currentColor == 0)      raw_r = val;
    else if (currentColor == 1) raw_g = val;
    else                        raw_b = val;

    int next = (currentColor + 1) % 3;

    // Beyaz ölçümü (tüm LED'ler açık)
    if (next == 0 && !whiteMeasured) {
      digitalWrite(RED, HIGH);
      digitalWrite(GREEN, HIGH);
      digitalWrite(BLUE, HIGH);
      delay(30);
      raw_w = analogRead(SENSOR);
      digitalWrite(RED, LOW);
      digitalWrite(GREEN, LOW);
      digitalWrite(BLUE, LOW);

      whiteMeasured = true;

      // TAHMİN YAP!
      predictColor();
    } else {
      whiteMeasured = false;
    }

    currentColor = next;
  }
}