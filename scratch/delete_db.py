import os

db_paths = [
    r"c:\Users\ACER\Optix\build\Desktop_MinGW_Manual_Debug\bin\optix.db",
    r"c:\Users\ACER\Optix\optix.db"
]

for p in db_paths:
    if os.path.exists(p):
        try:
            os.remove(p)
            print(f"Deleted {p}")
        except Exception as e:
            print(f"Could not delete {p}: {e}")
    else:
        print(f"{p} does not exist.")
