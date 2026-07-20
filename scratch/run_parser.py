import zipfile
import xml.etree.ElementTree as ET
import os

xlsx_path = r"c:\Users\ACER\Optix\Attendance List\CPE_CLASS_LIST.xlsx"
cpp_db_path = r"c:\Users\ACER\Optix\src\Repositories\Database.cpp"

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

        parsed_students = []
        for r in rows:
            cleaned = [c.strip() for c in r if c.strip()]
            if len(cleaned) >= 2:
                matric = ""
                name = ""
                sn = ""
                for item in cleaned:
                    if "/" in item and ("CPE" in item or "ENG" in item or "20" in item or "19" in item or "21" in item or "22" in item or "23" in item):
                        matric = item
                    elif len(item) > 3 and not item.isdigit() and not "/" in item and item.upper() not in ["FULL NAME", "STUDENT NAME", "NAME", "MATRIC NO"]:
                        if not name:
                            name = item
                    elif item.isdigit() and int(item) < 200:
                        if not sn:
                            sn = item

                if matric or name:
                    if name and name.upper() not in ["FULL NAME", "STUDENT NAME"]:
                        sid = sn if sn else str(len(parsed_students) + 1)
                        if not matric:
                            matric = f"CPE/2021/{int(sid):03d}"
                        parsed_students.append((sid, matric, name))

        print(f"Successfully extracted {len(parsed_students)} students from Excel.")

        # Build C++ vector code
        vector_lines = ["            std::vector<StudentSeed> defaultStudents = {"]
        for i, (sid, matric, name) in enumerate(parsed_students):
            safe_name = name.replace('"', '\\"')
            safe_matric = matric.replace('"', '\\"')
            # determine gender heuristic
            g = "F" if any(w in name.lower() for w in ["grace", "faith", "hannah", "joy", "blessing", "esther", "miracle", "peace", "sarah", "rebecca", "victoria", "mary", "elizabeth", "comfort", "gift", "priscilla", "mercy", "deborah", "dorcas", "florence", "titilope", "oluwaseun", "adewunmi", "toju", "fumilayo", "bukola", "omowunmi", "folakemi"]) else "M"
            comma = "," if i < len(parsed_students) - 1 else ""
            vector_lines.append(f'                {{"{sid}", "{safe_matric}", "{safe_name}", "Computer Engineering", 300, "{g}"}}{comma}')
        vector_lines.append("            };")

        vector_code = "\n".join(vector_lines)

        # Read Database.cpp
        with open(cpp_db_path, "r", encoding="utf-8") as f:
            cpp_content = f.read()

        # Find std::vector<StudentSeed> defaultStudents = { ... };
        import re
        pattern = r"std::vector<StudentSeed> defaultStudents = \{[\s\S]*?\};"
        if re.search(pattern, cpp_content):
            new_cpp_content = re.sub(pattern, vector_code, cpp_content)
            with open(cpp_db_path, "w", encoding="utf-8") as f:
                f.write(new_cpp_content)
            print("Database.cpp updated with all extracted students!")
        else:
            print("Pattern std::vector<StudentSeed> not found in Database.cpp")
