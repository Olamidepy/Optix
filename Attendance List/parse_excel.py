import zipfile
import xml.etree.ElementTree as ET
import os

xlsx_path = "CPE_CLASS_LIST.xlsx"
if not os.path.exists(xlsx_path):
    xlsx_path = "../attendance list/CPE_CLASS_LIST.xlsx"

try:
    with zipfile.ZipFile(xlsx_path, 'r') as zip_ref:
        # 1. Read shared strings
        shared_strings = []
        try:
            with zip_ref.open('xl/sharedStrings.xml') as f:
                tree = ET.parse(f)
                root = tree.getroot()
                # Namespaces in Excel XML
                ns = {'ns': 'http://schemas.openxmlformats.org/spreadsheetml/2006/main'}
                for si in root.findall('ns:si', ns):
                    t = si.find('ns:t', ns)
                    if t is not None:
                        shared_strings.append(t.text)
                    else:
                        # Sometimes <t> is nested inside <r><t> (formatted text)
                        parts = []
                        for r in si.findall('ns:r', ns):
                            rt = r.find('ns:t', ns)
                            if rt is not None and rt.text:
                                parts.append(rt.text)
                        shared_strings.append("".join(parts) if parts else "")
        except KeyError:
            pass # No shared strings
            
        # 2. Read sheet1 data
        with zip_ref.open('xl/worksheets/sheet1.xml') as f:
            tree = ET.parse(f)
            root = tree.getroot()
            ns = {'ns': 'http://schemas.openxmlformats.org/spreadsheetml/2006/main'}
            
            rows_data = []
            for row in root.findall('.//ns:row', ns):
                row_idx = int(row.get('r', 0))
                cols = {}
                for c in row.findall('ns:c', ns):
                    ref = c.get('r', '')
                    # Extract column letter (e.g. "A" from "A1")
                    col_letter = "".join([char for char in ref if char.isalpha()])
                    t = c.get('t', '')
                    v = c.find('ns:v', ns)
                    val = ""
                    if v is not None:
                        if t == 's': # shared string reference
                            idx = int(v.text)
                            val = shared_strings[idx] if idx < len(shared_strings) else ""
                        else:
                            val = v.text
                    cols[col_letter] = val
                rows_data.append((row_idx, cols))
                
        # Sort by row index
        rows_data.sort()
        
        # Write to a text file for the user/agent
        out_path = "parsed_students.txt"
        with open(out_path, "w", encoding="utf-8") as out:
            for row_idx, cols in rows_data:
                # Format each row nicely
                row_items = []
                for col in sorted(cols.keys()):
                    row_items.append(f"{col}:{cols[col]}")
                line = " | ".join(row_items)
                out.write(f"Row {row_idx}: {line}\n")
                
        print(f"Successfully parsed excel sheet! Saved results to {out_path}")
        
except Exception as e:
    print(f"Error parsing excel file: {e}")
