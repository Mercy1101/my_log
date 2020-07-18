echo off
: 计算当前时间作为文件名
set suffix=%date:~0,4%%date:~5,2%%date:~8,2%%time:~0,2%%time:~3,2%%time:~6,2%.txt
: 源文件路径
set src_path=..\src
: 头文件路径
set inc_path=..\include
: 输出生成文件全名
set report_name=reports/%suffix%

: 遍历include文件夹下的所有文件
for /R %src_path% %%f in (*.*) do ( 
: 作为命令行输出
cpplint %%f
: 作为文件输出
cpplint %%f >>%report_name%
)
: 遍历include文件夹下的所有文件
for /R %inc_path% %%f in (*.*) do ( 
: 作为命令行输出
cpplint %%f
: 作为文件输出
cpplint %%f >>%report_name% 2>&1
)
pause