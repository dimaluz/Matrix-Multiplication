#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#define ITERATIONS 10


unsigned int Average(int);
unsigned int stdMethod(int**, int**, int**, int);
unsigned int Vinograd(int**, int**, int**, int);
unsigned int fastVinograd(int**, int**, int**, int);
void initMatrix(int**, int);
void matrixShow(int**, int);
void Calculation(unsigned int(*func)(int**, int**, int**, int),int);

unsigned int statistic[ITERATIONS];

int main(void){
	unsigned int* MainStatistic;
	int startSize, endSize, step;
	int SIZE;
	unsigned int(*funcArray[3])(int**, int**, int**, int);
	funcArray[0]=&stdMethod;
	funcArray[1]=&Vinograd;
	funcArray[2]=&fastVinograd;
	char funcName[3][20]={"StdMethod", "Vinograd", "FastVinograd"};
	int funcIndex;
	char* fileName;
	
	printf("Enter a matrix initial size: ");
	scanf("%d",&startSize);
	printf("Enter a matrix finish size: ");
	scanf("%d", &endSize);
	printf("Enter a step: ");
	scanf("%d", &step);
	
	SIZE=(endSize-startSize)/step;
	MainStatistic=malloc(SIZE*sizeof(unsigned int));
	
	for(funcIndex=0; funcIndex<3; funcIndex++){
		FILE* fileMethod;
		printf("\n%s is running...", funcName[funcIndex]);
		for(int i=startSize, j=0; i<=endSize; i+=step, j++){
			Calculation(funcArray[funcIndex], i);
			MainStatistic[j]=Average(ITERATIONS);
		}
		printf("\n%s is finished successfully", funcName[funcIndex]);
		if(endSize%2==0 && startSize%2==0 && step%2==0){
			if(funcIndex==0)
				fileName="EvenStdMethod.txt";
			else if(funcIndex==1)
				fileName="EvenVinograd.txt";
			else
				fileName="EvenFastVinograd.txt";
		}
		else{
			if(funcIndex==0)
				fileName="OddStdMethod.txt";
			else if(funcIndex==1)
				fileName="OddVinograd.txt";
			else
				fileName="OddFastVinograd.txt";
		}
		fileMethod=fopen(fileName, "w");
		for(int i=0; i<=SIZE; i++)
			fprintf(fileMethod, "%d\n", MainStatistic[i]);
		fclose(fileMethod);
	}
	free(MainStatistic);
	return 0;
}
void Calculation(unsigned int (*func)(int**, int**, int**, int),int size){
	
	int** sourceMatrix; int** matrix1; int** matrix2;
	
	sourceMatrix=malloc(size*sizeof(int*));
	for(int i=0; i<size; i++)
		sourceMatrix[i]=malloc(size*sizeof(int));
	matrix1=malloc(size*sizeof(int*));
	for(int i=0; i<size; i++)
		matrix1[i]=malloc(size*sizeof(int));
	matrix2=malloc(size*sizeof(int*));
	for(int i=0; i<size; i++)
		matrix2[i]=malloc(size*sizeof(int));
	srand(time(NULL));
	
	initMatrix(matrix1, size);
	initMatrix(matrix2, size);
	
	for(int i=0; i<ITERATIONS; i++)
		statistic[i]=func(sourceMatrix, matrix1, matrix2, size);
	
	for(int i=0; i<size; i++)
		free(sourceMatrix[i]);
	free(sourceMatrix);
	for(int i=0; i<size; i++)
		free(matrix1[i]);
	free(matrix1);
	for(int i=0; i<size; i++)
		free(matrix2[i]);
	free(matrix2);
}
unsigned int Average(int size){
	unsigned int value=0;
	for(int i=0; i<size; i++)
		value+=statistic[i];
	value=value/size;
	return value;
}
void initMatrix(int** matrix, int size){
	for(int i=0; i<size; i++)
		for(int j=0; j<size; j++)
			matrix[i][j]=rand()%10;
}
void matrixShow(int** matrix, int size){
	for(int i=0; i<size; i++){
		for(int j=0; j<size; j++)
			printf("%d ", matrix[i][j]);
		putchar('\n');
	}
}

unsigned int stdMethod(int** source, int** A, int** B, int size){
	clock_t start=clock();
	for(int i=0; i<size; i++){
		for(int j=0; j<size; j++){
			for(int k=0; k<size; k++)
				source[i][j]+=A[i][k]*B[k][j];
		}
	}
	return clock()-start;
}
unsigned int Vinograd(int** source, int** A, int** B, int size){
	int* rowFactor=malloc(size*sizeof(int));
	int* colFactor=malloc(size*sizeof(int));
	int d=size/2;
	clock_t start=clock();
	//rowFactor calculation; 
	for(int i=0; i<size; i++){
		rowFactor[i]=A[i][0]*A[i][1];
		for(int j=1; j<d; j++)
			rowFactor[i]+=A[i][2*j]*A[i][2*j+1];
	}
	//colFactor calculation;
	for(int i=0; i<size; i++){
		colFactor[i]=B[0][i]*B[1][i];
		for(int j=1; j<d; j++)
			colFactor[i]+=B[2*j][i]*B[2*j+1][i];
	}
	//Matrix calculation;
	for(int i=0; i<size; i++){
		for(int j=0; j<size; j++){
			source[i][j]=-(rowFactor[i]+colFactor[j]);
			for(int k=0; k<d; k++){
				if((2*k+1)<size)
					source[i][j]+=(A[i][2*k]+B[2*k+1][j])*(A[i][2*k+1]+B[2*k][j]);
				else
					source[i][j]+=A[i][2*k]*B[2*k][j];
			}
		}
	}
	free(rowFactor);
	free(colFactor);
	return clock()-start;
}
unsigned int fastVinograd(int** source, int** A, int** B, int size){
	int* rowFactor=malloc(size*sizeof(int));
	int* colFactor=malloc(size*sizeof(int));
	int buffer; int d=size/2;
	clock_t start=clock();
	
	//rowFactor calculation; 
	for(int i=0; i<size; i++){
		rowFactor[i]=A[i][0]*A[i][1];
		for(int j=1; j<d; j++)
			rowFactor[i]+=A[i][2*j]*A[i][2*j+1];
	}
	//colFactor calculation;
	for(int i=0; i<size; i++){
		colFactor[i]=B[0][i]*B[1][i];
		for(int j=1; j<d; j++)
			colFactor[i]+=B[2*j][i]*B[2*j+1][i];
	}
	//Matrix calculation;
	for(int i=0; i<size; i++){
		for(int j=0; j<size; j++){
			buffer=-(rowFactor[i]+colFactor[j]);
			for(int k=1; k<size; k+=2){
				if(size%2==0)
					buffer+=(A[i][k-1]+B[k][j])*(A[i][k]+B[k-1][j]);
				else
					buffer+=A[i][size-1]*B[size-1][j];
				source[i][j]=buffer;
			}
		}
	}
	free(rowFactor);
	free(colFactor);
	return clock()-start;
}
