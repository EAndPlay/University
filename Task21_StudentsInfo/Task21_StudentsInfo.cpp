//Лабораторная работа по программированию №7. "Проверка" Вариант №21. Гонцов А.М 1-43

#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <filesystem>

#include "StringExtensions.h"

using std::cout, std::cin, std::endl, std::vector, std::string, std::fstream;

typedef float MarkType;

enum EStudentProperty : byte
{
    SND,
    Marks,
    FCG // Faculty Course Group
};

enum EFaculties : byte
{
    ISCE = 1, // ИВТФ
    EF = 2, // ЭМФ
    HEF = 3, // ТЭФ
    FEP = 4, // ИФФ
    FEM = 5, // ФЭУ
    FEE = 6 // ЭЭФ
};

enum EGraduateDegree : byte
{
    Bachelor,
    Master,
    Postgraduate
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

const char* ControlsNamesRU[] = {
        "ТК1","ПК1","ТК2","ПК2","Экз."
};

const int kMaxStringLength = 32;
typedef struct
{
    char Surname[kMaxStringLength];
    char Name[kMaxStringLength];
    char DadSurname[kMaxStringLength];
} FullName;

// TODO: compact every control to 1 byte
typedef struct
{
    //CC - Current Control
    //IC - Intermediate Control
    MarkType CC1;
    MarkType IC1;
    MarkType CC2;
    MarkType IC2;
    MarkType Exam;
} KnowledgeControls;

// Course-Group struct:
typedef struct student_t
{
    int LocalStudentId;
    FullName FullName;
    EFaculties Faculty;
    byte Course;
    byte Group;
    EGraduateDegree Degree;
    KnowledgeControls Controls;
    int __thiscall GetTotalRITM() const
    {
        auto controls = this->Controls;
        const float kCCMultiplier = 5;
        const float kICMultiplier = 15;
        const float kExamMultiplier = 60;
        return (controls.CC1 + controls.CC2) * kCCMultiplier + (controls.IC1 + controls.IC2) * kICMultiplier + controls.Exam * kExamMultiplier;
    }
} Student;

const int kLineBufferSize = 1023;
const int kSymbolsTable = 1251;
const int kMaxFilePath = 255;

const char* InvalidActionIdException = "Неверный номер действия.";
const char* InvalidInputTypeException = "Неверный тип данных.";
const char* NoSuchStudentIdException = "Студента с таким ID не найдено.";
const char* NotAvaibleInfoException = "N/A";
const char kDBFilePath[kMaxFilePath] = "Z:\\Development\\University\\Debug\\StudentsInfo.bin";

typedef struct
{
    const char* Description;
    void (*OnHandle)();
} MenuAction;

typedef struct tagMenu
{
    tagMenu* ParentMenu;
    const char* Caption;
    vector<MenuAction> Actions;
    bool AllowOverride = false;
    bool OverrideAfterBase = false;
    void (*OverrideOnShow)() = nullptr;
    void (*ActionCompleted)() = nullptr;
    void __thiscall BaseOnShow()
    {
        cout << Caption << endl;
        for (int i = 0; i < this->Actions.size(); i++)
        {
            cout << i + 1 << ": " << this->Actions[i].Description << endl;
        }
        int actionId;
        while (true)
        {
            cout << "Действие: ";
            cin >> actionId;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore();
                cout << "Должно быть введено число!" << endl;
                continue;
            }
            if (actionId < 1 || actionId > this->Actions.size())
            {
                cout << InvalidActionIdException << endl;
                continue;
            }
            break;
        }
        auto handle = this->Actions[actionId - 1].OnHandle;
        if (handle)
            handle();
        if (this->ActionCompleted)
            this->ActionCompleted();
    }
} IMenu;

static IMenu* MainMenu;
static IMenu* FindEditMenu;
static IMenu* ConfirmMenu;
static IMenu* MarksMenu;
static IMenu* FCGMenu;
static IMenu* CreateStudentMenu;
static IMenu* SortMenu;

