import os
import shutil

# Delete dataset raw folder and models folder
datasets_dir = r"c:\Users\ACER\Optix\datasets\raw"
models_dir = r"c:\Users\ACER\Optix\models"
bin_datasets_dir = r"c:\Users\ACER\Optix\build\Desktop_MinGW_Manual_Debug\bin\datasets"
bin_models_dir = r"c:\Users\ACER\Optix\build\Desktop_MinGW_Manual_Debug\bin\models"
bin_db = r"c:\Users\ACER\Optix\build\Desktop_MinGW_Manual_Debug\bin\optix.db"
root_db = r"c:\Users\ACER\Optix\optix.db"

for d in [datasets_dir, models_dir, bin_datasets_dir, bin_models_dir]:
    if os.path.exists(d):
        try:
            shutil.rmtree(d)
            print(f"Cleaned directory {d}")
        except Exception as e:
            print(f"Error removing {d}: {e}")

for f in [bin_db, root_db]:
    if os.path.exists(f):
        try:
            os.remove(f)
            print(f"Deleted database file {f}")
        except Exception as e:
            print(f"Error deleting {f}: {e}")

print("Clean sweep complete! All face datasets and database files wiped clean.")
