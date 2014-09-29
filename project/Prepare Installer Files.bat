@echo off

rd /S /Q MiceAmaze

md MiceAmaze
copy "C:\MinGW\bin\libgcc_s_dw2-1.dll" MiceAmaze
copy "C:\MinGW\bin\libstdc++-6.dll" MiceAmaze
copy LICENSE.txt MiceAmaze
xcopy rundir\*.* MiceAmaze /E
pause
