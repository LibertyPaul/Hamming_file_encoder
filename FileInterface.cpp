#include <fstream>
#include <vector>

#include "BitBuffer.h"
#include "Matrix.h"
#include "Hamming_encoded_block.h"
#include "StreamEncoder.h"

#include "FileInterface.h"


FileInterface::FileInterface(const string &inputFileName, const string &outputFileName):
src(inputFileName, ios_base::in | ios_base::binary), dst(outputFileName, ios_base::trunc | ios_base::out | ios_base::binary), frw(src, dst){}

FileInterface::~FileInterface(){
	src.close();
	dst.close();
}

void FileInterface::encode(){
	frw.encode();
}

void FileInterface::decode(){
	frw.decode();
}
