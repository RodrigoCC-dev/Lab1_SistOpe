#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>	//para usar open/close
#include <unistd.h>	//para usar read/write

//Función que permite obtener los datos de una línea delimitados por ","
//Ingresa una línea con los datos separados por comas
//Retorna una lista con los datos contenidos en la línea de entrada
/*double* obtenerDatos(char* linea){
	char* aux = (char*)malloc(sizeof(char)*300);
	strcpy(aux, linea);
	const char s[2] = ",";
	char *token;
	double* lista = (double*)malloc(sizeof(double)*8);
	token = strtok(aux, s);
	lista[0] = atof(token);
	token = strtok(NULL, s);
	lista[1] = atof(token);
	token = strtok(NULL, s);
	lista[2] = atof(token);
	token = strtok(NULL, s);
	lista[3] = atof(token);
	token = strtok(NULL, s);
	lista[4] = atof(token);
	return lista;
}*/

// ------------ PARA PRUEBAS -----------------

void imprimirLineas(char** lineasDesdeArchivo, int lineas){
	int i;
	for(i=0; i<lineas; i++){
		printf("%s\n", lineasDesdeArchivo[i]);
	}
}

// ------------ FIN PRUEBAS -----------------

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

//Función que permite 
double obtenerMediaReal(char** lineasHijo, int cantidadLineasHijo){
	int i;
	double visibilidadReal = 0;
	double* Datos;
	for(i = 0; i < cantidadLineasHijo; i++){
		Datos = separarDatos(lineasHijo[i], ",");
		visibilidadReal = visibilidadReal + Datos[2];
	}
	free(Datos);
	return (visibilidadReal*(1.0/cantidadLineasHijo));
}

double obtenerMediaImaginaria(char** lineasHijo, int cantidadLineasHijo){
	int i;
	double visibilidadImaginaria = 0;
	double* Datos;
	for(i = 0; i < cantidadLineasHijo; i++){
		Datos = separarDatos(lineasHijo[i], ",");
		visibilidadImaginaria = visibilidadImaginaria + Datos[3];
	}
	free(Datos);
	return (visibilidadImaginaria*(1.0/cantidadLineasHijo));
}

double obtenerPotencia(char** lineasHijo, int cantidadLineasHijo){
	int i;
	double aux = 0.0;
	double potencia = 0;
	double* Datos;
	for(i = 0; i < cantidadLineasHijo; i++){
		Datos = separarDatos(lineasHijo[i], ",");
		aux = aux + (Datos[2]*Datos[2]) + (Datos[3]*Datos[3]);
	}
	potencia = sqrt(aux);
	free(Datos);
	return potencia;
}

double obtenerRuido(char** lineasHijo, int cantidadLineasHijo){
	int i;
	double ruido = 0.0;
	double* Datos;
	for(i = 0; i < cantidadLineasHijo; i++){
		Datos = separarDatos(lineasHijo[i], ",");
		ruido = ruido + Datos[4];
	}
	free(Datos);
	return ruido;
}

void liberarCadenas(char** Cadena, int largo){
	int i;
	for(i=0; i<largo; i++){
		free(Cadena[i]);
	}
	free(Cadena);
}

//---------------- SOLO PARA HACER PRUEBAS ------------------------

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

/*void imprimirLineas(char** lineasDesdeArchivo, int lineas){
	int i;
	for(i=0; i<lineas; i++){
		printf("%s\n", lineasDesdeArchivo[i]);
	}
}*/

//----------------- FIN PRUEBAS -----------------------------

