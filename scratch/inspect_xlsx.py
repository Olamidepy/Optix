import zipfile
import xml.etree.ElementTree as ET
import os

xlsx_path = r"c:\Users\ACER\Optix\Attendance List\CPE_CLASS_LIST.xlsx"
output_txt = r"c:\Users\ACER\Optix\scratch\students_data.txt"

if os.path.exists(xlsx_path):
    with zipfile.ZipFile(xlsx_path, 'r') as z:
        # Load shared strings
        shared_strings = []
        if 'xl/sharedStrings.xml' in z.namelist():
            tree = ET.fromstring(z.read('xl/sharedStrings.xml'))
            for elem in tree.iter('{http://schemas.openxmlformats.org/spreadsheetml/2006/main}t'):
                shared_strings.append(elem.text if elem.text else "")

        # Load sheet1
        sheet_tree = ET.fromstring(z.read('xl/worksheets/sheet1.xml'))
        rows = []
        for row in sheet_tree.iter('{http://schemas.openxmlformats.org/spreadsheetml/2006/main}row'):
            row_vals = []
            for cell in row.iter('{http://schemas.openxmlformats.org/spreadsheetml/2006/main}c'):
                t = cell.attrib.get('t')
                val_elem = cell.find('{http://schemas.openxmlformats.org/spreadsheetml/2006/main}v')
                val = val_elem.text if val_elem is not None else ""
                if t == 's' and val.isdigit():
                    idx = int(val)
                    val = shared_strings[idx] if idx < len(shared_strings) else val
                row_vals.append(val)
            if any(row_vals):
                rows.append(row_vals)

        with open(output_txt, "w", encoding="utf-8") as out:
            for r in rows:
                out.write(" | ".join(r) + "\n")
        print(f"Extracted {len(rows)} rows to {output_txt}")
