#include"mylib.h"
#include"validity_pfs.h"

int PERCENT_SELECT = 30;
int PERCENT_WRONG = 0;
int PERCENT_REMOVE = 0;

int Bool[100];
int permu[100];
int *clust = NULL;
double caval = 0;
int isdemo = 0;

void xuat() {
    double max, tmp;
    int i,j,k,count = 0;
    
    if(clust == NULL){	
		for(i = 0;i<N;i++){
			max = 0;
			k = 0;
			for(j = 0;j<C;j++){
				if(max < U[i][j] * (2 - W[i][j])){
					max = U[i][j] * (2 - W[i][j]);
					k = j;	
				}			
			}
			if(cluster[i] == permu[k]){
				count++;
			}
		}
		tmp = (double)count/N;
		if(tmp > caval) caval = tmp;
	}else{
		for(i = 0;i<N;i++){
			max = 0;
			k = 0;
			for(j = 0;j<C;j++){
				if(max < U[i][j] * (2 - W[i][j])){
					max = U[i][j] * (2 - W[i][j]);
					k = j;	
				}			
			}
			if(clust[i] == permu[k]){
				count++;
			}
		}
		tmp = (double)count/NLA;
		if(tmp > caval) caval = tmp;
	}
}
 
void trypermu(int k) {	
	int i;
    for (i = 0; i < C; i++) {
        if (!Bool[i]) {
            permu[k] = i;
            Bool[i] = 1;
            if (k == C - 1) xuat();
            else trypermu(k + 1);
            Bool[i] = 0;
        }
    }
}

double validity_kuhn(){
	int i,j,k;
	int count = 0, num = 0;
	for(i = 0;i<C;i++){
		Bool[i] = 0;
		permu[i] = i;	
	}
	caval = 0;
	trypermu(0);
	return caval;
}

double FC_PFS(double prepareTime, char *folder, char *fileout, int *iter){
    int i,j,k,l,step = 0,isNext;
    double a,b,a1,b1,aver,tg;
    double tg1 = 0, tg2 = 0, tg3, runtime = 0;
    double maxU,maxT,maxW;
    time_t t;    
	char s[50];	
	runtime += randomUTW_PFS(1) + prepareTime;
    t = clock();  
	if(isdemo){		
		sprintf(s,"demo/fc_pfs_start.txt");
		outPutV(s, step, runtime, 3);			
	}
	isNext = 1;	
		
    do{		
		maxU = 0;
		maxT = 0;
		maxW = 0;
        // calculate V[i][j]
		for(j = 0;j<C;j++){ 			
			for(i = 0;i<D;i++){
				tg1 = 0;
				tg2 = 0;  
				for(k = 0;k<N;k++){
					tg1 += (double)pow(U[k][j]  * (2 - W[k][j]),M) * X[k][i];
					tg2 += (double)pow(U[k][j] * (2 - W[k][j]),M);        
				}
				if(tg2 == 0){
					printf("tg2 = 0, division by zero in calcV!\n");
					exit(0);
				}
				V[j][i] = tg1/tg2;				
			}
		}	
	
		// calculate U[i][j]
		for(j = 0;j<C;j++){             	
			for(k = 0;k<N;k++){
				tg1 = calcX_subtract_V(k,j);
				tg3 = 0;				
				for(i = 0;i<C;i++){
					tg2 = calcX_subtract_V(k,i);
					if(tg2 == 0){ 
						printf("tg2 = 0, division by zero in calcU1! T = %10.5lf W = %10.5lf\n",T[k][i],W[k][i]);
						tg2 = EPS;
					}else
						tg3 += (double)pow(tg1/tg2,1/(M-1));					
				}				
				
				//tg = (double)pow(1 + W[i][j],M/(M-1));				
				if(tg3 < 1){ 
					printf("tg3 = %10.5lf, division by zero in calcU2!\n",tg3);
					tg3 = 1 + EPS;
				}				
				tg = 1/((2 - W[k][j]) * tg3);
				tg1 = (double)fabs(tg - U[k][j]);				
				if(maxU < tg1) maxU = tg1;
				U[k][j] = tg;
				if(U[k][j] > 1 || U[k][j] < 0){ 
					printf("U[%d][%d] = %10.5f  W = %10.5f  tg3 = %10.5f\n",k,j,U[k][j],W[k][j],tg3);
//					scanf("%d",&i);
				}
			}		
        }

		// calculate T[i][j]		
		for(j = 0;j<C;j++){             	
			for(k = 0;k<N;k++){
				tg1 = 0;				
				tg2 = 0;
				for(i = 0;i<C;i++){
					tg1 += W[k][i];					
					tg2 += exp(-W[k][i]);
				}

				if(tg2 == 0){ 
					printf("tg2 = 0, division by zero in calcW!\n");
					tg2 = EPS;
				}								
				
				tg3 = ((1 - tg1 / C) * exp(-W[k][j])) / tg2;
				tg = (double)fabs(tg3 - T[k][j]);				
				if(maxT < tg) maxT = tg;
				T[k][j] = tg3;
								 
				if(T[k][j] > (1 - U[k][j]) || T[k][j] < 0){ 
					printf("T[%d][%d] = %10.5f tg1=%10.5f   W = %10.5f\n",k,j,T[k][j],tg1,W[k][j]);
					printf("U = %10.5f tg2 = %10.5f\n",U[k][j],tg2);					
				}
			}		
        }
		
		//calculate W[i][j]
		for(j = 0;j<C;j++){             	
			for(k = 0;k<N;k++){				
				tg = 1 - (U[k][j] + T[k][j]) - (double)pow((1 - (double)pow(U[k][j] + T[k][j],ALPHA)),1/ALPHA);
				tg1 = (double)fabs(W[k][j] - tg);
				if(maxW < tg1) maxW = tg1;
				W[k][j] = tg; 
			}		
        }		
		
		// Check stop condition
		if(step > 0){			
			if(maxU + maxT + maxW < EPS){
				isNext = 0;
				printf("Stop with maxU = %7.5lf maxT = %7.5lf maxW = %7.5lf\n",maxU,maxT,maxW);
			}
		}
						
		if(step < 3){
			//sprintf(s,"%s/step%d_%s",folder,step + 1,fileout);
			//outPutV(s, step, runtime, 3);	
		}		
		if(step % 10 == 0) printf("FC_PFS:    iter %d ok\n",step);	
		step++;     
		
        runtime += (double)(clock() - t) / CLOCKS_PER_SEC;
        t = clock();                        
    }while(isNext);      
	
	printf("FC_PFS: finish with %d iterations\n",step);
    //sprintf(s,"%sFC_PFS_finished_%s.txt",folder,fileout);
    //outPutV(s, step, runtime, 3); 
    if(isdemo){
	    sprintf(s,"demo/fc_pfs_end.txt");
		outPutV(s, step, runtime, 3);
	}
	*iter = step;	
	return runtime;
}

