set varX='1200'
set varY='800'
powershell -command "(get-content Mandelbrot.cpp) -replace 'XXX', %varX% | set-content Mandelbrot.cpp"
powershell -command "(get-content Mandelbrot.cpp) -replace 'YYY', %varY% | set-content Mandelbrot.cpp"
mingw32-make
Mandelbrot.exe -h
del Mandelbrot.exe
powershell -command "(get-content Mandelbrot.cpp) -replace %varX%, 'XXX' | set-content Mandelbrot.cpp"
powershell -command "(get-content Mandelbrot.cpp) -replace %varY%, 'YYY' | set-content Mandelbrot.cpp"
PAUSE