static IMenu* CurrentMenu;

void(*ActionToConfirm)();
EStudentProperty ChoiceToChange;
Student* TargetStudent;

static vector<Student> StudentsList;
static std::ofstream DBFileStreamWrite;
static std::ifstream DBFileStreamRead;

const int FileReadWriteFlags = std::ios_base::in | std::ios_base::out | std::ios_base::binary;
const int FileReadWriteRecreateFlags = FileReadWriteFlags | std::ios_base::trunc;

void UpdateDBFile();
vector<Student> GetSortedStudentsList(bool(*)(const Student&, const Student&));
void OutputStudentInfo(Student*);
void SwitchMenu(IMenu*, IMenu* = nullptr);
void InitializeMenus();
void ChangeStudentProperty(EStudentProperty);

int main()
{
    SetConsoleCP(kSymbolsTable);
    SetConsoleOutputCP(kSymbolsTable);
    InitializeMenus();
    DBFileStreamRead.open(kDBFilePath, FileReadWriteFlags);
    if (!DBFileStreamRead)
    {
        //DBFileStream.open(kDBFilePath, FileReadWriteRecreateFlags);
        cout << "Файл не существовал, поэтому был создан пустой файл." << endl << endl;
    }
    while (DBFileStreamRead.peek() != EOF)
    {
        Student student;
        DBFileStreamRead.read((char*)&student, sizeof(Student));
        StudentsList.push_back(student);
    }
    DBFileStreamRead.close();

    MainMenu->BaseOnShow();
    system("pause");
    return 0;
}

char* snd;
int splitCount;
char** sndSplit;
int eraseIndex;
void ChangeStudentProperty(EStudentProperty property)
{
    ChoiceToChange = property;
    switch (property)
    {
    case SND: {
        cout << "ФИО: ";
        cin.get();
        snd = ReadLine();
        splitCount = 0;
        sndSplit = strSplit(snd, " ", splitCount);
        if (splitCount != 3)
        {
            free(snd);
            if (sndSplit)
            {
                for (int i = 0; i < _msize(sndSplit) / sizeof(size_t); i++)
                    free(sndSplit[i]);
                free(sndSplit);
            }
            cout << InvalidInputTypeException << endl;
            SwitchMenu(CurrentMenu->ParentMenu);
            return;
        }
        ActionToConfirm = []
        {
            memset(&TargetStudent->FullName, 0, sizeof(FullName));
            for (char i = 0; i < 3; i++)
            {
                memcpy((char*)(&TargetStudent->FullName.Surname) + kMaxStringLength * i, sndSplit[i], strlen(sndSplit[i]));
                cout << i << endl;
                //if (sndSplit[i])
                //    free(sndSplit[i]);
            }
            free(sndSplit);
            free(snd);
            UpdateDBFile();
        };
        SwitchMenu(ConfirmMenu, FindEditMenu);
        break;
    }
    case Marks:
        SwitchMenu(MarksMenu, FindEditMenu);
        break;
    case FCG:
        SwitchMenu(FCGMenu, FindEditMenu);
        break;
    }
}

