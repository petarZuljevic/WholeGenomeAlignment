#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "nw.h"

using namespace std;
using std::iostream;
using std::string;
using std::vector;
using std::max;
int split_pos;
string r;
struct StringProcessor {

#define MAXN 100000
#define ALPHA 150

    int s[MAXN+5], SA[MAXN+5], n, rank[MAXN+5];
    int counter[(MAXN<<1)+5], h[MAXN+5];
    int LCP[MAXN+5];
    

    StringProcessor(string str_a, string str_b) {
        string all = str_a + "$" + str_b;
        n = all.size() + 3;
		r = all;
        split_pos = (int)str_a.size() + 1;
        memset(s, 0, sizeof s);
        for (int i = 0; i < all.size(); ++i) {
            if (all[i] != '$') s[i] = all[i] - 'A' + 2;
            else s[i] = 1;
        }
        suffix_array(s, SA, n, ALPHA);
        lcp();
    }

    inline bool fcmp(int a1, int a2, int b1, int b2) {
        return ((a1<b1)||(a1==b1&&a2<b2));
    }

    inline bool fcmp(int a1, int a2, int a3, int b1, int b2, int b3) {
        return ((a1<b1)||(a1==b1&&fcmp(a2, a3, b2, b3)));
    }

    void radix(int* a, int* b, int* s, int n, int K) {
        memset(counter, 0, sizeof(int)*(K+1));
        register int i, sum;
        for (i=0; i < n; i++)
            counter[s[a[i]]]++;
        for (i=0, sum=0; i<=K; i++) {
            int tmp=counter[i];
            counter[i]=sum;
            sum+=tmp;
        }
        for (i=0; i!=n; i++)
            b[counter[s[a[i]]]++]=a[i];
    }

    void suffix_array(int* s, int* SA, int n, int K) {
        int n0=(n+2)/3, n1=(n+1)/3, n2=n/3, n02=n0+n2;
        int *s12=new int[n02+3], *SA12=new int[n02+3], *s0=new int[n0], *SA0=new int[n0];
        s12[n02]=s12[n02+1]=s12[n02+2]=0;
        SA12[n02]=SA12[n02+1]=SA12[n02+2]=0;
        for (int i=0, j=0; i!=n+(n0-n1); i++)
            if (i%3)
                s12[j++]=i;
        radix(s12, SA12, s+2, n02, K);
        radix(SA12, s12, s+1, n02, K);
        radix(s12, SA12, s, n02, K);
        int wh=0, t0=-1, t1=-1, t2=-1;
        for (int i=0; i!=n02; i++) {
            if (s[SA12[i]]!=t0||s[SA12[i]+1]!=t1||s[SA12[i]+2]!=t2) {
                wh++;
                t0=s[SA12[i]];
                t1=s[SA12[i]+1];
                t2=s[SA12[i]+2];
            }
            if (SA12[i]%3==1)
                s12[SA12[i]/3]=wh;
            else
                s12[SA12[i]/3+n0]=wh;
        }
        if (wh<n02) {
            suffix_array(s12, SA12, n02, wh);
            for (int i=0; i!=n02; i++)
                s12[SA12[i]]=i+1;
        } else
            for (int i=0; i!=n02; i++)
                SA12[s12[i]-1]=i;
        for (int i=0, j=0; i!=n02; i++)
            if (SA12[i]<n0)
                s0[j++]=SA12[i]*3;
        radix(s0, SA0, s, n0, K);
        int p0=0, p12=n0-n1;
        for (int k=0; k<n; k++) {
            int i=SA0[p0], j=SA12[p12]<n0 ? SA12[p12]*3+1 : (SA12[p12]-n0)*3+2;
            if (SA12[p12]<n0 ? fcmp(s[i], s12[i/3], s[j], s12[SA12[p12]+n0]) : fcmp(s[i], s[i+1], s12[i/3+n0], s[j], s[j+1], s12[SA12[p12]-n0+1])) {
                SA[k]=i;
                p0++;
                if (p0==n0)
                    for (k++; p12<=n02; p12++, k++)
                        SA[k]=SA12[p12]<n0 ? SA12[p12]*3+1 : (SA12[p12]-n0)*3+2;
            } else {
                SA[k]=j;
                p12++;
                if (p12==n02)
                    for (k++; p0<=n0; p0++, k++)
                        SA[k]=SA0[p0];
            }
        }
        delete[]s12;
        delete[]SA12;
        delete[]s0;
        delete[]SA0;

    }

