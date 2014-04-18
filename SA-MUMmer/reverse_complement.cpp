#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;


string RC(char *f1) {
	FILE* F1 = fopen(f1, "r");
	char c;
	string output = "";
	do {
		c = fgetc(F1);	
		switch (c) {
			case 'A': output += 'T';
					  break;
			case 'C': output += 'G';
					  break;
			case 'T': output += 'A';
					  break;
			case 'G': output += 'C';
					  break;
		}
	} while ( c != EOF);
	output += "\0";

	for (int i = 0; i < output.size() / 2; ++i) {
		char tmp = output[i];
		output[i] = output[output.size() - 1 - i];
		output[output.size() - 1 - i] = tmp;
	}
	fclose(F1);
	return output;
}
/*
int main(int argc, char* argv[]) {

	
	string s1 = read_input(argv[1]);
	
	cout << s1 << endl;
	
    return 0;
}
*/
