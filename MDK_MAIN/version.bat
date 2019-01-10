@echo off
set "git_path="D:\Program Files\Git\bin\git.exe""

%git_path% log -1 --format="%%cd" --date=format:%%Y%%m%%d%%H%%M%%S > temp
::%git_path% log --pretty=format:"%%h" > temp
set/P version1=<temp

set h=%time:~0,2% 
set h=%h: =0%
set mydate=%date:~0,4%%date:~5,2%%date:~8,2%%h%%time:~3,2%%time:~6,2%
echo %mydate% > temp

set/P version2=<temp

set "version=%version1%_%version2%"

del temp

echo #ifndef __VERSION_H            >  version.h
echo #define __VERSION_H            >> version.h
echo #define VERSION "%version%"    >> version.h
echo #endif                         >> version.h

