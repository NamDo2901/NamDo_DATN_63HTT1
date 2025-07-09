double nmi_PFS(char *filename){
	double result,max,tg,hl,hr,hlr,mi,v;
	double time = input(filename);
	//printf("time = %10.5lf\n",*time);
	int i,j,k;
		
	int *h,*g,*h_u,*g_u;	
	double *pl,*pr,*m;
	int *res_u = (int*)malloc(C*sizeof(int));
	
	int *res = (int*)malloc(N * sizeof(int));
	int *chek = (int*)malloc(C * sizeof(int));
	
	for(j = 0;j<C;j++){
		chek[j] = 0;
		res_u[j] = j;
	}
	
	for(i = 0;i<N;i++){
		max = U[i][0]*(2 - W[i][0]);
		k = 0;
		for(j = 1;j<C;j++){
			tg = U[i][j]*(2 - W[i][j]);
			if(max < tg){
				max = tg;
				k = j;
			}
		}		
		res[i] = k;
		chek[k] = 1;
	}
	
	k = 0;
	for(j = 0;j<C;j++){
		k += chek[j];
	}	
	free(chek);
	if(k < C){
		printf("incorrect data in %s, k = %d\n",filename,k);
		free(res_u);
		free(res);		
		return -1000;
	}else{		
		
		h = repmat(cluster, N, 1, 1, C);
		h_u = repmat(res_u,1,C,N,1);
		
		g = repmat(res, N, 1, 1, C);
		g_u = repmat(res_u,1,C,N,1);
		
		pl = checkMatrix(h,h_u,N,C);
		pr = checkMatrix(g,g_u,N,C);
		
		free(h_u);
		free(g_u);
		free(res);
		free(res_u);
			
		hl = 0;
		hr = 0;
		for(i = 0;i<C;i++){
			hl += -pl[i] * log(pl[i] + EPS/10)/log(2);
			hr += -pr[i] * log(pr[i] + EPS/10)/log(2);;
		}
		
		m = (double*)malloc(C*C*sizeof(double));
			
		for(j = 0;j<C;j++){
			for(k = 0;k<C;k++){
				m[j*C + k] = 0;
				for(i = 0;i<N;i++){		
					m[j*C + k] += h[i * C + j] * g[i * C + k];
				}
				m[j*C + k] = m[j*C + k]/N;
//				printf("%5.3lf ",m[j*C + k]);
			}
//			printf("\n");
		}
		hlr = 0;		
		for(j = 0;j<C;j++){
			for(k = 0;k<C;k++){
				hlr += -m[j*C + k] * (log(m[j*C + k] + EPS/10)/log(2));
//				printf("%5.3lf[%5.3lf-%5.3lf] ",hlr,m[j*C + k],log(m[j*C + k] + EPS)/log(2));
			}
		}	
		
//		printf("\nhlr = %10.5lf\n",hlr);
		
		mi = hl + hr - hlr;
		free(h);
		free(g);	
		free(m);
		free(pl);
		free(pr);	
		return sqrt((mi/hl) * (mi/hr));
	}
}

double MA_PFS(char *filename){
	double time,max,tg;
	int i,j,k,count = 0,flag = 0,l,sub;	
	time = input(filename);
		
	for(i = 0;i<N;i++){
		max = U[i][0]*(2 - W[i][0]);
		k = 0;
		for(j = 1;j<C;j++){
			tg = U[i][j]*(2 - W[i][j]);
			if(max < tg){ 
				max = tg;
				k = j;
			}
		}	
		if(k == cluster[i]) count++;
	}
	return (count * 100) / N;
}

double DB_PFS(char *filename){
	double time,max,tg,tg1,tg2,a,tg3;
	int i,j,k,l,*size,*mark;
		
	time = input(filename);
	mark = (int*)malloc(N*sizeof(int));
	
	for(i = 0;i<N;i++){
		max = U[i][0] * (2 - W[i][0]);
		k = 0;
		for(j = 1;j<C;j++){
			a = U[i][j] * (2 - W[i][j]);
			if(max < a){
				max = a;
				k = j;
			}		
		}
		mark[i] = k;
	}
	tg = 0;
	for(i = 0;i<C;i++){		
		max = 0;
		tg1 = calcDBS(i,mark);
		for(j = 0;j<C;j++){			
			if(i != j){
				tg3 = calcDBM(i,j);
				if(tg3 < 0.01) tg3 += 0.01;
				tg2 = (tg1 + calcDBS(j,mark)) / tg3;
				if(max < tg2) max = tg2;
			}
		}
		tg += max;
	}
	free(mark);
	return tg / C;
}

