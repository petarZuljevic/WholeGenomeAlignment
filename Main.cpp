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
#include "Solver.h"
#include "Objects.h"
#include "Parameters.h"
#include <pthread.h>
#include <iostream>

//Grani&#269;nik
int split_pos = 0;

//Datoteka iz koje citamo inpute
FILE *fp;
const char *fname;

//Konkatenacija dvaju inputa
unsigned char *T_fwd;
//Sufiksno i LCP polje zadanih inputa
int *SA_fwd, *LCP_fwd;

//Konkatenacija prvog inputa i drugog koji je reverzno komplementiran
unsigned char *T_rev;
//SA i LCP polje prvog inputa i drugog koji je reverzno komplementiran
int *SA_rev, *LCP_rev;

//Ukupan broj znakova u dvama inputima
long n;

//Vektori koji pohranjuju generirane MUMove
vector <Mum> mumms_fwd, mumms_rev;

//Vektori za SA i LCP polja
vector<int> vSA_fwd, vLCP_fwd;
vector<int> vSA_rev, vLCP_rev;

//Klase koje se koriste za generiranje MUMova
Solver fwd;
Solver rev;

/** SetSplitPos
				Pronalazak split pozicije dvaju stringova

				@param T Type unsigned char* Konkatenirani string
				@return void
*/
void SetSplitPos(unsigned char* T) {
    while (*T++ != '$') {
        split_pos++;
    }
}

/** RevComplement
				Vraca komplementarnu bazu u odnosu na zadanu

				@param base Type unsigned char Znak koji predstavlja bazu
				@return unsigned char Komplementarna baza
*/
unsigned char RevComplement(unsigned char base) {
    switch (base) {
        case 'C': return 'G';
        case 'G': return 'C';
        case 'T': return 'A';
        case 'A': return 'T';
    }
    return base;
}

/** SetRevComplement
				Za zadanu konkatenaciju (T) dvaju inputa, postavlja konkatenaciju
				T_rev koja sadrzi umjesto drugog inputa njegov reverzni komplement

				@param T Type unsigned char* Konkatenacija input stringova
				@param split_pos Type int Indeks granicnika "$"
				@param n Type int Ukupna duljina konkatenacije
				@return boolean result
*/
void SetRevComplement(unsigned char* T, int split_pos, int n) {
    for (int i = 0; i <= split_pos; ++i) {
        T_rev[i] = T[i];
    }
    for (int i = split_pos + 1, j = 2; i < n - 1; ++i, ++j) {
        T_rev[i] = RevComplement(T[n - j]);
    }
}


/** constructFwd		
				Dretvina funkcija koja konstruira SA i LCP polje za forward inpute
				@param void
				@return void
*/
void *constructFwd(void *) {
    sais(T_fwd, SA_fwd, LCP_fwd, (int)n);
}

/** constructRev
				Dretvina funkcija koja konstruira SA i LCP polje za reverzno komplementiran drugi input
				@param void
				@return void
*/
void *constructRev(void *) {
    sais(T_rev, SA_rev, LCP_rev, (int)n);
}

/** generateFWD
				Dretvina funkcija koja generira MUMove za forward inpute
				@param void
				@return void
*/
void *generateFWD(void *) {
    fwd.initialize(vLCP_fwd, vSA_fwd);
    int i;
    clock_t start, finish;
    start = clock();
    fwd.GenerateMumms(mumms_fwd, T_fwd, vSA_fwd, vLCP_fwd, n, split_pos, false, num_of_iter, 1, 0, split_pos, split_pos + 1, n);
    finish = clock();
    fprintf ( stderr, "\nGeneriranje fwd mumova %.2f\n", (double)(finish - start)/(double)CLOCKS_PER_SEC / 2.0);
}

/** generateREV
				Dretvina funkcija koja generira MUMove za reverzno komplementiran drugi input
				@param void
				@return void
*/
void *generateREV(void *) {
    rev.initialize(vLCP_rev, vSA_rev);
    int i;
    clock_t start, finish;
    start = clock();
    rev.GenerateMumms(mumms_rev, T_rev, vSA_rev, vLCP_rev, n, split_pos, true, num_of_iter, 1, 0, split_pos, split_pos + 1, n);
    finish = clock();
    fprintf ( stderr, "Generiranje rev mumova %.2f\n", (double)(finish - start)/(double)CLOCKS_PER_SEC / 2.0);
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
    
    //Konstrukcija SA i LCP polja za nenvertirane i revezno komplmentirane inpute pomocu 2 dretve
    fprintf(stderr, "%s: %ld bytes ... \n", fname, n);
    start = clock();
    pthread_t tForward, tRev;
    pthread_create (&tForward, NULL, constructFwd, NULL);
    pthread_create (&tRev, NULL, constructRev, NULL);
    pthread_join(tForward, NULL);
    pthread_join(tRev, NULL);
    
    //Pretvorba SA i LCP polja u vektore
    int i;
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
    
    fprintf(stderr, "\nSA+LCP construction: %.4f sec\n", (double)(finish - start) / (double)CLOCKS_PER_SEC);
    
    //Generiranje MUMova
    start = clock();
    pthread_t tFwdMum;
    pthread_create(&tFwdMum, NULL, generateFWD, NULL);
    pthread_t tRevMum;
    pthread_create(&tRevMum, NULL, generateREV, NULL);
    pthread_join(tFwdMum, NULL);
    pthread_join(tRevMum, NULL);
    
    //Ispis MUMova u datoteku, koristi se za daljnju obradu
    for (i = 0; i < mumms_fwd.size(); ++i) {
        mumms_fwd[i].print_mum(T_fwd, split_pos);
    }
    for (i = 0; i < mumms_rev.size(); ++i) {
        mumms_rev[i].print_mum(T_rev, split_pos);
    }
    finish = clock();
    fprintf ( stderr, "\nNumber of mumms: fwd: %ld,  rev: %ld.\n", mumms_fwd.size(), mumms_rev.size());
    
    fprintf(stderr, "\nDuration of generating mumms: %.4f sec\n", (double)(finish - start) / (double)CLOCKS_PER_SEC / 2.0);
    
    /* Deallocate memory. */
    free(T_fwd);
    free(T_rev);
    
    return 0;
}
