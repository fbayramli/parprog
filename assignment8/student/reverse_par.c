#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "helper.h"

void reverse(char *str, int strlen)
{
	
	int np, rank;
    int i; 
    int *chunk_sizes; 
    int *chunk_starts; 
    int chunk_pointer = 0; 
    int remaining;
    char recv_str[strlen];

    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    remaining = strlen % np;
    chunk_sizes = malloc(sizeof(int) * np);
    chunk_starts = malloc(sizeof(int) * np);

    for (i = 0; i < np; i++)
    {
        chunk_sizes[i] = strlen / np;
        if (remaining > 0)
        {
            chunk_sizes[i] += 1;
            remaining -= 1;
        }
        chunk_starts[i] = chunk_pointer;
        chunk_pointer += chunk_sizes[i];
    }

    int j;
    char *my_str;
    if (0 == rank) {
        for (i = 0; i < np; i++) {
            my_str = malloc(sizeof(char) * chunk_sizes[i] + 1);
            for (j = 0; j < chunk_sizes[i]; j++)
            {
                my_str[j] = str[chunk_starts[i] + j];
            }
            my_str[chunk_sizes[i]] = '\0';
            free(my_str);
        }
    }

    
    MPI_Scatterv(str, chunk_sizes, chunk_starts, MPI_CHAR, &recv_str, strlen, MPI_CHAR, 0, MPI_COMM_WORLD);

    reverse_str(recv_str, chunk_sizes[rank]);


    if (rank == 0)
    {
        chunk_pointer = chunk_sizes[0];
        for (i = 0; i < chunk_sizes[0]; i++)
        {
            str[strlen - chunk_pointer + i] = recv_str[i];
        }
        for (i = 1; i < np; i++)
        {
            MPI_Recv(&recv_str, chunk_sizes[i], MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            chunk_pointer += chunk_sizes[i];
            for (j = 0; j < chunk_sizes[i]; j++)
            {
                str[strlen - chunk_pointer + j] = recv_str[j];
            }
        }
    }
    else
    {
        MPI_Send(&recv_str, chunk_sizes[rank], MPI_CHAR, 0, 0, MPI_COMM_WORLD);
     }

    if (rank == 0)
    {
        free(chunk_sizes);
        free(chunk_starts);
    }
}