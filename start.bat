set varX='800'
set varY='600'
powershell -command "(get-content Mandelbrot.cpp) -replace 'XXX', %varX% | set-content Mandelbrot.cpp"
powershell -command "(get-content Mandelbrot.cpp) -replace 'YYY', %varY% | set-content Mandelbrot.cpp"
mingw32-make
Mandelbrot.exe -n
del Mandelbrot.exe
PAUSE
powershell -command "(get-content Mandelbrot.cpp) -replace %varX%, 'XXX' | set-content Mandelbrot.cpp"
powershell -command "(get-content Mandelbrot.cpp) -replace %varY%, 'YYY' | set-content Mandelbrot.cpp"