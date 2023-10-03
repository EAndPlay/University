#include <iostream>
#include <locale>

int* get_array_elements_row();

int** get_2d_matrix();

//За элементы массива взяты целочисленные значения - int
int main()
{
	setlocale(LC_ALL, "ru");

	std::cout << "Введите одномерный массив: ";
	int* one_d_array = get_array_elements_row();

	std::cout << "Введите двумерный массив: (для прекращения ввода - на пустой строке нажмите Enter)" << std::endl;
	int** two_d_array = get_2d_matrix();

	for (int i = 0; i < _msize(one_d_array) / sizeof(int); i++)
	{
		int first_element = one_d_array[i];
		bool is_found = false;
		std::cout << i + 1 << " : ";

		for (int j = 0; j < _msize(two_d_array) / sizeof(size_t); j++)
		{
			for (int k = 0; k < _msize(two_d_array[j]) / sizeof(int); k++)
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
	int* array = (int*)malloc(sizeof(int));
	int i = 0;
	while (true)
	{
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
	std::cin.get();
	return array;
}

int** get_2d_matrix()
{
	int** array = (int**) malloc(sizeof(size_t));
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