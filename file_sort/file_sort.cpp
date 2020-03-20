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

// проверка корректности сортировки
bool check_file(std::string& file)
{
	std::ifstream in(file);
	int x, y;
	in >> x;
	if (!in.eof())
	{
		in >> y;
		while (!in.eof())
		{
			if (x < y)
				return false;
			x = y;
			in >> y;
		}
	}
	return true;
}

// многофазная сортировка, степень разбиения задается вручную
void sortFile(std::string& file, int n)
{
	// проверка на корректность степени разбиения, минимально возможное разбиение 3
	if (n < 3)
		n = 3;

	// solution файл начальных и конечных значений
	std::fstream solution(file, std::ios::in);

	// проверяем наличие каких-либо данных в файле
	int x;
	solution >> x;
	if (!solution.eof())
	{
		solution.close();
		solution.open(file, std::ios::in);
		// массив вспомогательных файлов
		std::fstream** buf = new std::fstream*[n];

		for (int i = 0; i < n; i++)
			buf[i] = new std::fstream("buf" + (i + 49), std::ios::out);

		// количество уровней разбиения
		int level = 1;
		// массив частичных сумм, необходим для рассчета фиктивных отрезков в файлах
		int* a = new int[n];
		// массив фиктивных отрезков
		int* d = new int[n];
		// начальные значения массивов
		for (int i = 0; i < n - 1; i++)
			d[i] = a[i] = 1;
		d[n - 1] = a[n - 1] = 0;

		// 1 ЭТАП - разбиение начальных данных

		// переменная k отвечает за номер вспомогательного файла, в который будет записан новый отрезок
		int k = 0;
		while (!solution.eof())
		{
			int x, y = 0;
			solution >> x;
			for (; !solution.eof() && x > y;)
			{
				*buf[k] << x << ' ';
				y = x;
				solution >> x;
			}
			*buf[k] << "-1 ";
			d[k]--;

			if (d[k] < d[k + 1])
				k++;
			else
				if (d[k] == 0)
				{
					level++;
					k = 0;
					int buf = a[0];
					for (int i = 0; i < n - 1; i++)
					{
						d[i] = a[i + 1] - a[i] + buf;
						a[i] = a[i + 1] + buf;
					}

				}
				else
					k == 0;
		}

		// 2 ЭТАП - слияние

		// закрываем, все что открыли, и удаляем вспомогательные файлы
		for (int i = 0; i < n; i++)
		{
			buf[i]->close();
			delete buf[i];
			std::remove("buf" + (i + 49));
		}
		delete[] buf;
	}
	solution.close();
}


int main()
{
    return 0;
}

