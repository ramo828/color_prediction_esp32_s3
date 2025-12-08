# 🌈 ESP32-S3 Advanced Color Prediction System (TensorFlow Lite)

[**English**](#english) | [**Türkçe**](#türkçe) | [**Azərbaycan**](#azərbaycan)

---

<a name="english"></a>
## 🇬🇧 English: High-Precision 30-Color Recognition with TinyML

### 📌 Overview
This project is a state-of-the-art **TinyML color recognition system** built on the **ESP32-S3**. Unlike simple RGB sensors, this system uses a standard **LDR (Light Dependent Resistor)** paired with an **RGB LED** to achieve medical-grade color classification accuracy.

By leveraging **TensorFlow Lite for Microcontrollers**, the system extracts **29 advanced optical features** from raw sensor data to distinguishing between 30 distinct colors, including complex shades like "Coral", "Salmon", "Turquoise", and "Silver" vs "Gray".

### ✨ Key Features
*   **30 Distinct Colors**: Detects a wide spectrum including metallic and subtle shades.
*   **Deep Feature Extraction**: Calculates **29 unique spectral features** (ratios, log-diffs, hyperbolic tangents) from 5 raw readings (Red, Green, Blue, White, Dark).
*   **Edge AI**: Runs a quantized neural network fully on-device (ESP32-S3).
*   **Auto-Calibration**: Uses "Dark" and "White" readings to normalize against ambient light and sensor variations.
*   **High Speed**: ~4 predictions per second.

### 🛠 Hardware Setup
*   **Microcontroller**: ESP32-S3 (Required for PSRAM & vector instructions).
*   **Sensor**: GL55xx LDR (Light Dependent Resistor) + 10kΩ Pull-down Resistor.
*   **Light Source**: Common Anode/Cathode RGB LED.
*   **Wiring**:

| Component | ESP32-S3 Pin | Function |
| :--- | :--- | :--- |
| **LDR Sensor** | **GPIO 7** | Analog Input (ADC) |
| **RGB LED - Red** | **GPIO 6** | Red Light Source |
| **RGB LED - Green** | **GPIO 4** | Green Light Source |
| **RGB LED - Blue** | **GPIO 5** | Blue Light Source |

*(Note: Ensure LDR is shielded from direct external light for best results.)*

### 🔬 Working Principle
1.  **Data Acquisition**: The ESP32 cycles the RGB LED (Red, Green, Blue, All On/White) and takes a "Dark" reading (All Off).
2.  **Feature Engineering**: The raw ADC values are inverted and normalized. Then, complex features are computed (e.g., `salmon_signature`, `silver_fix`, `green_purity`) to mimicking human color perception logic.
3.  **Inference**: These 29 features are fed into a **TensorFlow Lite** model loaded from the filesystem (`LittleFS`).
4.  **Prediction**: The model outputs probability scores for all 30 classes, selecting the highest confidence match.

### 🎨 Supported Colors (30)
Aquamarine, Beige, Black, Blue, Brown, Coral, Cyan, Fuchsia, Gold, Gray, Green, Indigo, Khaki, Lavender, Lime, Magenta, Maroon, Navy, Olive, Orange, Pink, Purple, Red, Salmon, Silver, Teal, Turquoise, Violet, White, Yellow.

### 📂 File Structure
*   `color_prediction.ino`: Main firmware for real-time inference.
*   `make_dataset/make_dataset.py`: Python script for collecting high-quality training data.
*   `control_led.cpp/h`: Low-level hardware abstraction for LED and LDR control.

---

<a name="türkçe"></a>
## 🇹🇷 Türkçe: ESP32-S3 ile Yüksek Hassasiyetli 30 Renk Tanıma Sistemi

### 📌 Genel Bakış
Bu proje, **ESP32-S3** üzerinde çalışan gelişmiş bir **TinyML renk tanıma sistemidir**. Pahalı renk sensörleri yerine, sadece basit bir **LDR (Işığa Duyarlı Direnç)** ve **RGB LED** kullanarak endüstriyel kalitede renk ayrımı yapar.

Sistem, **TensorFlow Lite** teknolojisini kullanarak, ham sensör verilerinden **29 karmaşık optik özellik** çıkarır. Bu sayede "Mercan (Coral)" ile "Somon (Salmon)" veya "Gümüş (Silver)" ile "Gri (Gray)" gibi ayırt edilmesi zor renkleri bile kusursuzca tanır.

### ✨ Temel Özellikler
*   **30 Farklı Renk**: Metalik ve pastel tonlar dahil geniş renk yelpazesi.
*   **Derin Özellik Çıkarımı**: Ham verilerden (Kırmızı, Yeşil, Mavi, Beyaz, Karanlık) türetilen **29 matematiksel özellik**.
*   **Uçta Yapay Zeka (Edge AI)**: İnternet gerektirmez, tüm işlem ESP32 işlemcisinde yapılır.
*   **Otomatik Kalibrasyon**: Ortam ışığından etkilenmemek için "Karanlık" ve "Beyaz" referans değerlerini kullanır.

### 🛠 Donanım Kurulumu
*   **Mikrodenetleyici**: ESP32-S3.
*   **Sensör**: LDR + 10kΩ Direnç.
*   **Işık Kaynağı**: RGB LED.
*   **Bağlantı Şeması**:

| Bileşen | ESP32-S3 Pini | Görev |
| :--- | :--- | :--- |
| **LDR Sensör** | **GPIO 7** | Analog Okuma (ADC) |
| **RGB LED - Kırmızı** | **GPIO 6** | Kırmızı Işık |
| **RGB LED - Yeşil** | **GPIO 4** | Yeşil Işık |
| **RGB LED - Mavi** | **GPIO 5** | Mavi Işık |

### 🔬 Çalışma Prensibi
1.  **Veri Toplama**: Sistem sırasıyla LED'leri yakar (R, G, B, Beyaz) ve bir de sönük (Karanlık) ölçüm alır.
2.  **Özellik Mühendisliği**: Okunan ham değerler işlenerek `green_purity`, `silver_fix` gibi renk karakteristiğini belirleyen 29 farklı sayısal değere dönüştürülür.
3.  **Tahminleme**: Bu değerler, eğitilmiş **TensorFlow Lite** modeline girer ve en yüksek olasılıklı renk ekrana yazdırılır.

### 🎨 Tanınan Renkler (30)
Aquamarine, Beige, Black, Blue, Brown, Coral, Cyan, Fuchsia, Gold, Gray, Green, Indigo, Khaki, Lavender, Lime, Magenta, Maroon, Navy, Olive, Orange, Pink, Purple, Red, Salmon, Silver, Teal, Turquoise, Violet, White, Yellow.

---

<a name="azərbaycan"></a>
## 🇦🇿 Azərbaycan: ESP32-S3 ilə Yüksək Dəqiqlikli 30 Rəng Tanıma Sistemi

### 📌 Ümumi Məlumat
Bu layihə, **ESP32-S3** mikrokontrolleri üzərində işləyən yüksək səviyyəli bir **TinyML rəng tanıma sistemidir**. Bahalı rəng sensorlarına ehtiyac olmadan, sadə bir **LDR (İşığa Həssas Rezistor)** və **RGB LED** istifadə edərək rəngləri mükəmməl dəqiqliklə ayırır.

Yaradılan sistem **TensorFlow Lite** modelindən istifadə edir və sensor oxumalarından **29 xüsusi optik göstərici** (feature) hesablayır. Bu üsulla "Salmon" və "Coral" kimi bir-birinə çox bənzəyən rəngləri belə səhvsiz tanıyır.

### ✨ Əsas Özəlliklər
*   **30 Fərqli Rəng**: Metalik rənglər (Gümüş, Qızıl) və incə tonlar daxil olmaqla.
*   **Ağıllı Alqoritm**: R, G, B, Ağ və Qaranlıq oxumalarından alınan **29 riyazi parametr**.
*   **Tam Avtonom**: İnternetə ehtiyac yoxdur, model birbaşa cihazda işləyir.
*   **Stabil İşləmə**: İşıq dəyişikliklərinə qarşı kalibrasiya sistemi mövcuddur.

### 🛠 Qoşulma Qaydası (Pinout)
*   **Beyin**: ESP32-S3.
*   **Göz**: LDR + 10kΩ Rezistor.
*   **İşıq**: RGB LED.
*   **Pinlər**:

| Komponent | ESP32-S3 Pini | Funksiya |
| :--- | :--- | :--- |
| **LDR Sensor** | **GPIO 7** | Analog Giriş |
| **RGB LED - Qırmızı** | **GPIO 6** | Qırmızı İşıq |
| **RGB LED - Yaşıl** | **GPIO 4** | Yaşıl İşıq |
| **RGB LED - Mavi** | **GPIO 5** | Mavi İşıq |

### 🔬 İş Prinsipi
1.  **Oxuma**: Cihaz sürətlə LED rənglərini dəyişir (Qırmızı, Yaşıl, Mavi, Ağ) və LDR üzərindən əks olunan işığı ölçür. Həmçinin LED sönülü ikən (Qaranlıq) mühit işığını ölçür.
2.  **Hesablama**: Alınan 5 xam dəyər 29 fərqli məntiqi dəyişənə çevrilir (Məsələn: qırmızının yaşıla nisbəti, parlaqlıq balansı və s.).
3.  **Nəticə**: Bu məlumatlar süni zəka modelinə göndərilir və model 30 rəng arasından ən uyğun olanını seçir.

### 🎨 Dəstəklənən Rənglər (30)
Aquamarine (Akvamarin), Beige (Bej), Black (Qara), Blue (Mavi), Brown (Qəhvəyi), Coral (Mərcan), Cyan (Mavi-yaşıl), Fuchsia (Fuksiya), Gold (Qızıl), Gray (Boz), Green (Yaşıl), Indigo (İndiqo), Khaki (Xaki), Lavender (Lavanda), Lime (Laym), Magenta (Maqneta), Maroon (Tünd qırmızı), Navy (Tünd göy), Olive (Zeytun), Orange (Narıncı), Pink (Çəhrayı), Purple (Bənövşəyi), Red (Qırmızı), Salmon (Somon), Silver (Gümüşü), Teal (Dəniz mavisi), Turquoise (Firuzəyi), Violet (Bənövşə), White (Ağ), Yellow (Sarı).
