#define RED    6        // Qırmızı LED 6-cı pində
#define GREEN  4        // Yaşıl LED 4-cü pində
#define BLUE   5        // Mavi LED 5-ci pində
#define SENSOR 7        // LDR (işıq sensoru) 7-ci analog pində

int pins[3] = {RED, GREEN, BLUE};                    // LED pinləri massivdə – dövr üçün rahat olsun
char colorLetters[3] = {'R', 'G', 'B'};              // Serial-da hansı rəngin ölçüldüyünü göstərmək üçün

unsigned long previousMillis = 0;                    // Son dəfə LED dəyişdirilən zaman (millisaniyə ilə)
const long interval = 80;                            // Hər rəng 80ms yanır – LDR üçün ən ideal sürət!
int currentColor = 0;                                // 0 = Qırmızı, 1 = Yaşıl, 2 = Mavi
bool whiteMeasured = false;                          // Ağ işıq (R+G+B birlikdə) ölçüldümü? (hər turda bir dəfə)

void setup() {
  Serial.begin(115200);                              // Serial monitoru 115200 sürətlə açırıq

  // Bütün LED pinlərini çıxış olaraq təyin edib söndürürük
  for (int i = 0; i < 3; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }

  delay(2000);                                       // Python skriptinin hazır olmasını gözləyirik
  Serial.println("START");                           // Python "START" görəndə veri toplamaya başlayacaq
}

void loop() {
  unsigned long now = millis();                      // Cari zamanı alırıq

  // Hər 80ms-də bir dəfə işləyirik
  if (now - previousMillis >= interval) {
    previousMillis = now;                            // Zamanlayıcı sıfırlanır

    // Əvvəlki LED-i söndürürük (modulo ilə dövrü idarə edirik)
    digitalWrite(pins[(currentColor + 2) % 3], LOW);

    // Yeni LED-i yandırırıq
    digitalWrite(pins[currentColor], HIGH);

    delay(30);                                       // LDR-nin dəyəri sabitləşsin (30ms kifayətdir)

    int value = analogRead(SENSOR);                 // LDR-dən dəyəri oxuyuruq

    // Serial-a göndəririk: R:1234, G:2345, B:3456 formatında
    Serial.print(colorLetters[currentColor]);
    Serial.print(":");
    Serial.println(value);

    // Bir tur tamamlandı mı? (yenidən qırmızıya qayıtdıqsa)
    int nextColor = (currentColor + 1) % 3;
    if (nextColor == 0 && !whiteMeasured) {
      // BÜTÜN LED-LƏRİ YANDIR – ağ işıq alırıq
      digitalWrite(RED, HIGH);
      digitalWrite(GREEN, HIGH);
      digitalWrite(BLUE, HIGH);

      delay(30);                                     // LDR sabitləşsin

      int whiteValue = analogRead(SENSOR);           // Ağ işıq ölçüsü (W)

      Serial.print("W:");                            // Python-a göndəririk
      Serial.println(whiteValue);

      // Bütün LED-ləri söndürürük – növbəti dövrə hazır olaq
      digitalWrite(RED, LOW);
      digitalWrite(GREEN, LOW);
      digitalWrite(BLUE, LOW);

      whiteMeasured = true;                          // Bu turda ağ işıq ölçüldü
    } else {
      whiteMeasured = false;                         // Növbəti turda yenidən ağ işıq ölçək
    }

    currentColor = nextColor;                        // Növbəti rəngə keçirik
  }
}