double IFV_PFS(char *filename){
	double time,max,tg,tg1,tg2,sum,a;
	int i,j,k,l,*size;
		
	time = input(filename);
	sum = 0;
	for(j = 0;j<C;j++){	
		tg1 = 0;
		for(i = 0;i<N;i++){
			a = U[i][j] * (2 - W[i][j]);
			if(a == 0) a = EPS;
			if(a == 1) a = 1 - EPS;
			tg1 += log(a) / log(2);
		}
		tg = pow(log(C)/log(2) - tg1/N,2);
		
		tg2 = 0;
		for(i = 0;i<N;i++){		
			a = U[i][j] * (2 - W[i][j]);
			tg2 += (double)pow(a,2);
		}
		tg2 = tg2/N;
		sum += tg * tg2;
//		printf("j = %d tg = %15.10lf sum = %15.10lf\n",j,tg,sum);	
	}
	
	return (sum * calcSDMax()) / (sigmaD() * C);	
}

double RI_PFS(char *filename){
	double time,max;
	int i,j,k,a = 0,b = 0,c = 0,d = 0;	
	time = input(filename);

	int *mark = (int*)malloc(N*sizeof(int));
	
	for(i = 0;i<N;i++){
		max = U[i][0]*(2 - W[i][0]);
		k = 0;
		for(j = 1;j<C;j++){
			if(max < U[i][j]*(2 - W[i][j])){ 
				max = U[i][j]*(2 - W[i][j]);
				k = j;
			}
		}	
		mark[i] = k;				
	}
	
	for(i = 0;i<N-1;i++){
		for(j = i + 1;j<N;j++){
			if(cluster[i] == cluster[j] && mark[i] == mark[j]){
				a++;
			}else if(cluster[i] != cluster[j] && mark[i] != mark[j]){
				b++;			
			}else if(cluster[i] != cluster[j] && mark[i] == mark[j]){
				c++;
			}else if(cluster[i] == cluster[j] && mark[i] != mark[j]){
				d++;
			}
		}
	}
	
	//printf("rand index a = %d b = %d c = %d d = %d\n",a,b,c,d);
	
	free(mark);
	if(a + b + c + d == 0){
		printf("Loi rand index!\n");
		return -1;
	}else{
		return (double)(a + b) / (a + b + c + d);	
	}	
}

double ASWC_PFS(char *filename){
	double time,max,tg,tg1,tg2,a,tg3;
	int i,j,k,l,*mark;	
	double sum, count = N;
	int markcount;
	
	int *countDif = (int*)malloc(C*sizeof(int));
	double *averageDif = (double*)malloc(C*sizeof(double));
	
	time = input(filename);
	
	mark = (int*)malloc(N*sizeof(int));
	
	for(i = 0;i<N;i++){
		max = U[i][0] * (2 - W[i][0]);
		k = 0;
		for(j = 1;j<C;j++){
			a = U[i][j] * (2 - W[i][j]);
			if(max < a){
				max = a;
				k = j;
			}		
		}
		mark[i] = k;
	}
		
	sum = 0;
//	printf("Begin ASWC C=%d\n",p->Cx);	
	for(i = 0;i<N;i++){
		markcount = 0;		
		for(j = 0;j<C;j++){
			averageDif[j] = 0;
			countDif[j] = 0;
		}
		for(j = 0;j<N;j++){
			if(i != j){
				tg = calcX_subtract_V2(X[i],X[j]);
				if(tg > 0) averageDif[mark[j]] += sqrt(tg);
				countDif[mark[j]]++;
			}
		}		
		max = -1;		
		for(j = 0;j<C;j++){
			if(countDif[j] > 0) averageDif[j] /= countDif[j];
			else averageDif[j] = 1;		
			if(j != mark[i]){
				if(max == -1 || max > averageDif[j]){
					max = averageDif[j];
				}			
			}
		}

		tg = max / (averageDif[mark[i]] + ASWC_EPS);
		sum += tg;
	}	
	
	free(averageDif);
	free(countDif);
	free(mark);
	return sum/N;	
}

double PBM_PFS(char *filename){
	int i,j,k;	
	double e1 = 0,ek = 0,dk = 0,tg;
	double time = input(filename);	
	
	double *value = (double*)malloc(D*sizeof(double));
	for(j = 0;j<D;j++){
		tg = 0;
		for(i = 0;i<N;i++){
			tg += X[i][j];	
		}
		value[j] = tg / N;	
	}
			
	for(i = 0;i<N;i++){
		tg = 0;
		for(j=0;j<D;j++){
			tg += pow(X[i][j] - value[j],2);
		}
		e1 += sqrt(tg);
	}

	for(i = 0;i<C;i++){
		for(j = 0;j<N;j++){
			ek += pow(U[j][i] * (2 - W[j][i]),M) * sqrt(calcX_subtract_V(j,i));			
		}		
	}
	
	for(i = 0;i<C - 1;i++){
		for(j = i + 1;j<C;j++){
			tg = 0;
			for(k = 0;k<D;k++){
				tg += pow(V[j][k] - V[i][k],2);
			}
			tg = sqrt(tg);
			if(dk < tg) dk = tg;
		}
	}
	free(value);
	if(ek == 0){
		printf("Loi PBM!\n");
		return -1;
		//exit(0);
	}
	return (e1 * dk)/(C * ek); 
}

