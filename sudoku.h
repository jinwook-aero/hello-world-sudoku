// sudoku.h
// Header file that contains classes of sudoku
//
// Authors      : Jinwook Lee, Sungwook Lee
// First version: May 24, 2020
// Last update  : May 24, 2020
//

#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <random>
#include <chrono>

#include <windows.h>

// Default size
constexpr int nRowDefault{ 9 };
constexpr int nColDefault{ nRowDefault };
constexpr int nBlkDefault{ nRowDefault };
constexpr int invalid{ 0 };
constexpr int minRange{ 1 };
constexpr int maxRange{ nRowDefault };

enum class elemType { input, solved, temporary, unknown };

// Sudoku class
template <typename T = int>
class Sudoku
{
public:
	Sudoku();
	Sudoku(T nRow,T nCol);
	~Sudoku() = default;
	Sudoku(const Sudoku<T> &);

	void Set();
	void Set(T inputArray [nRowDefault][nColDefault]);
	void Display();
	bool Solve();
	bool SolveDirect();

private:
	// Data structure
	T _nRow, _nCol, _nBlk;
	static T _nSolverCount;
	std::vector<std::vector<T>> _data2D; // Actual number data
	std::vector<std::vector<T>> _candiateCount2D; // Candidate 
	std::vector<std::vector<std::set<T>>> _candidateSet2D; // Candidate set
	std::vector<std::vector<elemType>> _type2D; // Date type

	std::vector<std::set<T>> _rowAvail; // Set of available numbers for the row
	std::vector<std::set<T>> _colAvail; // Set of available numbers for the col
	std::vector<std::set<T>> _blkAvail; // Set of available numbers for the block

	// Solver subroutine
	bool CandidateCount(bool includeInput); // Returns if any solution is found
	bool IsConsistent(); // Returns if current solution space is 
	bool IsSolved(); // Returns if current solution space is consistent
	void Guess(size_t iRow, size_t jCol, size_t nOrder); // nOrder'th guess on (iRow,jCol)
	void Remove(size_t iRow, size_t jCol, size_t nOrder); // Remove nOrder'th guess on (iRow,jCol)

	// Window console color text
	// REF(by Brandon): https://stackoverflow.com/questions/25559077/how-to-get-background-color-back-to-previous-color-after-use-of-std-handle
	void SetConsoleColour(WORD* Attributes, DWORD Colour)
	{
		CONSOLE_SCREEN_BUFFER_INFO Info;
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(hStdout, &Info);
		*Attributes = Info.wAttributes;
		SetConsoleTextAttribute(hStdout, Colour);
	}

	void ResetConsoleColour(WORD Attributes)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Attributes);
	}
};

template <typename T>
T Sudoku<T>::_nSolverCount = 0;

template <typename T>
Sudoku<T>::Sudoku():
	Sudoku{static_cast<T>(nRowDefault), static_cast<T>(nColDefault)}
{
}

template<typename T>
Sudoku<T>::Sudoku(T nRow, T nCol):
	_nRow{ nRow }, _nCol{ nCol }, _nBlk{ nRow }
{
	_data2D.resize(_nRow, std::vector<T>(_nCol, invalid));
	_candiateCount2D.resize(_nRow, std::vector<T>(_nCol, maxRange));
	_type2D.resize(_nRow, std::vector<elemType>(_nCol, elemType::unknown));
	_candidateSet2D.resize(_nRow, std::vector<std::set<T>>(_nCol));

	_rowAvail.resize(_nRow, std::set<T>{});
	_colAvail.resize(_nCol, std::set<T>{});
	_blkAvail.resize(_nBlk, std::set<T>{});
	for (size_t i = 0; i < _nRow; ++i) {
		for (size_t j = 0; j < _nCol; ++j) {
			_rowAvail[i].insert(j + 1);
			_colAvail[j].insert(i + 1);
		}
	}
	for (size_t k = 0; k < _nBlk; ++k) {
		for (size_t r = 0; r < maxRange; ++r) {
			_blkAvail[k].insert(r + 1);
		}
	}
}

template <typename T>
Sudoku<T>::Sudoku(const Sudoku<T> & rhs)
{
	_nRow = rhs._nRow;
	_nCol = rhs._nCol;
	_nBlk = rhs._nBlk;
	_nSolverCount = rhs._nSolverCount;

	_data2D = rhs._data2D;
	_type2D = rhs._type2D;
	_candiateCount2D = rhs._candiateCount2D;
	_candidateSet2D  = rhs._candidateSet2D;

	_rowAvail = rhs._rowAvail;
	_colAvail = rhs._colAvail;
	_blkAvail = rhs._blkAvail;
}

