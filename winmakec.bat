cl /EHsc /MD /Fe:birthdayshader_c.exe birthdayshader.c ^
  /I"E:\Dev\glfw-3.4.bin.WIN64\include" ^
  /I"E:\Dev\glew-2.1.0-win32\include" ^
  /link ^
  /LIBPATH:"E:\Dev\glfw-3.4.bin.WIN64\lib-vc2022" glfw3.lib ^
  /LIBPATH:"E:\Dev\glew-2.1.0-win32\lib\Release\x64" glew32.lib ^
  opengl32.lib gdi32.lib user32.lib shell32.lib
