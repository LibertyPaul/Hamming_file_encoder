#include <vector>
#include <cstdint>
#include <stdexcept>
#include <cmath>
#include <istream>
#include <ostream>

#include "stuff.h"
#include "BitBuffer.h"
#include "Matrix.h"

#include "Hamming_encoded_block.h"


#include <iostream>

Hamming_encoded_block::Hamming_encoded_block(){
	G = {{
			{1, 0, 0, 0, 1, 0, 1},
			{0, 1, 0, 0, 1, 1, 1},
			{0, 0, 1, 0, 1, 1, 0},
			{0, 0, 0, 1, 0, 1, 1}
	}};

	H = {{
		{1, 0, 1},
		{1, 1, 1},
		{1, 1, 0},
		{0, 1, 1},
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1}
	}};
}

BitBuffer Hamming_encoded_block::encode(const BitBuffer &data){
	vector<int64_t> infoWord = cast<uint8_t, int64_t>(data.getUncompressedData());
	vector<int64_t> codeWord = infoWord * G;


	BitBuffer result;

	for(auto bit : codeWord)
		result.insertBit(bit);

	return result;
}

BitBuffer Hamming_encoded_block::decode(const BitBuffer &data){

	vector<int64_t> codeWord = cast<uint8_t, int64_t>(data.getUncompressedData());
	vector<int64_t> syndrome = codeWord * H;

	Matrix syndroms = {{
		{1, 0, 1},
		{1, 1, 1},
		{1, 1, 0},
		{0, 1, 1},
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1}
	}};
	errorPos = syndroms.findRow(syndrome);

	if(errorPos >= 0 && errorPos <= 3)
		codeWord.at(errorPos) = (codeWord.at(errorPos) == 1) ? 0 : 1;

	codeWord.resize(infoWordSize);//обрезаем энтропию
	BitBuffer result;
	for(auto bit : codeWord)
		result.insertBit(bit);

	return result;
}


int64_t Hamming_encoded_block::getLastError(){
	uint64_t res = errorPos;
	errorPos = 0;
	return res;
}