template <typename T>
void Sudoku<T>::Set()
{
	T tempInput;
	for (size_t i = 0; i < _nRow; ++i) {
		for (size_t j = 0; j < _nCol; ++j) {
			std::cout << "Enter Data [" << i << "][" << j << "]: \n";
			std::cin >> tempInput;
			if (tempInput == invalid)
				_type2D[i][j] = elemType::unknown;
			else
				_type2D[i][j] = elemType::input;
			_data2D[i][j] = tempInput;
		}
		std::cout << "\n";
	}
}

template <typename T>
void Sudoku<T>::Set(T inputArray[nRowDefault][nColDefault])
{
	for (size_t i = 0; i < _nRow; ++i) {
		for (size_t j = 0; j < _nCol; ++j) {
			if (inputArray[i][j] == invalid)
				_type2D[i][j] = elemType::unknown;
			else
				_type2D[i][j] = elemType::input;
			_data2D[i][j] = inputArray[i][j];
		}
	}
}

template <typename T>
void Sudoku<T>::Display()
{
	WORD Attributes = 0;
	std::cout << "\n -----------------------\n";
	for (size_t i = 0; i < _nRow; ++i) {
		std::cout << " | ";
		for (size_t j = 0; j < _nCol; ++j) {
			if (_type2D[i][j] == elemType::input)
				SetConsoleColour(&Attributes, FOREGROUND_INTENSITY | BACKGROUND_BLUE);
			else if (_type2D[i][j] == elemType::solved)
				SetConsoleColour(&Attributes, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			else // elemType::unknown or temporary
				SetConsoleColour(&Attributes, FOREGROUND_INTENSITY | FOREGROUND_RED);
			if ((_type2D[i][j] == elemType::input) || (_type2D[i][j] == elemType::solved))
				std::cout << _data2D[i][j];
			else
				std::cout << "?";
			ResetConsoleColour(Attributes);
			std::cout << " ";
			if (j % 3 == 2)
				std::cout << "|";
		}
		std::cout << "\n";
		if (i % 3 == 2)
			std::cout << " -----------------------\n";
	}
}

template <typename T>
bool Sudoku<T>::CandidateCount(bool includeInput)
{
	// Update available number list
	for (size_t i = 0; i < _nRow; ++i) {
		for (size_t j = 0; j < _nCol; ++j) {
			if (_type2D[i][j] == elemType::solved || (!includeInput && _type2D[i][j] == elemType::input))
				continue;
			const T curElem = _data2D[i][j];
			const size_t k = (i / 3) * 3 + (j / 3);
			_rowAvail[i].erase(curElem);
			_colAvail[j].erase(curElem);
			_blkAvail[k].erase(curElem);
		}
	}

	// Recount candidate
	bool anySolutionFound = false;
	for (size_t i = 0; i < _nRow; ++i) {
		for (size_t j = 0; j < _nCol; ++j) {
			if (_type2D[i][j] == elemType::solved || (!includeInput && _type2D[i][j] == elemType::input))
				continue;
			T tempCount = 0;
			T tempAvail{ invalid };
			const size_t k = (i / 3) * 3 + (j / 3);
			_candidateSet2D[i][j].clear();
			for (T r = 0; r < maxRange; ++r)
				if ((_rowAvail[i].find(r+1) != _rowAvail[i].end()) &&
					(_colAvail[j].find(r+1) != _colAvail[j].end()) &&
					(_blkAvail[k].find(r+1) != _blkAvail[k].end())){
					tempAvail = r+1;
					_candidateSet2D[i][j].insert(tempAvail);
					++tempCount;
				}
			_candiateCount2D[i][j] = tempCount;

			// Solution found if count is one
			if (tempCount == 1 && _type2D[i][j] != elemType::input && _type2D[i][j] != elemType::solved){
				_type2D[i][j] = elemType::solved;
				_data2D[i][j] = tempAvail;
				_rowAvail[i].erase(tempAvail);
				_colAvail[j].erase(tempAvail);
				_blkAvail[k].erase(tempAvail);
				anySolutionFound = true;
			}
		}
	}
	return anySolutionFound;
}


template <typename T>
bool Sudoku<T>::IsSolved()
{
	for (size_t i = 0; i < _nRow; ++i) {
		for (size_t j = 0; j < _nCol; ++j)
			if (_type2D[i][j] != elemType::solved && _type2D[i][j] != elemType::input)
				return false;
	}
	return true;
}

template <typename T>
bool Sudoku<T>::IsConsistent()
{
	// Row consistency
	std::set<T> tempSet;
	for (size_t i = 0; i < _nRow; ++i) {
		tempSet.clear();
		for (size_t j = 0; j < _nCol; ++j)
			if (_type2D[i][j] == elemType::solved || _type2D[i][j] == elemType::input)
				if (tempSet.insert(_data2D[i][j]).second == false)
					return false; // Duplicate detected
	}

	// Col consistency
	for (size_t j = 0; j < _nCol; ++j) {
		tempSet.clear();
		for (size_t i = 0; i < _nRow; ++i)
			if (_type2D[i][j] == elemType::solved || _type2D[i][j] == elemType::input)
				if (tempSet.insert(_data2D[i][j]).second == false)
					return false; // Duplicate detected
	}

	// Blk consistency
	for (size_t k = 0; k < _nBlk; ++k){
		tempSet.clear();
		for (size_t i = 0; i < (k / 3 + 1); ++i) {
			for (size_t j = (k % 3)*3; j < (k % 3 + 1)*3; ++j)
				if (_type2D[i][j] == elemType::solved || _type2D[i][j] == elemType::input)
					if (tempSet.insert(_data2D[i][j]).second == false)
						return false; // Duplicate detected
		}
	}

	// Passed all tests
	return true;
}

template <typename T>
void Sudoku<T>::Guess(size_t i, size_t j, size_t nOrder)
{
	// nOrder'th candidate
	auto iter = (_candidateSet2D[i][j]).begin();
	std::advance(iter, nOrder);

	// Guess nOrder'th candidate
	_data2D[i][j] = *iter;
	_type2D[i][j] = elemType::solved;

	_rowAvail[i].erase(*iter);
	_colAvail[j].erase(*iter);

	const size_t k = (i / 3) * 3 + (j / 3);
	_blkAvail[k].erase(*iter);
}

template <typename T>
void Sudoku<T>::Remove(size_t i, size_t j, size_t nOrder)
{
	// nOrder'th candidate
	auto iter = _candidateSet2D[i][j].begin();
	std::advance(iter, nOrder);

	// Remove nOrder'th candidate
	_candidateSet2D[i][j].erase(*iter);
	--_candiateCount2D[i][j];

	// If only one left, it's solution
	if (_candiateCount2D[i][j] == 1) {
		_data2D[i][j] = *iter;
		_type2D[i][j] = elemType::solved;

		_rowAvail[i].erase(*iter);
		_colAvail[j].erase(*iter);

		const size_t k = (i / 3) * 3 + (j / 3);
		_blkAvail[k].erase(*iter);
	}
}

template <typename T>
bool Sudoku<T>::Solve()
{
	// Count candidate with input
	if (_nSolverCount == 0)
		CandidateCount(true);
	++_nSolverCount;

	// Count candidate without input until there is no more update in solution
	while (CandidateCount(false)) {}

	// Status check
	if (!IsConsistent())
		return false;
	if (IsSolved())
		return true;

	if (_nSolverCount % 1 == 0) {
		std::cout << "\n\nSolution step: " << _nSolverCount << std::endl;
		Display();
	}

	// Sweep to invalidate
	for (size_t i = 0; i < _nRow; ++i) {
		for (size_t j = 0; j < _nCol; ++j) {
			for (size_t test = 0; test < _candiateCount2D[i][j]; ++test) {
				Sudoku<T> testGame = *this;
				testGame.Guess(i, j, test);
				testGame.CandidateCount(false);
				if (!testGame.IsConsistent()){
					this->Remove(i, j, test);
					std::cout << "candidate removed on [" << i << "][" << j << "]\n";
				}
			}
		}
	}

	// Identify min-candidate case for recursive call
	size_t nGuess{ maxRange };
	for (size_t i = 0; i < _nRow; ++i)
		for (size_t j = 0; j < _nCol; ++j)
			if (_type2D[i][j] != elemType::solved && _type2D[i][j] != elemType::input &&
				(_candiateCount2D[i][j] < nGuess))
				nGuess = _candiateCount2D[i][j];
	size_t iTarget = maxRange;
	size_t jTarget = maxRange;
	for (size_t i = 0; i < _nRow; ++i){
		for (size_t j = 0; j < _nCol; ++j)
			if (_type2D[i][j] != elemType::solved && _type2D[i][j] != elemType::input &&
				_candiateCount2D[i][j] == nGuess){
				iTarget = i;
				jTarget = j;
				break;
			}
		if (iTarget < maxRange && jTarget < maxRange)
			break;
	}
	if (!nGuess)
		return false;

	// Recursive call on nGuess
	for (size_t nOrder = 0; nOrder < nGuess; ++nOrder)
	{
		Sudoku<T> recurGame = *this;
		recurGame.Guess(iTarget, jTarget, nOrder);
		if (recurGame.Solve()) {
			*this = recurGame;
			return true;
		}
	}
}
