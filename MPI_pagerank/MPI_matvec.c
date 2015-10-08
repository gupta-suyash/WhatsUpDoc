#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include <mpi.h>

void read_graphs(int size, int vr, int *vals, int *col_inds, int *row_ptrs, int *vec, int partitions, int *store_partitions[16]) {
	long i,j;
	int x;
	printf("Enter vals: \n");
	for(i=0; i<size; i++) {
		scanf("%d",&x);
		vals[i] = x;
	}

	printf("Enter cols: \n");
	for(i=0; i<size; i++) {
		scanf("%d",&x);
		col_inds[i] = x;
	}

	//scanf("%d",&vr);
	printf("Enter rows: \n");
	for(i=0; i<vr; i++) {
		scanf("%d",&x);
		row_ptrs[i] = x;
	}

	//Generating Vector.	//printf("Enter vector: \n");
	for(i=0; i<vr-1; i++) {
		vec[i] = 1;
	}

	printf("Enter partitions: \n");
	int part[partitions];
	for(i=0; i<partitions; i++) {
		part[i] = 0;
	}

	for(i=0; i<vr-1; i++) {
		scanf("%d",&x);
		store_partitions[x][part[x]] = i;
		part[x] = part[x]+1;
	}
}


void matvec(int myrank, int enteries,  long totElems, long nvr, long *mvals, long *mcols, long *mrows, double *mvecs, 
		long *rvmap, double *result, int partitions, int *store_partitions[16], int *local_partitions) {
	long i,j,k,index,kk,l=0;
	int flag;

	long *columns_needed = malloc(totElems * sizeof(long*));
	int total_columns_needed=0;
	for(j=0; j<totElems; j++) {
		flag = 0;
		for(kk=0; kk<nvr; kk++) {
			if(mcols[j] == rvmap[kk]) {
				flag = 1;
				break;
			}
		}
		if(flag == 0) {
			for(i=0; i<total_columns_needed; i++) {
				if(mcols[j] == columns_needed[i]) {
					flag = 1;
				}
			}
			if(flag == 0) {
				columns_needed[total_columns_needed] = mcols[j];
				total_columns_needed++;
			}
		}
	}

//	printf("\n Partions %d !! \n",myrank);
//	for(i=0; i<total_columns_needed; i++) {
//		printf("%ld -- ", columns_needed[i]);
//	}
//	printf("\n");

	// Now we need to find which partitions consists of which columns.
	long *part_columns[16];
	int *part 	= malloc(partitions * sizeof(int));
	int *srdispls 	= malloc(partitions *sizeof(int));
	int *sdispls 	= malloc(partitions *sizeof(int));
	int *rdispls 	= malloc(partitions *sizeof(int));
	int *recvbuf 	= malloc(partitions *sizeof(int));	// This tells how many elements each partition needs.
	int *srcounts 	= malloc(partitions *sizeof(int));
	
	for(i=0; i<partitions; i++) {
		part[i] = 0;
		part_columns[i] = malloc(total_columns_needed*sizeof(int));
	}

	kk=0; flag = 0;								// For tracking elements to send.
	for(j=0; j<partitions; j++) {						// For all the partitions.
		if(j != myrank) {
			flag = 0;
			for(i=0; i<total_columns_needed; i++) {			// For all the columns needed.
				for(k=0; k<enteries; k++) {			// For all the enteries.
					if(store_partitions[j][k] == columns_needed[i]) {
						//printf("\n yes...");
						part_columns[j][part[j]] = columns_needed[i];
						part[j] = part[j]+1;
						flag = 1;	kk++;
						break;
					}
				}
			}
			if(flag == 0) {
				kk++;
			}
		}
		else {
			kk++;
		}
	}

	//printf("\n Myran: %d -- Total: %d -- kk: %ld",myrank,total_columns_needed,kk);

	// Lets get things printed..
//	for(j=0; j<partitions; j++) {
//		if(j != myrank) {
//			//printf("\n My rank: %d --> Partition: %ld  -- Part[j]: %ld  !! ",myrank,j,part[j]);
//			k = part[j];
//			for(i=0; i<k; i++) {
//				printf(" %ld -- ",part_columns[j][i]);
//			}
//		}
//	}

	/* So we will have communication in three steps for the first round. 
	 * First each partition will tell all the other partitions the number of nodes they 
	 * expect to be sent. 
	 * Next, they will send the column ids they want from each partition.
	 * Final Communication will be sending the column values.
	 * Except for the first iteration, all other iterations will have only final communication.
	 */
	
	//int *sendcounts = malloc(partitions *sizeof(int));
	for(i=0; i<partitions; i++) {
		srdispls[i] 	= i;
		srcounts[i] 	= 1;
	}

	// First communication.
	MPI_Alltoallv(part, srcounts, srdispls, MPI_INT, recvbuf, srcounts, srdispls, MPI_INT, MPI_COMM_WORLD);

//	printf("\n");
//	for(i=0; i<partitions; i++) {
//		printf("Elements %ld want from me(%d) -- %d \n",i,myrank,recvbuf[i]);
//	}

	total_columns_needed = kk;				// Updated value.
	kk = 0;
	long *sendbuf = malloc(total_columns_needed * sizeof(long));
	for(i=0; i<partitions; i++) {				// Now we are trying to place the columns at the locations.
		sdispls[i] = kk;
		if(i != myrank) {
			k = part[i];
			for(j=0; j<k; j++) {
				sendbuf[kk] = part_columns[i][j];
				kk++;
			}
			if(j == 0) {				// If no element required from this partition.
				sendbuf[kk] = -1;
				kk++;
			}
		}
		else {
			sendbuf[kk] 	= -1;			// Receive from self a negative.
			kk++;
		}
	}

	//printf("\n value of total: %d -- kk: %ld ",total_columns_needed,kk);

//	printf("\n Columns needed by me(%d) -- ",myrank);
//	for(i=0; i<total_columns_needed; i++) {
//		printf("%ld ",sendbuf[i]);
//	}
//
//
//	printf("\n Displacement needed by me(%d) -- ",myrank);
//	for(i=0; i<partitions; i++) {
//		printf("%d ",sdispls[i]);
//	}


	int total_columns_send = 0;
	for(i=0; i<partitions; i++) {
		if(recvbuf[i] > 0) {
			total_columns_send += recvbuf[i];
		}
		else {						// For self or empty partitions
			total_columns_send++;
			recvbuf[i] = 1;				// TODO -- Basically for sending again.
		}
	}

	kk = 0;
	for(i=0; i<partitions; i++) {
		rdispls[i] = kk;
		if(i != myrank) {
			kk += recvbuf[i];
		}	
		else {
			kk++;
		}
	}

	long *receivebuf = malloc(total_columns_send *sizeof(long)); 

	// TODO -- We will now be modifying the part array by setting 1 for all 0, i.e. 
	// allowing transfer of on element.
	for(i=0; i<partitions; i++) {
		if(part[i] == 0)
			part[i] = 1;
	}


//	int temp=0;
//	for(i=0; i<partitions; i++) {
//		temp += part[i];
//	}
//
//	//int *rcounts = malloc(partitions *sizeof(int));
//
//
//	printf("\n I am partition %d -- My sendbuf is: %d (%d) -- My recvbuf is: %d \n",myrank,total_columns_needed,temp,total_columns_send);
//
	// Second communication.
	MPI_Alltoallv(sendbuf, part, sdispls, MPI_LONG, receivebuf, recvbuf, rdispls, MPI_LONG, MPI_COMM_WORLD);
	
//	j=0;
//	for(i=0; i<total_columns_send; i++) {
//		if(i == rdispls[j]) {
//			printf("\n I %d need to send following columns to partition %ld: ",myrank,j);
//			j++;
//		}
//		printf("%ld  ",receivebuf[i]);
//	}


	/* Now everyone knows what all columns of the vector they have to send to the requesting partitions.
	 * So everyone starts by collecting all the column enteries from their respective vectors and sending the same. 
	 * The point to note here is that vec (vector) is represented as a pointer. 
	 * Again we will be having an all-to-all communication, but now this time there will be a 
	 * converging loop, which will determine the extent of algorithm.
	 */

	double *sbuffer = malloc(total_columns_send * sizeof(double));		// Used for other calculation.
	double *rbuffer = malloc(total_columns_needed * sizeof(double));	// Used for my calculation.

	for(i=0; i<total_columns_send; i++) {
		sbuffer[i] = receivebuf[i];
	}

	for(i=0; i<nvr; i++) {
		result[i] = 0;
	}


	int converge = 1; 
	double negnum = -1, sum=0, temp=0, total_sum=0, check=0.00001;
	for(i=0; i<nvr; i++) {							// Computing the local vector sum.
		sum += mvecs[i];
	}

	MPI_Allreduce(&sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);	// Sum of all the vec elements.

	for(i=0; i<nvr; i++) {
		mvecs[i] = mvecs[i]/total_sum;
	}

	while(converge == 1) {
	//for(l=0; l<20; l++) {
	
		sum = 0;

		for(i=0; i<total_columns_send; i++) {			// The list of values to send.
			if(receivebuf[i] != negnum) {
				k = receivebuf[i];
				for(j=0; j<nvr; j++) {
					if(rvmap[j] == k) {
						sbuffer[i] = mvecs[j];
						break;
					}
				}
			}
		}
		
		// Third communication -- reverse of receivebuf and sendbuf.
		MPI_Alltoallv(sbuffer, recvbuf, rdispls, MPI_DOUBLE, rbuffer, part, sdispls, MPI_DOUBLE, MPI_COMM_WORLD);

		// The matrix multiplication part.
		double valneed;
		for(i=0; i<nvr; i++) {
			for(j=mrows[i]; j<mrows[i+1]; j++) {
				k = mcols[j];					// Need to fetch this col.
				flag = 0;
				for(kk=0; kk<nvr; kk++) {
					if(rvmap[kk] == k) {
						valneed = mvecs[kk];
						flag = 1;
						break;
					}
				}
				if(flag == 0) {
					for(kk=0; kk<total_columns_needed; kk++) {
						if(sendbuf[kk] == k) {
							valneed = rbuffer[kk];
							break;
						}
					}
				}
				
				result[i] += mvals[j] * valneed;
			}
		}

		for(i=0; i<nvr; i++) {							// Local result summation.
			sum += result[i];
		}

		MPI_Allreduce(&sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);	// Sum of all the vec elements.

		for(i=0; i<nvr; i++) {						
			result[i] = result[i]/total_sum;
		}
	
		sum = 0;
		for(i=0; i<nvr; i++) {
			temp = result[i] - mvecs[i];
			temp = pow(temp,2);
			sum += temp;
		}

		MPI_Allreduce(&sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);	// Sum of all the vec elements.

		total_sum = sqrt(total_sum);
		if(total_sum < check) {
			converge = 0;
		}
		
		// Trasferring result to mvecs.
		for(i=0; i<nvr; i++) {
			mvecs[i] = result[i];
		}

		// Printing result.
	//	for(i=0; i<nvr; i++) {
	//		printf("\n Myrank: %d   Row: %ld   Value: %f",myrank,rvmap[i],result[i]);
	//	}
	//	printf("\n \n");

		//printf("\n Myrank: %d Go on &&&&&&&&&&&&&&&&&&&&&&",myrank);

	}

	printf("\n Myrank: %d  out of loop !!!",myrank);

}


