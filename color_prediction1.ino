#define RED    6      // Qırmızı LED 6-cı pinə bağlıdır
#define GREEN  4      // Yaşıl LED 4-cü pinə bağlıdır
#define BLUE   5      // Mavi LED 5-ci pinə bağlıdır
#define SENSOR 7      // LDR (işıq sensoru) 7-ci pinə bağlıdır (analog giriş)

int pins[3] = {RED, GREEN, BLUE};  
// LED pinlərini bir massivdə saxlayırıq ki, dövr ilə asanlıqla idarə edək

unsigned long previousMillis = 0;  
// Son dəfə LED dəyişdirilən zamanı saxlayır (millisaniyə ilə)

const long interval = 80;  
// Hər LED 80 millisaniyə yanır – bu LDR üçün ən ideal sürətdir

int currentColor = 0;  
// Hansı rəng LED-in indi yanılı olduğunu göstərir (0=Qırmızı, 1=Yaşıl, 2=Mavi)

bool whiteMeasured = false;  
// Beyaz ölçmə (R+G+B birlikdə) edilib-edilmədiyini yoxlayır

int raw_r = 0;  // Qırmızı LED yanarkən LDR-dən alınan xam dəyər
int raw_g = 0;  // Yaşıl LED yanarkən alınan xam dəyər
int raw_b = 0;  // Mavi LED yanarkən alınan xam dəyər
int raw_w = 0;  // Bütün LED-lər yanarkən (ağ işıq) alınan xam dəyər

// Python-dakı LabelEncoder sırasına %100 uyğun rəng siyahısı!
// Bu sıra dəyişsə, model səhv rəng deyər!
const char* colors[] = {
  "Beige", "Black", "Blue", "Brown", "Coral", "Cyan", "Gold", "Gray",
  "Green", "Indigo", "Lavender", "Lime", "Magenta", "Maroon", "Navy",
  "Olive", "Orange", "Pink", "Purple", "Red", "Silver", "Teal",
  "Turquoise", "Violet", "White", "Yellow"
};
const int num_colors = 26;  // Cəmi 26 rəng tanıyırıq

// TensorFlow Lite Micro üçün lazım olan kitabxanalar
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "model.h"  // .tflite faylı burda C massivinə çevrilir

// Model üçün lazım olan dəyişənlər (global olaraq saxlanılır)
namespace {
  const tflite::Model* model = nullptr;           // TFLite modelinin özü
  tflite::MicroInterpreter* interpreter = nullptr; // Modelin işlədiyi interpreter
  TfLiteTensor* input = nullptr;                  // Modelə giriş tensoru
  TfLiteTensor* output = nullptr;                 // Modeldən çıxış tensoru

  constexpr int kTensorArenaSize = 300 * 1024;    // 300 KB yaddaş ayırırıq (PSRAM-dan)
  uint8_t* tensor_arena = nullptr;                // Bu yaddaşın göstəricisi
}

