#include <vector>
#include <sstream>
#include <istream>
#include <ostream>

#include "BitBuffer.h"
#include "Matrix.h"
#include "Hamming_encoded_block.h"
#include "StreamEncoder.h"

#include "StdInputInterface.h"


StdInputInterface::StdInputInterface(): inputBuffer(), outputBuffer(), frw(inputBuffer, outputBuffer){}

string StdInputInterface::encode(const string &str){
	inputBuffer << str;
	frw.encode();
	inputBuffer.clear();
	string res;
	outputBuffer >> res;
	outputBuffer.clear();
	return res;
}

string StdInputInterface::decode(const string &str){
	inputBuffer << str;
	frw.decode();
	inputBuffer.clear();
	string res;
	outputBuffer >> res;
	outputBuffer.clear();
	return res;
}

