#include <iostream>
#include <cstdint>
#include <vector>
#include <fstream>
#include <sstream>
#include "BitBuffer.h"
#include "Matrix.h"
#include "Hamming_encoded_block.h"
#include "StreamEncoder.h"

#include "FileInterface.h"
#include "StdInputInterface.h"

using namespace std;

void show_syntax(const string &progName){
	cout << "Usage: " << progName << " file|terminal [<src_filename> <dst_filename> encode|decode]" << endl;
	cout << "Example 1: " << progName << " file input.txt output.txt decode" << endl;
	cout << "Example 2: " << progName << " terminal" << endl;
}


int main(int argc, char **argv){
	if(argc < 2){
		show_syntax(argv[0]);
		return 0;
	}
	string mode(argv[1]);
	if(mode == "file"){
		if(argc < 5){
			show_syntax(argv[0]);
			return 0;
		}
		string srcFile(argv[2]);
		string dstFile(argv[3]);
		string action(argv[4]);

		FileInterface fi(srcFile, dstFile);
		if(action == "encode")
			fi.encode();
		else if(action == "decode")
			fi.decode();
		else{
			show_syntax(argv[0]);
			return 0;
		}
	}
	else if(mode == "terminal"){
		cout << "Input some text: ";
		char buf[256];
		cin.get(buf, 256);
		string text(buf);


		StdInputInterface sio1;
		string encoded = sio1.encode(text);
		StdInputInterface sio2;
		string decoded = sio2.decode(encoded);

		cout << "Encoded text: " << encoded << endl;
		cout << "Decoded text: " << decoded << endl;
	}
	else{
		show_syntax(argv[0]);
		return 0;
	}
}