void ramdomLabel(){
	int ita = 0,i,j,k,l,ij;
	int thres,tg,*markcla;
	if(clust == NULL) clust = (int*)malloc(N*sizeof(int)); 
	 
	NLA = (int)(PERCENT_SELECT * N / 100);	
	thres = (int)ceil(NLA/C);
	
	for(i = 0;i<N;i++){
		clust[i] = -1;	
	}		
	// Random select label
	//printf("Begin random select label\n");
	j = 0;
	while(ita < NLA){
		k = rand() % CLASS[j];
		l = 0;
		for(i = 0;i<N;i++){
			if(cluster[i] == j && l < k && clust[i] == -1){
				clust[i] = j;
				ita++;
				l++;
				break;
			}
		}		
		j++;
		if(j == C) j = 0;
	}	
	// Make wrong label
	//printf("Begin make wrong label\n");
	if(PERCENT_WRONG > 0){
		tg = (int)(PERCENT_WRONG * NLA / 100);
		markcla = (int*)malloc(N*sizeof(int)); 
		for(i = 0;i<N;i++){
			if(clust[i] != -1){
				markcla[i] = 1;
			}else{
				markcla[i] = 0;
			}
		}		
		for(ij = 0;ij<tg;ij++){
			k = rand() % NLA;
			j = 0;
			i = 0;
			while(1){
				if(markcla[j] == 1){
					if(i == k){
						markcla[j] = 0;
						if(C == 2){
							clust[j] = (clust[j] + 1) % 2;
						}else{					
							clust[j] = (clust[j] + (rand() % (C - 1) + 1)) % C;
						}						
						break;
					}
					i++;
				}
				j++;				
				if(j == N) j = 0;
			}			
		}		
		free(markcla);
	}
}

