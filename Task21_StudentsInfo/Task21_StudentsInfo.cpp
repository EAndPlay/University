//Лабораторная работа по программированию №7. "Проверка" Вариант №21. Гонцов А.М 1-43

#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <locale>

#include <cstring>

#include "StringExtensions.h"

#define MAX_FILE_PATH 255

enum MenuId : unsigned char
{
    Main,
};

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
    char Surname[32];
    char Name[32];
    char DadSurname[32];
} FullName;

// TODO: compact every control to 1 byte
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

// Course-Group struct:
typedef struct
{
    int LocalStudentId;
    FullName FullName;
    Faculties Faculty;
    unsigned char Course;
    unsigned char Group;
    GraduateDegree Degree;
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
const int kSymbolsTable = 1251;

const char kDBFilePath[MAX_FILE_PATH] = "Z:\\Development\\University\\Debug\\StudentsInfo.bin";

void HandleMenuAction(int currentMenu, int action);

typedef struct
{
    char* Description;
    void(*OnHandle)();
} MenuAction;

typedef struct tagMenu
{
    tagMenu *ParentMenu;
    std::vector<MenuAction> Actions;
    void OnShowMenu()
    {
        for (int i = 0; i < this->Actions.size(); i++)
        {
            std::cout << i + 1 << ": " << this->Actions[i].Description << std::endl;
        }
        int actionId;
        while (true)
        {
            std::cout << "Действие: ";
            std::cin >> actionId;
            if (std::cin.fail())
            {
                std::cin.clear();
                std::cout << "Должно быть введено число!" << std::endl;
                continue;
            }
            if (actionId < 1 || actionId > this->Actions.size() - 1)
            {
                std::cout << "Неверный номер действия." << std::endl;
                continue;
            }
            break;
        }
        this->Actions[actionId - 1].OnHandle();
    }
} IMenu;

static IMenu *MainMenu;
static IMenu *FindEditMenu;
static IMenu *CurrentMenu;

static std::vector<Student> StudentsList;
static std::ifstream DBFileStream;

void OnShowMenuDefault(IMenu*);
void OutputStudentInfo(Student*);

void InitializeMenus();
void SwitchMenu(IMenu*);

int main()
{
    //SetConsoleCP(kSymbolsTable);
    //SetConsoleOutputCP(kSymbolsTable);
    setlocale(LC_ALL, "ru");
    InitializeMenus();
    MainMenu->OnShowMenu();
    return 0;
    //DBFileStream(kDBFilePath, std::ios_base::in);

    while (DBFileStream.peek() != EOF)
    {
        Student student;
        DBFileStream.read((char *)&student, sizeof(Student));
        StudentsList.push_back(student);
    }
    
    system("pause");
    return 0;
}

void InitializeMenus()
{
    MainMenu = new IMenu();
    FindEditMenu = new IMenu();
    MainMenu->ParentMenu = nullptr;
    FindEditMenu->ParentMenu = MainMenu;

    // MainMenu
    {
        MenuAction outPutAll =
        {
            const_cast<char*>("Вывести всех"),
            []()
            {
                for (auto& student : StudentsList)
                {
                    OutputStudentInfo(&student);
                    std::cout << std::endl;
                }
            }
        };
        MenuAction switchToFindEdit =
        {
            const_cast<char*>("Поиск и редактирование"),
            []()
            {
                SwitchMenu(FindEditMenu);
            }
        };
        MenuAction exitApp =
        {
            const_cast<char*>("Завершить"),
            []()
            {
                exit(0);
            }
        };
        MainMenu->Actions = *new std::vector<MenuAction>();
        MainMenu->Actions.push_back(outPutAll);
        MainMenu->Actions.push_back(switchToFindEdit);
        MainMenu->Actions.push_back(exitApp);
    }
    // MainMenu

    auto actionSwitchBack = *new MenuAction();
    actionSwitchBack.Description = const_cast<char*>("Назад");
    actionSwitchBack.OnHandle = []()
    {
        if (CurrentMenu->ParentMenu)
        {
            SwitchMenu(CurrentMenu->ParentMenu);
        }
    };

    // FindEditMenu
    {

        FindEditMenu->Actions = *new std::vector<MenuAction>();
        FindEditMenu->Actions.push_back(actionSwitchBack);
    }
    // FindEditMenu

}

void SwitchMenu(IMenu* newMenu)
{
    CurrentMenu = newMenu;
    CurrentMenu->OnShowMenu();
}

void OutputStudentInfo(Student* student)
{
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

void OnShowMenuDefault(IMenu* menu)
{
    for (int i = 0; i < menu->Actions.size(); i++)
    {
        std::cout << i + 1 << ": " << menu->Actions[i].Description << std::endl;
    }
    int actionId;
    while (true)
    {
        std::cout << "Действие: ";
        std::cin >> actionId;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cout << "Должно быть введено число!" << std::endl;
            continue;
        }
        if (actionId < 1 || actionId - 1 > menu->Actions.size())
        {
            std::cout << ":" << actionId << "Неверный номер действия." << std::endl;
            continue;
        }
        break;
    }
    menu->Actions[actionId - 1].OnHandle();
}