int changeChoice;
int markIndex;
int i;
void InitializeMenus()
{
    MainMenu = new IMenu();
    CurrentMenu = MainMenu;
    FindEditMenu = new IMenu();
    ConfirmMenu = new IMenu();
    MarksMenu = new IMenu();
    FCGMenu = new IMenu();
    CreateStudentMenu = new IMenu();
    SortMenu = new IMenu();

    // MainMenu
    {
        MenuAction outPutAll =
        {
            "Вывести всех",
            []
            {
                if (StudentsList.size())
                {
                    cout << endl;
                    for (const auto& student : StudentsList)
                    {
                        OutputStudentInfo(const_cast<Student*>(&student));
                        cout << endl;
                    }
                }
                else
                {
                    cout << "Список студентов пуст." << endl;
                }
                SwitchMenu(CurrentMenu->ParentMenu);
            }
        };
        MenuAction actionRecreate =
        {
            "Очистить файл",
            []
            {
                DBFileStreamWrite.open(kDBFilePath, std::ios_base::trunc);
                DBFileStreamWrite.close();
            }
        };
        MenuAction switchToFindEdit =
        {
            "Поиск и редактирование",
            []
            {
                TargetStudent = nullptr;
                SwitchMenu(FindEditMenu);
            }
        };
        MenuAction actionAddStudent =
        {
            "Добавить студента",
            []
            {
                Student newStudent = Student();
                if (StudentsList.size())
                {
                    newStudent.LocalStudentId = 0;
                    auto sortedList = GetSortedStudentsList([](const Student& a, const Student& b) -> bool
                        {
                            return a.LocalStudentId < b.LocalStudentId;
                        });
                    for (int i = 1; i < sortedList.size(); i++)
                    {
                        if (sortedList[i].LocalStudentId - sortedList[i - 1].LocalStudentId > 1)
                        {
                            newStudent.LocalStudentId = sortedList[i - 1].LocalStudentId + 1;
                            break;
                        }
                    }
                    if (!newStudent.LocalStudentId)
                        newStudent.LocalStudentId = sortedList[sortedList.size() - 1].LocalStudentId + 1;
                }
                else
                {
                    newStudent.LocalStudentId = 1;
                }
                StudentsList.push_back(newStudent);
                UpdateDBFile();
                TargetStudent = &StudentsList[StudentsList.size() - 1];
                SwitchMenu(FindEditMenu);
            }
        };
        MenuAction actionDeleteStudent =
        {
            "Удалить студента",
            []
            {
                cout << "ID студента: ";
                int localStudentId;
                cin >> localStudentId;
                if (cin.fail())
                {
                    cin.clear();
                    cin.ignore();
                    cout << InvalidInputTypeException << endl;
                    SwitchMenu(CurrentMenu->ParentMenu);
                    return;
                }
                eraseIndex = -1;
                for (int i = 0; i < StudentsList.size(); i++)
                {
                    if (StudentsList[i].LocalStudentId == localStudentId)
                    {
                        eraseIndex = i;
                    }
                }
                if (eraseIndex == -1)
                {
                    cout << NoSuchStudentIdException << endl;
                    SwitchMenu(CurrentMenu->ParentMenu);
                    return;
                }
                ActionToConfirm = []
                {
                    StudentsList.erase(StudentsList.begin() + eraseIndex);
                    UpdateDBFile();
                };
                SwitchMenu(ConfirmMenu, MainMenu);
            }
        };
        MenuAction actionSortStudents =
        {
            "Сортировать список",
            []
            {
                SwitchMenu(SortMenu);
            }
        };
        MenuAction exitApp =
        {
            "Завершить",
            []
            {
                system("pause");
                exit(0);
            }
        };
        MainMenu->Actions = *new vector<MenuAction>();
        MainMenu->Actions.push_back(outPutAll);
        MainMenu->Actions.push_back(actionRecreate);
        MainMenu->Actions.push_back(switchToFindEdit);
        MainMenu->Actions.push_back(actionAddStudent);
        MainMenu->Actions.push_back(actionDeleteStudent);
        MainMenu->Actions.push_back(actionSortStudents);
        MainMenu->Actions.push_back(exitApp);
        MainMenu->ActionCompleted = []
        {
            SwitchMenu(MainMenu);
        };
        MainMenu->Caption = "Меню:";
        MainMenu->ParentMenu = MainMenu;
    }
    // MainMenu

    auto actionSwitchBack = *new MenuAction();
    actionSwitchBack.Description = "Назад";
    actionSwitchBack.OnHandle = []
    {
        if (CurrentMenu->ParentMenu)
        {
            SwitchMenu(CurrentMenu->ParentMenu);
        }
    };

    // FindEditMenu
    {
        MenuAction actionChooseSND = // SND = Surname Name Dadsurname
        {
            "ФИО",
            []
            {
                ChangeStudentProperty(SND);
            }
        };
        MenuAction actionChooseMark =
        {
            "Оценка",
            []
            {
                ChangeStudentProperty(Marks);
            }
        };
        MenuAction actionChooseFCG =
        {
            "Факультет/Курс/Группа",
            []
            {
                ChangeStudentProperty(FCG);
            }
        };
        FindEditMenu->Actions = *new vector<MenuAction>();
        FindEditMenu->Actions.push_back(actionChooseSND);
        FindEditMenu->Actions.push_back(actionChooseMark);
        FindEditMenu->Actions.push_back(actionChooseFCG);
        FindEditMenu->Actions.push_back(actionSwitchBack);
        FindEditMenu->OverrideOnShow = []
        {
            cout << "ID студента: ";
            if (TargetStudent)
            {
                cout << TargetStudent->LocalStudentId << endl;
                return;
            }
            int studentId;
            cin >> studentId;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore();
                cout << InvalidInputTypeException << endl;
                SwitchMenu(CurrentMenu->ParentMenu);
                return;
            }
            TargetStudent = nullptr;
            for (const auto& student : StudentsList)
            {
                if (student.LocalStudentId == studentId)
                {
                    TargetStudent = const_cast<Student*>(&student);
                    break;
                }
            }
            if (!TargetStudent)
            {
                cout << NoSuchStudentIdException << endl;
                SwitchMenu(CurrentMenu->ParentMenu);
            }
        };
        FindEditMenu->Caption = "Что изменить:";
        FindEditMenu->ParentMenu = MainMenu;
    }
    // FindEditMenu

    // ConfirmMenu
    {
        MenuAction actionConfirm =
        {
            "Да",
            []
            {
                ActionToConfirm();
            }
        };
        MenuAction actionCancel =
        {
            "Нет",
            nullptr
        };
        ConfirmMenu->Actions = *new vector<MenuAction>();
        ConfirmMenu->Actions.push_back(actionConfirm);
        ConfirmMenu->Actions.push_back(actionCancel);
        ConfirmMenu->ActionCompleted = []
        {
            SwitchMenu(CurrentMenu->ParentMenu);
            UpdateDBFile();
        };
        ConfirmMenu->Caption = "Вы уверены?";
    }
    // ConfirmMenu

    // MarksMenu
    {
        MarksMenu->Actions = *new vector<MenuAction>();
        MenuAction actionChooseCC1 =
        {
            ControlsNamesRU[0],
            []
            {
                markIndex = 0;
            }
        };
        MenuAction actionChooseIC1 =
        {
            ControlsNamesRU[1],
            []
            {
                markIndex = 1;
            }
        };
        MenuAction actionChooseCC2 =
        {
            ControlsNamesRU[2],
            []
            {
                markIndex = 2;
            }
        };
        MenuAction actionChooseIC2 =
        {
            ControlsNamesRU[3],
            []
            {
                markIndex = 3;
            }
        };
        MenuAction actionChooseExam =
        {
            ControlsNamesRU[4],
            []
            {
                markIndex = 4;
            }
        };
        MarksMenu->Actions.push_back(actionChooseCC1);
        MarksMenu->Actions.push_back(actionChooseIC1);
        MarksMenu->Actions.push_back(actionChooseCC2);
        MarksMenu->Actions.push_back(actionChooseIC2);
        MarksMenu->Actions.push_back(actionChooseExam);
        MarksMenu->ActionCompleted = []
        {
            cout << "Новый " << ControlsNamesRU[markIndex] << ": ";
            MarkType newMark;
            cin >> newMark;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore();
                cout << InvalidInputTypeException << endl;
                SwitchMenu(MainMenu);
                return;
            }
            *(&TargetStudent->Controls.CC1 + markIndex) = newMark;
            UpdateDBFile();
            SwitchMenu(MainMenu);
        };
        MarksMenu->ParentMenu = FindEditMenu;
        MarksMenu->Caption = "Оценка для изменения:";
    }
    // MarksMenu

    // FCGMenu
    {
        MenuAction actionFacultyChoice =
        {
            "Факультет",
            []()
            {
                changeChoice = 0;
            }
        };
        MenuAction actionCourseGroupChoice =
        {
            "Курс-Группа",
            []
            {
                changeChoice = 1;
            }
        };
        FCGMenu->ActionCompleted = []
        {
            cin.get();
            cout << FCGMenu->Actions[changeChoice].Description << ": ";
            auto inputLine = ReadLine();
            switch (changeChoice)
            {
                case 0:
                {
                    cout << "fac size: " << sizeof(FacultiesNamesRU) / sizeof(*FacultiesNamesRU) << endl;
                    TargetStudent->Faculty = static_cast<EFaculties>(0);
                    for (int i = 0; i < sizeof(FacultiesNamesRU) / sizeof(*FacultiesNamesRU); i++)
                    {
                        if (!strcmpic(inputLine, FacultiesNamesRU[i]))
                        {
                            TargetStudent->Faculty = static_cast<EFaculties>(i + 1);
                            break;
                        }
                    }
                    if (!TargetStudent->Faculty)
                    {
                        cout << "Введён несуществующий факультет. Факультет у студента был обнулён." << endl;
                    }
                    // TODO
                    break;
                }
                case 1:
                {
                    int splitCount;
                    auto inputSplit = strSplit(inputLine, "-", splitCount);
                    if (!inputSplit)
                    {
                        cin.clear();
                        cin.ignore();
                        cout << InvalidInputTypeException << endl;
                        SwitchMenu(CurrentMenu->ParentMenu);
                        goto FCGShowReturn;
                    }
                    TargetStudent->Course = atoi(inputSplit[0]);
                    TargetStudent->Group = atoi(inputSplit[1]);
                    switch (*((char*)inputSplit[1] + _msize(inputSplit[1]) - 1))
                    {
                    case 'М':
                    case 'м':
                        TargetStudent->Degree = Master;
                        break;
                    case 'А':
                    case 'а':
                        TargetStudent->Degree = Postgraduate;
                        break;
                    default:
                        TargetStudent->Degree = Bachelor;
                        break;
                    }

                    FCGShowReturn:
                    //if (splitCount)
                    //{
                    //    free(inputSplit[0]);
                    //    free(inputSplit[1]);
                    //    free(inputSplit);
                    //}
                    break;
                }
            }
            free(inputLine);
            UpdateDBFile();
            SwitchMenu(CurrentMenu->ParentMenu);
        };
        FCGMenu->Actions = *new vector<MenuAction>();
        FCGMenu->Actions.push_back(actionFacultyChoice);
        FCGMenu->Actions.push_back(actionCourseGroupChoice);
        //FCGMenu->ActionCompleted = []
        //{
        //    UpdateDBFile();
        //};
        FCGMenu->Caption = "Что изменить:";
    }
    // FCGMenu

    // SortMenu
    {
        MenuAction actionSortToLessFS = // FS = Final Score
        {
            "По убыванию ИС",
            []
            {
                ActionToConfirm = []
                {
                    StudentsList = GetSortedStudentsList([](const Student& a, const Student& b)
                        {
                            return a.GetTotalRITM() > b.GetTotalRITM();
                        });
                    UpdateDBFile();
                };
            }
        };
        MenuAction actionSortToHigherFS = // FS = Final Score
        {
            "По возрастанию ИС",
            []
            {
                ActionToConfirm = []
                {
                    StudentsList = GetSortedStudentsList([](const Student& a, const Student& b)
                        {
                            return a.GetTotalRITM() < b.GetTotalRITM();
                        });
                    UpdateDBFile();
                };
            }
        };
        MenuAction actionSortToHigherId = // FS = Final Score
        {
            "По возрастанию ID",
            []
            {
                ActionToConfirm = []
                {
                    StudentsList = GetSortedStudentsList([](const Student& a, const Student& b)
                        {
                            return a.LocalStudentId < b.LocalStudentId;
                        });
                    UpdateDBFile();
                };
            }
        };
        SortMenu->Actions = *new vector<MenuAction>();
        SortMenu->Actions.push_back(actionSortToLessFS);
        SortMenu->Actions.push_back(actionSortToHigherFS);
        SortMenu->Actions.push_back(actionSortToHigherId);
        SortMenu->ActionCompleted = []
        {
            SwitchMenu(ConfirmMenu, MainMenu);
        };
        SortMenu->Caption = "Метод сортировки:";
        SortMenu->ParentMenu = MainMenu;
    }
    //SortMenu
}

