import re

raw_list = """
1 | Abdulsemiu Khaalid Dolapo | CPE/23/4754
2 | Abodunrin Sharon Oluwamayowa | CPE/23/4755
3 | Aboyeji Peace Bolatito | CPE/23/4756
4 | Adebayo Ademide Rebecca | CPE/23/4757
5 | Adebayo Joshua Adebola | CPE/23/4758
6 | Adebayo Opeyemi James | CPE/23/4759
7 | Adebayo Yusuf Olatunji | CPE/23/4760
8 | Adebunmi Peter Ademidun | CPE/23/4761
9 | Adeniran Philip Ajibola | CPE/23/4764
10 | Adeoti Isaac Oluwabukunmi | CPE/23/4765
11 | Adeyanju Olanrewaju Daniel | CPE/23/4766
12 | Adeyemo Paul Oluwaseyi | CPE/23/4768
13 | Afolabi Ayokorede Samuel | CPE/23/4769
14 | Afolabi Ayomide Emmanuel | CPE/23/4770
15 | Afolabi Boluwatife Emmanuel | CPE/23/4771
16 | Afolami Anuoluwapo Oluwafemi | CPE/23/4772
17 | Agada Trinity John | CPE/23/4773
18 | Agbo Oche Joseph-Dominion | CPE/23/4774
19 | Akande Daniel Adeogo | CPE/23/4776
20 | Akinbuwa Osemayowa Emmanuel | CPE/23/4777
21 | Akintoye Favour Truth | CPE/23/4778
22 | Akinyemi Oluwaseun Alexander | CPE/23/4779
23 | Amos Godswill Emediong | CPE/23/4780
24 | Aniekan Victory Oyindamola | CPE/23/4781
25 | Arewa Samuel Oladotun | CPE/23/4783
26 | Ayegba Donald Ojotule | CPE/23/4784
27 | Ayodeji Enoch Ayomikun | CPE/23/4785
28 | Babalola Praise Feranmi | CPE/23/4786
29 | Badru Ibrahim Isola | CPE/23/4787
30 | Balogun Qusman Olawande | CPE/23/4788
31 | Balogun Victor Adeiza | CPE/23/4789
32 | Daramola Afolabi Joshua | CPE/23/4790
33 | Egbayelo Oluwajomiloju Ayomide | CPE/23/4791
34 | Emeka Jacob Chukudi | CPE/23/4793
35 | Emeka Kenneth Miracle | CPE/23/4794
36 | Fagbohunka Toluwani Dominion | CPE/23/4795
37 | Falomo Olamide David | CPE/23/4796
38 | Idowu Oluwateniola Emmanuel | CPE/23/4798
39 | Ikuejamofo Jesutofunmi Sharon | CPE/23/4799
40 | Ilesanmi Mosinmiloluwa Winner | CPE/23/4800
41 | Ilesanmi Praise Temitope | CPE/23/4801
42 | Ilesanmi Testimony | CPE/23/4802
43 | Izuagie Evelyn Oluwaseyi | CPE/23/4803
44 | Julius Micheal Ayomide | CPE/23/4804
45 | Muftaudeen Khadijat Ayomide | CPE/23/4805
46 | Ndee Armstrong Ben | CPE/23/4806
47 | Odubayo John Akintunde | CPE/23/4808
48 | Odunade Victor Oluwaremilekun | CPE/23/4809
49 | Ogbonnaya Ebube Emmanuel | CPE/23/4810
50 | Ogunfolaji Kayode Thomas | CPE/23/4811
51 | Ogunmefun Kevin Okikiola | CPE/23/4812
52 | Oladimeji Faith John | CPE/23/4813
53 | Olanibi Adeolu Samuel | CPE/23/4814
54 | Olaniyan Pelumi Israel | CPE/23/4815
55 | Olaoye Isaac Nifemi | CPE/23/4817
56 | Olarotimi Eniola Samuel | CPE/23/4818
57 | Olatudun Opeyemi Benedict | CPE/23/4819
58 | Olayide Precious Elizabeth | CPE/23/4820
59 | Olorunnisola Mujeeb Tobiloba | CPE/23/4821
60 | Omojola Damilola Isaac | CPE/23/4822
61 | Oni Caleb Temiloluwa | CPE/23/4823
62 | Opeoluwa Mustapha Olamide | CPE/23/4824
63 | Owoseni Daniels Opeyemi | CPE/23/4825
64 | Oyedeji Samuel Oluwagbayi | CPE/23/4826
65 | Oyewusi Zion Abiodun | CPE/23/4827
66 | Sadiq Gbenga Michael | CPE/23/4828
67 | Samuel Joshua Adinoyi | CPE/23/4829
68 | Shittu Priscilla Adedunni | CPE/23/4830
69 | Shoneye Iqmat Sunmisola | CPE/23/4831
70 | Taiwo James Oriola | CPE/23/4832
71 | Thomas Boluwatife Temidayo | CPE/23/4833
72 | Tolowoye Iresubomi Sharon | CPE/23/4834
73 | Tugbobo Ayodeji Samuel | CPE/23/4835
74 | Wasiu Qudus Akorede | CPE/23/4836
75 | Adebowale Ikoghene David | CPE/24/9860
76 | Ajenifuja Abdullahi Ajeniyi | CPE/24/9870
77 | Bamiduro Abiodun Daniel | CPE/24/9892
78 | Betiku Mercy Olamide | CPE/24/9893
79 | Nasrudeen Mojeed Akin | CPE/24/9905
80 | Omisore Daniel Damilola | CPE/24/9917
81 | Orelesi Elijah Olanrewaju | CPE/24/9919
"""

cpp_db_path = r"c:\Users\ACER\Optix\src\Repositories\Database.cpp"

students = []
for line in raw_list.strip().split("\n"):
    parts = [p.strip() for p in line.split("|")]
    if len(parts) == 3:
        sid, name, matric = parts
        g = "F" if any(w in name.lower() for w in ["sharon", "peace", "rebecca", "grace", "truth", "victory", "feranmi", "sharon", "winner", "testimony", "evelyn", "khadijat", "comfort", "faith", "elizabeth", "priscilla", "iqmat", "temidayo", "sharon", "mercy"]) else "M"
        students.append((sid, matric, name, g))

print(f"Total parsed official students: {len(students)}")

vector_lines = ["            std::vector<StudentSeed> defaultStudents = {"]
for i, (sid, matric, name, g) in enumerate(students):
    safe_name = name.replace('"', '\\"')
    comma = "," if i < len(students) - 1 else ""
    vector_lines.append(f'                {{"{sid}", "{matric}", "{safe_name}", "Computer Engineering", 300, "{g}"}}{comma}')
vector_lines.append("            };")

vector_code = "\n".join(vector_lines)

with open(cpp_db_path, "r", encoding="utf-8") as f:
    cpp_content = f.read()

pattern = r"std::vector<StudentSeed> defaultStudents = \{[\s\S]*?\};"
if re.search(pattern, cpp_content):
    new_cpp_content = re.sub(pattern, vector_code, cpp_content)
    # also update condition from studentCount < 83 to studentCount < 81
    new_cpp_content = new_cpp_content.replace("if (studentCount < 83)", "if (studentCount < 81)")
    with open(cpp_db_path, "w", encoding="utf-8") as f:
        f.write(new_cpp_content)
    print("Database.cpp updated with 81 official students!")
else:
    print("Pattern defaultStudents not found!")
