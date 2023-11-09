//Лабораторная работа по программированию №6. "Телефонный справочник" Вариант №21. Гонцов А.М 1-43

#include <iostream>
#include <fstream>
#include <Windows.h>

#include "StringExtensions.h"

//Encoding: Cyrillic (Windows 1251)

// +Code (XXX)-XXX-XX-XX
typedef struct tagPHONENUMBER
{
    unsigned char CountryCode;
    char *Number;
    bool IsPersonalPhone;
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

void write_phonenumber(field_phone *phone)
{
    char *phone_str = phone->Number;
    if (phone->IsPersonalPhone)
        std::cout << '(' << *phone_str << phone_str[1] << phone_str[2] << ")-" << phone_str[3] << phone_str[4] << phone_str[5] << '-' << phone_str[6] << phone_str[7] << '-' << phone_str[8] << phone_str[9];
    else
        std::cout << phone_str;
}

int main()
{
    SetConsoleCP(kSymbolsTable);
    SetConsoleOutputCP(kSymbolsTable);
    std::ifstream pb_stream(kDBFilePath, std::ios_base::in);
    
    auto **phonebook = (phonebook_field**)malloc(1);
    int fields_count = 0;
    char *line_buffer = (char*)malloc(kLineBufferSize + 1);
    memset(line_buffer, 0, kLineBufferSize);
    while (pb_stream.getline(line_buffer, kLineBufferSize))
    {
        char **field_split = str_split(line_buffer, (char*)" ");
        auto *field = (phonebook_field*)malloc(sizeof(phonebook_field));
        memcpy(field, field_split, sizeof(field_fullname));

        field->PhoneNumber.CountryCode = atoi(field_split[3] + 1);
        field->PhoneNumber.Number = field_split[4];
        if (strlen(field_split[4]) > 9)
            field->PhoneNumber.IsPersonalPhone = true;
        
        free(field_split[3]);
        free(field_split);

        phonebook = (phonebook_field**)realloc(phonebook, (fields_count + 1) * sizeof(phonebook_field));
        phonebook[fields_count] = field;
        fields_count++;
        memset(line_buffer, 0, kLineBufferSize);
    }
    free(line_buffer);
    pb_stream.close();

    char* input_line;

    field_fullname *fullname;
    field_phone *phone_number;

    while (true)
    {
        std::cout << "Введите фамилию: " << std::endl;
        input_line = read_line();
        if (!*input_line)
            break;
        
        int *found_people = (int*)malloc(1);
        int found_people_count = 0;
        for (int i = 0; i < fields_count; i++)
        {
            if (!strcmpic(input_line, phonebook[i]->FullName.Surname))
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
            std::cout << fullname->Surname << ' ' << *fullname->Name << ". " << *fullname->DadSurname << ". : +" << (int)phone_number->CountryCode << ' ';
            write_phonenumber(phone_number);
            std::cout << std::endl;
        }
        else
        {
            std::cout << "Выберите конкретного человека, введя его номер из списка:" << std::endl;
            for (int i = 0; i < found_people_count; i++)
            {
                fullname = &phonebook[found_people[i]]->FullName;
                std::cout << i + 1 << ": " << fullname->Surname << ' ' << fullname->Name << " " << fullname->DadSurname << std::endl;
            }
            int target_index;
            while (true)
            {
                free(input_line);
                input_line = read_line();
                if (!(target_index = atoi(input_line)))
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
            
            fullname = &phonebook[found_people[target_index]]->FullName;
            phone_number = &phonebook[found_people[target_index]]->PhoneNumber;
            
            std::cout << fullname->Surname << ' ' << *fullname->Name << ". " << *fullname->DadSurname << ". : +" << (int)phone_number->CountryCode << ' ';
            write_phonenumber(phone_number);
            std::cout << std::endl;
        }
        free(found_people);
        free(input_line);
        std::cout << std::endl;
    }
    free(input_line);
}