import pandas as pd

# import pandas as pd

# df = pd.read_csv("color_dataset_last1.csv")

# print(f"Önceki veri sayısı : {len(df):,}")

# # R > 3930 olan satırları SİL (tam istediğin gibi)
# df = df[df['R'] <= 3930]   # 3930 ve altı kalır, üstü gider

# print(f"Sonraki veri sayısı: {len(df):,}")
# print(f"Silinen satır sayısı: {len(pd.read_csv('color_dataset_last1.csv')) - len(df):,}")

# # Temiz dosyayı kaydet
# df.to_csv("color_dataset_last1_TEMIZ.csv", index=False)
# print("Temiz dosya kaydedildi → color_dataset_last1_TEMIZ.csv")

# # 1-ci dataset (məsələn təmiz kağız)
df1 = pd.read_csv("color_dataset.csv")

# # 2-ci dataset (məsələn poşetli + solğun)
df2 = pd.read_csv("color_dataset_latest.csv")

# # # 3-cü, 4-cü, 5-ci... istədiyin qədər əlavə et
# # df3 = pd.read_csv("dataset_foto_kagiz.csv")
# # df4 = pd.read_csv("dataset_gece_olcum.csv")
# # # ...

# # BÜTÜN DATASETLƏRİ BİR YERƏ YIĞ
# # df_combined = pd.concat([df1, df2, df3, df4], ignore_index=True)
df_combined = pd.concat([df1, df2], ignore_index=True)


# # CSV-yə yaz
df_combined.to_csv("full.csv", index=False)

# print(f"Toplam {len(df_combined)} örnək birləşdirildi!")
# print(f"Rəng sayı: {df_combined['Rəng'].nunique()}")
