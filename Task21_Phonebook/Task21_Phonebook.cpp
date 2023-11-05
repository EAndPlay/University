//Лабораторная работа по программированию №6. "Телефонный справочник" Вариант №21. Гонцов А.М 1-43

#include <iostream>
#include <fstream>
#include <locale>
#include <Windows.h>

#include "StringExtensions.h"

//Encoding: Cyrillic (Windows 1251)

// +Code (XXX)-XXX-XX-XX
typedef struct tagPHONENUMBER
{
	unsigned char CountryCode;
	char *Number;
} field_phone;

typedef struct tagFULLNAME
{
	char *Surname;
	char *Name;
	char *DadSurname;
} field_fullname;

//X X X Y
typedef struct tagPHONEBOOK_FIELD
{
	field_fullname FullName;
	field_phone PhoneNumber;
} phonebook_field;

const int kLineBufferSize = 1023;
const int kSymbolsTable = 1251;

//Путь до файла "телефонного справочника"
const char* kDBFilePath = "Z:\\Development\\University\\Debug\\PhoneBook";

int main()
{
	SetConsoleCP(kSymbolsTable);
	SetConsoleOutputCP(kSymbolsTable);
	std::ifstream pb_stream(kDBFilePath, std::ios_base::in);

	pb_stream.seekg(0, std::ios::end);
	std::streampos file_size = pb_stream.tellg();
	pb_stream.seekg(0, std::ios::beg);

	std::cout << "File size: " << file_size << std::endl;
	//int fields_count = file_size / sizeof(phonebook_field);
	phonebook_field** phonebook = (phonebook_field**)malloc(1);
	int fields_count = 0;
	char *line_buffer = (char*)malloc(kLineBufferSize + 1);
	memset(line_buffer, 0, kLineBufferSize);
	while (pb_stream.getline(line_buffer, kLineBufferSize))
	{
		//std::cout << line_buffer << std::endl;
		char** field_split = str_split(line_buffer, (char*)" ");
		phonebook_field* field = (phonebook_field*)malloc(sizeof(phonebook_field));

		//0 - surname, 1 - name, 2 - dad surname
		int surname_length = strlen(field_split[0]);
		int name_length = strlen(field_split[1]);
		int dadsurname_length = strlen(field_split[2]);
		int fullname_length = surname_length + name_length + dadsurname_length + 3;

		memcpy(field, field_split, sizeof(field_fullname));

		char** countrycode_split = str_split(field_split[3], (char*)"+");
		field->PhoneNumber.CountryCode = atoi(countrycode_split[1]);
		field->PhoneNumber.Number = field_split[4];

		free(countrycode_split[0]);
		//free(countrycode_split[1]); //heap corruption: [1] places after heap?
		free(countrycode_split);
		free(field_split[3]);
		free(field_split);

		phonebook = (phonebook_field**)realloc(phonebook, (fields_count + 1) * sizeof(phonebook_field));
		phonebook[fields_count] = field;
		fields_count++;
		memset(line_buffer, 0, kLineBufferSize);
	}

	char* input_line;
	int input_line_length;

	field_fullname* fullname;
	field_phone* phone_number;

	do
	{
		std::cout << "Введите фамилию: " << std::endl;
		input_line = read_line(input_line_length);
		int people_count = 0;
		int* found_people = (int*)malloc(1); // [local_arr_index] = phonebook_index
		int found_people_count = 0;
		for (int i = 0; i < fields_count; i++)
		{
			if (!_strnicmp(input_line, phonebook[i]->FullName.Surname, input_line_length))
			{
				found_people = (int*)realloc(found_people, (found_people_count + 1) * sizeof(int));
				found_people[found_people_count] = i;
				found_people_count++;
			}
		}

		if (!found_people_count)
		{
			std::cout << "Не найдено человека с такой фамилией." << std::endl;
		}
		else if (found_people_count == 1)
		{
			fullname = &phonebook[*found_people]->FullName;
			phone_number = &phonebook[*found_people]->PhoneNumber;
			std::cout << fullname->Surname << " " << fullname->Name << " " << fullname->DadSurname << " +" << phone_number->CountryCode << " " << phone_number->Number << std::endl;
		}
		else
		{
			std::cout << "Выберите конкретного человека, введя его номер из списка:" << std::endl;
			for (int i = 0; i < found_people_count; i++)
			{
				fullname = &phonebook[i]->FullName;
				std::cout << i + 1 << ": " << fullname->Surname << " " << fullname->Name << " " << fullname->DadSurname << std::endl;
			}
			int target_index;
			while (true)
			{
				std::cin >> target_index;
				if (std::cin.fail())
				{
					std::cout << "Введён неверный тип данных. Введите индекс снова:" << std::endl;
					continue;
				}
				else if (target_index < 1 || target_index > found_people_count)
				{
					std::cout << "Введён неправильный индекс. Введите индекс снова:" << std::endl;
					continue;
				}
				break;
			}
			target_index--;

			phone_number = &phonebook[target_index]->PhoneNumber;
			std::cout << '+' << phone_number->CountryCode << ' ' << phone_number->Number << std::endl;
		}
		free(found_people);
		free(input_line);
		std::cout << std::endl;
		input_line = read_line();
	} while (*input_line);
	free(input_line);
}