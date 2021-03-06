// file_sort.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <fstream>
#include <string>
#include <iostream>

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
			if (x > y)
			{
				return false;
				in.close();
			}
			x = y;
			in >> y;
		}
	}
	in.close();
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
		// названия вспомогательных файлов, для открытия\закрытия файлов после сдвига указателей
		char** buf_names = new char*[n];

		// кол-во символо, необходимое для записи названий всех файлов (логарифм + 1 символов - кол-во цифр в числе n и 1 сивол для конца строки)
		x = 2 + log10(n);
		for (int i = 0; i < n; i++)
		{
			int b;
			buf_names[i] = new char[x];
			// sprintf функция переводит число в строку, возвращает количество записанных символов, не устанавливает символ конца строки
			b = sprintf_s(buf_names[i], x, "%d", i);
			buf_names[i][b] = '\0';
			buf[i] = new std::fstream(buf_names[i], std::ios::out);
		}

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
		solution >> x;
		while (!solution.eof())
		{
			int y = 0;
			// записываем упорядоченный отрезок
			for (; !solution.eof() && x >= y;)
			{
				*buf[k] << ' ' << x;
				y = x;
				solution >> x;
			}
			*buf[k] << " -1";
			d[k]--;

			// учет горизонтального распределения фиктивных отрезков
			if (!solution.eof())
			{
				if(d[k] < d[k + 1])
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
					k = 0;
			}
		}

		solution.close();

		// 2 ЭТАП - слияние
		// на этом этапе массив а[] будет использоваться для слияния отрезков как в многопутевой сортировке


		// открываем все, кроме последнего файла, на чтение
		for (int i = 0; i < n - 1; i++)
		{
			buf[i]->close();
			buf[i]->open(buf_names[i], std::ios::in);
		}

		while (level)
		{
			// флаг, помечающий наличие фиктивных отрезков во всех файлах
			bool AllHaveFict = true;

			while (!buf[n - 2]->eof())
			{
				// удаление фиктивных отрезков и отслеживание их наличия во всех файлов
				for (int i = 0; i < n - 1; i++)
				{
					if (d[i])
					{
						d[i]--;
						a[i] = -1;
					}
					else
					{
						// если в i-том файле закончились фиктивные отрезки, записываем первый элемент из реального отрезка в i-тую ячейку массива
						AllHaveFict = false;
						*buf[i] >> a[i];
					}
				}

				if (AllHaveFict)
					d[n - 1]++;
				else
				{
					// проверка на наличие реальных отрезков для слияния
					bool HaveSmthToMerge;

					do
					{
						HaveSmthToMerge = false;
						int min = INT32_MAX, curIx;

						// ищем минимальный элемент, номер файла, из которого он был взят, и проверяем, что отрезки не закончились
						for (int i = 0; i < n - 1; i++)
							if (a[i] != -1 && a[i] < min)
							{
								HaveSmthToMerge = true;
								min = a[i];
								curIx = i;
							}

						// записываем минимальный элемент и берем новый элемент из файла
						if (HaveSmthToMerge)
						{
							*buf[n - 1] << ' ' << min;
							*buf[curIx] >> a[curIx];
						}

					} while (HaveSmthToMerge);
					*buf[n - 1] << " -1";
				}
			}

			buf[n - 2]->close();
			buf[n - 1]->close();

			buf[n - 2]->open(buf_names[n - 2], std::ios::out);
			buf[n - 1]->open(buf_names[n - 1], std::ios::in);

			// сдвигаем указатели на файлы, их имена и кол-во фиктивных отрезков
			int D = d[n - 1];
			char* S = buf_names[n - 1];
			std::fstream* B = buf[n - 1];

			for (int i = n - 1; i > 0; i--)
			{
				d[i] = d[i - 1];
				buf_names[i] = buf_names[i - 1];
				buf[i] = buf[i - 1];
			}
			d[0] = D;
			buf_names[0] = S;
			buf[0] = B;

			level--;
		}


		// записываем ответ в исходный файл
		solution.open(file, std::ios::out);

		for (*buf[0] >> x ; x != -1; *buf[0] >> x)
			solution << x << ' ';


		// закрываем, все что открыли, и удаляем вспомогательные файлы
		for (int i = 0; i < n; i++)
		{
			buf[i]->close();
			delete buf[i];
			std::remove(buf_names[i]);
			delete[] buf_names[i];
		}
		delete[] buf;
		delete[] buf_names;
		delete[] a;
		delete[] d;
	}
	solution.close();
}


int main()
{
	std::string file = "file.txt";
	
	generateFile(file, 20);
	
	sortFile(file, 3);

	std::fstream out(file, std::ios::app);
	if (check_file(file))
		out << "\ncorrect sort";
	else
		out << "\nwrong sort";

	out.close();
    return 0;
}

