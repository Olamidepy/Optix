import os

db_path = r"c:\Users\ACER\Optix\build\Desktop_MinGW_Manual_Debug\bin\optix.db"
if os.path.exists(db_path):
    try:
        os.remove(db_path)
        print("Database file optix.db deleted successfully!")
    except Exception as e:
        print(f"Could not delete optix.db: {e}")
else:
    print("Database file optix.db does not exist.")