    inline void lcp() {
        register int i, p;
        for (i = 0; i < n; ++i)
            rank[SA[i]] = i;
        for (i=0; i<n; i++) {
            if (rank[i]==0)
                h[i]=0;
            else {
                if (i==0 || h[i-1]<=1) {
                    p=0;
                    while (i+p < n && SA[rank[i]-1] + p<n && s[i+p] == s[SA[rank[i]-1]+p])
                        p++;
                    h[i] = p;
                } else {
                    int p = h[i-1] - 1;
                    while (i+p<n&&SA[rank[i]-1]+p<n&&s[i+p]==s[SA[rank[i]-1]+p])
                        p++;
                    h[i]=p;
                }
            }
        }
        for (i=0; i<n; i++)
            LCP[i]=h[SA[i]];
    }
	bool side(int a) {
		return ( a > split_pos );
	}
	
    vector<int> generate_mums(int m) {
        vector<int> ret;
        //int m = 0;
        for (int i = 1; i < n; ++i) {
            if (SA[i] <= split_pos && SA[i-1] <= split_pos)
                continue;
            if (SA[i] >= split_pos && SA[i-1] >= split_pos)
                continue;
            /*if ( !uniq || LCP[i] > LCP[i-1] && LCP[i] > LCP[i+1] ) {
                m = max(m, LCP[i]);
            }*/
        }
		bool visited[r.size()];
		memset (visited, false, sizeof visited);
		
        for (int i = 1; i < n; ++i) {
			 //if (SA[i] <= split_pos && SA[i-1] <= split_pos)
                //continue;
            //if (SA[i] >= split_pos && SA[i-1] >= split_pos)
              //  continue;
			if ( LCP[i] >= m && LCP[i] > LCP[i -1 ] ) {
				int a = SA[i-1];
				if (a > split_pos) a -= split_pos;
				int b = SA[i];
				if (b > split_pos) b -= split_pos;
                ret.push_back(a); // index u prvom stringu
                ret.push_back(b); // index u drugom 
                ret.push_back(LCP[i]); // duljina
            } 
        }
		
        return ret;
    }
};

string read_input(char *f1) {
	FILE* F1 = fopen(f1, "r");
	char c;
	string output = "";
	do {
		c = fgetc(F1);	
		if (c == 'A' || c == 'T' || c == 'G' || c == 'C') output += c;
	} while ( c != EOF);
	output += "\0";
	fclose(F1);
	return output;
}
struct mum {
		int ind1;
		int ind2;
		int len;
		bool operator<(const struct mum& rhs) const {
			return ind1 < rhs.ind1;		
		}
};
int main(int argc, char* argv[]) {

	
	string s1 = read_input(argv[1]);
	string s2 = read_input(argv[2]);
	
	FILE * NW;
	NW = fopen( "NW.txt", "w" );
	solve(NW,s1,s2);
	fclose(NW);

	StringProcessor proc(s1, s2);
	
    vector<int> p = proc.generate_mums(5);
	
	vector<struct mum> mums;
    for (int i = 0; i < p.size(); i += 3) {
		struct mum tmp;
		int t_1 = (p[i] < p[i+1]) ? p[i] : p[i+1];
		int t_2 = (p[i+1] < p[i]) ? p[i] : p[i+1];
		tmp.ind1 = t_1;
		tmp.ind2 = t_2;
		tmp.len = p[i + 2];
		mums.push_back(tmp);
    }
	vector<struct mum> new_mums;
	sort(mums.begin(), mums.end());
	int prev = mums[0].len;
	new_mums.push_back(mums[0]);
	for (int i = 1; i < mums.size(); ++i) {
		if (mums[i].ind1 < mums[i - 1].ind1 + mums[i - 1].len
			&& mums[i].ind2 < mums[i - 1].ind2 + mums[i - 1].len) continue;
		new_mums.push_back(mums[i]);
	}
	FILE *out;
	out = fopen("Mumms-pregled.txt", "w");
	fprintf ( out, "ind1\tind2\tduljina\tMUM\n" );
	FILE *plt;
	plt = fopen("Mumms-plot.txt", "w");
	for (int i = 0; i < new_mums.size(); ++i) {
		fprintf(out, "%d\t\t%d\t\t\t%d\t", new_mums[i].ind1, new_mums[i].ind2, new_mums[i].len);
		fprintf ( plt, "%d %d\n%d %d\n\n", new_mums[i].ind1, new_mums[i].ind2, new_mums[i].ind1 + new_mums[i].len, new_mums[i].ind2 + new_mums[i].len );
		if ( new_mums[i].len < 25) fprintf( out, "%s", (r.substr(new_mums[i].ind2, new_mums[i].len)).c_str());
		fprintf(out, "\n" );
	}
	fclose(out);
	fclose(plt);
    return 0;
}
