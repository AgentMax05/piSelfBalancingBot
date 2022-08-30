@echo off
echo copying data file from raspberry pi
set /p piAddress=enter raspberry pi IP address: 
set /p fileLocation=enter file location on raspberry pi: 
echo IP: %piAddress%
echo file location: %fileLocation%
echo file destination: %~dp0
echo confirm? (ctrl+c to cancel)
pause
scp pi@%piAddress%:%fileLocation% "%~dp0"