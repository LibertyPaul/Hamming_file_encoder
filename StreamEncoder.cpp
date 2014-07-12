#include <string>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "BitBuffer.h"
#include "Matrix.h"
#include "Hamming_encoded_block.h"
#include "StreamEncoder.h"


#include <iostream>

StreamEncoder::StreamEncoder(istream &src, ostream &dst): src(src), dst(dst){
	if(this->src.fail())
		throw runtime_error("Source file is not available");

	if(this->dst.fail())
		throw runtime_error("Destination file is not available");
}


StreamEncoder::~StreamEncoder(){
}

void StreamEncoder::replenishInputBuffer(){
	uint8_t byte;
	if(src.eof())
		throw runtime_error("Unexpected stream end");
	src.read(reinterpret_cast<char *>(&byte), sizeof(byte));
	inputBuffer.insertByte(byte);
}


BitBuffer StreamEncoder::readBits(uint64_t length){
	while(length > inputBuffer.getAvailableLength())
		replenishInputBuffer();
	return inputBuffer.getBuffer(length);
}

void StreamEncoder::writeBits(const BitBuffer &bb){
	outputBuffer.insertBuffer(bb);
	while(outputBuffer.getAvailableLength() >= 8){
		uint8_t byte = outputBuffer.readByte();
		dst.write(reinterpret_cast<const char *>(&byte), sizeof(byte));
	}
	outputBuffer.cutReadedData();
}

void StreamEncoder::flushOutput(){
	while(outputBuffer.getAvailableLength() >= 8){
		uint8_t byte = outputBuffer.readByte();
		dst.write(reinterpret_cast<const char *>(&byte), sizeof(byte));
	}
	if(outputBuffer.getAvailableLength() > 0){
		uint8_t byte = 0;//записываем часть последнего байта
		for(uint8_t i = 0; i < outputBuffer.getLength(); ++i)
			byte |= outputBuffer.getNextBit() ? 1 << (7 - i) : 0;
		dst.write(reinterpret_cast<const char *>(&byte), sizeof(byte));
	}
	outputBuffer.cutReadedData();
}



void StreamEncoder::encode(){
	src.seekg(0, src.end);
	fileSize = src.tellg();
	src.seekg(0, src.beg);

	dst.write(reinterpret_cast<const char *>(&StreamEncoder_magic), sizeof(StreamEncoder_magic));
	uint64_t rawSize_inBits = fileSize * 8;
	dst.write(reinterpret_cast<const char *>(&rawSize_inBits), sizeof(rawSize_inBits));
	uint64_t wordCount = rawSize_inBits / wordSize + (rawSize_inBits % wordSize ? 1 : 0);
	uint64_t encodedSize_inBits = wordCount * encodedWordSize;
	dst.write(reinterpret_cast<const char *>(&encodedSize_inBits), sizeof(encodedSize_inBits));




	for(uint32_t i = 0; i < wordCount; ++i){
		BitBuffer infoWord;
		if(wordSize < fileSize * 8 - (i * wordSize))
			infoWord = readBits(wordSize);
		else{
			for(uint64_t i = fileSize * 8 - (i * wordSize); i <= wordSize; ++i)//<=???????????
				infoWord.insertBit(false);

			infoWord = readBits(fileSize * 8 - (i * wordSize));
		}


		BitBuffer encodedWord = coder.encode(infoWord);
		writeBits(encodedWord);
	}
	flushOutput();
}

void StreamEncoder::decode(){
	src.seekg(0, src.end);
	fileSize = src.tellg();
	src.seekg(0, src.beg);


	uint32_t magic;
	src.read(reinterpret_cast<char *>(&magic), sizeof(magic));
	if(magic != StreamEncoder_magic)
		throw runtime_error("StreamEncoder magic is incorrect");

	uint64_t rawSize_inBits;
	src.read(reinterpret_cast<char *>(&rawSize_inBits), sizeof(rawSize_inBits));

	uint64_t encodedSize_inBits;
	src.read(reinterpret_cast<char *>(&encodedSize_inBits), sizeof(encodedSize_inBits));
	if(encodedSize_inBits % encodedWordSize)
		throw runtime_error("file size isn't correct");

	uint32_t blockCount = encodedSize_inBits / encodedWordSize;

	for(uint32_t i = 0; i < blockCount; ++i){
		uint64_t currentRawSize = min(rawSize_inBits - i * wordSize, wordSize);
		BitBuffer encodedWord = readBits(encodedWordSize);
		BitBuffer infoWord = coder.decode(encodedWord);


		writeBits(infoWord.getBuffer(currentRawSize));
	}
	flushOutput();
}










