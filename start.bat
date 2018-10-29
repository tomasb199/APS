cd C:\Users\Acer\Desktop\1.Semester\APS\Zadanie\newfolder
set varX='1060'
set varY='800'
powershell -command "(get-content Mandelbrot.cpp) -replace 'XXX', %varX% | set-content Mandelbrot.cpp"
powershell -command "(get-content Mandelbrot.cpp) -replace 'YYY', %varY% | set-content Mandelbrot.cpp"
mingw32-make
Mandelbrot.exe
powershell -command "(get-content Mandelbrot.cpp) -replace %varX%, 'XXX' | set-content Mandelbrot.cpp"
powershell -command "(get-content Mandelbrot.cpp) -replace %varY%, 'YYY' | set-content Mandelbrot.cpp"