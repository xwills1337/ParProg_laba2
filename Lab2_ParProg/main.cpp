﻿#include<iostream>
#include<vector>
#include<random>
#include<fstream>
#include<chrono>
#include<omp.h>
const int LOWER = -100;
const int UPPER = 100;
std::vector<std::vector<int>> CreateMatrix(const int size) {
	std::vector<std::vector<int>> matrix(size, std::vector<int>(size));
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<> distr(LOWER, UPPER);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			matrix[i][j] = distr(gen);
		}
	}
	return matrix;
}

std::vector<std::vector<int>> ReadMatrix(const std::string filename, int& size) {
	std::ifstream file(filename);
	if (!file.is_open()) throw std::runtime_error("File is not open!");
	file >> size;
	file >> size;
	std::vector<std::vector<int>> matrix(size, std::vector<int>(size));
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			file >> matrix[i][j];
		}
	}
	file.close();
	return matrix;
}

void WriteMatrix(const std::vector<std::vector<int>> matrix, const std::string filename) {
	std::ofstream file(filename);
	if (!file.is_open()) throw std::runtime_error("File is not open!");
	file << matrix.size() << " " << matrix[0].size() << std::endl;
	for (const auto& row : matrix) {
		for (const int& val : row) {
			file << val << " ";
		}
		file << std::endl;
	}
	file.close();
}

std::vector<std::vector<int>> MatrixMultiplication(const std::string& file1, const std::string& file2) {
	int size;
	std::vector<std::vector<int>> matrix1 = ReadMatrix(file1, size);
	std::vector<std::vector<int>> matrix2 = ReadMatrix(file2, size);
	std::vector<std::vector<int>> result(size, std::vector<int>(size, 0));
	#pragma omp parallel for num_threads(8)
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			for (int k = 0; k < size; ++k) {
				result[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}
	return result;
}

std::chrono::duration<double> test2(size_t size);

void test(size_t start, size_t end, size_t step) {
	std::ofstream file("test4.txt");
	double times = 0;
	for (size_t i = start; i <= end; i += step) {
		for (size_t j = 0; j < 10; ++j) {
			std::chrono::duration<double> t = test2(i);
			times += t.count();
		}
		times = times / 10;
		file << i << std::endl;

		file << times << std::endl;
	}
}

std::chrono::duration<double> test2(size_t size) {
	std::vector<std::vector<int>> m1 = CreateMatrix(size);
	std::vector<std::vector<int>> m2 = CreateMatrix(size);
	WriteMatrix(m1, "TestMatrix1.txt");
	WriteMatrix(m2, "TestMatrix2.txt");
	auto start = std::chrono::steady_clock::now();
	MatrixMultiplication("TestMatrix1.txt", "TestMatrix2.txt");
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	return elapsed_seconds;
}

int main() {
	try {
		const int size = 100;
		std::vector<std::vector<int>> matrix1 = CreateMatrix(size);
		std::vector<std::vector<int>> matrix2 = CreateMatrix(size);

		WriteMatrix(matrix1, "matrix1.txt");
		WriteMatrix(matrix2, "matrix2.txt");

		std::vector<std::vector<int>> result;
		auto start = std::chrono::high_resolution_clock::now();
		result = MatrixMultiplication("matrix1.txt", "matrix2.txt");
		auto end = std::chrono::high_resolution_clock::now();
		WriteMatrix(result, "result_matrix.txt");

		std::chrono::duration<double> duration = end - start;
		double meanTime = duration.count();
		std::cout << "The scope of the task: " << 2 * size * size << " elements." << std::endl;
		std::cout << "Execution time: " << meanTime << " seconds." << std::endl;

		test(20, 200, 20);

		return 0;
	}
	catch (const std::string err) {
		std::cout << err;
	}
}