import re

repo_path = "src/Repositories/StudentRepository.cpp"
try:
    with open(repo_path, "r", encoding="utf-8") as f:
        lines = f.readlines()
        
    in_array = False
    array_lines = []
    start_line_no = 0
    
    for idx, line in enumerate(lines):
        if "s_mockStudents = {" in line:
            in_array = True
            start_line_no = idx + 1
            continue
        if in_array:
            if "};" in line:
                in_array = False
                break
            array_lines.append((idx + 1, line.strip()))
            
    print(f"Checking {len(array_lines)} student records...")
    
    errors = 0
    for line_no, content in array_lines:
        if not content:
            continue
        # Remove trailing comma if exists
        cleaned = content.rstrip(",")
        # Check if line matches structure: {"...", "...", "...", "...", <number>, "...", "...", "...", <bool>, "..."}
        # Let's extract items inside the outer braces
        match = re.match(r"^\{(.*)\}$", cleaned)
        if not match:
            print(f"Line {line_no}: Invalid format (no outer braces found): {content}")
            errors += 1
            continue
            
        inner = match.group(1)
        # Parse fields respecting quotes
        fields = []
        current = []
        in_quotes = False
        
        i = 0
        while i < len(inner):
            char = inner[i]
            if char == '"':
                in_quotes = not in_quotes
                current.append(char)
            elif char == ',' and not in_quotes:
                fields.append("".join(current).strip())
                current = []
            else:
                current.append(char)
            i += 1
        if current:
            fields.append("".join(current).strip())
            
        if len(fields) != 10:
            print(f"Line {line_no}: Wrong number of fields (expected 10, got {len(fields)}): {content}")
            print(f"Parsed fields: {fields}")
            errors += 1
            continue
            
        # Verify types
        # fields[4] is level (should be a number)
        # fields[8] is face_registered (should be true or false)
        level_field = fields[4]
        face_field = fields[8]
        
        if not level_field.isdigit():
            print(f"Line {line_no}: Level is not a number: '{level_field}'")
            errors += 1
            
        if face_field not in ["true", "false"]:
            print(f"Line {line_no}: Face registration status is not 'true' or 'false': '{face_field}'")
            errors += 1
            
    if errors == 0:
        print("No syntax errors found in student records!")
    else:
        print(f"Found {errors} errors in student records.")
        
except Exception as e:
    print(f"Error checking file: {e}")
