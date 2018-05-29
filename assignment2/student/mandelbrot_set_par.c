#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <pthread.h>

#include "mandelbrot_set.h"

pthread_mutex_t *mutex;

struct pthread_args{

	int x_res_;
	int y_res_;
	int max_itr_;
	double vw_x0_;
	double vw_x1_;
	double vw_y0_;
	double vw_y1_;
	double x_step_;
	double y_step_;
	int plt_shift_;
	unsigned char (*img_)[1000000][3];
	int id_;
	int num_thrd_;

};


void* mandel(void* args){

	struct pthread_args *arg = (struct pthread_args*) args;

	int x_res = arg->x_res_;
	int y_res = arg->y_res_;
	int max_itr = arg->max_itr_;
	double vw_x0 = arg->vw_x0_;
	double vw_x1 = arg->vw_x1_;
	double vw_y0 = arg->vw_y0_;
	double vw_y1 = arg->vw_y1_;
	double x_step = arg->x_step_;
	double y_step = arg->y_step_;
	int plt_shift = arg->plt_shift_;
	unsigned char (*img)[x_res][3]  = arg->img_;
	int id = arg->id_;
	int part;
	int num_thrd = arg->num_thrd_;
	
	int start, stop;
	int last = 0;
	
	while (1){

	pthread_mutex_lock (&mutex);

	if (last != y_res){
		start = last;
		last += num_thrd;

		if (start + num_thrd >= y_res)
			stop = y_res;
		else
			last = stop;
			pthread_mutex_unlock(&mutex);
	}
	else
		pthread_mutex_unlock(&mutex);
		break;


	for (int i = id; i < y_res; i+=num_thrd)
	{
		for (int j = 0; j < x_res; j++)
		{
			int k;

			double y;
			double x;

			complex double Z;
			complex double C;

			y = vw_y1 - i * y_step;
			x = vw_x0 + j * x_step;

			Z = 0 + 0 * I;
			C = x + y * I;

			k = 0;

			do
			{
				Z = Z * Z + C;
				k++;
			} while (cabs(Z) < 2 && k < max_itr);

			if (k == max_itr)
			{
				memcpy(img[i][j], "\0\0\0", 3);
			}
			else
			{
				int index = (k + plt_shift) % (sizeof(colors) / sizeof(colors[0]));
				memcpy(img[i][j], colors[index], 3);
			}
		}
	}	

	}
	
	
return NULL;
}



void mandelbrot_draw(int x_resolution, int y_resolution, int max_iter,
	                double view_x0, double view_x1, double view_y0, double view_y1,
	                double x_stepsize, double y_stepsize,
	                int palette_shift, unsigned char (*image)[x_resolution][3],
						 int num_threads) {
	// TODO:
	// implement your solution in this file.
	
	

	pthread_t *threads = (pthread_t*) malloc (num_threads*sizeof(pthread_t));
	struct pthread_args* args = (struct pthread_args*) malloc (num_threads*sizeof(struct pthread_args));



	for(int i=0; i<num_threads; ++i){
		args[i].id_ = i;
		args[i].x_res_ = x_resolution;
		args[i].y_res_ = y_resolution;
		args[i].max_itr_ = max_iter;
		args[i].vw_x0_ = view_x0;
		args[i].vw_x1_ = view_x1;
		args[i].vw_y0_ = view_y0;
		args[i].vw_y1_ = view_y1;
		args[i].x_step_ = x_stepsize;
		args[i].y_step_ = y_stepsize;
		args[i].plt_shift_ = palette_shift;
		args[i].img_ = image;
		args[i].num_thrd_ = num_threads;

		pthread_create(&threads[i], NULL, mandel, args+i);
	}

	for(int i=0; i<num_threads; i++){
		pthread_join(threads[i], NULL);
	}	
	
	free(threads);
	free(args);

	



	

}
