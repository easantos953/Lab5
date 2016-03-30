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
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>

# define HW_REGS_BASE 0xC0000000
# define HW_REGS_SPAN 0x04000000
# define HW_REGS_MASK HW_REGS_SPAN - 1
# define ALT_LWFPGALVS_OFST 0xFF200000
# define SDRAM_OFST 0xC0000000 //FPGA SDRAM

#define LAYER 200
#define INPUT 784
#define FINAL 10


double bias1[LAYER]; //0-199
double bias2[LAYER]; //200-399
double weight1[LAYER][INPUT]; //400-157199
double weight2[LAYER][LAYER]; //157200-197199
double weight3[FINAL][LAYER]; //197200-199199
double testData[10][INPUT];

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
        *((double*)ptr + i) = num;
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
        *((double*)ptr + i) = num;
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
        *((double*)ptr + i) = num;
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
        *((double*)ptr + i) = num;
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
        *((double*)ptr + i) = num;
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
        *((double*)ptr + i) = num;
        i++;
    }
    fclose(file_source);
    
 //   printf("%d\n", i);



    //READ FROM SDRAM, STORE INTO MATRICES

    //bias1
    for(i = 0; i < 200; i++)
        bias1[i] = *((double*)ptr + i);
    //bias2
    for(i = 200; i < 400; i++)
        bias2[i-200] = *((double*)ptr + i);

    //weight1
    for(i = 0; i < 200; i++){
        for(j = 0; j < 784; j++)
            weight1[i][j] = *((double*)ptr + (400+i*784+j));
    }

    
    //weight2
    for(i = 0; i < 200; i++){
        for(j = 0; j < 200; j++)
            weight2[i][j] = *((double*)ptr + (157200+i*200+j));
    }

    //weight3
    for(i = 0; i < 10; i++){
        for(j = 0; j < 200; j++){
            weight3[i][j] = *((double*)ptr + (197200+i*200+j));
        }
    }
    
    //testData10
    for(i = 0; i < 10; i++){
        for(j = 0; j < 784; j++)
            testData[i][j] = *((double*)ptr + (199200 + i + j*10));
    } 

/*    for(i = 0; i < 10; i++){
        for(j=0; j < 200; j++)
            printf("%lf %d %lf\n", weight3[i][j], j, *(ptr + 197200 + i*200 +j));
        printf("------------------------- %d\n", i);
    }
*/
}

double sigmoid(double value){
return 1/(1+exp(-value));
}

int main(){
    
 //   double *sdram_ptr;
    int i,j,z;
    
  /*virtual base stuff*/

  void *virtual_base;
  int fd;
  void *sdram_ptr;

  if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
    printf( "ERROR: could not open \"/dev/mem\"...\n" );
    return( 1 );
  }
  virtual_base = mmap(NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE);

  if( virtual_base == MAP_FAILED ) {
    printf( "ERROR: mmap() failed...\n" );
    close( fd );
    return( 1 );
  }

  sdram_ptr = virtual_base + ((unsigned long)(SDRAM_OFST + 0x00) & (unsigned long)(HW_REGS_MASK)); 

 double layer1[LAYER];
    double layer2[LAYER];
    double result[FINAL];
struct timeval start, end; 
gettimeofday(&start, NULL);

for(z =0; z<10; z++){
    init(sdram_ptr); 
    
    //neural network
    
//  double input[INPUT];


    for(i=0; i< LAYER; i++){
	layer1[i] = 0;
	layer2[i] = 0;
	}
    for(i=0; i< FINAL; i++){
	result[i] = 0;
	}

    //layer1

    for(i=0; i<LAYER; i++){
        for(j=0; j<INPUT; j++) 
            layer1[i] += weight1[i][j] * testData[z][j];
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

//	for(i=0;i<10;i++){
//		printf("%lf %d\n", result[i], i+1);
//	}
//	printf("\n");
    //print results
/*	int k,max;

	k = 0;
	max = result[k];
    	for(i=0; i < 10; i++){
		if (result[i] > max){
		max = result[i];
		k = i;
		}

	}
	printf("%d\n", k);*/
}
  gettimeofday(&end, NULL);
  
  unsigned long time_start = 1000000 * start.tv_sec + start.tv_usec;
  unsigned long time_end = 1000000 * end.tv_sec + end.tv_usec;
  unsigned long total_time;
  total_time = time_end - time_start;
  
  printf("\n\nTime: %lu microseconds\n", total_time);

 
//    free(sdram_ptr);
    
    return 0;
}
