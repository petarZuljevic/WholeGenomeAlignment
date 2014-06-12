#ifndef _Solver_H
#define _Solver_H 1
#include <cstdio>
#include <algorithm>
#include <vector>
#include <string>
#include <pthread.h>
#include <iostream>
#include "Parameters.h"
#include "Objects.h"
#include "time.h"

using namespace std;

/** class Solver
Klasa koja za zadanu konkatenaciju nizova i zadana LCP / SA polja generira vektor MUMova.
*/
class Solver {
    public:
    Solver() {
    }
    
				//Segmentno stablo koje ce odgovarati na upite "koliki je najmanji LCP izmedu pozicija "a" i "b".
    Tournament tree;
				
				//Lista koja nam za indeks "i" kaze kojoj listi pripada element na poziciji "i" iz konkatenacije nizova.
    vector<int> which_list;
				//Sluzi da bismo kasnije mogli odgovoriti na pitanje, "Koji je indeks elementa u sufiksnom polju cija je vrijednost X", odgovor je SA_mapper[X]?
    vector<int> SA_mapper;
    
				/** LIS
								Funkcija koja filtrira dane MUMove kao prema alatu MUMmer 1.0
								@param generated_mumms TYPE vector <Mum> Generirani mumovi koje treba isfiltrirat
								@return vector <Mum> Lista filtriranih MUMova.
				*/
    vector <Mum> LIS (vector <Mum> generated_mumms) {
								//Za pocetak sortiramo MUMove po poziciji u referentnom nizu
        sort(generated_mumms.begin(), generated_mumms.end());
        vector <Mum> result;
        int N = generated_mumms.size();
        for (int i = 0; i < N; ++i) {
												//Ako se trenutni MUM preklapa sa zadnjim dodanim u listu result preskoci i idi dalje
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
												//Za slucaj da je pocetna pozicija u drugom nizu od MUMa koji se nalazi neposredno nakon trenutnog u listi, veca od
												//pozicije u drugom nizu trenutnog MUMa dodaj ga u vektor result.
												//Ako je MUM reverzan tada ne primjenjuj LIS
            if ((i < N - 1) && generated_mumms[i].get_pos_second() > generated_mumms[i + 1].get_pos_second()
            && generated_mumms[i].reversed == false ) {
                continue;
                } else {
                result.push_back(generated_mumms[i]);
            }
        }
        return result;
    }
    
				/** CreateList
								Za zadane intervale u prvom i drugom nizu postavlja pripadnost elemenata nizova na tim intervalima odgovarajućim listama.
								Svako netaknuto područje između para MUMova zove se netaknuto te mu pridružujemo listu LCP i SA.
								S obzirom da se radi o više takvih područja, potrebno je kreirati listu listi LCP i SA polja, a u ovoj funkciji dodajemo početne
								elemente LCP i SA liste za zadanu netaknutu regiju.
						
								@param lcp_list Type vector<int> Lista LCP listi
								@param sa_list Type vector<int> Lista SA listi
								@param from_first Type int Početna pozicija u prvom nizu
								@param to_first Type int Krajnja pozicija u prvom nizu
								@param from_second Type int Početna pozicija u drugom nizu
								@param to_second Type int Krajnja pozicija u drugom nizu
								@param split_pos Type int Indeks graničnika
								@return void
				*/
    void CreateList(vector < vector<int> >& lcp_list, vector < vector<int> >& sa_list, 
																				int from_first, int to_first, int from_second, int to_second, int split_pos) {
        vector <int> new_lcps;
        vector <int> new_sas;
								//Indeks predstavlja redni broj zadnje dodane liste
        int indeks = lcp_list.size();
        for (int i = from_first; i < to_first; ++i) {
            which_list[i] = indeks;
        }
        for (int i = from_second; i < to_second; ++i) {
            which_list[i] = indeks;
        }
        new_lcps.push_back(0);
        new_sas.push_back(split_pos);
        lcp_list.push_back(new_lcps);
        sa_list.push_back(new_sas);
    }
    
