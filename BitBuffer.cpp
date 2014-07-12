#include <vector>
#include <cstdint>
#include <stdexcept>
#include <istream>
#include <ostream>


#include "BitBuffer.h"


BitBuffer::BitBuffer(){
	readPos = writePos = 0;
}

BitBuffer::BitBuffer(uint64_t size): data(size / 8 + ((size % 8) ? 1 : 0), 0), writePos(size), readPos(0){}

BitBuffer::BitBuffer(const vector<uint8_t> &data, uint64_t size_in_bits){
	readPos = 0;
	this->data = data;
	writePos = size_in_bits;
	if(writePos / 8 >= data.size())
		this->data.push_back(0);
}



bool BitBuffer::readBit(uint64_t pos) const{
	if(pos >= writePos)
		throw logic_error("Out of range");
	uint8_t byte = data.at(pos / 8);
	uint8_t mask = 0x01 << (pos % 8);
	return byte & mask;
}

void BitBuffer::setBit(uint64_t pos, bool val){
	if(pos > writePos)
		throw logic_error("Out of range");
	if(val)
		data.at(pos / 8) |= (0x01 << (pos % 8));
	else
		data.at(pos / 8) &= ~(0x01 << (pos % 8));
}

void BitBuffer::resetReadPointer(){
	readPos = 0;
}


bool BitBuffer::getNextBit(){
	return readBit(readPos++);
}

void BitBuffer::insertBit(bool bit){
	if(writePos / 8 >= data.size())
		data.push_back(0);
	setBit(writePos, bit);
	++writePos;
}

void BitBuffer::insertByte(uint8_t n){
	for(int8_t i = 7; i >= 0; --i)
		insertBit(n & (0x01 << i));
}

uint8_t BitBuffer::readByte(){
	if(getAvailableLength() < 8)
		throw runtime_error("Available buffer size is less than 1 byte");
	uint8_t byte = 0;
	for(uint8_t i = 0; i < 8; ++i){
		uint8_t t = getNextBit() ? 1 : 0;
		byte |= t << (7 - i);
	}

	return byte;
}


uint64_t BitBuffer::getAvailableLength() const{
	return writePos - readPos;
}



void BitBuffer::insertFromLongInt(long n, uint8_t length){
	if(length > sizeof(long) * 8)
		throw runtime_error("length more than size of long int");

	for(uint8_t i = 0; i < length; ++i)
		insertBit(n & (0x01 << i));
}




uint64_t BitBuffer::getLength() const{
	return writePos;
}


void BitBuffer::cutReadedData(){
	BitBuffer bb = this->getBuffer(getAvailableLength());
	this->writePos -= this->readPos - bb.getLength();
	this->readPos = 0;
	this->data = bb.getData();
}

BitBuffer BitBuffer::operator=(const BitBuffer &bb){
	this->data = bb.data;
	this->writePos = bb.writePos;
	this->readPos = bb.readPos;

	return *this;
}



bool BitBuffer::operator==(const BitBuffer &bb){
	if(this->getLength() != bb.getLength())
		return false;

	for(uint64_t i = 0; i < getLength(); ++i)
		if(this->readBit(i) != bb.readBit(i))
			return false;

	return true;
}

bool BitBuffer::operator!=(const BitBuffer &bb){
	return !(*this == bb);
}


vector<uint8_t> BitBuffer::getData(){
	return data;
}

vector<uint8_t> BitBuffer::getUncompressedData() const{
	uint16_t length = getLength();
	vector<uint8_t> res(length);
	for(uint64_t i = 0; i < length; ++i)
		res.at(i) = readBit(i) ? 1 : 0;
	return res;
}

BitBuffer BitBuffer::getBuffer(uint64_t length, uint64_t pos) const{
	BitBuffer res;
	for(uint64_t i = 0; i < length; ++i)
		res.insertBit(this->readBit(pos + i));
	return res;
}

BitBuffer BitBuffer::getBuffer(uint64_t length){
	if(getAvailableLength() < length)
		throw runtime_error("Available buffer is too small");

	BitBuffer res;
	for(uint64_t i = 0; i < length; ++i)
		res.insertBit(this->getNextBit());

	return res;
}



void BitBuffer::insertBuffer(const BitBuffer &bb){
	for(uint64_t i = 0; i < bb.getLength(); ++i)
		insertBit(bb.readBit(i));
}

BitBuffer::operator long int() const{
	if(this->getLength() > sizeof(long) * 8)
		throw runtime_error("Buffer is too lagre to cast to long int");
	long res = 0;
	for(uint64_t i = 0; i < getLength(); ++i){
		long currentBit = readBit(i) ? 1 : 0;
		res <<= 1;
		res |= currentBit;
	}

	return res;
}


string BitBuffer::getVisibleStr() const{
	string res;
	for(uint64_t i = 0; i < writePos; ++i)
		res.push_back(readBit(i) ? '1' : '0');
	return res;
}

/*
ostream &operator<<(ostream &o, const BitBuffer &bb){
	o.write(reinterpret_cast<const char *>(&BitBuffer_magic), sizeof(BitBuffer_magic));
	uint64_t length = bb.getLength();
	o.write(reinterpret_cast<const char *>(&length), sizeof(length));

	for(uint64_t i = 0; i < bb.data.size(); ++i){
		o.write(reinterpret_cast<const char *>(&bb.data.at(i)), sizeof(bb.data.at(i)));
	}
	return o;
}


istream &operator>>(istream &i, BitBuffer &bb){
	uint32_t magic;
	i.read(reinterpret_cast<char *>(&magic), sizeof(magic));
	if(magic != BitBuffer_magic)
		throw runtime_error("Magic does not match");

	i.read(reinterpret_cast<char *>(&bb.writePos), sizeof(bb.writePos));

	for(uint64_t j = 0; j < bb.writePos / 8 + (bb.writePos % 8 ? 1 : 0); ++j){
		bb.data.push_back(0);
		i.read(reinterpret_cast<char *>(&bb.data.back()), sizeof(bb.data.back()));
	}
	return i;
}
*/

ostream &operator<<(ostream &o, const BitBuffer &bb){
	for(uint64_t i = 0; i < bb.getLength(); ++i){
		o << (bb.readBit(i) ? 1 : 0);
	}
	return o;
}