double mts3pfcm(double prepareTime, char *folder, char *fileout, int *iter){
    int i,j,k,l,step = 0,Nrm,numrm;
    double isNext = 1, lambda = 1;
    double tg,tg1 = 0,tgg,tg2 = 0, tg3,ts,ms, runtime = 0;
    
    double maxU,maxT,maxW;
	char s[100];
    clock_t t;
    runtime = FC_PFS(prepareTime, folder, fileout, iter);    
	t = clock();        
	
	double **W1;
	double **UT1;
	
	ramdomLabel();
	if(isdemo){
		FILE *fil = fopen("demo/label.txt","w");
		if(fil){
			fprintf(fil,"PERCENT_SELECT:%d PERCENT_WRONG:%d\n",PERCENT_SELECT,PERCENT_WRONG);
			for(i = 0;i<N;i++){
				fprintf(fil,"%d %d\n",i,clust[i]);
			}
			fclose(fil);
		}
	}
	W1 = (double**)malloc(N*sizeof(double*));
	UT1 = (double**)malloc(N*sizeof(double*));
	for(i = 0;i<N;i++){
		W1[i] = (double*)malloc(C*sizeof(double));	
		UT1[i] = (double*)malloc(C*sizeof(double));	
		for(j = 0;j<C;j++){
			W1[i][j] = W[i][j];
			UT1[i][j] = U[i][j] * (2 - W[i][j]);	
		}
	}	
	runtime += (double)(clock() - t) / CLOCKS_PER_SEC;
	runtime += randomUTW_PFS(0);
	t = clock();
	
	if(isdemo){
		outPutV("demo/mts3pfcm_init.txt", 0, 0, 3);	
	}
		
	//outPutSFCM("demo/init.txt", Vla, Ula, NLA, 0);	
	printf("mts3pfcm start\n");
	// PFCM for all
	do{		
		maxU = 0;
		maxT = 0;
		maxW = 0;
        // calculate V[i][j]
		for(j = 0;j<C;j++){ 			
			for(i = 0;i<D;i++){
				ts = 0;
				ms = 0;  
				for(k = 0;k<N;k++){
					ts += (double)pow(U[k][j]  * (2 - W[k][j]),M) * X[k][i];
					ms += (double)pow(U[k][j] * (2 - W[k][j]),M);        
				}

				for(k = 0;k<N;k++){
					if(clust[k] == -1){
						ts += (double)pow(U[k][j]  * (2 - W[k][j]),M) * X[k][i] / (double)pow(1+W1[k][j],M);
						ms += (double)pow(U[k][j] * (2 - W[k][j]),M) / (double)pow(1+W1[k][j],M);        	
					}else{
						if(clust[k] == j){
							ts += (double)pow(U[k][j]  * (2 - W[k][j]) - 1,M) * X[k][i] / (1 + (double)pow(UT1[k][j] - 1,M));
							ms += (double)pow(U[k][j]  * (2 - W[k][j]) - 1,M) / (1 + (double)pow(UT1[k][j] - 1,M));
						}else{
							ts += (double)pow(U[k][j]  * (2 - W[k][j]),M) * X[k][i] / (1 + (double)pow(UT1[k][j],M));
							ms += (double)pow(U[k][j]  * (2 - W[k][j]),M) / (1 + (double)pow(UT1[k][j],M));
						}
					}					
				}
				
				if(ms == 0){
					printf("tg2 = 0, division by zero in calcV!\n");
					exit(0);
				}
				V[j][i] = ts/ms;				
			}
		}	
	
		// calculate U[i][j]
		for(j = 0;j<C;j++){             	
			for(k = 0;k<N;k++){
				tg1 = calcX_subtract_V(k,j);								
				tg = 0;
				if(clust[k] == -1){
					ms = tg1 * (2 - W[k][j]) * (1 + (1.0 / W1[k][j]));
					tg3 = 0;
					for(i = 0;i<C;i++){
						tg2 = calcX_subtract_V(k,i);
						if(tg2 == 0){ 
							printf("tg2 = 0, division by zero in calcU1 fkj = 1!\n");
							tg2 = EPS;
						}			
						tg1 = 1 + (1.0 / W1[k][i]);			
						tg3 += 1.0 / (tg2 * tg1);																		
					}
					tg = 1.0 / (ms * tg3);
					//printf("ko nhan tg=%10.5lf\n",tg);
				}else{				
					if(clust[k] == j){
						tgg = (double)pow(UT1[k][j] - 1,M);
						tg2 = (2 - W[k][j]) * (2 + tgg);
						tg = 1 / tg2;
						ts = 1 - (1.0 / (2 + tgg));												
						ms = tg1 * (2 - W[k][j]) * (2 + tgg) / (1 + tgg);
						tg3 = 0;
						for(i = 0;i<C;i++){
							tg2 = calcX_subtract_V(k,i);
							if(tg2 == 0){ 
								printf("tg2 = 0, division by zero in calcU1 fkj = 1!\n");
								tg2 = EPS;
							}			
							tg1 = (double)pow(UT1[k][i] - 1,M);			
							tg3 += (1 + tg1)/(tg2 * (2 + tg1));																		
						}
						ms = ms * tg3;
						tg += ts/ms;
						//printf("co nhan dung tg=%10.5lf\n",tg);
					}else{
						ts = 0;												
						ms = tg1 * (2 - W[k][j]) * (2 + tgg) / (1 + tgg);
						tg3 = 0;
						for(i = 0;i<C;i++){
							tg2 = calcX_subtract_V(k,i);
							if(tg2 == 0){ 
								printf("tg2 = 0, division by zero in calcU1 fkj = 1!\n");
								tg2 = EPS;
							}
							tg1 = (double)pow(UT1[k][i] - 1,M);
							if(clust[k] == i){
								ts = 1.0 / (2 + tg1);
							}													
							tg3 += (1 + tg1)/(tg2 * (2 + tg1));																		
						}
						ms = ms * tg3;
						tg = (1.0 - ts)/ms;		
						//printf("co nhan sai tg=%10.5lf\n",tg);			
					}
				}
				if(tg >= 1)	getch();
				
				tg1 = (double)fabs(tg - U[k][j]);				
				if(maxU < tg1) maxU = tg1;
				U[k][j] = tg;
				if(U[k][j] > 1 || U[k][j] < 0){ 
					printf("U[%d][%d] = %10.5f  W = %10.5f\n",k,j,U[k][j],W[k][j]);
					getch();
//					scanf("%d",&i);
				}
			}		
        }

		// calculate T[i][j]		
		for(j = 0;j<C;j++){             	
			for(k = 0;k<N;k++){
				tg1 = 0;				
				tg2 = 0;
				for(i = 0;i<C;i++){
					tg1 += W[k][i];					
					tg2 += exp(-W[k][i]);
				}

				if(tg2 == 0){ 
					printf("tg2 = 0, division by zero in calcW!\n");
					tg2 = EPS;
				}								
				
				tg3 = ((1 - tg1 / C) * exp(-W[k][j])) / tg2;
				tg = (double)fabs(tg3 - T[k][j]);				
				if(maxT < tg) maxT = tg;
				T[k][j] = tg3;
								 
				if(T[k][j] > (1 - U[k][j]) || T[k][j] < 0){ 
					printf("T[%d][%d] = %10.5f tg1=%10.5f   W = %10.5f\n",k,j,T[k][j],tg1,W[k][j]);
					printf("U = %10.5f tg2 = %10.5f\n",U[k][j],tg2);					
				}
			}		
        }
		
		//calculate W[i][j]
		for(j = 0;j<C;j++){             	
			for(k = 0;k<N;k++){				
				tg = 1 - (U[k][j] + T[k][j]) - (double)pow((1 - (double)pow(U[k][j] + T[k][j],ALPHA)),1/ALPHA);
				tg1 = (double)fabs(W[k][j] - tg);
				if(maxW < tg1) maxW = tg1;
				W[k][j] = tg; 
			}		
        }		
				
		if(step == 0 && isdemo){
			outPutV("demo/mts3pfcm_firststep.txt", 0, 0, 3);	
		}
				
		// Check stop condition
		if(step > 0){			
			if(maxU + maxT + maxW < EPS){
				isNext = 0;
				printf("Stop with maxU = %7.5lf maxT = %7.5lf maxW = %7.5lf\n",maxU,maxT,maxW);
			}
		}
					
		if(step % 10 == 0) printf("MTS3PFCM:    iter %d ok\n",step);	
		step++;     
        runtime += (double)(clock() - t) / CLOCKS_PER_SEC;
        t = clock();                        
    }while(isNext != 0);     
    *iter = step;
    //if(step == 0) 
	//outPutSFCM("demo/sfcm_final.txt", V, U, N, 0);    
	sprintf(s,"%s%s",folder,fileout);
	outPutV(s, step, runtime,3); 
    printf("MTS3PFCM:   OutputV ok with %ld steps\n",step);
	
	//correct the clust
	for(i = 0;i<N;i++){
		if(clust[i] > -1){
			clust[i] = cluster[i];
		}		
	}
	    
	free(W1);
	free(UT1);		
	//free(CLASSLA);	
	return runtime;
}

