#include <vector>
#include <cstdio>
#include <utility>
#include <string>
#include <algorithm>

using namespace std;

#define inf 1000000000

int check_max(int &a, int b) {
   if (b > a) {
      a = b;

      if (a >  inf) a =  inf;
      if (a < -inf) a = -inf;

      return 1;
   }
   return 0;
}

pair<string, string> alignment(                                                          
        string       seq_1,          
        string       seq_2)
{
   int n = seq_1.length();
   int m = seq_2.length();

   vector<vector<int> > dp(n + 1, vector<int>(m + 1, -inf));
   vector<vector<int> > tp(n + 1, vector<int>(m + 1, -inf));

   dp[0][0] = 0;

   for (int i = 1; i <= n; ++i) {
      dp[i][0] = -2 * i;
      tp[i][0] = 2;
   }

   for (int i = 1; i <= m; ++i) {
      dp[0][i] = -2 * i;
      tp[0][i] = 1;
   }

   for (int i = 1; i <= n; ++i) {
      for (int j = 1; j <= m; ++j) {
         int ca = seq_1[i - 1];
         int cb = seq_2[j - 1];

         if (ca == cb) {
            if (check_max(dp[i][j], dp[i - 1][j - 1] + 2)) tp[i][j] = 0;
         }
         if (check_max(dp[i][j], dp[i][j - 1] - 2)) tp[i][j] = 1;
         if (check_max(dp[i][j], dp[i - 1][j] - 2)) tp[i][j] = 2;
      }
   }
   
   if (dp[n][m] == -inf) {
      perror("Greska");
      return make_pair(seq_1, seq_2);
   }
   printf ( "\n%d\n", dp[n][m] );
   string a, b;
   for (int i = n, j = m; i != 0 || j != 0; ) {
      switch (tp[i][j]) {
         case 0:
            a.push_back(seq_1[i - 1]);
            b.push_back(seq_2[j - 1]);
            --i, --j; 
            break;
         case 1: 
            a.push_back('-');
            b.push_back(seq_2[j - 1]);
            --j;
            break;
         case 2:
            b.push_back('-');
            a.push_back(seq_1[i - 1]);
            --i;
            break;
      }
   }

   reverse(a.begin(), a.end());
   reverse(b.begin(), b.end());

   return make_pair(a, b);
}

void solve(FILE      *dat,
           string       a,          
           string       b) {

   pair <string, string> s =  alignment(a, b);
   
   fprintf ( dat, "%s\n", s.first.c_str() );
   
   int n = s.first.length();
   
   for ( int i = 0; i < n; ++i ) {
      if ( s.first.c_str()[i] == s.second.c_str()[i] ) {
            fprintf ( dat, "|" );
      } else {
            fprintf ( dat, " " );
      }
   }
   fprintf ( dat, "\n%s\n", s.second.c_str() );
}
