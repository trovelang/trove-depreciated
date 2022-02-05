set config=%1

call ./scripts/clean.bat
call ./scripts/build.bat %config%
"./bin/%config%/trove.exe"