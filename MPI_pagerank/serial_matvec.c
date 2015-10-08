#include<stdio.h>
#include<math.h>
#include<stdlib.h>

void read_graphs(int size, int vr, long *vals, long *col_inds, long *row_ptrs, double *vec) {
	long i,x;
	printf("Enter vals: \n");
	for(i=0; i<size; i++) {
		scanf("%ld",&x);
		vals[i] = x;
	}

	printf("Enter cols: \n");
	for(i=0; i<size; i++) {
		scanf("%ld",&x);
		col_inds[i] = x;
	}

	printf("Enter rows: \n");
	for(i=0; i<vr; i++) {
		scanf("%ld",&x);
		row_ptrs[i] = x;
	}

	for(i=0; i<vr-1; i++) {
		vec[i] = 1;
	}
}

void pagerank(int size, int vr, long *vals, long *col_inds, long *row_ptrs, double *vec) {
	int nvr = vr-1, flag=1;
	int n;
	long i,j;
	double *result 	= malloc(nvr * sizeof(double*));
	double *rdiff 	= malloc(nvr * sizeof(double*));
	double temp, sum, check = 0.00001;


//	for(i=0; i<nvr; i++) {
//		sum += vec[i];
//	}
//
//	//sum = sqrt(sum);
//	for(i=0; i<nvr; i++) {
//		vec[i] = vec[i]/sum;
//	}

	while(flag == 1) {
		sum = 0;
		
		for(i=0; i<nvr; i++) {
			for(j=row_ptrs[i]; j<row_ptrs[i+1]; j++) {
				result[i] += vals[j] * vec[col_inds[j]];
			}
		}

	//	for(i=0; i<nvr; i++) {
	//		sum += result[i];
	//	}
	//
	//	//sum = sqrt(sum);
	//	for(i=0; i<nvr; i++) {
	//		result[i] = result[i]/sum;
	//	}
	//
	//	sum = 0;
	//	for(i=0; i<nvr; i++) {
	//		temp = result[i] - vec[i];
	//		temp = pow(temp,2);
	//		sum += temp;
	//	}

	//	sum = sqrt(sum);
	//	if(sum < check) {
	//		flag = 0;
	//	}
	
		printf("Result 1: ");
		for(i=0; i<nvr; i++) {
			printf("\n %f ",result[i]);
		}
		printf("\n");

		scanf("%d",&n);
		printf("\n \n");

		for(i=0; i<nvr; i++) {
			vec[i] = result[i];
		}
	}
	
}

int main(int argc, char *argv[]) {
	int size, vr, partitions;
	long *vals 	= malloc(4000000*sizeof(long));
	long *col_inds 	= malloc(4000000*sizeof(long));
	long *row_ptrs 	= malloc(4000000*sizeof(long));
	double *vec 	= malloc(4000000*sizeof(double));

	printf("Enter size: \n");
	scanf("%d",&size);
	printf("Enter row ptr size: \n");
	scanf("%d",&vr);

	read_graphs(size,vr,vals,col_inds,row_ptrs,vec);
		
				
	long i;
	printf("\n Vals: \n");
	for(i=0; i<size; i++) {
		printf("%ld ",vals[i]);
	}

	printf("\n Cols: \n");
	for(i=0; i<size; i++) {
		printf("%ld ",col_inds[i]);
	}

	printf("\n Rows: \n");
	for(i=0; i<vr; i++) {
		printf("%ld ",row_ptrs[i]);
	}

	printf("\n Vector: \n");
	for(i=0; i<vr-1; i++) {
		printf("%f ",vec[i]);
	}
	
	pagerank(size,vr,vals,col_inds,row_ptrs,vec);
}