int main(int  argc, char **argv){	
	char s1[100],s[100],s2[100];
	FILE *f;
	int i,n = 10,iter,averageIter = 0,count = 0;;
	double runtime = 0,runti,ifv = 0,db = 0,ma = 0,ca=0,nmi1 = 0,ti,ta,tg, runtime1;
	double db1,ri1,asw1,asw = 0,pbm1,pbm = 0,ifv1;
	double ma1 = 0,ma2 = 0,ri = 0,ca1=0,tgg;
	
	srand(time(NULL));
	
	// if (argc == 4){ 
	// 	sprintf(s1,"data/%s.txt",argv[1]);
	// 	n = atoi(argv[2]);
	// 	if(n < 1) n = 1; 
	// 	PERCENT_WRONG = atoi(argv[3]);		
	// }else{
	// 	printf("invalid input parameter ifs\n");
	// 	exit(0);
	// }

	sprintf(s1,"data/%s.txt",argv[1]);
	n = 10;
			
	runti = input(s1);
//	printf("input ok!\n");
	sprintf(s1,"results/MTS3PFCM_result_%s_%d.csv",argv[1],PERCENT_WRONG);
	f = fopen(s1,"w");
	
	if(!f){
		printf("Can't open file %s!",s1);		
		exit(0);
	}
	fprintf(f,"Result_file_%s:\n",argv[1]);
	fprintf(f,"Time,ca,ca_label,db,asw,pbm,ifv,runtime\n");
	for(i = 1;i<=n;i++){		
		printf("\nTime %d :\n",i);
		sprintf(s1,"results/%d/",i);
		sprintf(s2,"mts3pfcm_%s_%d.txt",argv[1],PERCENT_WRONG);
		sprintf(s,"%s%s",s1,s2);
			
		runtime1 = mts3pfcm(runti,s1,s2,&iter);		
		runtime += runtime1;
		
		printf("calc DB\n");
		db1 = DB_PFS(s);
		db += db1;
		printf("calc CA label\n");
		tgg = validity_kuhn();
		ca1 += tgg;
		free(clust);
		clust=NULL;
		printf("calc CA\n");
		tg = validity_kuhn();
		ca += tg;
		// printf("calc ASWC\n");
		// asw1 = ASWC_PFS(s);
		// asw += asw1;
		// printf("calc PBM\n");
		// pbm1 = PBM_PFS(s);
		// pbm += pbm1;		
		// printf("calc IFV\n");
		// ifv1 = IFV_PFS(s);
		// ifv += ifv1;
		
		fprintf(f,"%d,%10.5lf,%10.5lf,%10.5lf,%10.5lf,%10.5lf,%10.5lf,%10.5lf\n",i,tg,tgg,db1,asw1,pbm1,ifv1,runtime1);
				
		printf("ca = %10.5lf asw=%10.5lf pbm=%10.5lf runtime=%10.5lf\n",tg,asw1,pbm1,runtime1);
		printf("ca_label = %10.5lf DB = %10.5lf IFV = %10.5lf",tgg,db1,ifv1);
		//printf("RI=%10.5lf",ri1);
		averageIter += iter;	
	}
		
	//fprintf(f,"\nRI: %15.10f\n",ri/n);
	fprintf(f,"DB: %15.10f\n",db/n);	
	fprintf(f,"ASW: %15.10f\n",asw/n);	
	fprintf(f,"PBM: %15.10f\n",pbm/n);	
//	if(count){
//		tg = nmi1/count;		
//	}else tg = -1;
//	fprintf(f,"NMI: %15.10f\n",tg);
	fprintf(f,"IFV: %15.10f\n",ifv/n);
	fprintf(f,"Ca: %15.10f\n",ca/n);
	fprintf(f,"Ca_label: %15.10f\n",ca1/n);
	fprintf(f,"Time: %15.10f\n",(runtime/n) + runti);
	fprintf(f,"average_iteration: %15.10f\n",(double)averageIter/n);
	
	fclose(f);
	free(X);
	free(U);	
	free(V);
	free(cluster);
	return 0;
}

