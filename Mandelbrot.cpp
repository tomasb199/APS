#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <GL/glut.h>  // GLUT, includes glu.h and gl.h
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

#define width XXX //1 920
#define height YYY //1 080

SYSTEM_INFO sysinfo;
//Variables for arguments
bool normal = false;
bool nor = false;
bool compare = false;
bool sec = false;
bool paralel = false;
bool threads_test = false;
//Variables for time measurement
int in = 0;
int num_of_cores = 0;
int total_time = 0;
int total_time_sec = 0;
int total_time_paralel = 0;
int best_time = 0;
int best_count_thread;

typedef struct{
	GLbyte r, g, b;
} pixel;

typedef struct{
	double real_min, real_max;
	double img_min, img_max;
	double real_factor, imag_factor;
	int iterations;
	int thread_count;
	pixel image[height][width];
} DATA;

DATA data;
GLuint texture;

void *RenderFrame(void* ptr){

	//bottom border
	int id = (int) ptr;
	data.real_factor = (data.real_max - data.real_min) / (width - 2);
    data.imag_factor = (data.img_max - data.img_min) / (height - 2);
	double d = (double)data.iterations / 4096;

	int y = 0, x = 0;
    //Lines of compute image(one lines, one thread)
	for (y = id; y < height; y += data.thread_count){
		int n;
		double c_im, c_re;
		bool isInside;
        c_im = data.img_max - y*data.imag_factor;

		for (x = 0; x < width; x++){
            c_re = data.real_min + x*data.real_factor;
			double Z_re = c_re, Z_im = c_im;
			isInside = true;
            //Mandelbrot set recursion
			for (n = 0; n<data.iterations; n++){
                double Z_re2 = Z_re*Z_re, Z_im2 = Z_im*Z_im;
				if (Z_re2 + Z_im2 > 4){
					isInside = false;
					break;
				}
				Z_im = 2 * Z_re*Z_im + c_im;
				Z_re = Z_re2 - Z_im2 + c_re;
			}
			if (isInside){
				pixel p;
				p.r = 0;
				p.g = 0;
				p.b = 0;
				data.image[y][x] = p;
			}
			else{
				unsigned dn = n;
				double color = (double)dn / d;
				int nc = (int)color;

				pixel p;
				p.r = nc;
				p.g = 0;
				p.b = 0;
				data.image[y][x] = p;
			}
		}
    }
    return NULL;
}

// Generate and display the image.
void display(){

    DWORD start, end;
    pthread_t workers[100000];

    if(normal || threads_test){
        //Function for Normal Mode and Thread test Mode

        start = GetTickCount();
        for (int i = 0; i < data.thread_count; i++) pthread_create(&workers[i], NULL, &RenderFrame, (void*) i);
        for (int i = 0; i < data.thread_count; i++) pthread_join(workers[i], NULL);
        end = GetTickCount();
        int t = end-start;
        if(normal){
            printf("Cas vykonavania operacie: %d\n", t);
            total_time+= t;
        }
        if(threads_test){
            printf("Cas vykonavania pri pocte threadov: %d -> %d\n", data.thread_count, t);

            if(t < best_time){
                best_time = t;
                best_count_thread = data.thread_count;
            }
        }
    }
    if(compare){
        //Function for Compare Mode

        int count_threads = data.thread_count;
        data.thread_count = 1;

        start = GetTickCount();
        for (int i = 0; i < data.thread_count; i++) pthread_create(&workers[i], NULL, &RenderFrame, (void*) i);
        for (int i = 0; i < data.thread_count; i++) pthread_join(workers[i], NULL);
        end = GetTickCount();
        int sek = end-start;

        printf("Cas sekvencneho vykonavania operacie: %d\n", sek);
        total_time_sec += sek;
        data.thread_count = count_threads;

        start = GetTickCount();
        for (int i = 0; i < data.thread_count; i++) pthread_create(&workers[i], NULL, &RenderFrame, (void*) i);
        for (int i = 0; i < data.thread_count; i++) pthread_join(workers[i], NULL);
        end = GetTickCount();
        int paralel = end-start;

        printf("Cas paralelneho vykonavania operacie: %d\n", paralel);

        total_time_paralel += paralel;
        double zrychlenie = (double) total_time_sec/total_time_paralel;
        printf("Celkovy cas sekvencneho vypoctu: %dms\nCelkovy cas paralelneho vypoctu: %dms\nZrychlenie: %lfx\n",total_time_sec, total_time_paralel, zrychlenie);

    }

	// Copy image to texture memory
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.image);
	// Clear screen buffer
	glClear(GL_COLOR_BUFFER_BIT);
	// Render a quad
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0);
	glVertex2f(1, -1);
	glTexCoord2f(1, 1);
	glVertex2f(1, 1);
	glTexCoord2f(0, 1);
	glVertex2f(-1, 1);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glEnd();

	// Display result
	glFlush();
	glutSwapBuffers();

}
//Function for keybord input
void keypress(unsigned char key, int x, int y){

	double real_diff = fabs(data.real_min - data.real_max) * 0.05;
	double img_diff = fabs(data.img_min - data.img_max) * 0.05;

	switch (key){
	case 'O':
	case 'o':
		data.real_min -= real_diff;
		data.real_max += real_diff;
		data.img_min -= img_diff;
		data.img_max += img_diff;
		break;
	case 'I':
	case 'i':
		data.real_min += real_diff;
		data.real_max -= real_diff;
		data.img_min += img_diff;
		data.img_max -= img_diff;
		break;
	case 'G':
	case 'g':
		if (data.iterations > 1) data.iterations -= 1;
		break;
	case 'F':
	case 'f':
		data.iterations += 1;
		break;
	case 'L':
	case 'l':
        if(normal || threads_test){
            data.thread_count += 1;
            printf("\nThreads:\t%d\n",data.thread_count);
        }
		break;
	case 'K':
	case 'k':
        if(normal){
            if (data.thread_count > 1) data.thread_count -= 1;
                printf("\nThreads:\t%d\n",data.thread_count);
        }
		break;
	case 'W':
	case 'w':
		data.img_min -= img_diff;
		data.img_max -= img_diff;
		break;
	case 'S':
	case 's':
		data.img_min += img_diff;
		data.img_max += img_diff;
		break;
	case 'A':
	case 'a':
		data.real_min -= real_diff;
		data.real_max -= real_diff;
		break;
	case 'D':
	case 'd':
		data.real_min += real_diff;
		data.real_max += real_diff;
		break;
	case 27:
		exit(EXIT_SUCCESS);
		break;
	case 'q':
		break;
	default:
		break;
	}
	if (x < 2147483647)
		display();
}

