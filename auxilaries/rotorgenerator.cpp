/*
 *Random
*/
#include <stdlib.h>
#include <string>
#include <fstream>
#include <time.h>

using Byte = unsigned char;

Byte* order_256() {
  Byte* order = (Byte*)malloc(256);
	for(size_t i=0; i<256; ++i) {
		order[i] = i;
	}
	return order;
}

Byte* permutaion(Byte* array) {
	size_t a;
	srand(time(NULL));
	for(size_t i=0; i<256; ++i) {
		a = rand() % (256-i);
		std::swap(array[a], array[255-i]);
	}
	return array;
}

Byte* invert(const Byte* perm) {
	Byte* inv_perm = (Byte*)malloc(256);
	for(size_t i=0; i<256; ++i) {
		inv_perm[perm[i]] = i;
	}
	return inv_perm;
}

int main(int argc,char** argv) {
	char* filename = argv[1];

	Byte* perm = permutaion(order_256());
	Byte* inv_perm = invert(perm);

	std::string str_filename(filename);
	for(size_t i=0; i<str_filename.length(); ++i) {
		std::string name = std::string("rotor_") + str_filename[i] + std::string(".txt");
		FILE* myfile = fopen(name.c_str(), "wb");
		fwrite(perm,1,256,myfile);
		fclose(myfile);
		name = std::string("rotor_") + str_filename[i] + std::string("_reverse.txt");
		myfile = fopen(name.c_str(), "wb");
		fwrite(inv_perm,1,256,myfile);
		fclose(myfile);
	}

	free(perm);
	free(inv_perm);
	return 0;		
}
