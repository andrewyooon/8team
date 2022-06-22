XCOPY /y opencv\build\x64\vc15\bin\opencv_world455.dll x64\Release\opencv_world455.dll*
XCOPY /y opencv\build\x64\vc15\bin\opencv_world455d.dll x64\Debug\opencv_world455d.dll*
XCOPY /y opencv\build\x64\vc15\bin\opencv_videoio_ffmpeg455_64.dll x64\Release\opencv_videoio_ffmpeg455_64.dll*
XCOPY /y opencv\build\x64\vc15\bin\opencv_videoio_ffmpeg455_64.dll x64\Debug\opencv_videoio_ffmpeg455_64.dll*
XCOPY /y opencv\build\x64\vc15\bin\opencv_videoio_msmf455_64d.dll x64\Debug\opencv_videoio_msmf455_64d.dll*
XCOPY /y opencv\build\x64\vc15\bin\opencv_videoio_msmf455_64.dll x64\Release\opencv_videoio_msmf455_64.dll*
XCOPY /y db-18.1.40\libdb181.dll x64\Release\libdb181.dll*
XCOPY /y db-18.1.40\libdb181.dll x64\Debug\libdb181.dll*
XCOPY /y faker\datafile.txt x64\Release\datafile.txt*
XCOPY /y faker\datafile.txt x64\Debug\datafile.txt*
xcopy /S /E /H /Y /I "rundata" "x64\Release"
xcopy /S /E /H /Y /I "rundata" "x64\Debug"
XCOPY /y mqtt\libcrypto-1_1-x64.dll x64\Release\libcrypto-1_1-x64.dll*
XCOPY /y mqtt\libcrypto-1_1-x64.dll x64\Debug\libcrypto-1_1-x64.dll*
XCOPY /y mqtt\libssl-1_1-x64.dll x64\Release\libssl-1_1-x64.dll*
XCOPY /y mqtt\libssl-1_1-x64.dll x64\Debug\libssl-1_1-x64.dll*
XCOPY /y mqtt\mosquitto.dll x64\Release\mosquitto.dll*
XCOPY /y mqtt\mosquitto.dll x64\Debug\mosquitto.dll*
XCOPY /y mqtt\mosquittopp.dll x64\Release\mosquittopp.dll*
XCOPY /y mqtt\mosquittopp.dll x64\Debug\mosquittopp.dll*
XCOPY /y sqlite3\sqlite3.dll x64\Release\sqlite3.dll*
XCOPY /y sqlite3\sqlite3.dll x64\Debug\sqlite3.dll*