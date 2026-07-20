import zipfile
import xml.etree.ElementTree as ET
import os

xlsx_path = r"c:\Users\ACER\Optix\Attendance List\CPE_CLASS_LIST.xlsx"
out_cpp_path = r"c:\Users\ACER\Optix\scratch\all_students_cpp.txt"

if os.path.exists(xlsx_path):
    with zipfile.ZipFile(xlsx_path, 'r') as z:
        shared_strings = []
        if 'xl/sharedStrings.xml' in z.namelist():
            tree = ET.fromstring(z.read('xl/sharedStrings.xml'))
            for elem in tree.iter('{http://schemas.openxmlformats.org/spreadsheetml/2006/main}t'):
                shared_strings.append(elem.text if elem.text else "")

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

        print(f"Total raw rows: {len(rows)}")
        
        parsed_students = []
        for r in rows:
            # Check for name and matric
            # e.g., ["1", "CPE/2021/001", "NAME", ...]
            cleaned = [c.strip() for c in r if c.strip()]
            if len(cleaned) >= 2:
                matric = ""
                name = ""
                sn = ""
                for item in cleaned:
                    if "/" in item and ("CPE" in item or "ENG" in item or "20" in item or "19" in item or "21" in item or "22" in item or "23" in item):
                        matric = item
                    elif len(item) > 3 and not item.isdigit() and not "/" in item and item != "FULL NAME" and item != "STUDENT NAME":
                        name = item
                    elif item.isdigit() and int(item) < 200:
                        sn = item
                
                if matric and name:
                    sid = sn if sn else str(len(parsed_students) + 1)
                    parsed_students.append((sid, matric, name))

        print(f"Parsed {len(parsed_students)} students")

        with open(out_cpp_path, "w", encoding="utf-8") as f:
            f.write("std::vector<StudentSeed> defaultStudents = {\n")
            for sid, matric, name in parsed_students:
                # determine gender fallback
                g = "F" if any(w in name.lower() for w in ["grace", "faith", "hannah", "joy", "blessing", "esther", "miracle", "peace", "sarah", "rebecca", "victoria", "mary", "elizabeth", "comfort", "gift", "priscilla", "mercy", "deborah", "dorcas", "florence", "titilope", "oluwaseun", "adewunmi"]) else "M"
                safe_name = name.replace('"', '\\"')
                f.write(f'    {{"{sid}", "{matric}", "{safe_name}", "Computer Engineering", 300, "{g}"}},\n')
            f.write("};\n")

        print(f"Wrote C++ vector to {out_cpp_path}")
