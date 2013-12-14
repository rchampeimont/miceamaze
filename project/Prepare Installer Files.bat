@echo off

rd /S /Q MiceAmaze

md MiceAmaze
copy Release\*.dll MiceAmaze
copy Release\*.exe MiceAmaze
copy LICENSE.txt MiceAmaze
copy ChangeLog.txt MiceAmaze
xcopy rundir\*.* MiceAmaze /E
pause
