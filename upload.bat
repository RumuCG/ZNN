@echo off
set /p msg=any text? 
git add .
git commit -m "%msg%"
git push
pause
