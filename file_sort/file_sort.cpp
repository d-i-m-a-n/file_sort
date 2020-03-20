// file_sort.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>

// генерация N случайных чисел в диапозоне от max до min в file
void generateFile(std::string& file, int n, int max = 100, int min = 0)
{
	std::ofstream out(file);
	for (int i = 0; i < n; i++)
	{
		int x = (double)rand() / RAND_MAX*(max - min) + min;
		out << x << ' ';
	}
	out.close();
}

int main()
{
    return 0;
}

