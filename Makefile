all:
	g++ b.cpp -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lglut -lGLU -lm -o mandelbrot

screen:
	sudo cp mandelbrot /usr/lib/xscreensaver/
	xscreensaver

exec:
	g++ b.cpp -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lglut -lGLU -lm -o mandelbrot
	./mandelbrot