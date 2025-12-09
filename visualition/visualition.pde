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

// Bu kod Processing IDE (3.x və ya 4.x) istifadə olunaraq işlədilir.
// Məqsəd: Arduino-dan gələn serial məlumatları (Rəng adı və Dəqiqlik) oxuyaraq ekranda vizuallaşdırmaq.

import processing.serial.*;

Serial port;

float targetR = 0, targetG = 0, targetB = 0;
float currentR = 0, currentG = 0, currentB = 0;
String currentColorName = "Bağlantı gözlənilir...";
float confidence = 0;

// Setup funksiyası: Proqram başlayanda bir dəfə işləyir.
// Serial portları axtarır və uyğun gələnə qoşulur.
void setup() {
  size(900, 600);
  surface.setTitle("Ramo828 – Real-time Rəng Tanıyıcı");
  textAlign(CENTER, CENTER);

  println("Mövcud portlar:");
  printArray(Serial.list());

  if (Serial.list().length == 0) {
    println("Port tapılmadı!");
    return;
  }

  String portName = "";
  for (String p : Serial.list()) {
    if (p.contains("ttyACM") || p.contains("ttyUSB") || p.contains("COM")) {
      portName = p;
      break;
    }
  }
  if (portName.equals("")) portName = Serial.list()[0];

  port = new Serial(this, portName, 115200);
  port.clear();
  println("Qoşuldu: " + portName);
}

// Draw funksiyası: Sonsuz dövrü olaraq işləyir və ekranı yeniləyir.
// Rənglərin hamar keçidini (lerp) təmin edir və mətnləri ekrana yazır.
void draw() {
  currentR = lerp(currentR, targetR, 0.15);
  currentG = lerp(currentG, targetG, 0.15);
  currentB = lerp(currentB, targetB, 0.15);

  background(currentR, currentG, currentB);

  fill(255, 240);
  noStroke();
  rect(40, 40, width-80, height-80, 50);

  fill(20);
  textSize(80);
  text(currentColorName, width/2, height/2 - 50);

  textSize(40);
  // NaN olmasın deyə əlavə yoxlama
  String confText = (Float.isNaN(confidence)) ? "--" : nf(confidence, 0, 1);
  text(confText + "% etibarlılıq", width/2, height/2 + 30);

  textSize(24);
  fill(100);
  text("Ramo828 – ESP32-S3 Rəng Tanıyıcı", width/2, height - 70);
}
// SerialEvent: Arduino-dan yeni məlumat gəldikdə avtomatik çağırılır.
// Gələn mətni oxuyur, "→" işarəsinə görə ayırır və rəng adını çıxarır.
void serialEvent(Serial p) {
  String incoming = p.readStringUntil('\n');
  if (incoming == null) return;
  
  incoming = incoming.trim();
  if (incoming.length() < 20) return;

  println("Gəldi: " + incoming);

  int arrowIndex = incoming.indexOf("→");
  if (arrowIndex == -1) return;

  String result = incoming.substring(arrowIndex + 1).trim(); // "Navy (99.5%%)"

  // ( və ) arasında nə varsa alırıq
  int start = result.indexOf("(");
  int end   = result.indexOf(")");

  if (start == -1 || end == -1 || end <= start) return;

  currentColorName = result.substring(0, start).trim(); // "Navy"

  // (99.5%%) → 99.5%% alırıq
  String rawConf = result.substring(start + 1, end);

  // 99.5%% → 99.5 edirik (son %% silinir)
  if (rawConf.endsWith("%")) {
    rawConf = rawConf.substring(0, rawConf.length() - 1);
  }
  if (rawConf.endsWith("%")) {
    rawConf = rawConf.substring(0, rawConf.length() - 1);
  }

  try {
    confidence = float(rawConf);
  } catch (Exception e) {
    confidence = 0;
  }

  color c = getColorByName(currentColorName);
  targetR = red(c);
  targetG = green(c);
  targetB = blue(c);

  println("Tanıdı: " + currentColorName + " → " + confidence + "%");
}
// Bu funksiya verilmiş rəng adına uyğun HEX (RGB) kodunu qaytarır.
// Vizualizasiya zamanı arxa fon rəngini təyin etmək üçün istifadə olunur.
color getColorByName(String name) {
  switch(name) {
    case "Aquamarine": return #7FFFD4;
    case "Beige":      return #F5F5DC;
    case "Black":      return #000000;
    case "Blue":       return #0000FF;
    case "Brown":      return #A52A2A;
    case "Coral":      return #FF7F50;
    case "Cyan":       return #00FFFF;
    case "Fuchsia":    return #FF00FF;
    case "Gold":       return #FFD700;
    case "Gray":       return #808080;
    case "Green":      return #008000;
    case "Indigo":     return #4B0082;
    case "Khaki":      return #F0E68C;
    case "Lavender":   return #E6E6FA;
    case "Lime":       return #00FF00;
    case "Magenta":    return #FF00FF;
    case "Maroon":     return #800000;
    case "Navy":       return #000080;
    case "Olive":      return #808000;
    case "Orange":     return #FFA500;
    case "Pink":       return #FFC0CB;
    case "Purple":     return #800080;
    case "Red":        return #FF0000;
    case "Salmon":     return #FA8072;
    case "Silver":     return #C0C0C0;
    case "Teal":       return #008080;
    case "Turquoise":  return #40E0D0;
    case "Violet":     return #EE82EE;
    case "White":      return #FFFFFF;
    case "Yellow":     return #FFFF00;
    default:           return #666666;
  }
}
