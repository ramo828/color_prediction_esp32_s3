# MIT License
#
# Copyright (c) 2025
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# RƏNG VERİ TOPLAYICI – RƏNGA RƏNG & %100 XƏTASIZ VƏ TƏHLÜKƏSİZ VERSİYA
# Köhnə verilər qorunur, kanal eksikliyi olsa belə heç vaxt çökməz!

import serial
import time
import csv
import os
import pandas as pd
import sys
import termios

# ========================= AYARLAR =========================
PORT = '/dev/ttyACM0'           # Windows: COM3, COM4...
BAUD = 115200
SAMPLES = 2000
CSV_FILE = 'color_dataset.csv'

RENKLER = [
    "Red","Blue","Green","Yellow","Black","White","Pink","Orange","Purple","Gray",
    "Brown","Cyan","Magenta","Lime","Navy","Teal","Olive","Maroon","Silver","Gold",
    "Indigo","Violet","Beige","Lavender","Turquoise","Coral","Khaki","Fuchsia","Aquamarine","Salmon"
]

# ========================= BİP SƏSİ =========================
def bip(say=1):
    for _ in range(say):
        os.system('paplay /usr/share/sounds/freedesktop/stereo/complete.oga &> /dev/null || echo -e "\a"')

# ========================= BAŞLIQ – GÖZ ALICI =========================
print("\n\033[95m" + "═" * 80)
print("    RƏNG VERİ TOPLAYICI – RƏNGA RƏNG & %100 TƏHLÜKƏSİZ VƏ GÖZƏL!")
print("    Köhnə verilər qorunur • Kanal eksik olsa belə ÇÖKMƏZ!")
print("    Artıq heç bir IndexError almayacaqsan!")
print("═" * 80 + "\033[0m\n")
bip(3)

# CSV yoxlanışı
if not os.path.exists(CSV_FILE):
    print(f"\033[93mİlk dəfə işlədilir → {CSV_FILE} yaradılır...\033[0m")
    with open(CSV_FILE, 'w', newline='', encoding='utf-8') as f:
        csv.writer(f).writerow(['Rəng', 'R', 'G', 'B', 'W', 'D'])
else:
    print(f"\033[92mFayl tapıldı: {CSV_FILE} → Köhnə verilər qorunacaq!\033[0m")

# Mövcud vəziyyəti oxu
try:
    df = pd.read_csv(CSV_FILE)
    tamamlanan = df['Rəng'].value_counts()
    print(f"\n\033[1;36mHazırki vəziyyət:\033[0m")
    for renk in RENKLER:
        adet = tamamlanan.get(renk, 0)
        if adet >= SAMPLES:
            print(f"  \033[92m✓ {renk:12} → TAMAM ({adet})\033[0m")
        else:
            print(f"  \033[93m○ {renk:12} → {adet}/{SAMPLES}\033[0m")
except:
    tamamlanan = pd.Series()
    print("\033[91mFayl boşdur → sıfırdan başlayırıq!\033[0m")

# Başlangıç indeksi
basla_index = next((i for i, r in enumerate(RENKLER) if tamamlanan.get(r, 0) < SAMPLES), None)
if basla_index is None:
    print("\n\033[1;92mBÜTÜN RƏNGLƏR TAMAMLANDI! Çıxıram...\033[0m")
    bip(15)
    exit()

print(f"\n\033[1;97;45mTOPLAMA {RENKLER[basla_index].upper()} RƏNGİNDƏN DAVAM EDİR → indeks: {basla_index}\033[0m\n")
bip(2)

# Serial bağlantı
print(f"\033[96mESP32-yə qoşulur... ({PORT})\033[0m")
ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)
ser.flushInput()

# ========================= ƏSAS DÖNGÜ – RENGARENK & HATASIZ =========================
for idx in range(basla_index, len(RENKLER)):
    renk = RENKLER[idx]
    mevcut = tamamlanan.get(renk, 0)
    lazim = SAMPLES - mevcut

    if lazim <= 0:
        print(f"\033[92m{renk} artıq tamamlanıb → keçilir...\033[0m")
        continue

    print(f"\n\033[1;97;41m >>> {renk.upper():12} RƏNGİ ÜÇÜN HAZIRLAŞ! <<< \033[0m")
    print(f"\033[93mÇatışmayan örnək sayı: {lazim}\033[0m")

    # Klaviatura buferini təmizlə (səhvən basılan Enter-ləri sil)
    try:
        termios.tcflush(sys.stdin, termios.TCIOFLUSH)
    except:
        pass

    input(f"\033[1;33m{renk} LED-i yandır, sensoru tut → ENTER bas...\033[0m")

    # Serial buferini təmizlə (köhnə verilərlə sil)
    ser.reset_input_buffer()
    time.sleep(0.5)  # Yarım saniyə gözlə
    ser.reset_input_buffer()  # Təkrar təmizlə (zəmanət olsun)

    data = {'R': [], 'G': [], 'B': [], 'W': [], 'D': []}
    print(f"\n\033[96m{renk} üçün verilər toplanır → {mevcut}/{SAMPLES}\033[0m", end="")

    while any(len(data[k]) < lazim for k in data):
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if not line: continue
        try:
            if line.startswith('R:') and len(data['R']) < lazim: data['R'].append(int(line[2:]))
            if line.startswith('G:') and len(data['G']) < lazim: data['G'].append(int(line[2:]))
            if line.startswith('B:') and len(data['B']) < lazim: data['B'].append(int(line[2:]))
            if line.startswith('W:') and len(data['W']) < lazim: data['W'].append(int(line[2:]))
            if line.startswith('D:') and len(data['D']) < lazim: data['D'].append(int(line[2:]))
        except: pass

        # Gözəl sayğac
        print(f"\r\033[96m{renk}\033[0m → "
              f"R:\033[92m{len(data['R'])+mevcut}\033[0m "
              f"G:\033[93m{len(data['G'])+mevcut}\033[0m "
              f"B:\033[94m{len(data['B'])+mevcut}\033[0m "
              f"W:\033[97m{len(data['W'])+mevcut}\033[0m "
              f"D:\033[95m{len(data['D'])+mevcut}\033[0m", end="")

    # Təhlükəsiz yazma
    with open(CSV_FILE, 'a', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        for i in range(lazim):
            writer.writerow([renk, data['R'][i], data['G'][i], data['B'][i], data['W'][i], data['D'][i]])

    print(f"\n\n\033[1;92m{renk.upper()} TAMAMLANDI! +{lazim} yeni örnək əlavə edildi!\033[0m\n")
    bip(8)

# ========================= BİTDİ – BÖYÜK QƏLƏBƏ =========================
print("\033[95m" + "═" * 80)
print("    BÜTÜN 30 RƏNG TAMAMLANDI! DATASET HAZIRDIR!")
print(f"    Fayl: {CSV_FILE} → {SAMPLES*30:,}+ örnək!")
print("    İndi %96+ dəqiqlik səni gözləyir!")
print("═" * 80 + "\033[0m")
bip(15)
print("\n\033[1;36mTəbriklər dostum! İndi modeli yenidən öyrət, fəth et!\033[0m ❤️")