# 8team



# [Project Build Guide] 
1. Clone git source.
`$ git clone https://github.com/andrewyooon/8team.git`

2. Download extra resources from NAS server.
`download link : http://bonkab.com:8989/download/extend.zip`

3. Move file(extend.zip) into the top path of source code and unzip in folder.

4. Process patch file.
`setup.bat`

5. Build project.



# [How to generate a new datafile using Faker]
1. ex, if you want to generate 10000-records into datafile.txt
 - Open faker/gen.py file.
 - Modify 11-th line like below.
```
-for i in range(150):
+for i in range(10000):
```
