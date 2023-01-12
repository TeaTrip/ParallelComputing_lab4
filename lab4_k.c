#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>
#include <float.h>

#ifdef _OPENMP
#include "omp.h"

#else
void omp_set_num_threads(int M) { }

double omp_get_wtime() {
    struct timeval T1;
    gettimeofday(&T1, NULL);
    return (double) T1.tv_sec + (double) T1.tv_usec / 1000000;
}

int omp_get_num_procs() { return 1; }
#endif

void count_percent(const int *percent) {
    int value;
    for(;;) {
        value = *percent;
 //       printf("Current percent: %d\n", value);
        if (value >= 100) break;
        sleep(1);
    }
}

//глупая сортировка
int stupid_sort(double arr[], int size) {
   int i;
   for(i = 0; i < size - 1; i++) {
      if(arr[i] > arr[i + 1]){
         int tmp = arr[i];
         arr[i] = arr[i + 1];
         arr[i + 1] = tmp;
         i = -1;
      }
   }
   return 0;
}

//глупая сортировка на 2 части
void stupid_sort_two_halves(double arr[], int size) {
    int i;
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            for (i = 0; i < size / 2 - 1; i++) {
                if (arr[i] > arr[i + 1]) {
                    int tmp = arr[i];
                    arr[i] = arr[i + 1];
                    arr[i + 1] = tmp;
                    i = -1;
                }
            }
        }
        #pragma omp section
        {
            for (i = size / 2; i < size - 1; i++) {
                if (arr[i] > arr[i + 1]) {
                    int tmp = arr[i];
                    arr[i] = arr[i + 1];
                    arr[i + 1] = tmp;
                    i = size / 2 - 1;
                }
            }
        }
    }
    //merge
    int j, k;
    double temp[size];
    for (i = 0, j = size / 2, k = 0; k < size; k++) {
        if (j == size) {
            temp[k] = arr[i++];
        } else if (i == size / 2) {
            temp[k] = arr[j++];
        } else if (arr[j] < arr[i]) {
            temp[k] = arr[j++];
        } else {
            temp[k] = arr[i++];
        }
    }
    for (i = 0; i < size; i++) {
        arr[i] = temp[i];
    }
}


void stupid_sort_k_halves(double arr[], int size) {
    int k = omp_get_num_procs();
    int partition_size = size / k;
    #pragma omp parallel for
    for (int i = 0; i < k; i++) {
        int start = i * partition_size;
        int end = (i + 1) * partition_size - 1;
        if (i == k - 1) {
            end = size - 1;
        }
        stupid_sort(&arr[start], end - start + 1);
    }
    //merge
    int current_partition[k];
    for (int i = 0; i < k; i++) {
        current_partition[i] = i * partition_size;
    }
    double temp[size];
    for (int i = 0; i < size; i++) {
        int min_partition = -1;
        double min_value = DBL_MAX;
        for (int p = 0; p < k; p++) {
        if (current_partition[p] < (p + 1) * partition_size && arr[current_partition[p]] < min_value) {
                min_partition = p;
                min_value = arr[current_partition[p]];
            }
        }
        temp[i] = min_value;
        current_partition[min_partition]++;
    }
    for (int i = 0; i < size; i++) {
        arr[i] = temp[i];
    }
}

//гиперболический котангенс корня числа
double cth(int x){
    return (exp(2*sqrt(x))+1)/(exp(2*sqrt(x))-1);
}

//Кубический корень после умножения на число Пи
double cbrtpi(int x){
    return cbrt(x * M_PI);
}

double min_but_not_zero(double arr[], int size) {
   int i = 0;
   double min;
   do {
      min = arr[i];
      i++;
   } while(min == 0 && i < size);

   //all elements are equal to zero
   if(min == 0){
      return 0;
   }

   for(i = 0; i < size; i++){
      if(arr[i] != 0 && arr[i] < min)
         min = arr[i];
   }
   return min;
}

double reduce(double arr[], int size) {
   double sum = 0;
   int i;
   int min = min_but_not_zero(arr, size);

   if(min == 0){
      return -1;
   }
   
   #pragma omp parallel for default(none) shared(size, arr, min) reduction(+:sum)
   for(i = 0; i < size; i++){
      if(((int)(arr[i]/min)) % 2 == 0)
         sum += sin(arr[i]);
   }
   return sum;
}

// int main_loop(int argc, char* argv[], int *percent){
//     int i, N, T;
//     double T1, T2;
//     long delta_ms;
//     unsigned int seeds;
//     int TESTS_COUNT = 100;
    
//     N = atoi(argv[1]); /* N равен первому параметру командной строки */
//     T = atoi(argv[2]);
    
//     omp_set_num_threads(T);

//     omp_sched_t kind;
//         int chunk_size;

//     omp_get_schedule(&kind, &chunk_size);
//     // switch(kind){
//     //     case omp_sched_static:
//     //     printf("static");
//     //     break;
//     //     case omp_sched_dynamic:
//     //     printf("omp_sched_dynamic, size %d", chunk_size);
//     //     break;
//     //     case omp_sched_guided:
//     //     printf("omp_sched_guided");
//     //     break;
//     //     case omp_sched_auto:
//     //     printf("omp_sched_auto");
//     //     break;
//     //     default:
//     //     printf("mono");
//     //     break;
//     // }
//     T1 = omp_get_wtime();
//     //gettimeofday(&T1, NULL); /* запомнить текущее время T1 */
//     double M1[N];
//     double M2[N/2];
//     double M2_copy[N/2];
//     for (i=0; i<TESTS_COUNT; i++) /* 100 экспериментов */
//     {
//         srand(i); /* инициализировать начальное значение ГСЧ */
//         /* Заполнить массив исходных данных размером N */

