@echo off
echo clear %windir%\system32\drivers
del /f /s /q %windir%\system32\drivers\usbnwifi_new.sys
del /f /s /q %windir%\system32\drivers\usbnwifi.sys
del /f /s /q %windir%\system32\drivers\nic_test.sys

del /f /s /q %windir%\system32\DRIVERS\usbnwifi_new.sys
del /f /s /q %windir%\system32\DRIVERS\usbnwifi.sys
del /f /s /q %windir%\system32\DRIVERS\nic_test.sys

echo "intall driver, press enter"
echo. & pause

copy Y:\driver_svn\os\windows\usbnwifi\x64\Debug\usbnwifi_new\usbnwifi_new.sys %windir%\system32\drivers\
pnputil -i -a Y:\driver_svn\os\windows\usbnwifi\x64\Debug\usbnwifi_new\usbnwifi_new.inf
echo. & pause