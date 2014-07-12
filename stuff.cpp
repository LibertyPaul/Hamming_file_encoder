#include <ostream>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include "stuff.h"


bool isPowerOfTwo(uint64_t n){
	return (n > 0) && ((n & (n - 1)) == 0);
}

uint8_t getIncludedPowerOfTwo(uint64_t n){
	uint8_t res = 0;
	while(n){
		n >>= 1;
		++res;
	}
	return res;
}

ostream &operator<<(ostream &o, const vector<int64_t> &v){
	for(auto &n : v)
		o << n << " ";
	return o;
}


vector<int64_t> operator*(const vector<int64_t> &v1, const vector<int64_t> &v2){
	if(v1.size() != v2.size())
		throw logic_error("Vector size does not match");
	vector<int64_t> res(v1.size());

	for(uint16_t i = 0; i < res.size(); ++i)
		if(v1.at(i) && v2.at(i))
			res.at(i) = 1;
		else
			res.at(i) = 0;

	return res;
}

int64_t sum(const vector<int64_t> &v){
	int64_t res = 0;
	for(auto value : v)
		if(res == value)
			res = 0;
		else
			res = 1;

	return res;
}
