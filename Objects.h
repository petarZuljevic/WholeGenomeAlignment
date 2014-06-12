#ifndef _OBJECTS_H
#define _OBJECTS_H 1
#include <cstdlib>
#include <iostream>
#include <vector>
#include <pthread.h>
using namespace std;

/* MUM
			Maximal Unique Match - predstavlja MUM i sve informacije vezane uz njega poput
			lokacije u jednom i drugom nizu, duljinu te svojstvo da li je reverzno komplementiran.
*/
class Mum {
    int pos_first;
    int pos_second;
    int length;
    public:
    bool reversed;
    Mum(int x, int y, int l, bool rev = false, bool mm = false){
        pos_first = x;
        pos_second = y;
        length = l;
        reversed = rev;
    }
    
    int get_pos_first() {
        return pos_first;
    }
    
    int get_pos_second() {
        return pos_second;
    }
    
    int get_length() {
        return length;
    };
    
    bool operator > (const Mum& other) const {
        return this->pos_first > other.pos_first;
    }
    
    bool operator < (const Mum& other) const {
        return this->pos_first < other.pos_first;
    }
    
    bool operator == (const Mum& other) const {
        return this->pos_first == other.pos_first;
    }
    
    void print_mum (unsigned char* T, int split_pos = 0) {
        if (reversed) std::cout << "- - ";
        std::cout <<  pos_first + 1 << " " << pos_second - split_pos << "\n";
        if (reversed) std::cout << "- - ",
        std::cout << pos_first + 1 + length << " " << pos_second - split_pos - length<< "\n\n";
        else
        std::cout << pos_first + 1 + length << " " << pos_second - split_pos + length<< "\n\n";
    }
};


const int inf = 1000000000;

/** tournament
				Segmentno ili turnirsko stablo. Daje odgovor u O(log(N)) koji je najmanji broj na intervalu od [a, b>.
*/
class Tournament {
    
    private:
    //Odmak, oznacava poziciju prvog lista u stablu
    int offset;
				//vector<int>a pohranjuje citavo stablo.
    vector<int> a;
				
				//Interne varijable koje koristimo za pronalazak minimuma
    int from, to; 

				/** min
								Pronalazi minimum na intervalu.
								@param i Type Indeks trenutnog cvora
								@param lo Type Donja meda
								@param hi Type Gornja meda
								@return int Minimum na intervalu
				*/   
    int min( int i, int lo, int hi ) {
        if( from >= hi || to <= lo ) return inf;
        if( lo >= from && hi <= to ) return a[i];
        int minLeft = min( 2*i, lo, (lo+hi)/2 );
        int minRight = min( 2*i+1, (lo+hi)/2, hi );
        return  minLeft < minRight ? minLeft : minRight;
    }
    
    public:
    Tournament() {
    }
    
				/** init
								Postavlja vektor koji pamti stablo na velicinu 2*N
								@param N Type int
								@return void
				*/
    void init( int N ) {
        for( offset = 1; offset < N; offset *= 2 );
        a.resize( 2*offset );
    }

    /** set
								Postavlja listove stabla na zadane elemete.
								@param LCP Type vector<int> LCP polje
								@return void
				*/
    void set(vector<int>& LCP) {
        int i = offset;
								//Prvo postavimo listove
        for (int k = 0; k < LCP.size(); ++k) {
            a[i + k] = LCP[k];
        }
								//Svaki roditelj skupi manje dijete
        for( i = offset - 1; i > 0; --i ) {
            a[i] = std::min(a[2*i], a[2*i+1]);
        }
    }
    
    int min( int lo, int hi ) { from = lo; to = hi; return min( 1, 0, offset ); }
};

#endif /* _OBJECTS_H */
