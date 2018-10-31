#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <GL/glut.h>  // GLUT, includes glu.h and gl.h
#include <stdbool.h>
#include <math.h>

#define width XXX //1 920
#define height YYY //1 080

SYSTEM_INFO sysinfo;

bool normal = false;
bool compare = false;
int num_of_cores = 0;

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
	double d = (double)data.iterations / 4096; //256

	int y = 0, x = 0;

	for (y = id; y < height; y += data.thread_count){

		int n;
		double c_im, c_re;
		bool isInside;
        c_im = data.img_max - y*data.imag_factor;

		for (x = 0; x < width; x++){
            c_re = data.real_min + x*data.real_factor;

			double Z_re = c_re, Z_im = c_im;
			isInside = true;

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
    if(normal){
    start = GetTickCount();
    for (int i = 0; i < data.thread_count; i++) pthread_create(&workers[i], NULL, &RenderFrame, (void*) i);
    for (int i = 0; i < data.thread_count; i++) pthread_join(workers[i], NULL);
	end = GetTickCount();
	int t = end-start;
	printf("Cas sekvencneho vykonavania operacie: %d\n", t);
    }
    if(compare){
    int count_threads = data.thread_count;
    data.thread_count = 1;
    start = GetTickCount();
    for (int i = 0; i < data.thread_count; i++) pthread_create(&workers[i], NULL, &RenderFrame, (void*) i);
    for (int i = 0; i < data.thread_count; i++) pthread_join(workers[i], NULL);
	end = GetTickCount();
	int sek = end-start;
	printf("Cas sekvencneho vykonavania operacie: %d\n", sek);
	data.thread_count = count_threads;
	start = GetTickCount();
    for (int i = 0; i < data.thread_count; i++) pthread_create(&workers[i], NULL, &RenderFrame, (void*) i);
    for (int i = 0; i < data.thread_count; i++) pthread_join(workers[i], NULL);
	end = GetTickCount();
	int paralel = end-start;
	printf("Cas paralelneho vykonavania operacie: %d\n", paralel);
	printf("Casove zlepsenie: %d", sek-paralel);
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

void keypress(unsigned char key, int x, int y){

	printf("\n%c ------------\n", key);
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
        if(normal){
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
    printf("-n :\n");
    printf("--- Menu pre spustenie s prepinacom: '-n' ---\n");
    printf("f g\t-\tfraktal++ fraktal--\n");
    printf("l k\t-\tthreads++ thread--\n");
    printf("i\t-\tzoom in\n");
    printf("o\t-\tzoom out\n");
    printf("w a s d\t-\tpohyb\n");
    printf("-c :\n");
    printf("--- Menu pre spustenie s prepinacom '-c' ---\n");
    printf("f g\t-\tfraktal++ fraktal--\n");
    printf("i\t-\tzoom in\n");
    printf("o\t-\tzoom out\n");
    printf("w a s d\t-\tpohyb\n");
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv){

    if(argc == 2){
        printf("%s\n", argv[1]);
    if(strcmp(argv[1], "-n")){
        printf("--- Menu pre spustenie s prepinacom '-n'  ---\n");
        printf("f g\t-\tfraktal++ fraktal--\n");
        printf("l k\t-\tthreads++ thread--\n");
        printf("i\t-\tzoom in\n");
        printf("o\t-\tzoom out\n");
        printf("w a s d\t-\tpohyb\n");
        normal = true;
        data.thread_count = 1;

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
    }
    else{
    printf("Neexistujuci argument!\n");
    help();
    return 0;
    }
    }
    else{
    printf("Nespravny pocet argumentov\n");
    help();
    return 0;
    }

	data.real_min = -2.9; //left border
	data.real_max = 1.4; //right border
	data.img_min = -1.5; //top border
	data.img_max = data.img_min + (data.real_max - data.real_min)*height / width;

	data.iterations = XXX; //50

    // Init GLUT
    glutInit(&argc, argv);
    glutInitWindowSize(width, height);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("OpenGL Window");

    init();
    // Run the control loop
    display();
    glutKeyboardFunc(keypress);

    //Main exec. loop
    glutMainLoop();

	return EXIT_SUCCESS;
}
