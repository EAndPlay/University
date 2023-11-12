//������������ ������ �� ���������������� �5. "����������" ������� �21. ������ �.� 1-43

#include <iostream>
#include <Windows.h>

//��������� ������ �������������� �������
char* read_line();

//���������� ������ ������� ���������� ���������� "value" � "input_text "
int find_index(char*, char*);

//���������� ������ ������ ����� ���������� ����� "separator"
char** str_split(char*, char*);

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	std::cout << "������� �����: ";
	char* input_line = read_line();
	int input_line_length = strlen(input_line);
	if (!input_line_length)
	{
		std::cout << "������ �� �������." << std::endl;
		system("pause");
		return 0;
	}
	char** split = str_split(input_line, (char*)" ");
	int split_count = _msize(split) / sizeof(size_t);
	bool palindrom_found = false;

	for (int i = 0; i < split_count; i++)
	{
		char* str = split[i];
		int str_length = strlen(str);
		if (str_length < 3) continue;

		char* reversed_str = (char*)malloc(str_length + 1);

		//������������� ������
		for (int j = 0; j < str_length; j++)
		{
			reversed_str[j] = str[str_length - j - 1];
		}
		reversed_str[str_length] = 0;
		if (!strcmp(str, reversed_str))
		{
			palindrom_found = true;
			std::cout << str << std::endl;
		}
		free(reversed_str);
	}

	if (!palindrom_found)
	{
		std::cout << "����������� �� �������." << std::endl;
	}
	system("pause");
}

char* read_line()
{
	char* output_line = (char*)malloc(1);
	int length = 0;
	int current_char = getchar();
	while (current_char != 10) //\10 = \n
	{
		switch (current_char)
		{
			case ',':
			case '.':
			case '!':
			case '?':
			case ':':
			case ';':
			case '(':
			case ')':
				break;
			default:
			{
				length++;
				output_line = (char*)realloc(output_line, length + 1);
				current_char |= 0b00100000; // 6th bit - register
				output_line[length - 1] = (char)current_char;
				break;
			}
		}
		current_char = getchar();
		
	}
	output_line[length] = 0;
	return output_line;
}

int find_index(char* input_text, char* value)
{
	int text_length = strlen(input_text);
	int value_length = strlen(value);

	for (int i = 0; i < text_length - value_length; i++)
	{
		if (input_text[i] == value[0])
		{
			for (int j = 1; j < value_length; j++)
			{
				if (input_text[i + j + 1] != value[j])
					goto IContinue;
			}
			return i;
		}
	IContinue:
		continue;
	}
	return -1;
}

char** str_split(char* input_text, char* separator)
{
	char** split_array = (char**)malloc(sizeof(size_t));
	int split_count = 0;
	int text_length = strlen(input_text);
	int separator_length = strlen(separator);
	int actual_length = text_length - separator_length;

	int i = 0;
	int separator_index = find_index(input_text, separator);

	if (separator_index != -1)
	{
		while (true)
		{
			split_array = (char**)realloc(split_array, sizeof(size_t) * (split_count + 1));

			int split_length = separator_index - i;
			char* split_str = (char*)malloc(split_length + 1);
			split_array[split_count] = split_str;
			split_count++;
			memcpy(split_str, input_text + i, split_length);
			split_str[split_length] = 0;

			i += split_length + separator_length;
			separator_index = find_index(input_text + i, separator);
			if (separator_index == -1)
				break;
			separator_index += i;
		}
	}

	//���� ���-�� ������� > 0 � i �� ����� �� ����� ������ - ����������� ������� ������
	if (split_count && i < text_length)
	{
		split_array = (char**)realloc(split_array, sizeof(size_t) * (split_count + 1));
		int split_length = text_length - i;
		char* split_str = (char*)malloc(split_length);
		memcpy(split_str, input_text + i, text_length - i + 1);
		split_str[split_length] = 0;
		split_array[split_count] = split_str;
		split_count++;
	}
	else if (!split_count)
	{
		*split_array = input_text;
	}

	return split_array;
}