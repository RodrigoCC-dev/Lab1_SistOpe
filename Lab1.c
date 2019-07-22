#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>
#include <getopt.h>
#include <fcntl.h>

// Funcion que permite obtener las lineas de texto que tiene un archivo
int cuantasLineasTieneUnArchivo(FILE* archivo){
	char temp[200]; // Variable temporal para poder leer linea por linea
	int cantidadLineas = 0;
	if(archivo==NULL){
		printf("Error al leer el archivo\n");
	}
	while (!feof(archivo)){
		fgets(temp, 200, archivo);
		cantidadLineas++;
	}
	rewind(archivo);
	return cantidadLineas;
}

// Funcion que permite a través de un archivo y un numero, entregar la linea en el archivo como un string
char* archivoPorLinea(FILE* archivo, int linea){
	char* lectura = (char*)malloc(sizeof(char)*300);
	int i;
	for (i = 0; i < linea; i++){
		fscanf(archivo, " %[^\n]",lectura);
	}
	rewind(archivo);
	return lectura;
}

double distanciaDelCentro(double x, double y){
	return sqrt((x*x) + (y*y));
}


//Función que permite separar los datos de una cadena con delimitador
double* separarDatos(char* cadena, char* delimitador){
	char* token;
	char* linea;
	double* aux;
	int i = 0;
	linea = (char*)malloc(sizeof(char)*70);
	aux = (double*)malloc(sizeof(double)*5);
	strcpy(linea, cadena);
	token = strtok(linea, delimitador);
	if(token != NULL){
		while(token != NULL){
			aux[i] = atof(token);
			token = strtok(NULL, delimitador);
			i++;
		}
	}
	free(linea);
	return aux;
}

int queHijoVa(int ancho, int cantidad, int distancia){
    int contador = 0;
    while (distancia >= ancho){
        distancia = distancia - ancho;
        contador++;
    }
    if (contador == 0){
        return 0;
    }
    else if (contador >= cantidad){
        return cantidad - 1;
    }
    else{
        return contador;
    }
}

void liberarMemoria(char** puntero, int largo){
	int i;
	for(i=0;i<largo;i++){
		free(puntero[i]);
	}
	free(puntero);
}

