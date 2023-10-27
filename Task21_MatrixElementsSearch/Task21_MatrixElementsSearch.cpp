#include <iostream>
#include <locale>

//Для удаления повторяющихся значений из строки матрицы
#define NO_REPEATS

int* get_array_elements_row();

int** get_2d_matrix();

//За элементы массива взяты целочисленные значения - int
int main()
{
	setlocale(LC_ALL, "ru");

	std::cout << "Введите одномерный массив: ";
	int* one_d_array = get_array_elements_row();

	int one_d_length = _msize(one_d_array) / sizeof(int);

	if (!one_d_length)
	{
		std::cout << "Одномерная матрица пуста.";
		system("pause");
		return 0;
	}

	std::cout << "Введите двумерный массив: (для прекращения ввода - на пустой строке нажмите Enter)" << std::endl;
	int** two_d_array = get_2d_matrix();

	int two_d_length = _msize(two_d_array) / sizeof(size_t);

	if (!two_d_length)
	{
		std::cout << "Двумерная матрица пуста.";
		system("pause");
		return 0;
	}

	for (int i = 0; i < one_d_length; i++)
	{
		int first_element = one_d_array[i];
		bool is_found = false;
		std::cout << i + 1 << " : ";

		for (int j = 0; j < two_d_length; j++)
		{
			int one_d_element_length = _msize(two_d_array[j]) / sizeof(int);

			for (int k = 0; k < one_d_element_length; k++)
			{
				int second_element = two_d_array[j][k];

				if (first_element == second_element)
				{
					std::cout << j + 1 << "." << k + 1;
					is_found = true;
					break;
				}
			}

			if (is_found) break;
		}
		
		if (!is_found)
			std::cout << "Пара не найдена";
		std::cout << std::endl;
	}

	system("pause");
}

int* get_array_elements_row()
{
	int* array = (int*) malloc(1);
	int i = 0;
	while (true)
	{
		//Если последний введённый символ Ввод - цикл завершается
		if (std::cin.peek() == '\n') break;
		array = (int*) realloc(array, (i + 1) * sizeof(int));
		std::cin >> array[i];

		if (std::cin.fail())
		{
			std::cout << "Введён неправильный тип данных, для повтора начните всё заного" << std::endl;
			system("pause");
			exit(0);
		}
		i++;
	}

#ifdef NO_REPEATS
	//Исключает повторяющиеся значения

	//Бесконечный цикл - для рекурсии
	while (true)
	{
		bool flag = false;
		for (int j = 0; j < i; j++)
		{
			int j_element = array[j];
			for (int k = j + 1; k < i; k++)
			{
				if (j_element == array[k])
				{
					int* new_array = (int*) calloc(i - 1, sizeof(int));
					memcpy(new_array, array, k * sizeof(int));
					memcpy(new_array + k, array + k + 1, (i - k) * sizeof(int));
					i--;
					flag = true;
					free(array);
					array = new_array;
				}
			}
		}
		if (!flag) break;
	}
#endif

	std::cin.get();
	return array;
}

int** get_2d_matrix()
{
	int** array = (int**) malloc(1);
	int i = 0;
	while (true)
	{
		std::cout << i + 1 << ": ";
		if (std::cin.peek() == '\n') break;
		array = (int**) realloc(array, (i + 1) * sizeof(size_t));
		array[i] = get_array_elements_row();
		i++;
	}
	std::cout << "Конец ввода" << std::endl << std::endl;
	return array;
}