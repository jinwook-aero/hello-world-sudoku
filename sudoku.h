// sudoku.h
// Header file that contains classes of sudoku
//
// Authors      : Jinwook Lee, Sungwook Lee
// First version: May 24, 2020
// Last update  : May 25, 2020
//

#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <windows.h>

// Default size
constexpr int nSizeDefault{ 9 };
constexpr int nSizeBlkDefault{ nSizeDefault/3 };
constexpr int invalid{ 0 };
constexpr int minRange{ 1 };
constexpr int maxRange{ nSizeDefault };

enum class elemType { input, solved, unknown };

// Sudoku class
template <typename T = int>
class Sudoku
{
public:
	Sudoku();
	Sudoku(T nSize);
	~Sudoku() = default;
	Sudoku(const Sudoku<T> &);

	void Set();
	void Set(std::vector<std::vector<T>> inputVec2D);
	void Display();
	bool Solve(bool isQuietMode = true);

private:
	// Data structure
	T _nSize;
	static T _solverCallCount;
	std::vector<std::vector<T>> _data2D; // Actual number data
	std::vector<std::vector<elemType>> _type2D; // Date type
	std::vector<std::vector<T>> _candiateCount2D; // Candidate 
	std::vector<std::vector<std::set<T>>> _candidateSet2D; // Candidate set

	std::vector<std::set<T>> _rowAvail; // Available list 
	std::vector<std::set<T>> _colAvail; // Available list
	std::vector<std::set<T>> _blkAvail; // Available list

	// Solver subroutine
	void InitializeAvailability(); // Initialize availability list
	void InitializeCandidate(); // Initialize candidate list
	T CountTotalCandidate(); // Returns sum of candidate count
	void ReduceCandidate();
	void GuessCandidate(size_t iRow, size_t jCol, size_t nOrder); // Select nOrder'th candidate on (iRow,jCol)
	void RemoveCandidate(size_t iRow, size_t jCol, size_t nOrder); // Remove nOrder'th candiate from (iRow,jCol)
	void Convert2Solution(); // Convert single candidate to solution

