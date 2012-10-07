@echo off
set DIR=target\zip
mkdir %DIR%

xcopy /Y target\Release\*.dll %DIR%
xcopy /Y target\Release\*.exe %DIR%
xcopy /S /Y target\Release\etc %DIR%\etc\
xcopy /S /Y target\Release\lib %DIR%\lib\

xcopy /S /Y ..\help %DIR%\help\
call tools\dos2unix.cmd ..\help\tips\tips.txt %DIR%\help\tips\tips.txt
rmdir /S /Q %DIR%\help\cvs
for %%f in (c de en fr tips) do rmdir /S /Q %DIR%\help\%%f\cvs

xcopy /S /Y ..\pixmaps\*.png %DIR%\pixmaps\

for %%f in (CS DA DE EL EO ES FA FR HE IT NL PL PT_BR RO RU ZH_CN) do (
	mkdir target\zip\lib\locale\%%f\LC_MESSAGES
	msgfmt.exe ..\po\%%f.po --output-file=target\zip\lib\locale\%%f\LC_MESSAGES\grisbi.mo
)

cd %DIR%
7z.exe a -r grisbi.zip ..\grisbi *
pause
