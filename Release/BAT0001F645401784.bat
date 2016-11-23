@echo off
copy /y "c:\proj\lm14\Release\app.exe" "C:\proj\lm14\app.exe"
if errorlevel 1 goto VCReportError
goto VCEnd
:VCReportError
echo Project : error PRJ0019: A tool returned an error code from "Performing Post-Build Event..."
exit 1
:VCEnd