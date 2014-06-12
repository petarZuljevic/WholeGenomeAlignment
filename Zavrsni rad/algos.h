#ifndef _ALGOS_H
#define _ALGOS_H 1
#include <cstdio>
#include <algorithm>
#include <vector>
#include <string>
#include <pthread.h>
#include <iostream>
#include "structs.h"
#include "time.h"

using namespace std;

class Solver {
public:
    Solver() {
    }
    
vector<int> which_list;
vector<int> SA_mapper;

vector <Mum> LIS (vector <Mum> generated_mumms) {
	sort(generated_mumms.begin(), generated_mumms.end());
	vector <Mum> result;
	int N = generated_mumms.size();
	for (int i = 0; i < N; ++i) {
        if (result.size() > 0) {
            if (generated_mumms[i].get_pos_first() < 
                result[result.size() - 1].get_pos_first() + result[result.size() - 1].get_length() )
            continue;
        }
        if (i < N - 1) {
            if (generated_mumms[i].get_length() < generated_mumms[i + 1].get_length() && 
      generated_mumms[i].get_pos_first() + generated_mumms[i].get_length() > generated_mumms[i + 1].get_pos_first()
        ||
       generated_mumms[i].get_pos_second() + generated_mumms[i].get_length() > generated_mumms[i + 1].get_pos_second())
             continue;
        }
		if ((i < N - 1) && generated_mumms[i].get_pos_second() > generated_mumms[i + 1].get_pos_second() 
			&& generated_mumms[i].reversed == false ) {
			continue;
		} else {
			result.push_back(generated_mumms[i]);
		}
	}
	return result;
}

void CreateList(vector < vector<int> >& lcp_list, vector < vector<int> >& sa_list, int from_first, int to_first, int from_second, int to_second, int split_pos) {
    vector <int> new_lcps;
    vector <int> new_sas;
    int indeks = lcp_list.size();
    cout << "OD [" << from_first << " DO " << to_first << "> = "<< indeks << endl;
    for (int i = from_first; i < to_first; ++i) {
        which_list[i] = indeks;
    }
    cout << "OD [" << from_second << " DO " << to_second << "> = "<< indeks << endl;
    for (int i = from_second; i < to_second; ++i) {
        which_list[i] = indeks;
    }
    new_lcps.push_back(0);
    new_sas.push_back(split_pos);
    lcp_list.push_back(new_lcps);
    sa_list.push_back(new_sas);
}

void FillLists(unsigned char *T, vector<int>& SA, vector<int>& LCP, int N, int s_pos, vector< vector<int> >& lcp_list, vector< vector<int> >& sa_list, tournament& tree, vector<int>& from1, vector<int>& to1, vector<int>& from2, vector<int>& to2) {
    int min = N;
    vector<int> last;
    for (int i = 0; i < lcp_list.size(); ++i) {
        last.push_back(s_pos);
    }

    for (int i = 0; i < SA.size(); ++i) {
        if(which_list[SA[i]] == -1 || which_list[SA[i]] > lcp_list.size()) {
            continue;
        }
        int indeks = which_list[SA[i]];
        if (SA[i] < from1[indeks] || SA[i] >= to1[indeks] && SA[i] < from2[indeks] || SA[i] >= to2[indeks]) {
            continue;        
        }
        
        sa_list[indeks].push_back(SA[i]);
        if(SA_mapper[last[indeks]] < SA_mapper[SA[i]] ) {
            lcp_list[indeks].push_back(tree.min(SA_mapper[last[indeks]] + 1, SA_mapper[SA[i]] + 1)); 
        } else {
            lcp_list[indeks].push_back(0);         
        }
        
        last[indeks] = SA[i];
    }
}

int found[10];
tournament tree;


void initialize(vector<int>& LCP, vector<int>&SA) {
        for(int i = 0; i < 10; ++i) 
            found[i] = 0;
        tree.init(LCP.size());
        clock_t start, finish;
        start = clock();
        fprintf ( stderr, "Krenulo \n" );
        tree.set(LCP);   
        finish = clock();
        fprintf (stderr, "Gotovo %.2f\n", (double)(finish - start) /4.0 /(double)CLOCKS_PER_SEC );
        which_list.resize(LCP.size());
        SA_mapper.resize(LCP.size());
        for(int i = 0; i < LCP.size(); ++i) which_list[i] = -1;
        for (int i = 0; i < SA.size(); ++i) {
            SA_mapper[SA[i]] = i;
        }
}

void extractMumms(vector<Mum>& mumms_tmp, unsigned char* T, vector<int>& SA, vector<int>& LCP, int N, int s_pos, int min_len, bool rev, int from1, int to1, int from2, int to2) {
   int b;
    Mum tmp_beg(from1, from2, 0);
    mumms_tmp.push_back(tmp_beg);
    int br = 0;
    
    for (int i = 1; i < SA.size(); ++i) {
		if (SA[i] <= s_pos && SA[i - 1] <= s_pos) {
			continue;		
		}

		if (SA[i] >= s_pos && SA[i - 1] >= s_pos) {
			continue;		
		}
		
		if (LCP[i] >= min_len 
			&& LCP[i] > LCP[i - 1] && (LCP[i] > LCP[i + 1] || i == SA.size() - 1)
			&& (SA[i] == 0 || SA[i - 1] == 0 || T[ SA[i] - 1] != T[ SA[i - 1] - 1])) {
			int pos1 = (SA[i] <= SA[i - 1]) ? SA[i] : SA[i - 1];
			int pos2 = (SA[i - 1] < SA[i]) ? SA[i] : SA[i - 1];	
            if (rev) {
                pos2 = N - pos2 + s_pos - 1;        
            }
			Mum tmp(pos1, pos2, LCP[i]);
            tmp.reversed = rev;
			mumms_tmp.push_back(tmp);
            ++br;
		}
	}
    Mum tmp_end(to1, to2, 0);
    mumms_tmp.push_back(tmp_end); 
    if (rev == false) {
        mumms_tmp = LIS(mumms_tmp);    
    }  else {
        sort(mumms_tmp.begin(), mumms_tmp.end());    
    }
}

void GenerateMumms(vector<Mum>& mumms, unsigned char* T, vector<int>& SA, vector<int>& LCP, int N, int s_pos, int min_len, bool rev, int num_of_iter, int cur_iter, int from1, int to1, int from2, int to2) {
    int length[3];
length[0] = 80;
length[1] = 70;
length[2] = 60;    
    min_len = length[cur_iter];
    if (rev) min_len += 15;
    vector<Mum> mumms_tmp;
    clock_t start, finish;
    extractMumms(mumms_tmp, T, SA, LCP, N, s_pos, min_len, rev, from1, to1, from2, to2);
    int num_of_mumms = mumms_tmp.size();
    if (num_of_mumms > 2)
     for (int i = 0; i < mumms_tmp.size(); ++i) {
           if (mumms_tmp[i].get_length() > 0) {
		       mumms.push_back(mumms_tmp[i]);	
                found[cur_iter]++;
           }
	}
    if (num_of_mumms < 4) {
        return;
    } 
    if (cur_iter  >= num_of_iter) {
        return;
    }
    vector< vector<int> > lcp_list;   
    vector< vector<int> > sa_list; 
    vector <int> f1, t1, f2, t2;
    start = clock();
    int dist = min_len*2 + 4000;
    
    for (int i = 1; i < num_of_mumms; ++i) {
            int from_first = mumms_tmp[i - 1].get_pos_first() + mumms_tmp[i - 1].get_length();
            int to_first = mumms_tmp[i].get_pos_first();
            int from_second = mumms_tmp[i - 1].get_pos_second() + mumms_tmp[i - 1].get_length();
            int to_second = mumms_tmp[i].get_pos_second();
        
            if (rev == false && (abs(to_first - from_first) < dist || abs(to_second - from_second) < dist)) {continue;}
            if (rev == true && abs(to_first - from_first) < dist) {continue;}
            CreateList(lcp_list, sa_list, from_first, to_first, from_second, to_second, s_pos);
            f1.push_back(from_first);
            f2.push_back(from_second);
            t1.push_back(to_first);
            t2.push_back(to_second);
    }  
     
    finish = clock();
    if (cur_iter == 1)
    fprintf (stderr, "%.2f listing %d\n", (double)(finish - start)/(double)(CLOCKS_PER_SEC), rev == true);
   
    if (lcp_list.size() == 0) {
        return;    
    }
    FillLists(T, SA, LCP, N, s_pos, lcp_list, sa_list, tree, f1, t1, f2, t2);
   
    for (int i = 0; i < lcp_list.size(); ++i) {
        GenerateMumms(mumms, T, sa_list[i], lcp_list[i], N, s_pos, min_len, 
                        rev, num_of_iter, cur_iter + 1, f1[i], t1[i], f2[i], t2[i]); 
    }
    
    if(cur_iter == 1) {
        for (int i = 1; i <= num_of_iter; ++i)
         fprintf(stderr, "%s pretraga, %d. iteracija => pronadeno: %d\n", (rev == true) ? "rev" : "fwd", i, found[i] );
    }
}
};



#endif /* _ALGOS_H */