int main(int argc, char *argv[]){
	int numLineas, pipe1, pipe2;
	int contador = 0;
	int i;
	double mediaReal = 0.0;
	double mediaImg = 0.0;
	double potencia = 0.0;
	double ruido = 0.0;
	char* bandera;
	char* detener = "0";
	char* resultado;
	char* totalProcesado;
	char* dirPipe;
	char** lineasArchivo;
	bandera = (char*)malloc(sizeof(char)*10);
	resultado = (char*)malloc(sizeof(char)*80);
	totalProcesado = (char*)malloc(sizeof(char)*100);
	dirPipe = (char*)malloc(sizeof(char)*50);
	memset(bandera, '\0', 10);
	memset(resultado, '\0', 80);
	memset(totalProcesado, '\0', 100);
	memset(dirPipe, '\0', 50);
	sprintf(dirPipe, "%s", argv[1]);
	numLineas = atoi(argv[2]);
	sprintf(bandera, "%s", argv[3]);
	printf("Parámetros de entrada: %s %d %s\n", dirPipe, numLineas, bandera);

// ----------- Con archivo de entrada -------------------

/*	FILE* archivoEntrada;
	archivoEntrada = fopen("prueba1(50).csv", "r");
	numLineas = cuantasLineasTieneUnArchivo(archivoEntrada);
	lineasArchivo = (char**)malloc(sizeof(char*)*numLineas);
	for(i=0;i<numLineas;i++){
		lineasArchivo[i] = (char*)malloc(sizeof(char)*70);
		lineasArchivo[i] = archivoPorLinea(archivoEntrada, i+1);
		//printf("%s\n", lineasArchivo[i]);
	}
*/
// ------------------------------------------------------

// ---------- Con pipes ---------------------------------

	lineasArchivo = (char**)malloc(sizeof(char*)*numLineas);
	for(i=0;i<numLineas;i++){
		lineasArchivo[i] = (char*)malloc(sizeof(char)*300);
		memset(lineasArchivo, '\0', 300);
	}
	//pipe1 = open(dirPipe, O_RDONLY);
	pipe2 = open(bandera, O_RDONLY);
	read(pipe2, detener, sizeof(char));
	printf("Valor detener hijo: %s\n", detener);
	close(pipe2);

	int verificacion = strcmp(detener, "0");
	printf("verificación: %d\n", verificacion);

	if((strcmp(detener, "0")) == 0){
		do{
			pipe1 = open(dirPipe, O_RDONLY);
			printf("Abriendo pipe %s\n", dirPipe);
			read(pipe1, lineasArchivo[contador], 1024);
			//while(lineasArchivo[contador] == NULL){
			//	read(pipe1, lineasArchivo[contador], sizeof(char)*300);
			//}
			printf("Linea recibida: %s\n", lineasArchivo[contador]);
			close(pipe1);
			contador++;
			pipe2 = open(bandera, O_RDONLY);
			read(pipe2, detener, sizeof(char));
			printf("Valor detener actual: %s\n", detener);
			close(pipe2);
		}while((strcmp(detener, "1")) != 0);
	}
	//close(pipe1);
	//close(pipe2);

// ---------- Impresión de resultados -------------------

	printf("Número de líneas: %d\n", contador);
	mediaReal = obtenerMediaReal(lineasArchivo, contador);
	mediaImg = obtenerMediaImaginaria(lineasArchivo, contador);
	potencia = obtenerPotencia(lineasArchivo, contador);
	ruido = obtenerRuido(lineasArchivo, contador);
	printf("Media Real: %f\n", mediaReal);
	printf("Media Imaginaria: %f\n", mediaImg);
	printf("Potencia: %f\n", potencia);
	printf("Ruido: %f\n", ruido);
	sprintf(resultado, "%f,%f,%f,%f", mediaReal, mediaImg, potencia, ruido);
	sprintf(totalProcesado, "%d", contador);
//--------------------
	//char prueba[20];
	//sprintf(prueba, "%f", mediaReal);
	//printf("Media Real en string: %s\n", prueba);
	printf("Resultados en string: %s\n", resultado);
	printf("Total procesado: %s\n", totalProcesado);
//--------------------
	liberarCadenas(lineasArchivo, numLineas);
	free(bandera);
	free(totalProcesado);
	free(resultado);
	free(dirPipe);
	return 0;
}