// 	    seeds = i;
	
//         for(int m1=0; m1<N-1; m1++){
//             M1[m1] = rand_r(&seeds)%320+1;
//         }

//         for(int m2=0; m2<N/2-1; m2++){
//             M2[m2] = rand_r(&seeds)%2881+320;
//             M2_copy[m2] = M2[m2];
//         }

//         /* Решить поставленную задачу, заполнить массив с результатами */
//         //Array M1
//         #pragma omp parallel for default(none) shared(N, M1)
//         for(int m1=0; m1<N-1; m1++){
//             M1[m1] = cth(M1[m1]);
//         }
    
//         //ArrayM2
//         #pragma omp parallel for default(none) shared(N, M2, M2_copy)
//         for(int m2=1; m2<N/2-1; m2++){
//             M2[m2] = M2[m2] + M2_copy[m2-1];
//         }
//         #pragma omp parallel for default(none) shared(N, M2)
//         for(int m2=0; m2<N/2-1; m2++){
//             M2[m2] = cbrtpi(M2[m2]);
//         }

//         //Array M1[m]*M2[2]
//         #pragma omp parallel for default(none) shared(N, M1, M2)
//         for(int m2=0; m2<N/2-1; m2++){
//             M2[m2] = M1[m2]*M2[m2];
//         }

         /* Отсортировать массив с результатами указанным методом */\
//         //stupid_sort(M2, N/2);
//         stupid_sort_two_halves(M2, N/2);
//         reduce(M2, N/2);
//         *percent = (100 * (i + 1)) / TESTS_COUNT;
//         //printf("\nVerification=%f.\n", reduce(M2, N/2));
//     }
//     T2 = omp_get_wtime();
//     //gettimeofday(&T2, NULL); /* запомнить текущее время T2 */
//     delta_ms = (T2 - T1) * 1000;
//     //delta_ms = 1000*(T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec)/1000;
//     printf("\nN=%d. Milliseconds passed: %ld\n", N, delta_ms); /* T2 - T1 */

    
//     return 0;
// }



// int main(int argc, char *argv[]) {
//     int *percent = malloc(sizeof(int));
//     *percent = 0;
//     #ifdef _OPENMP
//         omp_set_nested(1);
//     #pragma omp parallel sections default(none) shared(percent, argc, argv)
//         {
//     // #pragma omp section
//     //         count_percent(percent);
//     #pragma omp section
//             main_loop(argc, argv, percent);
//         }
//     #else
//         main_loop(argc, argv, percent);
//     #endif
//         free(percent);
// }



int main(int argc, char* argv[])
{
    int i, N, T;
    double T1, T2;
    long delta_ms;
    unsigned int seeds;
    
    N = atoi(argv[1]); /* N равен первому параметру командной строки */
    T = atoi(argv[2]) + 1;
    
    omp_set_num_threads(T);

    omp_sched_t kind;
        int chunk_size;

    omp_get_schedule(&kind, &chunk_size);
    // switch(kind){
    //     case omp_sched_static:
    //     printf("static");
    //     break;
    //     case omp_sched_dynamic:
    //     printf("omp_sched_dynamic, size %d", chunk_size);
    //     break;
    //     case omp_sched_guided:
    //     printf("omp_sched_guided");
    //     break;
    //     case omp_sched_auto:
    //     printf("omp_sched_auto");
    //     break;
    //     default:
    //     printf("mono");
    //     break;
    // }
    T1 = omp_get_wtime();
    //gettimeofday(&T1, NULL); /* запомнить текущее время T1 */
    double M1[N];
    double M2[N/2];
    double M2_copy[N/2];
    for (i=0; i<100; i++) /* 100 экспериментов */
    {
        srand(i); /* инициализировать начальное значение ГСЧ */
        /* Заполнить массив исходных данных размером N */

	    seeds = i;
	
        for(int m1=0; m1<N-1; m1++){
            M1[m1] = rand_r(&seeds)%320+1;
        }

        for(int m2=0; m2<N/2-1; m2++){
            M2[m2] = rand_r(&seeds)%2881+320;
            M2_copy[m2] = M2[m2];
        }

        /* Решить поставленную задачу, заполнить массив с результатами */
        //Array M1
        #pragma omp parallel for default(none) shared(N, M1)
        for(int m1=0; m1<N-1; m1++){
            M1[m1] = cth(M1[m1]);
        }
    
        //ArrayM2
        #pragma omp parallel for default(none) shared(N, M2, M2_copy)
        for(int m2=1; m2<N/2-1; m2++){
            M2[m2] = M2[m2] + M2_copy[m2-1];
        }
        #pragma omp parallel for default(none) shared(N, M2)
        for(int m2=0; m2<N/2-1; m2++){
            M2[m2] = cbrtpi(M2[m2]);
        }

        //Array M1[m]*M2[2]
        #pragma omp parallel for default(none) shared(N, M1, M2)
        for(int m2=0; m2<N/2-1; m2++){
            M2[m2] = M1[m2]*M2[m2];
        }

         /* Отсортировать массив с результатами указанным методом */\
        //stupid_sort(M2, N/2);
        //stupid_sort_two_halves(M2, N/2);
        stupid_sort_k_halves(M2, N/2);
        reduce(M2, N/2);
        //printf("\nVerification=%f.\n", reduce(M2, N/2));
    }
    T2 = omp_get_wtime();
    //gettimeofday(&T2, NULL); /* запомнить текущее время T2 */
    delta_ms = (T2 - T1) * 1000;
    //delta_ms = 1000*(T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec)/1000;
    printf("\nN=%d. Milliseconds passed: %ld\n", N, delta_ms); /* T2 - T1 */

    
    return 0;
}
