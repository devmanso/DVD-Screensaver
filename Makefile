game:
	gcc src/main.c -o play -I include -L lib -l SDL2-2.0.0 -l SDL2_image -l lua