int main(int argc, char *argv[]){
	char* nombreArchivoEntrada = (char*)malloc(sizeof(char)*30);
	char* nombreArchivoSalida = (char*)malloc(sizeof(char)*30);
	int cantidadDiscos = 0;
	int anchoDiscos = 0;
	int bandera = 0;
	int opcion = 0;
	while((opcion = getopt(argc, argv, "i:o:n:d:b")) != -1){
        	switch (opcion){
			case 'i':
				nombreArchivoEntrada = optarg;
				printf("El nombre del archivo de entrada es: %s\n", nombreArchivoEntrada);
				break;
			case 'o':
				nombreArchivoSalida = optarg;
				printf("El nombre del archivo de salida es: %s\n", nombreArchivoSalida);
				break;
			case 'n':
				cantidadDiscos = atoi(optarg);
				printf("La cantidad de discos es: %i\n", cantidadDiscos);
				break;
			case 'd':
				anchoDiscos = atoi(optarg);
				printf("El ancho de los discos son: %i\n", anchoDiscos);
				break;
			case 'b':
				bandera = 1;
				printf("Bandera seleccionada\n");
				break;
			default:
				printf("Opcion incorrecta\n");
		}
	}

// -------- Creación de Pipes ------------

	int i;	
	char* fifo = "/tmp/pipe";
	char* fifo2 = "/tmp/cont";
	char** pipes = (char**)malloc(sizeof(char*)*cantidadDiscos);
	char** banderas = (char**)malloc(sizeof(char*)*cantidadDiscos);
	//char *fifo2 = "/temp/otherfifo";
	for(i=0;i<cantidadDiscos;i++){
		pipes[i] = (char*)malloc(sizeof(char)*(sizeof(fifo)+sizeof(cantidadDiscos)));
		banderas[i] = (char*)malloc(sizeof(char)*(sizeof(fifo)+sizeof(cantidadDiscos)));
		sprintf(pipes[i], "%s%d", fifo, i);
		sprintf(banderas[i], "%s%d", fifo2, i);
		mkfifo(pipes[i], 0666);
		mkfifo(banderas[i], 0666);
	}
	//mkfifo(continuar, 0666);

// ----------- Archivo --------

	FILE* archivoEntrada = fopen(nombreArchivoEntrada, "r");
	int cantidadLineas = cuantasLineasTieneUnArchivo(archivoEntrada);

// ------ Declaración de variables --------

	int pipe1, pipe2;
	double ejeuLinea, ejevLinea; 		// Corresponde al eje u y eje v de una línea.
	int distancia; 				// Corresponde a la distancia del centro a partir de u y v
	int hijoCorrespondiente; 		// Corresponde al hijo que ira a partir de su radio
	double* datos;
	char* detener = "0";

// ------------ Reserva de memoria ------------

	//int* pipe1 = (int*)malloc(sizeof(int)*cantidadDiscos);
	//int* pipe2 = (int*)malloc(sizeof(int)*cantidadDiscos);
	int* pid = (int*)malloc(sizeof(int)*cantidadDiscos);
	char* numLineasString = (char*)malloc(sizeof(char)*16);
	//int* cuantasPorHijo = (int*)malloc(sizeof(int)*8000); 				// cantidad de coasa por hijo
	char* lineaArchivo = (char*)malloc(sizeof(char)*300);
	//char*** contenidoHijos = (char***)malloc(sizeof(char**)*cantidadDiscos); 	// Cada hijo tendra una lista de lineas que tiene que procesar
    
	memset(numLineasString, '\0', 16);
	sprintf(numLineasString, "%d", cantidadLineas);

	for(i=0;i<cantidadDiscos;i++){
		pid[i] = -2;
		//cuantasPorHijo[i] = 0;
		//contenidoHijos[i] = (char**)malloc(sizeof(char*)*cantidadDiscos);
		//for(j = 0; j < cantidadDiscos; j++){
		//	contenidoHijos[i][j] = (char*)malloc(sizeof(char)*300);
		//}
	}
    
// ----------- Creación de procesos hijo -----------------------

	for(i=0;i<cantidadDiscos;i++){
		if(pid[0] == -2){
			pid[0] = fork();
		}
		else if(pid[i] == -2 && pid[i-1] > 0){
			pid[i] = fork();
		}
	}
	
// ----------- Cambio de imagen de los procesos hijo -----------

	for(i=0;i<cantidadDiscos;i++){
		if(pid[i] == 0){
			char *argumentos[] = {"./vis", pipes[i], numLineasString, banderas[i], NULL};
			execv(argumentos[0], argumentos);
		}
	}

	//for(i=0;i<cantidadDiscos;i++){
	//	pipe1[i] = open(banderas[i], O_WRONLY);
	//	pipe2[i] = open(pipes[i], O_WRONLY);
	//}

// ----------- Qué línea corresponde a cada hijo ---------------
    
	for(i=1;i<cantidadLineas;i++){
		printf("analizando linea %i\n", i);
		lineaArchivo = archivoPorLinea(archivoEntrada, i);
		printf("Linea: %s\n", lineaArchivo);
		datos = separarDatos(lineaArchivo, ",");
		//rewind(archivoEntrada);
		ejeuLinea = datos[0];
		ejevLinea = datos[1];
		distancia = distanciaDelCentro(ejeuLinea, ejevLinea);
		hijoCorrespondiente = queHijoVa(anchoDiscos, cantidadDiscos, distancia);
		printf("hijo: %d\n",hijoCorrespondiente); 
		pipe1 = open(banderas[hijoCorrespondiente], O_WRONLY);
		printf("valor detener padre: %s\n", detener);
		write(pipe1, detener, sizeof(char));
		close(pipe1);
		pipe2 = open(pipes[hijoCorrespondiente], O_WRONLY);
		printf("Abriendo pipeW %s\n", pipes[hijoCorrespondiente]);
		write(pipe2, lineaArchivo, 300);
		close(pipe2);
		//contenidoHijos[hijoCorrespondiente][cuantasPorHijo[hijoCorrespondiente]] = lineaArchivo;
		//printf("Estoy guardando: \nLinea = %s\nEn el hijo %i\nPosicion = %i\n\n", lineaArchivo, hijoCorrespondiente, cuantasPorHijo[hijoCorrespondiente]);
		//cuantasPorHijo[hijoCorrespondiente] = cuantasPorHijo[hijoCorrespondiente] + 1;
		printf("fin análisis linea %i\n", i);
	}


	detener = "1";
	for(i=0;i<cantidadDiscos;i++){
		pipe1 = open(banderas[i], O_WRONLY);
		write(pipe1, detener, sizeof(char));
		close(pipe1);
		//close(pipe2);
	}


/*
	double mediaReal = 0.0;
	double mediaImg = 0.0;
	double potencia = 0.0;
	double ruido = 0.0;
	for (i = 0; i < cantidadDiscos; i++){
		printf("El hijo %i tiene %i lineas\n", i+1, cuantasPorHijo[i]);
		mediaReal = obtenerMediaReal(contenidoHijos[i], cuantasPorHijo[i]);
		mediaImg = obtenerMediaImaginaria(contenidoHijos[i], cuantasPorHijo[i]);
		potencia = obtenerPotencia(contenidoHijos[i], cuantasPorHijo[i]);
		ruido = obtenerRuido(contenidoHijos[i], cuantasPorHijo[i]);
		printf("Visibilidad Real = %f\n", mediaReal);
		printf("Visibilidad Img = %f\n", mediaImg);
		printf("Potencia = %f\n", potencia);
		printf("Ruido = %f\n\n", ruido);
	}
*/
	fclose(archivoEntrada);

// ---------- Liberación de memoria --------------	

	free(nombreArchivoEntrada);
	free(nombreArchivoSalida);
	liberarMemoria(pipes, cantidadDiscos);
	free(pid);
	free(numLineasString);
	free(lineaArchivo);

	//free(pipe1);
	//free(pipe2);

//------------------------------------------------
	return 0;
}
