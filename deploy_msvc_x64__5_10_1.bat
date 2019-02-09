set depoy_dir=Stratum_msvc_x64
set source_dir=build-Stratum-Desktop_Qt_5_10_1_MSVC2015_64bit-Release
set qt_dir=c:\Qt\5.10.1\msvc2015_64\bin\
set qt_plugins_dir=c:\Qt\5.10.1\msvc2015_64\plugins\

rmdir %depoy_dir%
md %depoy_dir%

copy %source_dir%\StData\release\StData.dll .\%depoy_dir%\StData.dll
copy %source_dir%\StVmData\release\StVmData.dll .\%depoy_dir%\StVmData.dll
copy %source_dir%\StInterfaces\release\StInterfaces.dll %depoy_dir%\StInterfaces.dll
copy %source_dir%\StSpace\release\StSpace.dll %depoy_dir%\StSpace.dll
copy %source_dir%\StCompiler\release\StCompiler.dll %depoy_dir%\StCompiler.dll
copy %source_dir%\StSerialization\release\StSerialization.dll %depoy_dir%\StSerialization.dll
copy %source_dir%\StVirtualMachine\release\StVirtualMachine.dll %depoy_dir%\StVirtualMachine.dll
copy %source_dir%\StCore\release\StCore.dll %depoy_dir%\StCore.dll
copy %source_dir%\StGui\release\Stratum.exe %depoy_dir%\Stratum.exe

copy %qt_dir%\icudt54.dll %depoy_dir%\icudt54.dll
copy %qt_dir%\icuin54.dll %depoy_dir%\icuin54.dll
copy %qt_dir%\icuuc54.dll %depoy_dir%\icuuc54.dll
copy %qt_dir%\Qt5Core.dll %depoy_dir%\Qt5Core.dll
copy %qt_dir%\Qt5Gui.dll %depoy_dir%\Qt5Gui.dll
copy %qt_dir%\Qt5Widgets.dll %depoy_dir%\Qt5Widgets.dll
copy %qt_dir%\Qt5Network.dll %depoy_dir%\Qt5Network.dll

echo F|xcopy %qt_plugins_dir%\platforms\qwindows.dll %depoy_dir%\plugins\platforms\qwindows.dll
echo F|xcopy %qt_plugins_dir%\platforms\qminimal.dll %depoy_dir%\plugins\platforms\qminimal.dll
echo F|xcopy %qt_plugins_dir%\platforms\qoffscreen.dll %depoy_dir%\plugins\platforms\qoffscreen.dll
echo F|xcopy %qt_plugins_dir%\imageformats\qico.dll %depoy_dir%\plugins\imageformats\qico.dll
echo F|xcopy %qt_plugins_dir%\imageformats\qjpeg.dll %depoy_dir%\plugins\imageformats\qjpeg.dll

xcopy Resources\library %depoy_dir%\library /I /Y /E
xcopy Resources\template %depoy_dir%\template /I /Y /E
xcopy Resources\icons %depoy_dir%\icons /I /Y /E

pause

rar a -ep1 -r -m5 -ag_DD.MM.YYYY Stratum_msvc_x32 Stratum_msvc_x32\*

pause