	bool IsConsistent(); // Returns if current solution space is 
	bool IsSolved(); // Returns if current solution space is consistent

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
T Sudoku<T>::_solverCallCount = 0;

template <typename T>
Sudoku<T>::Sudoku():
	Sudoku{static_cast<T>(nSizeDefault)}
{
}

template<typename T>
Sudoku<T>::Sudoku(T nSize):
	_nSize{ nSize }
{
	_data2D.resize(_nSize, std::vector<T>(_nSize, invalid));
	_type2D.resize(_nSize, std::vector<elemType>(_nSize, elemType::unknown));
	_candiateCount2D.resize(_nSize, std::vector<T>(_nSize));
	_candidateSet2D.resize(_nSize, std::vector<std::set<T>>(_nSize));

	_rowAvail.resize(_nSize, std::set<T>{});
	_colAvail.resize(_nSize, std::set<T>{});
	_blkAvail.resize(_nSize, std::set<T>{});
}

template <typename T>
Sudoku<T>::Sudoku(const Sudoku<T> & rhs)
{
	_nSize = rhs._nSize;

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
	for (size_t i = 0; i < _nSize; ++i) {
		for (size_t j = 0; j < _nSize; ++j) {
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
void Sudoku<T>::Set(std::vector<std::vector<T>> inputVec2D)
{
	for (size_t i = 0; i < _nSize; ++i) {
		for (size_t j = 0; j < _nSize; ++j) {
			if (inputVec2D[i][j] == invalid)
				_type2D[i][j] = elemType::unknown;
			else
				_type2D[i][j] = elemType::input;
			_data2D[i][j] = inputVec2D[i][j];
		}
	}
}

template <typename T>
void Sudoku<T>::Display()
{
	auto drawLine = [](){ std::cout << " -----------------------\n"; };

	WORD Attributes = 0;
	drawLine();
	for (size_t i = 0; i < _nSize; ++i) {
		std::cout << " | ";
		for (size_t j = 0; j < _nSize; ++j) {
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
			drawLine();
	}
}

template <typename T>
void Sudoku<T>::InitializeAvailability()
{
	// Initialize availability list (duplicate will be ignored)
	for (size_t i = 0; i < _nSize; ++i) {
		for (size_t j = 0; j < _nSize; ++j) {
			_rowAvail[i].insert(j + 1);
			_colAvail[j].insert(i + 1);
		}
	}
	for (size_t k = 0; k < _nSize; ++k) {
		for (size_t r = 0; r < maxRange; ++r) {
			_blkAvail[k].insert(r + 1);
		}
	}

	// Reduce available numbers if encountered as input or solved
	for (size_t i = 0; i < _nSize; ++i) {
		for (size_t j = 0; j < _nSize; ++j) {
			if (_type2D[i][j] != elemType::unknown){
				const T curElem = _data2D[i][j];
				const size_t k = (i / nSizeBlkDefault) * nSizeBlkDefault + (j / nSizeBlkDefault);
				_rowAvail[i].erase(curElem);
				_colAvail[j].erase(curElem);
				_blkAvail[k].erase(curElem);
			}
		}
	}
}


template <typename T>
void Sudoku<T>::InitializeCandidate()
{
	// Initialize candidate list
	for (size_t i = 0; i < _nSize; ++i) {
		for (size_t j = 0; j < _nSize; ++j) {
			if (_type2D[i][j] == elemType::unknown) {
				T tempCount = 0;
				T tempAvail{ invalid };
				const size_t k = (i / nSizeBlkDefault) * nSizeBlkDefault + (j / nSizeBlkDefault);
				_candidateSet2D[i][j].clear();
				for (T r = 0; r < maxRange; ++r)
					if ((_rowAvail[i].find(r + 1) != _rowAvail[i].end()) &&
						(_colAvail[j].find(r + 1) != _colAvail[j].end()) &&
						(_blkAvail[k].find(r + 1) != _blkAvail[k].end())) {
						tempAvail = r + 1;
						_candidateSet2D[i][j].insert(tempAvail);
						++tempCount;
					}
				_candiateCount2D[i][j] = tempCount;
			}
		}
	}
}

template <typename T>
T Sudoku<T>::CountTotalCandidate() {
	T tempSum = 0;
	for (size_t i = 0; i < _nSize; ++i)
		for (size_t j = 0; j < _nSize; ++j)
			if (_type2D[i][j] == elemType::unknown)
				tempSum += _candiateCount2D[i][j];
	return tempSum;
}


template <typename T>
void Sudoku<T>::Convert2Solution()
{
	// Single candidate is solution
	for (size_t i = 0; i < _nSize; ++i)
		for (size_t j = 0; j < _nSize; ++j)
			if (_type2D[i][j] == elemType::unknown && _candiateCount2D[i][j] == 1) {
				_data2D[i][j] = *(_candidateSet2D[i][j].begin());
				_type2D[i][j] = elemType::solved;
			}
}

template <typename T>
void Sudoku<T>::ReduceCandidate()
{
	// Reduce candidate space
	bool isAnythingChanged = false;
	for (size_t i = 0; i < _nSize; ++i)
		for (size_t j = 0; j < _nSize; ++j) {
			if (_type2D[i][j] != elemType::unknown)
				continue;
			for (size_t test = 0; test < _candiateCount2D[i][j]; ++test) {
				Sudoku<T> testGame = *this;
				testGame.GuessCandidate(i, j, test);
				if (!testGame.IsConsistent()) {
					this->RemoveCandidate(i, j, test);
					isAnythingChanged = true;
				}
			}
		}
	if(isAnythingChanged) {
		Convert2Solution();
		InitializeAvailability(); // Reinitialize _rowAvail, _colAvail, _blkAvail
	}

	// Row insufficiency - Search for unique candidate that exists only in one cell along the row
	isAnythingChanged = false;
	for (size_t i = 0; i < _nSize; ++i)
		for (size_t avail : _rowAvail[i]){
			size_t tempCount = 0;
			for (size_t j = 0; j < _nSize; ++j)
				if (_candidateSet2D[i][j].count(avail) > 0)
					++tempCount;
			if (tempCount == 1) {// Unique candidate ==> Solution
				for (size_t j = 0; j < _nSize; ++j)
					if (_candidateSet2D[i][j].count(avail) > 0) {
						_data2D[i][j] = avail;
						_type2D[i][j] = elemType::solved;
						isAnythingChanged = true;
						break;
					}
			}
		}
	if (isAnythingChanged) 
		InitializeAvailability(); // Reinitialize _rowAvail, _colAvail, _blkAvail

	// Col insufficiency - Search for unique candidate that exists only in one cell along the col
	isAnythingChanged = false;
	for (size_t j = 0; j < _nSize; ++j)
		for (size_t avail : _colAvail[j]) {
			size_t tempCount = 0;
			for (size_t i = 0; i < _nSize; ++i)
				if (_type2D[i][j] == elemType::unknown && _candidateSet2D[i][j].count(avail) > 0)
					++tempCount;
			if (tempCount == 1) {// Unique candidate
				for (size_t i = 0; i < _nSize; ++i)
					if (_type2D[i][j] == elemType::unknown && _candidateSet2D[i][j].count(avail) > 0) {
						_data2D[i][j] = avail;
						_type2D[i][j] = elemType::solved;
						isAnythingChanged = true;
						break;
					}
			}
		}
	if (isAnythingChanged)
		InitializeAvailability(); // Reinitialize _rowAvail, _colAvail, _blkAvail

	// Blk insufficiency - Search for unique candidate that exists only in one cell in the block
	isAnythingChanged = false;
	for (size_t k = 0; k < _nSize; ++k) 
		for (size_t avail : _blkAvail[k]) {
			size_t tempCount = 0;
			for (size_t i = (k / 3) * 3; i < (k / 3 + 1) * 3; ++i)
				for (size_t j = (k % 3) * 3; j < (k % 3 + 1) * 3; ++j)
					if (_type2D[i][j] == elemType::unknown && _candidateSet2D[i][j].count(avail) > 0)
						++tempCount;
			if (tempCount == 1) {// Unique candidate
				for (size_t i = (k / 3) * 3; i < (k / 3 + 1) * 3; ++i)
					for (size_t j = (k % 3) * 3; j < (k % 3 + 1) * 3; ++j)
						if (_type2D[i][j] == elemType::unknown && _candidateSet2D[i][j].count(avail) > 0) {
							_data2D[i][j] = avail;
							_type2D[i][j] = elemType::solved;
							isAnythingChanged = true;
							break;
						}
			}
		}
	if (isAnythingChanged)
		InitializeAvailability(); // Reinitialize _rowAvail, _colAvail, _blkAvail
}

template <typename T>
bool Sudoku<T>::IsSolved()
{
	for (size_t i = 0; i < _nSize; ++i) {
		for (size_t j = 0; j < _nSize; ++j)
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
	for (size_t i = 0; i < _nSize; ++i) {
		tempSet.clear();
		for (size_t j = 0; j < _nSize; ++j)
			if (_type2D[i][j] == elemType::solved || _type2D[i][j] == elemType::input)
				if (tempSet.insert(_data2D[i][j]).second == false)
					return false; // Duplicate detected
	}

	// Col consistency
	for (size_t j = 0; j < _nSize; ++j) {
		tempSet.clear();
		for (size_t i = 0; i < _nSize; ++i)
			if (_type2D[i][j] == elemType::solved || _type2D[i][j] == elemType::input)
				if (tempSet.insert(_data2D[i][j]).second == false)
					return false; // Duplicate detected
	}

	// Blk consistency
	for (size_t k = 0; k < _nSize; ++k){
		tempSet.clear();
		for (size_t i = (k / 3)*3; i < (k / 3 + 1)*3; ++i) {
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
void Sudoku<T>::GuessCandidate(size_t i, size_t j, size_t nOrder)
{
	// nOrder'th candidate
	auto iter = (_candidateSet2D[i][j]).begin();
	std::advance(iter, nOrder);

	// Select nOrder'th candidate
	_data2D[i][j] = *iter;
	_type2D[i][j] = elemType::solved;
	_candiateCount2D[i][j] = 0;
	_candidateSet2D[i][j].clear();
}

template <typename T>
void Sudoku<T>::RemoveCandidate(size_t i, size_t j, size_t nOrder)
{
	// nOrder'th candidate
	auto iter = _candidateSet2D[i][j].begin();
	std::advance(iter, nOrder);

	// Remove nOrder'th candidate
	_candidateSet2D[i][j].erase(*iter);
	--_candiateCount2D[i][j];
}

template <typename T>
bool Sudoku<T>::Solve(bool isQuietMode)
{
	// Count candidate with input
	if (_solverCallCount == 0){
		InitializeAvailability();
		InitializeCandidate();
	}
	++_solverCallCount;
	if (!isQuietMode)
		std::cout << "Iteration: " << _solverCallCount << std::endl;

	// Reduce candidate until there is no more update in candidate list
	T previousCount = CountTotalCandidate();
	T newCount;
	while (true) {
		ReduceCandidate();
		newCount = CountTotalCandidate();
		if (previousCount > newCount) {
			previousCount = newCount;
		}
		else
			break;
	}

	// Status check
	if (!IsConsistent())
		return false;
	if (IsSolved()){
		if (!isQuietMode)
			std::cout << "Solution found at iteration: " << _solverCallCount << std::endl;
		return true;
	}

	// Identify min-candidate case for recursive call
	size_t nGuess{ maxRange };
	for (size_t i = 0; i < _nSize; ++i)
		for (size_t j = 0; j < _nSize; ++j)
			if (_type2D[i][j] == elemType::unknown && _candiateCount2D[i][j] < nGuess )
				nGuess = _candiateCount2D[i][j];
	size_t iTarget = maxRange;
	size_t jTarget = maxRange;
	for (size_t i = 0; i < _nSize; ++i){
		for (size_t j = 0; j < _nSize; ++j)
			if (_type2D[i][j] == elemType::unknown && _candiateCount2D[i][j] == nGuess) {
				iTarget = i;
				jTarget = j;
				break;
			}
		if (iTarget < maxRange && jTarget < maxRange)
			break;
	}

	// Recursive call on nGuess
	for (size_t nOrder = 0; nOrder < nGuess; ++nOrder)
	{
		Sudoku<T> recurGame = *this;
		recurGame.GuessCandidate(iTarget, jTarget, nOrder);
		if (recurGame.Solve(isQuietMode)) {
			*this = recurGame;
			return true;
		}
	}
	return false; // Solution not found in current guess
}
