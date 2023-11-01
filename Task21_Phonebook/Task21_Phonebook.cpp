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

const int kLineBufferSize = 128;

int main()
{
	//setlocale(LC_ALL, "ru");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	std::ifstream pb_stream("Z:\\Development\\University\\Debug\\PhoneBook", std::ios_base::in);
	pb_stream.seekg(0, std::ios::end);
	std::streampos file_size = pb_stream.tellg();
	pb_stream.seekg(0, std::ios::beg);
	std::cout << "File size: " << file_size << std::endl;
	//int fields_count = file_size / sizeof(phonebook_field);
	phonebook_field** phonebook = (phonebook_field**)malloc(1);
	int fields_count = 0;
	char *line_buffer = (char*)malloc(kLineBufferSize);
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
	do
	{
		input_line = read_line();
		//TODO: req for family
		//same family reqs choice for exact human by nums
		free(input_line);
		input_line = read_line();
	} while (*input_line);
	free(input_line);
	//for (int i = 0; i < fields_count; i++)
	//{
	//	std::cout << phonebook[i]->FullName.Surname << " " << phonebook[i]->FullName.Name << " " << phonebook[i]->FullName.DadSurname << " +" << (int)phonebook[i]->PhoneNumber.CountryCode << " " << phonebook[i]->PhoneNumber.Number << std::endl;
	//}


}