// Initialize OpenGL state
void init(){
	// Texture setup
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// Other
	glClearColor(0, 0, 0, 0);
	gluOrtho2D(-1, 1, -1, 1);
	glLoadIdentity();
	glColor3f(1, 1, 1);
}

void help(){
    printf("Vita ta program pre vypocet a vykreslovanie mandelbrotovej mnoziny.\n");
    printf("Program je mozne spustat s tymito prepinacmi:\n\n");
    printf("-n : Normal mode\n---Spusti sekvencne vykonavanie+interakcia pomocou klavesnice---\n");
    printf("-c : Compare mode\n---Spusti sucasne sekvencne a paralelne vykonavanie+interakcia pomocou klavesnice---\n");
    printf("-t cislo: Thread test\n");
    printf("---Program otestuje cas vykonavania vypoctu na <1,cislo> threadoch---\n");
    printf("-p cislo: Zooming test\n");
    printf("--- Program otestuje cas vykonavania <1,cislo> priblizenie a oddialeni ---\n");
}
//Set program by arguments
void set_program_mode(int argc, char** argv){

    for(int j = 1; j < argc; j+=2){
    if(strcmp(argv[j], "-n") == 0){
        printf("--- Menu pre spustenie s prepinacom '-n'  ---\n");
        printf("f g\t-\tfraktal++ fraktal--\n");
        printf("l k\t-\tthreads++ thread--\n");
        printf("i\t-\tzoom in\n");
        printf("o\t-\tzoom out\n");
        printf("w a s d\t-\tpohyb\n");
        normal = true;
        data.thread_count = 1;
        //Main exec. loop
        display();
        glutKeyboardFunc(keypress);
        glutMainLoop();
        break;

    }
    else if(strcmp(argv[1], "-c") == 0){
        printf("Spustene porovnavacie vykonavanie\n");
        GetSystemInfo(&sysinfo);
        int num_of_cores = sysinfo.dwNumberOfProcessors;
        printf("Architektura procesora: %d-jadrova\n", num_of_cores);
        data.thread_count = sysinfo.dwNumberOfProcessors;
        printf("--- Menu pre spustenie s prepinacom '-c' ---\n");
        printf("f g\t-\tfraktal++ fraktal--\n");
        printf("i\t-\tzoom in\n");
        printf("o\t-\tzoom out\n");
        printf("w a s d\t-\tpohyb\n");
        compare = true;
        //Main exec. loop
        display();
        glutKeyboardFunc(keypress);
        glutMainLoop();
        break;
    }
    else if(strcmp(argv[j], "-s") == 0 && isdigit(argv[j+1][0])){
        in = atoi(argv[j+1]);
        data.thread_count = 1;
        normal = true;
        total_time = 0;
        for(int i = 0; i < in; i++){
        keypress('i',0,0);
        }
        for(int i = in; 0 < i; i--){
        keypress('o',0,0);
        }
        printf("Total execution time: %d\n", total_time);
    }
    else if(strcmp(argv[j], "-p") == 0 && isdigit(argv[j+1][0])){
        in = atoi(argv[j+1]);
        GetSystemInfo(&sysinfo);
        data.thread_count = sysinfo.dwNumberOfProcessors;
        normal = true;
        total_time = 0;
        for(int i = 0; i < in; i++){
        keypress('i',0,0);
        }
        for(int i = in; 0 < i; i--){
        keypress('o',0,0);
        }
        printf("Total execution time: %d\n", total_time);
    }
    else if(strcmp(argv[j], "-t") == 0 && isdigit(argv[j+1][0])){
        in = atoi(argv[j+1]);
        data.thread_count = 1;
        best_time = 100000;
        threads_test = true;
        display();
        for(int i = 1; i < in; i++){
        keypress('l',0,0);
        }
        //printf("Total time of execution: %d", total_time_sec-total_time_paralel);
        printf("Best time: %d and threads: %d\n", best_time, best_count_thread);
    }
    else if(strcmp(argv[j], "-h") == 0){
        help();
    }
    else{
        printf("Neexistujuci argument!\n");
        help();
        break;
    }
    }

}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv){

	data.real_min = -2.9; //left border
	data.real_max = 1.4; //right border
	data.img_min = -1.5; //top border
	data.img_max = data.img_min + (data.real_max - data.real_min)*height / width;

	data.iterations = 850;

    // Init GLUT
    glutInit(&argc, argv);
    glutInitWindowSize(width, height);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("OpenGL Window");

    init();

    set_program_mode(argc, argv);

	return EXIT_SUCCESS;
}