void SwitchMenu(IMenu* newMenu, IMenu* parentMenu)
{
    if (parentMenu)
        newMenu->ParentMenu = parentMenu;
    CurrentMenu = newMenu;
    if (CurrentMenu->OverrideOnShow)
        CurrentMenu->OverrideOnShow();
    if (!CurrentMenu->AllowOverride)
        CurrentMenu->BaseOnShow();
}

void OutputStudentInfo(Student* student)
{
    cout << "ID: " << student->LocalStudentId << endl;
    cout << " ФИО: ";
    if (*student->FullName.Surname)
        cout << student->FullName.Surname << ' ' << student->FullName.Name << ' ' << student->FullName.DadSurname;
    else
        cout << NotAvaibleInfoException;
    cout << endl;
    cout << " Факультет: ";
    if (student->Faculty)
        cout << FacultiesNamesRU[student->Faculty - 1];
    else
        cout << NotAvaibleInfoException;
    cout << endl;
    cout << " Группа: ";
    if (student->Course)
    {
        cout << static_cast<int>(student->Course) << '-' << static_cast<int>(student->Group);
        if (student->Degree)
        {
            switch (student->Degree)
            {
            case Master:
                cout << 'М';
                break;
            case Postgraduate:
                cout << 'А';
                break;
            }
        }
    }
    else
    {
        cout << NotAvaibleInfoException;
    }
    cout << endl;
    cout << " Оценка за экзамен: " << student->Controls.Exam << endl;
    const char* ratingStr = " Рейтинг: ";
    const char* ratingSpace = "     ";
    cout << ratingStr << endl;
    for (int j = 0; j < 4; j++)
    {
        const char* controlName = j % 2 ? "ПК" : "ТК";
        cout << ratingSpace << controlName << (j < 2 ? 1 : 2) << ": " << *(reinterpret_cast<float*>(&student->Controls) + j) << endl;
    }

    auto totalScore = student->GetTotalRITM();
    cout << " СИ: " << totalScore << endl; // СИ - Суммарный Индекс
    char* finalScore = 0;
    for (int j = 0; j < sizeof(MarkLimits) / sizeof(short); j++)
    {
        if (totalScore < MarkLimits[j])
        {
            finalScore = const_cast<char*>(NameOfFinalScore[j]);
            break;
        }
    }
    cout << " Итоговый балл: " << finalScore << endl;
}

vector<Student> GetSortedStudentsList(bool(*sortFunction)(const Student&, const Student&))
{
    vector<Student> newList(StudentsList);
    bool isSorted = true;
    while (isSorted)
    {
        isSorted = false;
        for (int i = 1; i < newList.size(); i++)
        {
            if (!sortFunction(newList[i - 1], newList[i]))
            //if (StudentsList[i].GetTotalRITM() > StudentsList[i - 1].GetTotalRITM())
            {
                std::swap(newList[i], newList[i - 1]);
                isSorted = true;
            }
        }
    }
    return newList;
}

void UpdateDBFile()
{
    DBFileStreamWrite.open(kDBFilePath, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    for (int i = 0; i < StudentsList.size(); i++)
    {
        DBFileStreamWrite.write((char*)&StudentsList[i], sizeof(Student));
    }
    //DBFileStreamWrite.write((char*)&StudentsList, sizeof(Student) * StudentsList.size());
    DBFileStreamWrite.close();
}