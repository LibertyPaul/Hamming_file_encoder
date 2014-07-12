#include <vector>
#include <stdexcept>
#include <ostream>
#include <sstream>
#include <algorithm>


#include "stuff.h"
#include "BitBuffer.h"
#include "Matrix.h"



Matrix::Matrix(){
}

Matrix::Matrix(uint16_t rowNumber, uint16_t colNumber){
	resize(rowNumber, colNumber);
}

Matrix::Matrix(const vector<vector<int64_t>> &matrix){
	for(auto i = matrix.begin(); i != matrix.end() - 1; ++i)
		if(i->size() != (i + 1)->size())
			throw logic_error("Row size does not match");
	this->matrix = matrix;
}


void Matrix::resize(uint16_t rowNumber, uint16_t colNumber){
	matrix.resize(rowNumber);
	for(auto &row : matrix)
		row.resize(colNumber);
}


void Matrix::insertRow(const vector<int64_t> &row){
	if(matrix.empty() == false)
		if(matrix.front().size() != row.size())
			throw logic_error("row size doesn't match");
	matrix.push_back(row);
}

vector<int64_t> Matrix::getRow(uint16_t rowNumber) const{
	return matrix.at(rowNumber);
}

void Matrix::replaceRow(uint16_t rowNumber, const vector<int64_t> &row){
	if(matrix.empty())
		throw runtime_error("Matrix is empty");

	if(matrix.size() <= rowNumber)
		throw logic_error("Row number is too high");

	if(rowNumber > 0 && matrix.at(0).size() != row.size())
		throw logic_error("row size doesn't match");

	if(rowNumber < matrix.size() - 1 && matrix.back().size() != row.size())
		throw logic_error("row size doesn't match");

	matrix.at(rowNumber) = row;
}

int64_t Matrix::getNumber(uint16_t row, uint16_t col) const{
	return matrix.at(row).at(col);
}


uint16_t Matrix::getRowCount() const{
	return matrix.size();
}


uint16_t Matrix::getColumnCount() const{
	if(matrix.empty())
		return 0;
	return matrix.front().size();
}


bool Matrix::isSquare() const{
	return matrix.size() == matrix.front().size();
}

void Matrix::transpose(){
	vector<vector<int64_t>> mCopy = matrix;
	matrix.clear();
	matrix.resize(mCopy.front().size());
	for(auto &row : matrix)
		row.resize(mCopy.size());

	for(uint16_t i = 0; i < mCopy.size(); ++i)
		for(uint16_t j = 0; j < mCopy.at(i).size(); ++j)
			matrix.at(j).at(i) = mCopy.at(i).at(j);
}

void Matrix::replaceCol(uint16_t colNumber, const vector<int64_t> &col){
	if(matrix.empty())
		throw runtime_error("Matrix is empty");
	if(matrix.size() != col.size())
		throw logic_error("col size does not match");

	for(uint16_t i = 0; i < matrix.size(); ++i)
		matrix.at(i).at(colNumber) = col.at(i);
}

vector<int64_t> Matrix::getCol(uint16_t colNumber) const{
	if(matrix.empty())
		throw runtime_error("Matrix is empty");
	if(colNumber >= matrix.front().size())
		throw runtime_error("colNumber is too high");

	vector<int64_t> col(matrix.size());
	for(uint16_t i = 0; i < matrix.size(); ++i)
		col.at(i) = matrix.at(i).at(colNumber);
	return col;
}


Matrix Matrix::getSubMatrix(uint16_t rowNumber, uint16_t colNumber) const{
	uint16_t rowCount = getRowCount();
	uint16_t colCount = getColumnCount();
	if(colNumber >= colCount)
		throw logic_error("colNumber is too high");
	if(rowNumber >= rowCount)
		throw logic_error("rowNumber is too high");

	Matrix newMatrix;

	for(uint16_t row = 0; row < rowCount; ++row){
		if(row != rowNumber){
			vector<int64_t> currentRow;
			currentRow.reserve(rowCount - 1);
			for(uint16_t col = 0; col < colCount; ++col)
				if(col != colNumber)
					currentRow.push_back(matrix.at(row).at(col));
			newMatrix.insertRow(currentRow);
		}
	}
	return newMatrix;
}

int32_t Matrix::findRow(const vector<int64_t> &row) const{
	auto it = find(matrix.begin(), matrix.end(), row);
	if(it != matrix.end())
		return it - matrix.begin();
	else
		return -1;
}



Matrix Matrix::operator*(const Matrix &m) const{
	if(this->getColumnCount() != m.getRowCount())
		throw logic_error("Incorrect matirix sizes");

	Matrix result;
	for(uint16_t rowNumber = 0; rowNumber < this->getRowCount(); ++rowNumber){
		vector<int64_t> row(m.getColumnCount());

		for(uint16_t colNumber = 0; colNumber < m.getColumnCount(); ++colNumber)
			row.at(colNumber) = sum(this->getRow(rowNumber) * m.getCol(colNumber));
		result.insertRow(row);
	}
	return result;
}




Matrix::operator BitBuffer() const{
	if(getRowCount() > 1)
		throw logic_error("Cant cast to BitBuffer more than 1 row");

	BitBuffer res;
	for(auto n : matrix.at(0))
		res.insertBit(n);

	return res;
}



ostream &operator<<(ostream &o, const Matrix &matrix){
	for(auto &row : matrix.matrix){
		for(auto &n : row)
			o << n << " ";
		o << endl;
	}
	return o;

}

vector<int64_t> operator*(const vector<int64_t> &v, const Matrix &m){
	Matrix t;
	t.insertRow(v);
	return (t * m).getRow(0);
}



