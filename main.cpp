// main.cpp 
// Main file for testing hello_math
//
// Authors      : Jinwook Lee, Sungwook Lee
// First version: May 24, 2020
// Last update  : May 25, 2020
//

#pragma once
#include <iostream>
#include <chrono> 
#include "sudoku.h"

std::vector<std::vector<size_t>> SudokuExample(size_t difficultyLevel);

int main()
{	
	// Game initialization
	Sudoku<size_t> newGame;
	constexpr size_t difficulty = 3; // 0 [5->5ms] 1[2417->184ms] 2[8078->1294ms] 3[11411->447ms]
	newGame.Set(SudokuExample(difficulty));

	// Input status
	std::cout << "\n\nInput:\n";
	newGame.Display();

	// Solution (and measuring time)
	auto start = std::chrono::high_resolution_clock::now();
	newGame.Solve();
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

	// Display
	std::cout << "\nSolution (" << duration.count() << "ms):\n";
	newGame.Display();

	return 0;
}

std::vector<std::vector<size_t>> SudokuExample(size_t difficultyLevel)
{
	// Sudoku examples: https://dingo.sbs.arizona.edu/~sandiway/sudoku/examples.html
	// Level4:  https://www.conceptispuzzles.com/index.aspx?uri=info/article/424
	std::vector<std::vector<size_t>> example;
	switch (difficultyLevel) {
	case 0:
		example = {
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
		break;
	case 1:
		example = {
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
		break;
	case 2:
		example = {
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
		break;
	case 3:
		example = {
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
		break;
	case 4:
		example = {
			{8,0,0,  0,0,0,  0,0,0}, // 1
			{0,0,3,  6,0,0,  0,0,0}, // 2
			{0,7,0,  0,9,0,  2,0,0}, // 3

			{0,5,0,  0,0,7,  0,0,0}, // 4
			{0,0,0,  0,4,5,  7,0,0}, // 5
			{0,0,0,  1,0,0,  0,3,0}, // 6

			{0,0,1,  0,0,0,  0,6,8}, // 7
			{0,0,8,  5,0,0,  0,1,0}, // 8
			{0,9,0,  0,0,0,  4,0,0}  // 9
		};
		break;
	default:
		std::cout << "Invalid difficulty level!" << std::endl;
	}
	return example;
}

