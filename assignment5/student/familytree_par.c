#include "familytree.h"
#include <omp.h>

void traverse(tree *node, int numThreads){
	
	if(node == NULL)
		return;
	omp_set_num_threads(numThreads);
	omp_set_nested(1);
	omp_set_dynamic(1);
	
	node->IQ = compute_IQ(node->data);
	genius[node->id] = node->IQ;
	
	#pragma omp parallel
	#pragma omp sections
	{
		
		#pragma omp section
			traverse(node->right, numThreads);
		#pragma omp section
			traverse(node->left, numThreads);
	}

}