void mpi_pagerank(int myrank, int size, int vr, int *vals, int *col_inds, int *row_ptrs, int *vec, int partitions, int *store_partitions[16]) {
	int enteries; //= (vr-1)/partitions;
	int *local_partitions = malloc(enteries*sizeof(int));

	// The appropriate size data structures.
	long totElems=0, nvr, temp;	
	long *mvals, *mcols, *mrows, *rvmap; 
	double *result, *mvecs;
	
	//int *local_partitions = malloc(10*sizeof(int));

	if(myrank == 0) {
		int i,j,k,kk;
		int *tg_partitions = malloc(enteries*sizeof(int));
		enteries = (vr-1)/partitions;
		//printf("vr: %d -- partitions: %d -- Enteries: %d\n",vr,partitions,enteries);

		nvr 	= (vr-1)/partitions;
		mrows 	= malloc((nvr+1) * sizeof(long*));
                rvmap 	= malloc(nvr * sizeof(long*));
                mvecs 	= malloc(nvr * sizeof(double*));
		result 	= malloc(nvr * sizeof(double*));

		// Constructing the vector.
		for(i=0; i<nvr; i++) {
			mvecs[i] = 1;
		}

		for(i=1; i<partitions; i++) {
			MPI_Send(&size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&vr, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(&partitions, 1, MPI_INT, i, 2, MPI_COMM_WORLD);

			// Sending all the partitions to each node.
			for(k=0; k<partitions; k++) {
				for(j=0; j<enteries; j++) {
					local_partitions[j] = store_partitions[k][j];
				}
				if(k == i) {
					for(j=0; j<enteries; j++) {
						tg_partitions[j] = store_partitions[k][j];
					}
				}
				MPI_Send(local_partitions, enteries, MPI_INT, i, 3*partitions+k, MPI_COMM_WORLD);
			}

			// Sending the required vals only.
			long numElem = 0; kk=0; temp=0; totElems=0;
			for(j=0; j<enteries; j++) {
				if(j != 0) {
					mrows[j] = numElem + mrows[j-1];		// Storing the row at its new position.
				}
				else {
					mrows[j] = numElem;
				}
				temp = tg_partitions[j]; 
				rvmap[j] = temp;
				numElem = row_ptrs[temp+1]-row_ptrs[temp];		// Getting number of elements in a row.
				//printf("\n row: %ld -- number_of_elem: %ld -- from: %d to %d",temp,numElem,row_ptrs[temp+1],row_ptrs[temp]);
				for(k=row_ptrs[temp]; k<row_ptrs[temp+1]; k++) {
					totElems++;
				}
			}
			//printf("\n Total elems: %ld",totElems);
			mrows[j] = numElem + mrows[j-1];				// Last entry of new csr.

			mvals = malloc(totElems * sizeof(long*));
                	mcols = malloc(totElems * sizeof(long*));
			for(j=0; j<enteries; j++) {
				temp = tg_partitions[j]; 
				for(k=row_ptrs[temp]; k<row_ptrs[temp+1]; k++) {				
					mvals[kk] = vals[k];				// Value subset created.
					mcols[kk] = col_inds[k];			// Column subset created.
					kk++;
				}
			}

			printf("\n For partition %d -- info is: ",i);
		//	printf("\n vals: ");
		//	for(k=0; k<totElems; k++) {
		//		printf("%ld ",mvals[k]); 
		//	}
		//	printf("\n cols: ");
		//	for(k=0; k<totElems; k++) {
		//		printf("%ld ",mcols[k]); 
		//	}
		//	printf("\n rows: ");
		//	for(k=0; k<nvr+1; k++) {
		//		printf("%ld ",mrows[k]); 
		//	}
		//	printf("\n rvmap: ");
		//	for(k=0; k<nvr; k++) {
		//		printf("%ld ",rvmap[k]); 
		//	}
		//	printf("\n");


		//	MPI_Send(vals, size, MPI_INT, i, 4, MPI_COMM_WORLD);
		//	MPI_Send(col_inds, size, MPI_INT, i, 5, MPI_COMM_WORLD);
		//	MPI_Send(row_ptrs, vr, MPI_INT, i, 6, MPI_COMM_WORLD);
		//	MPI_Send(vec, vr-1, MPI_INT, i, 7, MPI_COMM_WORLD);

			MPI_Send(&totElems, 1, MPI_LONG, i, 4, MPI_COMM_WORLD);
			MPI_Send(mvals, totElems, MPI_LONG, i, 5, MPI_COMM_WORLD);
			MPI_Send(mcols, totElems, MPI_LONG, i, 6, MPI_COMM_WORLD);
			MPI_Send(mrows, nvr+1, MPI_LONG, i, 7, MPI_COMM_WORLD);
			MPI_Send(mvecs, nvr, MPI_DOUBLE, i, 8, MPI_COMM_WORLD);
			MPI_Send(rvmap, nvr, MPI_LONG, i, 9, MPI_COMM_WORLD);
		}

		// Setting the similar data structures for partition zero.
		for(j=0; j<enteries; j++) {
			local_partitions[j] = store_partitions[myrank][j];
		}

		long numElem = 0; kk=0; temp=0; totElems=0;
		for(j=0; j<enteries; j++) {
			if(j != 0) {
				mrows[j] = numElem + mrows[j-1];		// Storing the row at its new position.
			}
			else {
				mrows[j] = numElem;
			}
			temp = local_partitions[j]; 
			rvmap[j] = temp;
			numElem = row_ptrs[temp+1]-row_ptrs[temp];		// Getting number of elements in a row.
			//printf("\n row: %ld -- number_of_elem: %ld -- from: %d to %d",temp,numElem,row_ptrs[temp+1],row_ptrs[temp]);
			for(k=row_ptrs[temp]; k<row_ptrs[temp+1]; k++) {
				totElems++;
			}
		}
		//printf("\n Total elems: %ld",totElems);
		mrows[j] = numElem + mrows[j-1];				// Last entry of new csr.

		mvals = malloc(totElems * sizeof(long*));
                mcols = malloc(totElems * sizeof(long*));
		for(j=0; j<enteries; j++) {
			temp = local_partitions[j]; 
			for(k=row_ptrs[temp]; k<row_ptrs[temp+1]; k++) {				
				mvals[kk] = vals[k];				// Value subset created.
				mcols[kk] = col_inds[k];			// Column subset created.
				kk++;
			}
		}
	}
	else {
		int i,j,k;
		printf("My rank: %d\n",myrank);

		MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&vr, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&partitions, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		nvr 	= (vr-1)/partitions;
		mrows 	= malloc((nvr+1) * sizeof(long*));
		rvmap 	= malloc(nvr * sizeof(long*));
		mvecs 	= malloc(nvr * sizeof(double*));
		result 	= malloc(nvr * sizeof(double*));

		// Initializing the all partition array.
		for(i=0; i<partitions; i++) {
			store_partitions[i] = malloc(2000000*sizeof(int));
		}

		enteries = 0;
		enteries = (vr-1)/partitions;

		// Receiving all the partitions.
		for(k=0; k<partitions; k++) {
			MPI_Recv(local_partitions, enteries, MPI_INT, 0, 3*partitions+k, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for(j=0; j<enteries; j++) {
				store_partitions[k][j] = local_partitions[j];
			}
		}

		// Storing the local partition.
		for(j=0; j<enteries; j++) {
			local_partitions[j] = store_partitions[myrank][j];
		}
		
	//	printf("\n Printing Local Partition \n");
	//	for(i=0; i<enteries; i++) {
	//		printf("%d ",local_partitions[i]);
	//	}

	//	MPI_Recv(vals, size, MPI_INT, 0, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//	MPI_Recv(col_inds, size, MPI_INT, 0, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//	MPI_Recv(row_ptrs, vr, MPI_INT, 0, 6, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//	MPI_Recv(vec, vr-1, MPI_INT, 0, 7, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		totElems = 0;
		MPI_Recv(&totElems, 1, MPI_LONG, 0, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		//printf("\n My rank:  %d -- Tot elems: %ld",myrank,totElems);

		mvals = malloc(totElems * sizeof(long*));
		mcols = malloc(totElems * sizeof(long*));

		//printf("\n My rank: %d -- nvr: %ld ",myrank,nvr+1);

		MPI_Recv(mvals, totElems, MPI_LONG, 0, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(mcols, totElems, MPI_LONG, 0, 6, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(mrows, nvr+1, MPI_LONG, 0, 7, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(mvecs, nvr, MPI_DOUBLE, 0, 8, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(rvmap, nvr, MPI_LONG, 0, 9, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	//	long ii;
	//	printf("\n Vals: \n");
	//	for(ii=0; ii<totElems; ii++) {
	//		printf("%ld ",mvals[ii]);
	//	}
	//
	//	printf("\n Cols: \n");
	//	for(i=0; i<totElems; i++) {
	//		printf("%ld ",mcols[i]);
	//	}
	//	printf("\n");
	//
	//	printf("\n Rows: \n");
	//	for(i=0; i<nvr+1; i++) {
	//		printf("%ld ",mrows[i]);
	//	}
	//	printf("\n");

	//	printf("\n Rows Map: \n");
	//	for(i=0; i<nvr; i++) {
	//		printf("%ld ",rvmap[i]);
	//	}
	//	printf("\n");

	//	printf("\n Done \n");
	}

	fflush(stdout); // Flushing the output before proceeding beyond the barrier.

	MPI_Barrier(MPI_COMM_WORLD);	

	// Now the timing part starts.
	if(myrank == 0) {
		// Get the start time.
		//matvec(myrank,enteries,size,vr,vals,col_inds,row_ptrs,vec,result,partitions,store_partitions,local_partitions);
		matvec(myrank,enteries,totElems,nvr,mvals,mcols,mrows,mvecs,rvmap,result,partitions,store_partitions,local_partitions);
		// Get the end time.
	}
	else {
		//matvec(myrank,enteries,size,vr,vals,col_inds,row_ptrs,vec,result,partitions,store_partitions,local_partitions);
		matvec(myrank,enteries,totElems,nvr,mvals,mcols,mrows,mvecs,rvmap,result,partitions,store_partitions,local_partitions);
	}
	//printf("\n Myrank: %d is here ^^^^^^^^^^^^^^^",myrank);
}



int main(int argc, char *argv[]) {
	int numprocs, myrank;

	int size, vr, partitions;
	//int vals[400000]; //col_inds[4000000], row_ptrs[4000000]; //, vec[4000000], result[4000000]; //store_partitions[partitions][(vr-1)/partitions];
	int *vals 	= malloc(4000000*sizeof(int));
	int *col_inds 	= malloc(4000000*sizeof(int));
	int *row_ptrs 	= malloc(4000000*sizeof(int));
	int *vec 	= malloc(4000000*sizeof(int));
	//int *result 	= malloc(4000000*sizeof(int));
	int *store_partitions[16]; 

	

	MPI_Init(&argc, &argv); 
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs); 
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank); 

	if(myrank == 0) {
		printf("Enter size: \n");
		scanf("%d",&size);
		printf("Enter row ptr size: \n");
		scanf("%d",&vr);
		printf("Enter partitions: \n");
		scanf("%d",&partitions);

		int ii;
		for(ii=0; ii<partitions; ii++) {
			store_partitions[ii] = malloc(2000000*sizeof(int));
		}

		read_graphs(size,vr,vals,col_inds,row_ptrs,vec,partitions,store_partitions);
		
				
		long i;
		printf("\n Vals: \n");
		for(i=0; i<size; i++) {
			printf("%d ",vals[i]);
		}
	
		printf("\n Cols: \n");
		for(i=0; i<size; i++) {
			printf("%d ",col_inds[i]);
		}
	
		printf("\n Rows: \n");
		for(i=0; i<vr; i++) {
			printf("%d ",row_ptrs[i]);
		}
	
		printf("\n Vector: \n");
		for(i=0; i<vr-1; i++) {
			printf("%d ",vec[i]);
		}
	
		printf("\n Partitions: \n");
		long j;
		for(i=0; i<partitions; i++) {
			printf("\n Partition %ld -- Node: \n",i);
			for(j=0; j<(vr-1)/partitions; j++) {
				printf("%d ",store_partitions[i][j]);
			}
		}
		printf("\n");
		
	}
	else {
		;	// Do nothing
	}
	fflush(stdout); // Flushing the output before proceeding beyond the barrier.
	

	// Task of reading input completed.
	MPI_Barrier(MPI_COMM_WORLD);		

	mpi_pagerank(myrank,size,vr,vals,col_inds,row_ptrs,vec,partitions,store_partitions);
	fflush(stdout);
	

	MPI_Barrier(MPI_COMM_WORLD);

	printf("\n Time to print output !!!");

	// Page-rank
	//matvec(vr,vals,col_inds,row_ptrs,vec,result);
	//matvec(myrank,size,vr,vals,col_inds,row_ptrs,vec,result,partitions,store_partitions);
//
//	// Printing Result.
//	printf("\n Result: \n");
//	for(i=0; i<vr-1; i++) {
//		printf("%d ",result[i]);
//	}
//
//	printf("\n Bye \n");

	MPI_Finalize();
}

