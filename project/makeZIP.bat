@echo off

del MiceAmaze_RC.zip

rd /S /Q MiceAmaze

md MiceAmaze
copy Release\*.dll MiceAmaze
copy Release\*.exe MiceAmaze
copy LICENSE.txt MiceAmaze
copy ChangeLog.txt MiceAmaze
xcopy rundir\*.* MiceAmaze /E
..\7-Zip\7za a MiceAmaze_RC.zip MiceAmaze

