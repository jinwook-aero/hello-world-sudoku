// main.cpp 
// Main file for testing hello_math
//
// Authors      : Jinwook Lee, Sungwook Lee
// First version: May 24, 2020
// Last update  : May 24, 2020
//

#pragma once
#include <iostream>
#include <chrono> 
#include "sudoku.h"

#define DIFFICULTY 3 // 0 1 2 3

int main()
{	

	// Game initialization
	Sudoku<size_t> newGame;
	// Sudoku examples: https://dingo.sbs.arizona.edu/~sandiway/sudoku/examples.html
#if DIFFICULTY==0
	size_t input2game[nSizeDefault][nSizeDefault] = {
		{0,0,0,  2,6,0,  7,0,1}, // 1
		{6,8,0,  0,7,0,  0,9,0}, // 2
		{1,9,0,  0,0,4,  5,0,0}, // 3

		{8,2,0,  1,0,0,  0,4,0}, // 4
		{0,0,4,  6,0,2,  9,0,0}, // 5
		{0,5,0,  0,0,3,  0,2,8}, // 6

		{0,0,9,  3,0,0,  0,7,4}, // 7
		{0,4,0,  0,5,0,  0,3,6}, // 8
		{7,0,3,  0,1,8,  0,0,0}  // 9
	};
#elif DIFFICULTY==1
	size_t input2game[nSizeDefault][nSizeDefault] = {
		{0,2,0,  6,0,8,  0,0,0}, // 1
		{5,8,0,  0,0,9,  7,0,0}, // 2
		{0,0,0,  0,4,0,  0,0,0}, // 3

		{3,7,0,  0,0,0,  5,0,0}, // 4
		{6,0,0,  0,0,0,  0,0,4}, // 5
		{0,0,8,  0,0,0,  0,1,3}, // 6

		{0,0,0,  0,2,0,  0,0,0}, // 7
		{0,0,9,  8,0,0,  0,3,6}, // 8
		{0,0,0,  3,0,6,  0,9,0}  // 9
	};
#elif DIFFICULTY==2
	size_t input2game[nSizeDefault][nSizeDefault] = {
		{0,0,0,  6,0,0,  4,0,0}, // 1
		{7,0,0,  0,0,3,  6,0,0}, // 2
		{0,0,0,  0,9,1,  0,8,0}, // 3

		{0,0,0,  0,0,0,  0,0,0}, // 4
		{0,5,0,  1,8,0,  0,0,3}, // 5
		{0,0,0,  3,0,6,  0,4,5}, // 6

		{0,4,0,  2,0,0,  0,6,0}, // 7
		{9,0,3,  0,0,0,  0,0,0}, // 8
		{0,2,0,  0,0,0,  1,0,0}  // 9
	};
#elif DIFFICULTY==3
	/*size_t input2game[nSizeDefault][nSizeDefault] = {
		{0,2,0,  0,0,0,  0,0,0}, // 1
		{0,0,0,  6,0,0,  0,0,3}, // 2
		{0,7,4,  0,8,0,  0,0,0}, // 3

		{0,0,0,  0,0,3,  0,0,2}, // 4
		{0,8,0,  0,4,0,  5,1,0}, // 5
		{6,0,0,  5,0,8,  3,9,0}, // 6

		{0,0,0,  0,1,0,  7,8,5}, // 7
		{5,0,0,  0,0,9,  0,3,1}, // 8
		{0,0,0,  0,0,0,  0,4,0}  // 9
	};*/
	size_t input2game[nSizeDefault][nSizeDefault] = {
		{0,2,0,  0,0,0,  0,0,0}, // 1
		{0,0,0,  6,0,0,  0,0,3}, // 2
		{0,7,4,  0,8,0,  0,0,0}, // 3

		{0,0,0,  0,0,3,  0,0,2}, // 4
		{0,8,0,  0,4,0,  0,1,0}, // 5
		{6,0,0,  5,0,0,  0,0,0}, // 6

		{0,0,0,  0,1,0,  7,8,0}, // 7
		{5,0,0,  0,0,9,  0,0,0}, // 8
		{0,0,0,  0,0,0,  0,4,0}  // 9
	};
#endif

	newGame.Set(input2game);

	// Input status
	std::cout << "\n\n============ Input ============\n";
	newGame.Display();

	// Solution (and measuring time)
	std::cout << "\n\n============ Solving ============\n";
	auto start = std::chrono::high_resolution_clock::now();
	newGame.Solve();
	//newGame.SolveDirect();
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	// Display
	std::cout << "\n\n============ Solution ============\n";
	newGame.Display();
	std::cout << "Solved in " << duration.count() << " miliseconds" << std::endl;

	return 0;
}

