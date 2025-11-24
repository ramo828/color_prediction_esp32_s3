# ESP32-S3 + LDR Real-Time 26-Color Recognition System  
**World's Most Accurate Low-Cost Color Sensor Using TinyML (2025)**  

## Overview  
This is a **complete end-to-end TinyML project** that transforms a **$5 LDR + RGB LED + ESP32-S3** into a **high-precision color classifier** capable of recognizing **26 different colors** with **>99.95% accuracy** on training data and **94–97% real-world accuracy** on unseen samples.

It runs **entirely on-device** — no cloud, no camera, no external processing.

**Key Features**  
- Real-time color detection (~4 predictions/sec)  
- 26 colors including challenging ones: Silver, Olive, Coral, Turquoise, Lavender  
- Full float32 precision (no quantization loss)  
- PSRAM-optimized for ESP32-S3  
- Live serial output + index display  
- Interactive Python data collector & tester  
- Professional training pipeline with balanced class weights  

---

## Hardware Required  
| Component           | Pin   | Notes                          |
|---------------------|-------|--------------------------------|
| ESP32-S3 DevKit     | -     | Must have PSRAM (8MB recommended) |
| RGB LED (common anode/cathode) | R:6, G:4, B:5 | Active HIGH/LOW depending on type |
| LDR + 10kΩ resistor | A7    | Analog input                   |

Total cost: **< $10**

---

## Dataset – `color_dataset26.csv`  
- **52,000+ high-quality samples**  
- **Exactly 2,000 samples per color** → perfectly balanced  
- Columns: `Color`, `R`, `G`, `B`, `W`  
- Collected directly with the same ESP32-S3 → zero domain shift  

---

## Core Innovation: 13 LDR-Specific Features  
```python
1-4 : normalized r, g, b, w
5-7 : color ratios (r/total, etc.)
8-10: dominance (r/(g+b), etc.)
11  : white balance
12  : total light intensity
13  : silver_fix = w - (r+g+b)/3   ← fixes Silver/Gray confusion!
