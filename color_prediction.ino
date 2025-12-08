/*
 * MIT License
 *
 * Copyright (c) 2025
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "LittleFS.h"
#include "control_led.h"

// Pin təyinatları (Pin tanımları)
#define RED 6
#define GREEN 4
#define BLUE 5
#define SENSOR 7

// LED strukturu yaradılır (LED yapısı oluşturuluyor)
ControlLED::Leds leds = {
  .R = RED,
  .G = GREEN,
  .B = BLUE,
  .S = SENSOR, 
};

// ControlLED obyekti yaradılır (ControlLED nesnesi oluşturuluyor)
ControlLED control = ControlLED(&leds);

// Rəng adları siyahısı (Renk isimleri listesi)
const char* colors[30] = {
  "Aquamarine", "Beige", "Black", "Blue", "Brown", "Coral", "Cyan", "Fuchsia", "Gold", "Gray",
  "Green", "Indigo", "Khaki", "Lavender", "Lime", "Magenta", "Maroon", "Navy", "Olive", "Orange",
  "Pink", "Purple", "Red", "Salmon", "Silver", "Teal", "Turquoise", "Violet", "White", "Yellow"
};

// TensorFlow Lite üçün qlobal dəyişənlər (Global variables for TensorFlow Lite)
namespace {
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
// Tensor arenası üçün yaddaş ayrılır (Tensor arena için bellek ayrılıyor)
constexpr int kTensorArenaSize = 3800 * 1024;  // 3.8 MB → 29 xüsusiyyət + böyük model üçün
uint8_t* tensor_arena = nullptr;
}

// Modeli LittleFS-dən yükləmək üçün funksiya (Modeli LittleFS'den yüklemek için fonksiyon)
bool loadModel() {
  File f = LittleFS.open("/model.tflite", "r");
  if (!f || f.size() == 0) {
    Serial.println("model.tflite yoxdur!");
    return false;
  }

  size_t sz = f.size();
  uint8_t* buf = (uint8_t*)ps_malloc(sz);
  if (!buf) {
    Serial.println("PSRAM kifayət etmir!");
    f.close();
    return false;
  }
  f.read(buf, sz);
  f.close();

  model = tflite::GetModel(buf);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model versiya xətası!");
    return false;
  }
  Serial.printf("Model yükləndi: %d KB\n", sz / 1024);
  return true;
}

// Rəngi proqnozlaşdırmaq üçün funksiya (Rengi tahmin etmek için fonksiyon)
void predictColor(int raw_r, int raw_g, int raw_b, int raw_w, int raw_d) {
  const float max_val = 3993.0f;

  // Python-dakı kimi tərsinə çevir
  float r_raw = max_val - raw_r;
  float g_raw = max_val - raw_g;
  float b_raw = max_val - raw_b;
  float w_raw = max_val - raw_w;
  float d_raw = raw_d;
  d_raw = max(0.0f, min(d_raw, max_val));  // clip

  // Dəyərləri normallaşdırır (Değerleri normalleştirir)
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

  // PYTHON-DAKI İLƏ EYNİ 29 XÜSUSİYYƏT!
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
  if (max_conf < 0.6f) Serial.println("  Aşağı etibarlılıq!");
  Serial.println("────────────────────────────────────");
}

// Setup funksiyası (Setup fonksiyonu)
void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  control.allLedsOff();

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS xətası!");
    while (1)
      ;
  }
  if (!loadModel()) {
    Serial.println("Model yüklənmədi!");
    while (1)
      ;
  }

  tensor_arena = (uint8_t*)ps_malloc(kTensorArenaSize);
  if (!tensor_arena) {
    Serial.println("PSRAM xətası!");
    while (1)
      ;
  }

  static tflite::MicroMutableOpResolver<5> resolver;
  resolver.AddFullyConnected();
  resolver.AddRelu();
  resolver.AddSoftmax();
  resolver.AddTanh();
  resolver.AddLogistic();  // log1p üçün

  static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;
  interpreter->AllocateTensors();

  input = interpreter->input(0);
  output = interpreter->output(0);

  Serial.println("LDR RƏNG TANIYICI – Python ilə %100 eyni xüsusiyyətlər!");
  delay(2000);
}

// Əsas döngü (Ana döngü)
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