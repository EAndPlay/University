#include <iostream>
#include <windows.h>
#include <fstream>

#include <cstring>

char* ReadLine()
{
    const int kMaxLineLength = 1024;
    char* output_line = (char*)malloc(kMaxLineLength);
    memset(output_line, 0, kMaxLineLength);
    gets_s(output_line, kMaxLineLength);
    return output_line;
}

int strFindIndex(const char* inputText, const char* value)
{
    int text_length = strlen(inputText);
    int value_length = strlen(value);

    for (int i = 0; i < text_length - value_length; i++)
    {
        if (inputText[i] == value[0])
        {
            for (int j = 1; j < value_length; j++)
            {
                if (inputText[i + j + 1] != value[j])
                    goto IContinue;
            }
            return i;
        }
    IContinue:
        continue;
    }
    return -1;
}

char** strSplit(const char* input_text, const char* separator)
{
    char** split_array = (char**)malloc(sizeof(size_t));
    int split_count = 0;
    int text_length = strlen(input_text);
    int separator_length = strlen(separator);
    int actual_length = text_length - separator_length;

    int i = 0;
    int separator_index = strFindIndex(input_text, separator);

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
            separator_index = strFindIndex(input_text + i, separator);
            if (separator_index == -1)
                break;
            separator_index += i;
        }
    }

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
        *split_array = (char*)input_text;
    }

    return split_array;
}

enum Faculties : unsigned char
{
    ISCE, // ИВТФ
    EF, // ЭМФ
    HEF, // ТЭФ
    FEP, // ИФФ
    FEM, // ФЭУ
    FEE // ЭЭФ
};

enum GraduateDegree : unsigned char
{
    Bachelor,
    Master
};

const short MarkLimits[] = {
        250, 350, 435, 501 // +1 for <
};

const char* NameOfFinalScore[] = {
        "Неудовл.", "Удовл.", "Хорошо", "Отлично"
};

const char* FacultiesNamesRU[] = {
        "ИВТФ","ЭМФ","ТЭФ","ИФФ","ФЭУ","ЭЭФ"
};

typedef struct
{
    char* Surname;
    char* Name;
    char* DadSurname;
} FullName;

typedef struct
{
    //CC - Current Control
    //IC - Intermediate Control
    float CC1;
    float IC1;
    float CC2;
    float IC2;
    float Exam;
} KnowledgeControls;


typedef struct
{
    FullName FullName;
    Faculties Faculty;
    unsigned char Course;
    unsigned char Group;
    GraduateDegree Degree;
    short CourseGroup;
    KnowledgeControls Controls;
} Student;

int GetTotalRITM(Student* student)
{
    auto controls = student->Controls;
    const float kCCMultiplier = 5;
    const float kICMultiplier = 15;
    const float kExamMultiplier = 60;
    return (controls.CC1 + controls.CC2) * kCCMultiplier + (controls.IC1 + controls.IC2) * kICMultiplier + controls.Exam * kExamMultiplier;
}

const int kLineBufferSize = 1023;
const int kSymbolsTable = 1'251;

const char* kDBFilePath = "Z:\\Development\\University\\Debug\\StudentsInfo";

int main()
{
    SetConsoleCP(kSymbolsTable);
    SetConsoleOutputCP(kSymbolsTable);
    std::ifstream sourceFileStream(kDBFilePath, std::ios_base::in);

    auto studentsList = (Student**)malloc(1);
    auto studentsCount = 0;

    char* lineBuffer = (char*)malloc(kLineBufferSize + 1);
    memset(lineBuffer, 0, kLineBufferSize);

    while (sourceFileStream.getline(lineBuffer, kLineBufferSize))
    {
        const int kMaxSplits = 11;
        char** lineSplit = strSplit(lineBuffer, " ");
        const auto student = (Student*)malloc(sizeof(Student));
        memcpy(student, lineSplit, sizeof(FullName));

        student->Faculty = (Faculties)-1;
        for (int i = 0; i < sizeof(FacultiesNamesRU) / sizeof(size_t); i++)
        {
            if (!strcmp(FacultiesNamesRU[i], lineSplit[3]))
                student->Faculty = (Faculties)i;
        }
        if (student->Faculty == (unsigned char)-1)
        {
            std::cout << "В строке <" << studentsCount + 1 << "> неверно указан факультет" << std::endl;
            system("pause");
            return 0;
        }

        student->Course = atoi(lineSplit[4]);
        student->Group = atoi(lineSplit[5]);

        if ((unsigned char)lineSplit[5][_msize(lineSplit[5]) - 2] == 236) // 236 = м
        {
            student->Degree = Master;
        }

        for (int i = 0; i < 5; i++)
        {
            *((float*)&student->Controls + i) = atof(lineSplit[6 + i]);
        }

        for (int i = 3; i < kMaxSplits - 1; i++)
            free(lineSplit[i]);
        free(lineSplit);

        studentsList = (Student**)realloc(studentsList, (studentsCount + 1) * sizeof(size_t));
        bool added = false;
        for (int i = 0; i < studentsCount; i++)
        {
            auto readyStudent = studentsList[i];
            if (GetTotalRITM(student) > GetTotalRITM(readyStudent))
            {
                memmove(studentsList + i + 1, studentsList + i, (studentsCount - i) * sizeof(size_t));
                studentsList[i] = student;
                added = true;
                break;
            }
        }
        if (!added)
            studentsList[studentsCount] = student;
        studentsCount++;
    }
    free(lineBuffer);
    sourceFileStream.close();

    for (int i = 0; i < studentsCount; ++i)
    {
        auto student = studentsList[i];
        std::cout << i + 1 << "." << std::endl;
        std::cout << " ФИО: " << student->FullName.Surname << ' ' << *student->FullName.Name << ". " << *student->FullName.DadSurname << '.' << std::endl;
        std::cout << " Факультет: " << FacultiesNamesRU[student->Faculty] << std::endl;
        std::cout << " Группа: " << static_cast<int>(student->Course) << '-' << static_cast<int>(student->Group);
        if (student->Degree == Master)
            std::cout << "м";

        std::cout << std::endl;
        std::cout << " Оценка за экзамен: " << student->Controls.Exam << std::endl;
        const char* ratingStr = " Рейтинг: ";
        const char* ratingSpace = "     ";
        std::cout << ratingStr << std::endl;
        for (int j = 0; j < 4; j++)
        {
            const char* controlName = j % 2 ? "ПК" : "ТК";
            std::cout << ratingSpace << controlName << (j < 2 ? 1 : 2) << ": " << *((float*)&student->Controls + j) << std::endl;
        }

        auto totalScore = GetTotalRITM(student);
        std::cout << " СИ: " << totalScore << std::endl; // СИ - Суммарный Индекс
        char* finalScore = 0;
        for (int j = 0; j < sizeof(MarkLimits) / sizeof(short); j++)
        {
            if (totalScore < MarkLimits[j])
            {
                finalScore = (char*)NameOfFinalScore[j];
                break;
            }
        }
        std::cout << " Итоговый балл: " << finalScore << std::endl;
    }
    system("pause");
    return 0;
}
