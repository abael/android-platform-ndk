#if defined(__clang__)
int main() { return 0; }
#else /* !__clang__ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <unistd.h>
#include <sys/time.h>

#define NMAX 33 //41
int Fibonacci(int n)
{   int x, y;
    if (n < 2)
        return n;
    else {
        x = Fibonacci(n - 1);
        y = Fibonacci(n - 2);
        return (x + y);
}    }
int FibonacciTask(int n)
{   int x, y;
    if (n < 2)
        return n;
    else {
#pragma omp task shared(x)
        x = Fibonacci(n - 1);
#pragma omp task shared(y)
        y = Fibonacci(n - 2);
#pragma omp taskwait
        return (x + y);
 }    }

int main()
{
    int FibNumber[NMAX] = {0};
    struct timeval time_start, time_end;
    int i = 0;

#if 0
#if __ANDROID__ && __arm__
    if (access("/proc/sys/fs/binfmt_misc/arm", F_OK) == 0) {
        printf("We're running in emulated ARM mode.\n"
               "For some reason OMP-enabled binary cause crash in such ARM emulator.\n"
               "To avoid it, just skip this test here.\n");
        return 0;
    }
#endif /* __ANDROID__ && __arm__ */
#endif

    // openmp related print message
    printf("CPU_ONLN= %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
    printf("Number of CPUs=%d\n", omp_get_num_procs());
    printf("Number of max threads=%d\n", omp_get_max_threads());
    printf("Number of executing thread=%d\n", omp_get_thread_num());
    printf("Number of threads=%d\n", omp_get_num_threads());
    omp_set_num_threads( omp_get_num_procs()  );
    gettimeofday(&time_start, NULL);

#pragma omp parallel
    {
#pragma omp single private(i)
        for(i = 1; i < NMAX; i++) {
            FibNumber[i] = FibonacciTask(i);
        }
    }

    gettimeofday(&time_end, NULL);
    time_end.tv_usec = time_end.tv_usec-time_start.tv_usec;
    time_end.tv_sec = time_end.tv_sec-time_start.tv_sec;
    time_end.tv_usec += (time_end.tv_sec*1000000);
    printf("Execution time of The Fibonacci Numbers with OpenMP : %lf sec\n", time_end.tv_usec / 1000000.0);
    for(i = 0; i < NMAX; i++)
        printf("%d ", FibNumber[i]);
    printf("\n-------------------------------\n");
    return 0;
}

#endif /* !__clang__ */
