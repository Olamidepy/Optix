import openpyxl
import os

excel_path = r"c:\Users\ACER\Optix\Attendance List\CPE_CLASS_LIST.xlsx"
db_cpp_path = r"c:\Users\ACER\Optix\src\Repositories\Database.cpp"

if os.path.exists(excel_path):
    wb = openpyxl.load_workbook(excel_path)
    sheet = wb.active
    
    rows = list(sheet.iter_rows(values_only=True))
    header = [str(c).strip() if c else "" for c in rows[0]]
    print(f"Header: {header}")

    students = []
    student_id_counter = 1

    for r in rows[1:]:
        if not r or not any(r):
            continue
        # Extract fields from row
        # Common layout: [S/N, Matric No, Full Name, Department/Level...]
        vals = [str(c).strip() if c is not None else "" for c in r]
        
        matric = ""
        name = ""
        gender = "M"
        dept = "Computer Engineering"
        level = 300
        
        # Parse fields dynamically based on row values
        for v in vals:
            if "/" in v and ("CPE" in v or "ENG" in v or "20" in v or "19" in v or "21" in v or "22" in v or "23" in v):
                matric = v
            elif len(v) > 3 and not v.isdigit() and not "/" in v:
                if not name:
                    name = v

        if not name and len(vals) >= 3:
            name = vals[2] if len(vals) > 2 else vals[1]
        if not matric and len(vals) >= 2:
            matric = vals[1] if len(vals) > 1 else f"CPE/2026/{student_id_counter:03d}"

        if name and name != "Full Name" and name != "Name":
            sid = str(student_id_counter)
            students.append((sid, matric, name, dept, level, gender))
            student_id_counter += 1

    print(f"Total students parsed: {len(students)}")
    for s in students[:10]:
        print(s)

    # Generate C++ SQL seeding code
    cpp_seed_lines = []
    cpp_seed_lines.append("    // 5. Seed initial student list from CPE_CLASS_LIST.xlsx")
    cpp_seed_lines.append("    const std::string selectStudentsCount = \"SELECT COUNT(*) FROM Students;\";")
    cpp_seed_lines.append("    rc = sqlite3_prepare_v2(m_db, selectStudentsCount.c_str(), -1, &stmt, nullptr);")
    cpp_seed_lines.append("    int studentCount = 0;")
    cpp_seed_lines.append("    if (rc == SQLITE_OK) {")
    cpp_seed_lines.append("        if (sqlite3_step(stmt) == SQLITE_ROW) {")
    cpp_seed_lines.append("            studentCount = sqlite3_column_int(stmt, 0);")
    cpp_seed_lines.append("        }")
    cpp_seed_lines.append("        sqlite3_finalize(stmt);")
    cpp_seed_lines.append("    }")
    cpp_seed_lines.append("    if (studentCount == 0) {")
    cpp_seed_lines.append("        const std::string insertStudentSql = \"INSERT INTO Students (student_id, matric_number, full_name, department, level, gender, face_registered) VALUES (?, ?, ?, ?, ?, ?, 0);\";")

    for sid, matric, name, dept, lvl, gdr in students:
        # Escape quotes in strings
        safe_name = name.replace('"', '\\"')
        safe_matric = matric.replace('"', '\\"')
        cpp_seed_lines.append(f"        if (sqlite3_prepare_v2(m_db, insertStudentSql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {{")
        cpp_seed_lines.append(f'            sqlite3_bind_text(stmt, 1, "{sid}", -1, SQLITE_TRANSIENT);')
        cpp_seed_lines.append(f'            sqlite3_bind_text(stmt, 2, "{safe_matric}", -1, SQLITE_TRANSIENT);')
        cpp_seed_lines.append(f'            sqlite3_bind_text(stmt, 3, "{safe_name}", -1, SQLITE_TRANSIENT);')
        cpp_seed_lines.append(f'            sqlite3_bind_text(stmt, 4, "{dept}", -1, SQLITE_TRANSIENT);')
        cpp_seed_lines.append(f'            sqlite3_bind_int(stmt, 5, {lvl});')
        cpp_seed_lines.append(f'            sqlite3_bind_text(stmt, 6, "{gdr}", -1, SQLITE_TRANSIENT);')
        cpp_seed_lines.append(f"            sqlite3_step(stmt);")
        cpp_seed_lines.append(f"            sqlite3_finalize(stmt);")
        cpp_seed_lines.append(f"        }}")

    cpp_seed_lines.append("    }")

    # Read existing Database.cpp
    with open(db_cpp_path, "r", encoding="utf-8") as f:
        content = f.read()

    # Find insertion point before 'return true;' at end of initializeSchema
    target = "    return true;\n#endif\n}"
    replacement = "\n".join(cpp_seed_lines) + "\n\n    return true;\n#endif\n}"

    if target in content:
        new_content = content.replace(target, replacement)
        with open(db_cpp_path, "w", encoding="utf-8") as f:
            f.write(new_content)
        print("Database.cpp successfully updated with student seeds!")
    else:
        print("Target insertion point not found in Database.cpp")
else:
    print("Excel file not found!")