				/** FillLists
								Funkcija koja puni LCP i SA liste netaknutih regija tako da se nad njima mogu ponovno raditi iteracije tj. traziti MUMovi.
								Za punjenje LCP i SA listi trebamo proći kroz izvorne LCP i SA liste i za svaki element vidjeti kojoj listi odnosno netaknutoj 
								regiji pripada, a zatim ga dodati u odgovarajuću listu.
								
								@param T Type unsigned char* Konkatenacija inputa
								@param SA Type vector<int> Polje SA za područje nad kojim smo pretražili MUMove
								@param LCP Type vector<int> Polje LCP za područje nad kojim smo pretražili MUMove
								@param N Type int Broj elemenata u nizu T
								@param s_pos Type int Indeks pozicije graničnika
								@param lcp_list Type vector <vector <int> > Lista listi LCP polja
								@param sa_list Type vector <vector <int> > Lista listi SA polja
								@param tree Type  tournament Segmentno stablo
								@param from1 Type int Početna pozicija u prvom stringu
								@param to1 Type int Krajnja pozicija u prvom stringu
								@param from2 Type int Početna pozicija u drugom stringu
								@param to2 Type int Krajnja pozicija u drugom stringu
								@return void
				*/
    void FillLists(unsigned char *T, vector<int>& SA, vector<int>& LCP, int N, int s_pos, vector< vector<int> >& lcp_list, 
																			vector< vector<int> >& sa_list, Tournament& tree, vector<int>& from1, vector<int>& to1, vector<int>& from2, vector<int>& to2) {
        int min = N;
								//vektor last na poziciji "i" cuva indeks zadnjeg dodanog LCP-a iz izvornog polja LCP-ova
							 //to nam je potrebno da bismo mogli kreirati LCP listu za određenu netaknutu regiju.
        vector<int> last;
        for (int i = 0; i < lcp_list.size(); ++i) {
												//Na početku inicijaliziramo pozicijom graničnika, prvi element ionako nije bitan
            last.push_back(s_pos);
        }
        
        for (int i = 0; i < SA.size(); ++i) {
												//Preskoči ako element na ovoj pozicji nije dio neke netaknute regije
            if(which_list[SA[i]] == -1 || which_list[SA[i]] > lcp_list.size()) {
                continue;
            }
            int indeks = which_list[SA[i]];
            if (SA[i] < from1[indeks] || SA[i] >= to1[indeks] && SA[i] < from2[indeks] || SA[i] >= to2[indeks]) {
                continue;
            }
           
            sa_list[indeks].push_back(SA[i]);
            if(SA_mapper[last[indeks]] < SA_mapper[SA[i]] ) {
																//Odgovarajući LCP je minimum na intervalu zadnjeg dodanog i trenutnog elementa u izvornoj LCP listi           
											     lcp_list[indeks].push_back(tree.min(SA_mapper[last[indeks]] + 1, SA_mapper[SA[i]] + 1));
                } else {
                lcp_list[indeks].push_back(0);
            }
        				//Pohrani zadnji dodani element za listu s rednim brojem "indeks"
            last[indeks] = SA[i];
        }
    }
    /** initialize
								Funkcija koja inicijalizira strukture podataka tako da se kasnije mogu ispravno koristiti.
							
								@param	LCP Type vector<int> Vektor LCPova							
								@param	SA Type vector<int> Vektor SAova
								@return void
				*/
    void initialize(vector<int>& LCP, vector<int>&SA) {
								//Inicijalizacija segmentnog stabla i punjenje njegovih cvorova elementima LCP polja        
								tree.init(LCP.size());
        tree.set(LCP);
        which_list.resize(LCP.size());
        SA_mapper.resize(LCP.size());
        for(int i = 0; i < LCP.size(); ++i) { 
												which_list[i] = -1;
								}
        for (int i = 0; i < SA.size(); ++i) {
            SA_mapper[SA[i]] = i;
        }
    }

    /** extractMumms
								Konstruira sve MUMove dulje od kritične duljine iz zadanih LCP i SA polja.
							
								@param mumms_tmp Type vector<Mum> Trenutni vektor MUMova u kojeg cemo pohraniti novo-pronadene
								@param T Type unsigned char* Konkatenacija inputa
								@param SA Type vector<int> Polje SA
								@param LCP Type vector<int> Polje LCP
								@param N Type int Broj elemenata SA, LCP polja tj. broj znakova u konkateniranom nizu
								@param s_pos Type int Pozicija graničnika
								@param min_len Type int Kritična duljina od koje MUM mora biti duži da bismo ga prihvatili
								@param rev Type bool Oznaka da li se radi o reverznom komplementu
								@param from1 Type int Pozicija na kojoj pretrazujemo u prvom nizu
								@param to1 Type int Pozicija do koje pretrazujemo u prvom nizu
								@param from2 Type int Pozicija od koje pretrazujemo u drugom nizu
								@param to2 Type int Pozicija do koje pretrazujemo u drugom nizu
				*/
    void extractMumms(vector<Mum>& mumms_tmp, unsigned char* T, vector<int>& SA, vector<int>& LCP, int N, 
																						int s_pos, int min_len, bool rev, int from1, int to1, int from2, int to2) {
        //tmp_beg je graničnik, MUM duljine 0 kojeg stavljamo na početak
        Mum tmp_beg(from1, from2, 0);
        mumms_tmp.push_back(tmp_beg);
        int br = 0;

        //Obavljamo algoritam za pronalazak MUMova opisan u pdfu
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
								
								//Postavljamo MUM granicnik na kraj
								//Mumovi granicnici su potrebni tako da mozemo obuhvatiti netaknute regije koje su na samom pocetku ili kraju
        Mum tmp_end(to1, to2, 0);
        mumms_tmp.push_back(tmp_end);

								//Ako se radi o reverznom komplementu nemoj primjeniti LIS, samo sortiraj
        if (rev == false) {
            mumms_tmp = LIS(mumms_tmp);
            }  else {
            sort(mumms_tmp.begin(), mumms_tmp.end());
        }
    }
    
