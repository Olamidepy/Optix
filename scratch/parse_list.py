import openpyxl
import os
import json

excel_path = r"c:\Users\ACER\Optix\Attendance List\CPE_CLASS_LIST.xlsx"

if os.path.exists(excel_path):
    wb = openpyxl.load_workbook(excel_path)
    sheet = wb.active
    students = []
    
    for row in sheet.iter_rows(values_only=True):
        if not row or not any(row):
            continue
        students.append([str(c) if c is not None else "" for c in row])
        
    print(f"Total rows found: {len(students)}")
    for s in students[:15]:
        print(s)
else:
    print("File not found!")
