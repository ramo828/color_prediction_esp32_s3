#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "LittleFS.h"
#include "control_led.h"

#define RED 6
#define GREEN 4
#define BLUE 5
#define SENSOR 7

ControlLED::Leds leds = {
  .R = RED,
  .G = GREEN,
  .B = BLUE,
  .S = SENSOR, 
};

ControlLED control = ControlLED(&leds);

const char* colors[30] = {
  "Aquamarine", "Beige", "Black", "Blue", "Brown", "Coral", "Cyan", "Fuchsia", "Gold", "Gray",
  "Green", "Indigo", "Khaki", "Lavender", "Lime", "Magenta", "Maroon", "Navy", "Olive", "Orange",
  "Pink", "Purple", "Red", "Salmon", "Silver", "Teal", "Turquoise", "Violet", "White", "Yellow"
};

namespace {
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
constexpr int kTensorArenaSize = 3800 * 1024;  // 3.8 MB → 29 özellik + büyük model için
uint8_t* tensor_arena = nullptr;
}

bool loadModel() {
  File f = LittleFS.open("/model.tflite", "r");
  if (!f || f.size() == 0) {
    Serial.println("model.tflite yok!");
    return false;
  }

  size_t sz = f.size();
  uint8_t* buf = (uint8_t*)ps_malloc(sz);
  if (!buf) {
    Serial.println("PSRAM yetersiz!");
    f.close();
    return false;
  }
  f.read(buf, sz);
  f.close();

  model = tflite::GetModel(buf);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model versiyon hatası!");
    return false;
  }
  Serial.printf("Model yüklendi: %d KB\n", sz / 1024);
  return true;
}

void predictColor(int raw_r, int raw_g, int raw_b, int raw_w, int raw_d) {
  const float max_val = 3993.0f;

  // Python'daki gibi ters çevir
  float r_raw = max_val - raw_r;
  float g_raw = max_val - raw_g;
  float b_raw = max_val - raw_b;
  float w_raw = max_val - raw_w;
  float d_raw = raw_d;
  d_raw = max(0.0f, min(d_raw, max_val));  // clip

  float r = r_raw / max_val;
  float g = g_raw / max_val;
  float b = b_raw / max_val;
  float w = w_raw / max_val;
  float d = d_raw / max_val;

  float total = r + g + b + 1e-8f;
  float total_light = total;

  float r_ratio = r / total;
  float g_ratio = g / total;
  float b_ratio = b / total;

  float red_power = r / (g + g + b + 1e-8f);
  float green_power = g / (r + b + 1e-8f);
  float blue_power = b / (r + g + 1e-8f);

  float white_balance = w / total;
  float silver_fix = w - (r + g + b) / 3.0f;

  auto safe_log1p = [](float x) {
    return log1p(max(0.0f, x));
  };

  float salmon_signature = safe_log1p(r_raw * g_raw / (b_raw * b_raw + w_raw * w_raw + 1e-8f));
  float green_dominance = tanh((g_raw - b_raw) / 1000.0f);
  float rg_over_white = safe_log1p((r_raw + g_raw) / (w_raw + 1e-8f));
  float fuchsia_index = safe_log1p(b_raw * b_raw / (r_raw + g_raw + 1e-8f));
  float red_weakness = safe_log1p((g_raw + b_raw) / (r_raw + 1e-8f));
  float blue_over_green = tanh((b_raw - g_raw) / 1000.0f);
  float green_purity = g_raw / (r_raw + g_raw + b_raw + w_raw + 1e-8f);

  float beige_index = total_light - (r + g + b) / 3.0f;
  float lightness_ratio = total_light / (r + g + b + 1e-8f);
  float lime_index = g_ratio * total_light;
  float yellow_index = (r_raw + g_raw) / (b_raw + w_raw + 1e-8f);
  float coral_salmon_diff = safe_log1p(r_raw * g_raw / (b_raw * b_raw + w_raw * w_raw + 1e-8f));
  float beige_white_diff = total_light - (r + g + b);
  float lime_score = g_ratio * (1.0f - white_balance);
  float gold_orange_diff = (r_raw + g_raw) / (b_raw + 1e-8f) - w_raw / max_val;

  // PYTHON'DAKİ İLE BİREBİR AYNI 29 ÖZELLİK!
  float features[29] = {
    r, g, b, w, d,
    r_ratio, g_ratio, b_ratio,
    red_power, green_power, blue_power,
    white_balance, total_light, silver_fix,
    salmon_signature, green_dominance,
    rg_over_white, fuchsia_index, red_weakness, blue_over_green,
    green_purity,
    yellow_index, lime_score, lime_index,
    beige_index, beige_white_diff,
    lightness_ratio, coral_salmon_diff, gold_orange_diff
  };

  for (int i = 0; i < 29; i++) input->data.f[i] = features[i];
  interpreter->Invoke();

  float max_conf = 0;
  int best = 0;
  for (int i = 0; i < 30; i++) {
    float c = output->data.f[i];
    if (c > max_conf) {
      max_conf = c;
      best = i;
    }
  }

  Serial.printf("R:%4d G:%4d B:%4d W:%4d D:%4d → %s (%.1f%%)\n",
                raw_r, raw_g, raw_b, raw_w, raw_d,
                colors[best], max_conf * 100);
  if (max_conf < 0.6f) Serial.println("  Düşük güven!");
  Serial.println("────────────────────────────────────");
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  control.allLedsOff();

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS hatası!");
    while (1)
      ;
  }
  if (!loadModel()) {
    Serial.println("Model yüklenemedi!");
    while (1)
      ;
  }

  tensor_arena = (uint8_t*)ps_malloc(kTensorArenaSize);
  if (!tensor_arena) {
    Serial.println("PSRAM hatası!");
    while (1)
      ;
  }

  static tflite::MicroMutableOpResolver<5> resolver;
  resolver.AddFullyConnected();
  resolver.AddRelu();
  resolver.AddSoftmax();
  resolver.AddTanh();
  resolver.AddLogistic();  // log1p için

  static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;
  interpreter->AllocateTensors();

  input = interpreter->input(0);
  output = interpreter->output(0);

  Serial.println("LDR RENK TANIYICI – Python ile %100 aynı özellikler!");
  delay(2000);
}

void loop() {
  control.allLedsOff();
  delay(50);
  int raw_d = control.readAvg();
  control.allLedsOn();
  delay(50);
  int raw_w = control.readAvg();
  control.ledOn(RED);
  delay(50);
  int raw_r = control.readAvg();
  control.ledOn(GREEN);
  delay(50);
  int raw_g = control.readAvg();
  control.ledOn(BLUE);
  delay(50);
  int raw_b = control.readAvg();
  control.allLedsOff();
  predictColor(raw_r, raw_g, raw_b, raw_w, raw_d);
  delay(1000);
}