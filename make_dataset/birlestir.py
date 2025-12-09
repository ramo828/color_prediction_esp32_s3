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

import pandas as pd

# import pandas as pd

# df = pd.read_csv("color_dataset_last1.csv")

# print(f"Əvvəlki verilərin sayı : {len(df):,}")

# # R > 3930 olan sətirləri SİL (tam istədiyin kimi)
# df = df[df['R'] <= 3930]   # 3930 və aşağı qalır, yuxarı gedir

# print(f"Sonrakı verilərin sayı: {len(df):,}")
# print(f"Silinən sətir sayı: {len(pd.read_csv('color_dataset_last1.csv')) - len(df):,}")

# # Təmiz faylı yadda saxla
# df.to_csv("color_dataset_last1_TEMIZ.csv", index=False)
# print("Təmiz fayl yadda saxlanıldı → color_dataset_last1_TEMIZ.csv")

# # 1-ci dataset (məsələn təmiz kağız)
df1 = pd.read_csv("../datasets/color_dataset.csv")

# # 2-ci dataset (məsələn poşetli + solğun)
df2 = pd.read_csv("../datasets/color_dataset_30_color_0.csv")

# # # 3-cü, 4-cü, 5-ci... istədiyin qədər əlavə et
# # df3 = pd.read_csv("dataset_foto_kagiz.csv")
# # df4 = pd.read_csv("dataset_gece_olcum.csv")
# # # ...

# # BÜTÜN DATASETLƏRİ BİR YERƏ YIĞ
# # df_combined = pd.concat([df1, df2, df3, df4], ignore_index=True)
df_combined = pd.concat([df1, df2], ignore_index=True)


# # CSV-yə yaz
df_combined.to_csv("color_dataset.csv", index=False)

# print(f"Toplam {len(df_combined)} örnək birləşdirildi!")
# print(f"Rəng sayı: {df_combined['Rəng'].nunique()}")
