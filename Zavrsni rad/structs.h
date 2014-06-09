#ifndef _STRUCTS_H
#define _STRUCTS_H 1
#include <cstdlib>
#include <iostream>
#include <vector>
#include <pthread.h>
using namespace std;
//Maximal unique matches
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

class tournament {

private:

    int offset;
    vector<int> a;
    int from, to;

    int min( int i, int lo, int hi ) {
        if( from >= hi || to <= lo ) return inf;
        if( lo >= from && hi <= to ) return a[i];
        int minLeft = min( 2*i, lo, (lo+hi)/2 );
        int minRight = min( 2*i+1, (lo+hi)/2, hi );
        return  minLeft < minRight ? minLeft : minRight;
    }

public:
    tournament() {
    }

    void init( int N ) {
        for( offset = 1; offset < N; offset *= 2 );
        a.resize( 2*offset );
    }
    
    void set(vector<int>& LCP) { 
        int i = offset;
        for (int k = 0; k < LCP.size(); ++k) {
            a[i + k] = LCP[k];         
        }
        for( i = offset - 1; i > 0; --i ) {
            a[i] = std::min(a[2*i], a[2*i+1]);
        }
    }

    int min( int lo, int hi ) { from = lo; to = hi; return min( 1, 0, offset ); }
}; 

#endif /* _STRUCTS_H */
