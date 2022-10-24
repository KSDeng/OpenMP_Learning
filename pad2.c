#include <iostream>
#include <omp.h>
#include <time.h>
#include <sys/time.h>
using namespace std;

#define NUM_THREADS 16
#define PAD 4
#define NUM_RUN_TIME 50
static long num_steps = 10000000;
double step;

long long wall_clock_time()
{
#ifdef LINUX
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return (long long)(tp.tv_nsec + (long long)tp.tv_sec * 1000000000ll);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)(tv.tv_usec * 1000 + (long long)tv.tv_sec * 1000000000ll);
#endif
}


int main() {
	int k, n_threads;
	int n_run_time = NUM_RUN_TIME;
	int pad_bytes = PAD * 8;
	float time_total = 0;
	for (k = 0; k < NUM_RUN_TIME; ++k) {
			long long before, after;
			before = wall_clock_time();
			int i;
			double pi, sum[NUM_THREADS][PAD];

			step = 1.0 / (double)num_steps;
			omp_set_num_threads(NUM_THREADS);

			#pragma omp parallel
			{
				int i, id, nthrds;
				double x;
				id = omp_get_thread_num();
				nthrds = omp_get_num_threads();
				if (id == 0) n_threads = nthrds;
				for (i = id, sum[id][0] = 0.0; i < num_steps; i = i + nthrds) {
					x = (i + 0.5) * step;
					sum[id][0] += 4.0 / (1.0 + x * x);
				}
			}
			for (i = 0, pi = 0.0; i < n_threads; i++) pi += sum[i][0] * step;

			after = wall_clock_time();
			//printf("%d thread(s) using %f seconds, pi = %f\n", n_threads, ((float)(after - before)) / 1000000000, pi);
			time_total += ((float)(after - before) / 1000000000);
	}
	printf("%d thread(s), pad %d bytes, average running time %f seconds\n", n_threads, pad_bytes, time_total / n_run_time);
    return 0;
}
