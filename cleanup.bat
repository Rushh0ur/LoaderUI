@echo offf
@del *.sdf
rem @del /s /f /q LoaderUI\*.user
@del /s /f /q LoaderUI\GeneratedFiles\*.cpp
@del /s /f /q LoaderUI\GeneratedFiles\*.h
@del /s /f /q out\*.exe
@del /s /f /q out\*.pdb
@del /s /f /q out\*.ilk
@rmdir /s /q tmp
@rmdir /s /q ipch