@echo off
echo %~p0
set /p ipAddress=enter rpi IP address: 
set /p directory=enter destination path: 
echo IP: %piAddress%
echo destination: %directory%
echo confirm? (ctrl+c to cancel)
pause
scp -r "%~p0" pi@%piAddress%:%directory%