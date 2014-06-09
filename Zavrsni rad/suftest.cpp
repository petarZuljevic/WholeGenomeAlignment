/*
 * suftest.c for sais-lite
 * Copyright (c) 2008-2010 Yuta Mori All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sais.h"
#include "algos.h"
#include "structs.h"
#include <pthread.h>
#include <iostream>

int split_pos = 0;
FILE *fp;
const char *fname;
unsigned char *T_fwd;
int *SA_fwd;
int *LCP_fwd;
unsigned char *T_rev;
int *SA_rev;
int *LCP_rev;
long n;
int min_len;
int num_of_iter;
vector <Mum> mumms_fwd; 
vector <Mum> mumms_rev;
vector<int> vSA_fwd;
vector<int> vLCP_fwd;
vector<int> vSA_rev;
vector<int> vLCP_rev;
Solver fwd;
Solver rev;

void SetSplitPos(unsigned char* T) {
    while (*T++ != '$') {
        split_pos++;
    }
}

unsigned char RevComplement(unsigned char base) {
    switch (base) {
        case 'C': return 'G';
        case 'G': return 'C';
        case 'T': return 'A';
        case 'A': return 'T';    
    }
    return base;
}

void SetRevComplement(unsigned char* T, int split_pos, int n) {
    for (int i = 0; i <= split_pos; ++i) {
        T_rev[i] = T[i];
    }
    for (int i = split_pos + 1, j = 2; i < n - 1; ++i, ++j) {
        T_rev[i] = RevComplement(T[n - j]);
    }
}

void *constructFwd(void *) {
  sais(T_fwd, SA_fwd, LCP_fwd, (int)n);
}

void *constructRev(void *) {
  sais(T_rev, SA_rev, LCP_rev, (int)n);
}

void *generateFWD(void *) {
    fwd.initialize(vLCP_fwd, vSA_fwd);  
    int i;
    clock_t start, finish;
    start = clock();
    fwd.GenerateMumms(mumms_fwd, T_fwd, vSA_fwd, vLCP_fwd, n, split_pos, min_len, false, num_of_iter, 1, 0, split_pos, split_pos + 1, n);
    finish = clock();
    fprintf ( stderr, "Generiranje fwd %.2f\n", (double)(finish - start)/(double)CLOCKS_PER_SEC / 2.0);
}

void *generateREV(void *) {
    rev.initialize(vLCP_rev, vSA_rev); 
    int i;
     clock_t start, finish;
    start = clock();
   rev.GenerateMumms(mumms_rev, T_rev, vSA_rev, vLCP_rev, n, split_pos, min_len, true, num_of_iter, 1, 0, split_pos, split_pos + 1, n);
    finish = clock();
    fprintf ( stderr, "Generiranje rev %.2f\n", (double)(finish - start)/(double)CLOCKS_PER_SEC / 2.0);
}

int main(int argc, const char *argv[]) {
  
  clock_t start, finish;
  /* Open a file for reading. */
  if((fp = fopen(fname = argv[1], "rb")) == NULL) {
    fprintf(stderr, "%s: Cannot open file `%s': ", argv[0], fname);
    perror(NULL);
    exit(EXIT_FAILURE);
  }

  /* Get the file size. */
  if(fseek(fp, 0, SEEK_END) == 0) {
    n = ftell(fp);
    rewind(fp);
    if(n < 0) {
      fprintf(stderr, "%s: Cannot ftell `%s': ", argv[0], fname);
      perror(NULL);
      exit(EXIT_FAILURE);
    }
  } else {
    fprintf(stderr, "%s: Cannot fseek `%s': ", argv[0], fname);
    perror(NULL);
    exit(EXIT_FAILURE);
  }
    
  /* Allocate 9n bytes of memory. */
  T_fwd = (unsigned char *)malloc((size_t)n * sizeof(unsigned char));
  SA_fwd = (int *)malloc((size_t)(n+1) * sizeof(int)); // +1 for computing LCP
  LCP_fwd = (int *)malloc((size_t)n * sizeof(int));
  T_rev = (unsigned char *)malloc((size_t)n * sizeof(unsigned char));
  SA_rev = (int *)malloc((size_t)(n+1) * sizeof(int)); // +1 for computing LCP
  LCP_rev = (int *)malloc((size_t)n * sizeof(int));

  if((T_fwd == NULL) || (SA_fwd == NULL) || (LCP_fwd == NULL)) {
    fprintf(stderr, "%s dat: Cannot allocate memory.\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* Read n bytes of data. */
  if(fread(T_fwd, sizeof(unsigned char), (size_t)n, fp) != (size_t)n) {
    fprintf(stderr, "%s: %s `%s': ",
      argv[0],
      (ferror(fp) || !feof(fp)) ? "Cannot read from" : "Unexpected EOF in",
      argv[1]);
    perror(NULL);
    exit(EXIT_FAILURE);
  }
  fclose(fp);

    T_fwd[n-1]=0;
    SetSplitPos(T_fwd);
    SetRevComplement(T_fwd, split_pos, n);
    T_rev[n-1]=0;
  
    fprintf(stderr, "%s: %ld bytes ... \n", fname, n);
    start = clock();
    pthread_t tForward, tRev;
    pthread_create (&tForward, NULL, constructFwd, NULL);
    pthread_create (&tRev, NULL, constructRev, NULL);
    pthread_join(tForward, NULL);
    pthread_join(tRev, NULL);
  
	int i;
	
    min_len = atoi(argv[2]);
    num_of_iter = atoi(argv[3]);
    
    for (i = 0; i < n; ++i) {
        vSA_fwd.push_back(SA_fwd[i]);  
        vLCP_fwd.push_back(LCP_fwd[i]); 
        vSA_rev.push_back(SA_rev[i]);  
        vLCP_rev.push_back(LCP_rev[i]);      
    }
    free(SA_rev);
    free(SA_fwd);
    free(LCP_fwd);
    free(LCP_rev);
    finish = clock();
   
    fprintf(stderr, "SA+LCP construction: %.4f sec\n", (double)(finish - start) / (double)CLOCKS_PER_SEC);
	
    start = clock();
    pthread_t tFwdMum;
    pthread_create(&tFwdMum, NULL, generateFWD, NULL);
    pthread_t tRevMum;
    pthread_create(&tRevMum, NULL, generateREV, NULL);
    pthread_join(tFwdMum, NULL);
    pthread_join(tRevMum, NULL);
    for (i = 0; i < mumms_fwd.size(); ++i) {
		mumms_fwd[i].print_mum(T_fwd, split_pos);	
	}
    for (i = 0; i < mumms_rev.size(); ++i) {
	    mumms_rev[i].print_mum(T_rev, split_pos);
	}
    finish = clock();
    fprintf ( stderr, "after LIS mumms: fwd: %ld,  rev: %ld...\n", mumms_fwd.size(), mumms_rev.size());
    
  	fprintf(stderr, "\nmumms generating total: %.4f sec\n", (double)(finish - start) / (double)CLOCKS_PER_SEC / 2.0);
 
    /* Deallocate memory. */
    free(T_fwd);
    free(T_rev);

    return 0;
}
