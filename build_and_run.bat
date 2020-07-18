echo off
echo 'start cmake in windows'
rd /s /q build
mkdir build
cd  build
:: 创建工程
cmake .. -G "Visual Studio 16 2019"

:: 设置路径并编译Release
set Path=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin
MSBuild.exe my_log.sln -p:Configuration=Release -t:Rebuild
pause

:: 执行代码格式检查
cd ..
cd tools
set Path=C:\Python38\Scripts
call .\check.bat

:: 执行编译出来的执行文件
cd ..
cd bin\Release
my_log.exe

echo 'end all'
pause