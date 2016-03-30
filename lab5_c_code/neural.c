//Bill Ma, Elmer Santos
//EEC 181, Winter 2016
//neural.cpp

//1. get .txt files from matlab, store into sdram

//2. read from sdram, store into matrices

//3. matrix multiplication

//4. get results probability + index

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LAYER 200
#define INPUT 784
#define FINAL 10

double bias1[LAYER]; //0-199
double bias2[LAYER]; //200-399
double weight1[LAYER][INPUT]; //400-157199
double weight2[LAYER][LAYER]; //157200-197199
double weight3[FINAL][LAYER]; //197200-199199
double testData[INPUT][10]; //199200-207040

void init(double* ptr){

    FILE *file_source;
    int i = 0;
    int j;
    double num;

    //WRITE TO SDRAM
    
    // bias1 200x1
    file_source = fopen("finalB1L1.csv", "r");
    if(file_source == NULL){
        printf("fopen error");
        exit(1);
    }

    while(fscanf(file_source, "%lf", &num) == 1){
        *(ptr + i) = num;
        i++;
    }

    fclose(file_source);
    
    // bias2 200x1

    file_source = fopen("finalB1L2.csv", "r");
    if(file_source == NULL){
        printf("fopen error");
        exit(1);
    }
    
    while(fscanf(file_source, "%lf", &num) == 1){
        *(ptr + i) = num;
        i++;
    }

    fclose(file_source);

    // weight1 784x200
    file_source = fopen("finalW1L1.csv", "r");
    if(file_source == NULL){
        printf("fopen error");
        exit(1);
    }
    

    while(fscanf(file_source, "%lf", &num) == 1){
        *(ptr + i) = num;
        i++;
    }

    fclose(file_source);

    // weight2 200x200
    file_source = fopen("finalW1L2.txt", "r");
    if(file_source == NULL){
        printf("fopen error");
        exit(1);
    }
    
    while(fscanf(file_source, "%lf", &num) == 1){
        *(ptr + i) = num;
        i++;
    }
    fclose(file_source);
    
    // weight_final 10x200
    file_source = fopen("finalSoftmaxTheta.txt", "r");
    if(file_source == NULL){
        printf("fopen error");
        exit(1);
    }
    
    while(fscanf(file_source, "%lf", &num) == 1){
        *(ptr + i) = num;
        i++;
    }
    fclose(file_source);

    // testData 784x10
    file_source = fopen("testData10.txt", "r");
    if(file_source == NULL){
        printf("fopen error");
        exit(1);
    }
    
    while(fscanf(file_source, "%lf", &num) == 1){
        *(ptr + i) = num;
        i++;
    }
    fclose(file_source);
    
    printf("%d\n", i);



    //READ FROM SDRAM, STORE INTO MATRICES

    //bias1
    for(i = 0; i < 200; i++)
        bias1[i] = *(ptr + i);
    //bias2
    for(i = 200; i < 400; i++)
        bias2[i-200] = *(ptr + i);

    //weight1
    for(i = 0; i < 200; i++){
        for(j = 0; j < 784; j++)
            weight1[i][j] = *(ptr + (400+i*784+j));
    }

    
    //weight2
    for(i = 0; i < 200; i++){
        for(j = 0; j < 200; j++)
            weight2[i][j] = *(ptr + (157200+i*200+j));
    }

    //weight3
    for(i = 0; i < 10; i++){
        for(j = 0; j < 200; j++){
            weight3[i][j] = *(ptr + (197200+i*200+j));
        }
    }
    
    //testData10
    for(i = 0; i < 1; i++){
        for(j = 0; j < 784; j++)
            testData[i][j] = *(ptr + (199200+i + j*10));
    } 

/*    for(i = 0; i < 10; i++){
        for(j=0; j < 200; j++)
            printf("%lf %d %lf\n", weight3[i][j], j, *(ptr + 197400 + i*200 +j));
        printf("------------------------- %d\n", i);
    }
*/
}

double sigmoid(double value){

return 1/(1+exp(-value));
}

int main(){
    
    double *sdram_ptr;
    int i,j;
    
    sdram_ptr = (double*) malloc(sizeof(double) * 207240);
    init(sdram_ptr); 
    
    //neural network
    
//  double input[INPUT];
    double layer1[LAYER];
    double layer2[LAYER];
    double result[FINAL];

    for(i=0; i< LAYER; i++){
        layer1[i] = 0.0;
        layer2[i] = 0.0;
    }
    for(i=0; i< FINAL; i++){
        result[i] = 0.0;
        }

    //layer1

    for(i=0; i<LAYER; i++){
        for(j=0; j<INPUT; j++) 
            layer1[i] += weight1[i][j] * testData[0][j];
        layer1[i] += bias1[i];       
        layer1[i] = sigmoid(layer1[i]);
    }

    
    //layer2

    for(i=0; i<LAYER; i++){
        for(j=0; j<LAYER; j++)
            layer2[i] += weight2[i][j] * layer1[j];
        layer2[i] += bias2[i];
        layer2[i] = sigmoid(layer2[i]);
    }

    //result
    
    for(i=0; i<FINAL; i++){
        for(j=0; j<LAYER; j++)
            result[i] += weight3[i][j] * layer2[j];
        result[i] = sigmoid(result[i]);
    }

    //print results

    for(i=0; i < 10; i++){
    printf("%lf %d\n", result[i], i+1);
    }
 
    free(sdram_ptr);
    
    return 0;
}