// RƏNG TƏXMİN FUNKSİYASI – ƏSAS İŞ BURADA BAŞ VERİR!
void predictColor() {
  const float max_val = 3373.0f;  // Sənin məlumat bazandakı ən yüksək dəyər (Python-da hesablanmış)

  // LDR-də yüksək dəyər = az işıq → ona görə əksinə çevirib normallaşdırırıq
  float r = (max_val - raw_r) / max_val;
  float g = (max_val - raw_g) / max_val;
  float b = (max_val - raw_b) / max_val;
  float w = (max_val - raw_w) / max_val;

  float total = r + g + b + 1e-8f;  // R+G+B cəmi (sıfıra bölünməmək üçün kiçik əlavə)
  float silver_fix = w - (r + g + b) / 3.0f;  // Gümüşü və boz tonları daha yaxşı ayırmaq üçün xüsusi düzəliş

  // Modelin gözlədiyi 13 xüsusiyyəti yaradırıq (Python-da olduğu kimi!)
  float features[13] = {
    r, g, b, w,
    r/total, g/total, b/total,
    r/(g+b+1e-8f), g/(r+b+1e-8f), b/(r+g+1e-8f),
    w/total, total,
    silver_fix
  };

  // Xüsusiyyətləri modelə göndəririk
  for (int i = 0; i < 13; i++) {
    input->data.f[i] = features[i];
  }

  // Model işləyir
  if (interpreter->Invoke() != kTfLiteOk) {
    Serial.println("Model işləmədi!");
    return;
  }

  // Ən yüksək ehtimalı tapırıq
  int best_index = 0;
  float best_score = output->data.f[0];
  for (int i = 1; i < num_colors; i++) {
    if (output->data.f[i] > best_score) {
      best_score = output->data.f[i];
      best_index = i;
    }
  }

  // Nəticəni gözəl şəkildə çap edirik
  Serial.printf("Dəyərlər → %4d %4d %4d %4d  |  ", raw_r, raw_g, raw_b, raw_w);
  Serial.printf("İNDEKS: %2d  →  ", best_index);
  Serial.print(colors[best_index]);
  Serial.print("  (%");
  Serial.print(best_score * 100.0f, 1);
  Serial.println(" etibarlılıq)");
  Serial.println("─────────────────────────────────────────────");
}

// SETUP – Cihaz ilk işə düşəndə bir dəfə icra olunur
void setup() {
  Serial.begin(115200);  // Seri monitoru aktivləşdiririk

  // PSRAM-dan 300 KB yaddaş ayırırıq (model üçün lazımdır)
  tensor_arena = (uint8_t*)heap_caps_malloc(kTensorArenaSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!tensor_arena) {
    Serial.println("PSRAM-dan yaddaş ayrıla bilmədi! PSRAM aktiv deyil?");
    while (1);  // Sonsuz döngüyə düşürük
  }

  // LED pinlərini çıxış olaraq təyin edib söndürürük
  for (int i = 0; i < 3; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }

  // Modeli yükləyirik
  model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model versiyası uyğun deyil!");
    while (1);
  }

  // Lazım olan əməliyyatları əlavə edirik (FullyConnected, ReLU6, Softmax)
  static tflite::MicroMutableOpResolver<3> resolver;
  resolver.AddFullyConnected();
  resolver.AddRelu6();
  resolver.AddSoftmax();

  // Interpreter-i yaradırıq
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  // Model üçün yaddaş ayırırıq
  if (interpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("Model üçün yaddaş ayrıla bilmədi!");
    while (1);
  }

  // Giriş və çıxış tensorlarını alırıq
  input = interpreter->input(0);
  output = interpreter->output(0);

  Serial.println("LDR RƏNG TANIYICI BAŞLATILDI!");
  Serial.println("Canlı ölçüm başladı...\n");
}

// LOOP – Sonsuz döngüdə daim işləyir
void loop() {
  unsigned long now = millis();

  // Hər 80ms-də bir dəfə LED dəyişdiririk
  if (now - previousMillis >= interval) {
    previousMillis = now;

    // Əvvəlki LED-i söndürürük
    digitalWrite(pins[(currentColor + 2) % 3], LOW);
    // Yeni LED-i yandırırıq
    digitalWrite(pins[currentColor], HIGH);

    delay(30);  // LDR-nin dəyəri sabitləşsin
    int val = analogRead(SENSOR);

    // Hansı rəng LED yanırdısa, onun dəyərini saxlayırıq
    if (currentColor == 0)      raw_r = val;
    else if (currentColor == 1) raw_g = val;
    else                        raw_b = val;

    int next = (currentColor + 1) % 3;

    // 3 rəng bitdikdən sonra ağ işıq ölçürük
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

      // BÜTÜN DƏYƏRLƏR TAMAM – İNDİ RƏNG TƏXMİN EDƏK!
      predictColor();
    } else {
      whiteMeasured = false;
    }

    currentColor = next;  // Növbəti rəngə keç
  }
}