				/** GenerateMumms
								Za zadano polje SA i LCP te ostale parametre izgeneriraj MUMmove i to kroz zadani broj iteracija.								

								@param mumms Type vector<Mum> Lista u koju cemo pohraniti MUMove, (izvorna lista)
								@param T Type unsigned char* Konkatenacija nizova
								@param SA Type vector<int> SA polje 
								@param LCP Type vector<int> LCP polje
								@param N Type int Ukupan broj elemenata u LCP, SA polju tj. konkatenaciji nizova
								@param s_pos Type Pozicija granicnika
								@param rev Type bool Oznacava da li se radi o reverznom komplementu
								@param num_of_iter Type int Ukupan broj iteracija
								@param cur_iter Type int Trenutna iteracija
								@param from1 Type int Pozicija na kojoj pretrazujemo u prvom nizu
								@param to1 Type int Pozicija do koje pretrazujemo u prvom nizu
								@param from2 Type int Pozicija od koje pretrazujemo u drugom nizu
								@param to2 Type int Pozicija do koje pretrazujemo u drugom nizu
				*/
    void GenerateMumms(vector<Mum>& mumms, unsigned char* T, vector<int>& SA, vector<int>& LCP, int N, 
																							int s_pos, bool rev, int num_of_iter, int cur_iter, int from1, int to1, int from2, int to2) {
								//Postavi minimalnu duljinu na ono sto je navedeno u Parameters.h       
							 int min_len = min_len_fwd[cur_iter];
								//Ako se radi o reverznom komplementu tada postavi na minimalnu duljinu predvidenu za reverzni komplement
        if (rev) {
												min_len = min_len_rev[cur_iter];
								}
						
        vector<Mum> mumms_tmp;
	
							 //Pronadi MUMove i pohrani ih u mumms_tmp
        extractMumms(mumms_tmp, T, SA, LCP, N, s_pos, min_len, rev, from1, to1, from2, to2);

        int num_of_mumms = mumms_tmp.size();
								//Prebaci sve pronađene MUMove u izvornu listu MUMova
        if (num_of_mumms > 2) {
        				for (int i = 0; i < mumms_tmp.size(); ++i) {
        				    if (mumms_tmp[i].get_length() > 0) {
        				        mumms.push_back(mumms_tmp[i]);
        				    }
        				}
								}
	
								//Pronašli smo samo jedan MUM, izlazimo
        if (num_of_mumms < 4) {
            return;
        }
								//Premašen broj iteracija, izlazimo
        if (cur_iter  >= num_of_iter) {
            return;
        }


								//Za područja između MUMova određujemo netaknute regije
								//Za svaku regiju potrebni su lcp i sa lista te indeksi pocetka i kraja te regije
        vector< vector<int> > lcp_list;
        vector< vector<int> > sa_list;
        vector <int> f1, t1, f2, t2;
        int dist = min_width;
        
        for (int i = 1; i < num_of_mumms; ++i) {
            int from_first = mumms_tmp[i - 1].get_pos_first() + mumms_tmp[i - 1].get_length();
            int to_first = mumms_tmp[i].get_pos_first();
            int from_second = mumms_tmp[i - 1].get_pos_second() + mumms_tmp[i - 1].get_length();
            int to_second = mumms_tmp[i].get_pos_second();
            
												//Ukoliko su MUMovi preblizu ne isplati se pretrazivati
            if (rev == false && (abs(to_first - from_first) < dist || abs(to_second - from_second) < dist)) {
                continue;
            }
            if (rev == true && abs(to_first - from_first) < dist) {
                continue;
            }
										
												//Pronasli smo novu netaknutu regiju te dodajemo u listu listi lcp i sa polje
            CreateList(lcp_list, sa_list, from_first, to_first, from_second, to_second, s_pos);
            f1.push_back(from_first);
            f2.push_back(from_second);
            t1.push_back(to_first);
            t2.push_back(to_second);
        }
        
								//Nemamo nitijednu netaknutu regiju, izlazimo
        if (lcp_list.size() == 0) {
            return;
        }
        
								//Za svaku netaknutu regiju popunimo lcp i sa liste listi s elementima
        FillLists(T, SA, LCP, N, s_pos, lcp_list, sa_list, tree, f1, t1, f2, t2);
        
								//Za svaku netaknutu regiju ponovno generiramo MUMove na podrucju te regije
        for (int i = 0; i < lcp_list.size(); ++i) {
            GenerateMumms(mumms, T, sa_list[i], lcp_list[i], N, s_pos,
            rev, num_of_iter, cur_iter + 1, f1[i], t1[i], f2[i], t2[i]);
        }
    }
};



#endif /* _Solver_H */
