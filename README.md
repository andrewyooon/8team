# 8team



# [Project Build Guide] - Need to be more automatical later.
1. Clone git source.
`$ git clone https://github.com/andrewyooon/8team.git`

2. Copy excluded sources and libs into the top path.
`copy db-18.1.40, opencv into source top path`

3. Process patch file.
`setup.bat`

4. Copy mosquitto relative dll files.
`copy mosquitto.dll, mosquitopp.dll, libssl-1_1-x64.dll, libcrypto-1_1-x64.dll`

5. Build project.



# [How to generate a new datafile using Faker]
1. ex, if you want to generate 10000-records into datafile.txt
 - Open faker/gen.py file.
 - Modify 11-th line like below.
```
-for i in range(150):
+for i in range(10000):
```