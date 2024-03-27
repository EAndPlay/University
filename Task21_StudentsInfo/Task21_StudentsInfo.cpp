//Лабораторная работа по программированию №7. "Проверка" Вариант №21. Гонцов А.М 1-43

#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <filesystem>

#include "StringExtensions.h"

using std::cout, std::cin, std::endl, std::vector, std::string, std::fstream;

enum class EStudentProperty : byte
{
    SND,
    Marks,
    FCG // Faculty Course Group
};

enum class EFaculties : byte
{
    ISCE = 1, // ИВТФ
    EF = 2, // ЭМФ
    HEF = 3, // ТЭФ
    FEP = 4, // ИФФ
    FEM = 5, // ФЭУ
    FEE = 6 // ЭЭФ
};

enum class EGraduateDegree : byte
{
    Bachelor,
    Master,
    Postgraduate
};

enum class ESortingType : byte
{
    Quick,
    Merge
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

typedef float MarkType;
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

    char* __thiscall GetCleanFullName()
    {
        const int kStringsCount = sizeof(FullName) / kMaxStringLength;

        auto fullName = (char*)&this->FullName;
        auto cleanSND = (char*)alloca(sizeof(FullName));

        int fullNameLength = 0;
        for (int i = 0; i < kStringsCount; i++)
        {
            //error
            cout << fullName;
            auto string = reinterpret_cast<char*>(fullName + kMaxStringLength * i);
            cout << string;
            auto strLength = strlen(string);
            memcpy((void*)(size_t(cleanSND) + fullNameLength), string, strLength);
            fullNameLength += strLength;
        }
        return (char*)realloc(cleanSND, fullNameLength);
    }
} Student;

const int kLineBufferSize = 1023;
const int kSymbolsTable = 1251;
const int kMaxFilePath = 255;

const char* kInvalidActionIdException = "Неверный номер действия.";
const char* kInvalidInputTypeException = "Неверный тип данных.";
const char* kStudentIdNotFoundException = "Студента с таким ID не найдено.";
const char* kStudentSNDNotFoundException = "Студента с таким ФИО не найдено.";
const char* kNotAvaibleInfoException = "N/A";
const char kDBFilePath[kMaxFilePath] = "Z:\\Development\\University\\Debug\\StudentsInfo.bin";

typedef Student* TreeNodeElementType;

typedef struct tagTREENODE
{
    TreeNodeElementType Element;

    tagTREENODE* RightNode;
    tagTREENODE* LeftNode;
    tagTREENODE* ParentNode;

    tagTREENODE(tagTREENODE* parentNode)
    {
        Element = nullptr;
        LeftNode = RightNode = ParentNode = nullptr;
        ParentNode = parentNode;
    }
    tagTREENODE() : tagTREENODE(nullptr) {}

    void* GetData(void* (*func)(tagTREENODE*))
    {
        return func(this);
    }

    void Delete()
    {
        for (tagTREENODE** i = &LeftNode; i < &RightNode; i++)
        {
            if (!*i) continue;
            delete(*i);
            *i = nullptr;
        }
        delete(this);
    }
} TreeNode;

typedef struct
{
    const char* Description;
    void (*OnHandle)();
} MenuAction;

typedef struct tagMENU
{
    tagMENU* ParentMenu;
    const char* Caption;
    vector<MenuAction> Actions;
    bool AllowOverride = false;
    bool OverrideAfterBase = false;
    void (*OverrideOnShow)() = nullptr;
    void (*ActionCompleted)() = nullptr;
    void __thiscall BaseOnShow()
    {
        cout << endl << Caption << endl;
        for (int i = 0; i < this->Actions.size(); i++)
        {
            cout << i + 1 << ": " << this->Actions[i].Description << endl;
        }
        int actionId;
        while (true)
        {
            cout << "Действие: ";
            cin >> actionId;
            cout << endl;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore();
                cout << "Должно быть введено число!" << endl;
                continue;
            }
            if (actionId < 1 || actionId > this->Actions.size())
            {
                cout << kInvalidActionIdException << endl;
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
static IMenu* BinaryTreeSearchMenu;

IMenu* CurrentMenu;

void(*ActionToConfirm)();
EStudentProperty ChoiceToChange;
Student* TargetStudent;

static vector<Student> StudentsList;
static std::ofstream DBFileStreamWrite;
static std::ifstream DBFileStreamRead;

static TreeNode* BinaryTreeStart;

const int FileReadWriteFlags = std::ios_base::in | std::ios_base::out | std::ios_base::binary;
const int FileReadWriteRecreateFlags = FileReadWriteFlags | std::ios_base::trunc;

void UpdateDBFile();
vector<Student> GetSortedStudentsList(bool(*)(const Student&, const Student&));
vector<Student> GetSortedStudentsList(ESortingType);
void* GetCleanSND(TreeNode*);
void GenerateBinaryTree();
void __fastcall OutputStudentInfo(Student*);
void __fastcall SwitchMenu(IMenu*, IMenu* = nullptr);
void InitializeMenus();
void ChangeStudentProperty(EStudentProperty);
inline void LeaveMenu()
{
    SwitchMenu(CurrentMenu->ParentMenu);
}

int main()
{
    SetConsoleCP(kSymbolsTable);
    SetConsoleOutputCP(kSymbolsTable);
    InitializeMenus();
    DBFileStreamRead.open(kDBFilePath, FileReadWriteFlags);
    if (!DBFileStreamRead)
    {
        cout << "Файл не существовал, поэтому был создан пустой файл." << endl << endl;
    }
    while (DBFileStreamRead.peek() != EOF)
    {
        Student student{};
        DBFileStreamRead.read((char*)&student, sizeof(Student));
        StudentsList.push_back(student);
    }
    DBFileStreamRead.close();
    BinaryTreeStart = nullptr;
    GenerateBinaryTree();

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
    case EStudentProperty::SND:
    {
        cout << "ФИО: ";
        if (*TargetStudent->FullName.Surname)
            cout << TargetStudent->FullName.Surname << ' ' << TargetStudent->FullName.Name << ' ' << TargetStudent->FullName.DadSurname;
        else
            cout << kNotAvaibleInfoException;
        cout << endl << "Новое ФИО: ";
        cin.get();
        snd = ReadLine();
        if (strlen(snd))
        {
            splitCount = 0;
            sndSplit = strSplit(snd, " ", splitCount);
            if (splitCount != 3)
            {
                free(snd);
                if (sndSplit)
                {
                    //for (int i = 0; i < splitCount; i++)
                    //    free(sndSplit[i]);
                    free(sndSplit);
                }
                cout << kInvalidInputTypeException << endl;
                LeaveMenu();
                return;
            }
            ActionToConfirm = []
            {
                memset(&TargetStudent->FullName, 0, sizeof(FullName));
                for (char i = 0; i < 3; i++)
                {
                    memcpy((char*)(&TargetStudent->FullName.Surname) + kMaxStringLength * i, sndSplit[i], strlen(sndSplit[i]));
                    //cout << i << endl;
                    //if (sndSplit[i])
                    //    free(sndSplit[i]);
                }
                free(sndSplit);
                free(snd);
                UpdateDBFile();
            };
            SwitchMenu(ConfirmMenu, FindEditMenu);
        }
        else
        {
            cout << "Ввод отменён." << endl << endl;
            SwitchMenu(FindEditMenu, MainMenu);
        }
        break;
    }
    case EStudentProperty::Marks:
    {
        SwitchMenu(MarksMenu, FindEditMenu);
        break;
    }
    case EStudentProperty::FCG:
    {
        SwitchMenu(FCGMenu, FindEditMenu);
        break;
    }
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
    BinaryTreeSearchMenu = new IMenu();

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
                LeaveMenu();
            }
        };
        MenuAction actionRecreate =
        {
            "Очистить файл и данные",
            []
            {
                ActionToConfirm = []
                {
                    DBFileStreamWrite.open(kDBFilePath, std::ios_base::trunc);
                    DBFileStreamWrite.close();
                    UpdateDBFile();
                };
                SwitchMenu(ConfirmMenu, MainMenu);
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
                    cout << kInvalidInputTypeException << endl;
                    LeaveMenu();
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
                    cout << kStudentIdNotFoundException << endl;
                    LeaveMenu();
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

    MenuAction actionSwitchBack;
    actionSwitchBack.Description = "Назад";
    actionSwitchBack.OnHandle = []
    {
        if (CurrentMenu->ParentMenu)
        {
            LeaveMenu();
        }
    };

    // FindEditMenu
    {
        MenuAction actionChooseSND = // SND = Surname Name Dadsurname
        {
            "ФИО",
            []
            {
                ChangeStudentProperty(EStudentProperty::SND);
            }
        };
        MenuAction actionChooseMark =
        {
            "Оценка",
            []
            {
                ChangeStudentProperty(EStudentProperty::Marks);
            }
        };
        MenuAction actionChooseFCG =
        {
            "Факультет/Курс/Группа",
            []
            {
                ChangeStudentProperty(EStudentProperty::FCG);
            }
        };
        FindEditMenu->Actions.push_back(actionChooseSND);
        FindEditMenu->Actions.push_back(actionChooseMark);
        FindEditMenu->Actions.push_back(actionChooseFCG);
        FindEditMenu->Actions.push_back(actionSwitchBack);
        FindEditMenu->OverrideOnShow = []
        {
            if (TargetStudent)
            {
                OutputStudentInfo(TargetStudent);
                return;
            }
            cout << "ID студента: ";
            int studentId;
            cin >> studentId;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore();
                cout << kInvalidInputTypeException << endl;
                LeaveMenu();
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
                cout << kStudentIdNotFoundException << endl;
                LeaveMenu();
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
        ConfirmMenu->Actions.push_back(actionConfirm);
        ConfirmMenu->Actions.push_back(actionCancel);
        ConfirmMenu->ActionCompleted = []
        {
            LeaveMenu();
            UpdateDBFile();
        };
        ConfirmMenu->Caption = "Вы уверены?";
    }
    // ConfirmMenu

    // MarksMenu
    {
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
        MarksMenu->Actions.push_back(actionSwitchBack);
        MarksMenu->ActionCompleted = []
        {
            cout << ControlsNamesRU[markIndex] << ": " << *(&TargetStudent->Controls.CC1 + markIndex) << endl;
            cout << "Новый " << ControlsNamesRU[markIndex] << ": ";
            MarkType newMark;
            cin >> newMark;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore();
                cout << kInvalidInputTypeException << endl;
                SwitchMenu(MarksMenu);
                return;
            }
            *(&TargetStudent->Controls.CC1 + markIndex) = newMark;
            UpdateDBFile();
            SwitchMenu(MarksMenu);
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
                if (!(int)TargetStudent->Faculty)
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
                    cout << kInvalidInputTypeException << endl;
                    LeaveMenu();
                    goto FCGShowReturn;
                }
                TargetStudent->Course = atoi(inputSplit[0]);
                TargetStudent->Group = atoi(inputSplit[1]);
                switch (*((char*)inputSplit[1] + _msize(inputSplit[1]) - 1))
                {
                case 'М':
                case 'м':
                    TargetStudent->Degree = EGraduateDegree::Master;
                    break;
                case 'А':
                case 'а':
                    TargetStudent->Degree = EGraduateDegree::Postgraduate;
                    break;
                default:
                    TargetStudent->Degree = EGraduateDegree::Bachelor;
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
            LeaveMenu();
        };
        FCGMenu->Actions.push_back(actionFacultyChoice);
        FCGMenu->Actions.push_back(actionCourseGroupChoice);
        FCGMenu->Actions.push_back(actionSwitchBack);
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
            "По убыванию СИ",
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
            "По возрастанию СИ",
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
        MenuAction actionSortToHigherId
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
        MenuAction actionSortToHigherFS_Quick =
        {
            "По возрастанию СИ (быстрая)",
            []
            {
                ActionToConfirm = []
                {
                    StudentsList = GetSortedStudentsList(ESortingType::Quick);
                    UpdateDBFile();
                };
            }
        };
        MenuAction actionSortToHigherFS_Merge = // FS = Final Score
        {
            "По возрастанию СИ (слиянием)",
            []
            {
                ActionToConfirm = []
                {
                    StudentsList = GetSortedStudentsList(ESortingType::Merge);
                    UpdateDBFile();
                };
            }
        };
        SortMenu->Actions.push_back(actionSortToLessFS);
        SortMenu->Actions.push_back(actionSortToHigherFS);
        SortMenu->Actions.push_back(actionSortToHigherId);
        SortMenu->Actions.push_back(actionSortToHigherFS_Quick);
        SortMenu->Actions.push_back(actionSortToHigherFS_Merge);
        SortMenu->Actions.push_back(actionSwitchBack);
        SortMenu->ActionCompleted = []
        {
            SwitchMenu(ConfirmMenu, MainMenu);
        };
        SortMenu->Caption = "Метод сортировки:";
        SortMenu->ParentMenu = MainMenu;
    }
    // SortMenu

    // BinaryTreeSearchMenu
    {

        BinaryTreeSearchMenu->Actions.push_back(actionSwitchBack);
        FindEditMenu->OverrideOnShow = []
        {
            cout << "ФИО студента: ";
            snd = ReadLine();
            if (strlen(snd))
            {
                splitCount = 0;
                sndSplit = strSplit(snd, " ", splitCount);
                if (splitCount > 3)
                {
                    free(snd);
                    if (sndSplit)
                    {
                        //for (int i = 0; i < splitCount; i++)
                        //    free(sndSplit[i]);
                        free(sndSplit);
                    }
                    cout << kInvalidInputTypeException << endl;
                    LeaveMenu();
                    return;
                }
                auto cleanSND = (char*) malloc(strlen(snd) - 2); // 2 = spaces between words
                int cleanSNDLength = 0;
                for (int i = 0; i < splitCount; i++)
                {
                    auto splitLength = strlen(sndSplit[i]);
                    memcpy((void*)(size_t(cleanSND) + cleanSNDLength), sndSplit[i], splitLength);
                    cleanSNDLength += splitLength;
                }
                free(sndSplit);
                free(snd);
            }
            else
            {
                cout << "Ввод отменён." << endl << endl;
                SwitchMenu(MainMenu, MainMenu);
            }
            TargetStudent = nullptr;
            auto treeNode = BinaryTreeStart;
            auto cleanNewSND = StudentsList[i].GetCleanFullName();
            char* cleanSourceSND = nullptr;
            while (treeNode)
            {
                auto cleanSourceSND = (char*)treeNode->GetData(GetCleanSND);
                auto cmpStrs = strcmpico(cleanNewSND, cleanSourceSND);
                if (cmpStrs == -1 || cmpStrs == 3)
                {
                    TargetStudent = treeNode->Element;
                    free(cleanSourceSND);
                    goto CycleExit;
                }
                auto nodeToPlace = &treeNode->RightNode + cmpStrs;
                if (*nodeToPlace)
                {
                    treeNode = *nodeToPlace;
                    free(cleanSourceSND);
                }
                else
                {
                    *nodeToPlace = new TreeNode(treeNode);
                    (*nodeToPlace)->Element = &StudentsList[i];
                }
            }
            CycleExit:
            free(cleanNewSND);
            if (!TargetStudent)
            {
                cout << kStudentSNDNotFoundException << endl;
                LeaveMenu();
            }
            else
            {
                SwitchMenu(FindEditMenu, MainMenu);
            }
        };
        BinaryTreeSearchMenu->ActionCompleted = []
        {
            SwitchMenu(ConfirmMenu, MainMenu);
        };
        BinaryTreeSearchMenu->AllowOverride = true;
        BinaryTreeSearchMenu->Caption = "Поиск в бинарном дереве по ФИО:";
        BinaryTreeSearchMenu->ParentMenu = MainMenu;
    }
    // BinaryTreeSearchMenu
}

void __fastcall SwitchMenu(IMenu* newMenu, IMenu* parentMenu)
{
    if (parentMenu)
        newMenu->ParentMenu = parentMenu;
    CurrentMenu = newMenu;
    if (CurrentMenu->OverrideOnShow)
        CurrentMenu->OverrideOnShow();
    if (!CurrentMenu->AllowOverride)
        CurrentMenu->BaseOnShow();
}

void __fastcall OutputStudentInfo(Student* student)
{
    cout << "ID: " << student->LocalStudentId << endl;
    cout << " ФИО: ";
    if (*student->FullName.Surname)
        cout << student->FullName.Surname << ' ' << student->FullName.Name << ' ' << student->FullName.DadSurname;
    else
        cout << kNotAvaibleInfoException;
    cout << endl;
    cout << " Факультет: ";
    if ((int)student->Faculty)
        cout << FacultiesNamesRU[(int)student->Faculty - 1];
    else
        cout << kNotAvaibleInfoException;
    cout << endl;
    cout << " Группа: ";
    if (student->Course)
    {
        cout << static_cast<int>(student->Course) << '-' << static_cast<int>(student->Group);
        if ((int)student->Degree)
        {
            switch (student->Degree)
            {
            case EGraduateDegree::Master:
                cout << 'М';
                break;
            case EGraduateDegree::Postgraduate:
                cout << 'А';
                break;
            }
        }
    }
    else
    {
        cout << kNotAvaibleInfoException;
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

void __fastcall QuickSortInternal(vector<Student>& array, int startIndex, int count)
{
    auto left = startIndex;
    auto right = count - 1;
    auto value = array[(startIndex + count) / 2].GetTotalRITM();
    do
    {
        while (array[left].GetTotalRITM() < value)
            left++;

        while (array[right].GetTotalRITM() > value)
            right--;

        if (left <= right)
        {
            std::swap(array[left], array[right]);
            left++;
            right--;
        }
    } while (left <= right);

    if (startIndex < right)
        QuickSortInternal(array, right, count);
    if (count > left)
        QuickSortInternal(array, startIndex, left);
}

void __fastcall MergeInternal(std::vector<Student>& unsortedArray, std::vector<Student>& sortedArray, int i, int rightIndex, int size)
{
    int j = i + rightIndex;
    int n1 = min(j, size);
    int n2 = min(j + rightIndex, size);
    int k = i;

    while (i < n1 && j < n2)
        sortedArray[k++] = unsortedArray[(unsortedArray[i].GetTotalRITM() <= unsortedArray[j].GetTotalRITM() ? i : j)++];
    while (i < n1)
        sortedArray[k++] = unsortedArray[i++];
    while (j < n2)
        sortedArray[k++] = unsortedArray[j++];
}

vector<Student> GetSortedStudentsList(ESortingType sortingType)
{
    auto listClone = std::move(StudentsList);
    auto arraySize = listClone.size();
    switch (sortingType)
    {
        case ESortingType::Quick:
        {
            QuickSortInternal(listClone, 0, arraySize);
            break;
        }
        case ESortingType::Merge:
        {
            vector<Student> tempArray(arraySize);
            int rightIndex = 1;
            int i;
            while (rightIndex < arraySize)
            {
                for (i = 0; i < arraySize; i += 2 * rightIndex)
                    MergeInternal(listClone, tempArray, i, rightIndex, arraySize);
                rightIndex *= 2;
                for (i = 0; i < arraySize; i += 2 * rightIndex)
                    MergeInternal(tempArray, listClone, i, rightIndex, arraySize);
                rightIndex *= 2;
            }
            break;
        }
    }
    return listClone;
}

void* GetCleanSND(TreeNode* treeNode)
{
    return treeNode->Element->GetCleanFullName();
}

void GenerateBinaryTree()
{
    if (BinaryTreeStart)
    {
        BinaryTreeStart->Delete();
    }
    BinaryTreeStart = new TreeNode();
    BinaryTreeStart->Element = &StudentsList[0];

    for (int i = 1; i < StudentsList.size(); i++)
    {
        auto treeNode = BinaryTreeStart;
        auto cleanNewSND = StudentsList[i].GetCleanFullName();
        char* cleanSourceSND = nullptr;
        while (treeNode)
        {
            auto cleanSourceSND = (char*)treeNode->GetData(GetCleanSND);
            auto cmpStrs = strcmpico(cleanNewSND, cleanSourceSND);
            if (cmpStrs == -1) break; // if (cmpStrs & 0x8000'0000)
            if (cmpStrs >= 2) cmpStrs -= 2;
            auto nodeToPlace = &treeNode->RightNode + cmpStrs;
            if (*nodeToPlace)
            {
                treeNode = *nodeToPlace;
                free(cleanSourceSND);
            }
            else
            {
                *nodeToPlace = new TreeNode(treeNode);
                (*nodeToPlace)->Element = &StudentsList[i];
            }
        }
        free(cleanNewSND);
    }
}

void UpdateDBFile()
{
    GenerateBinaryTree();
    DBFileStreamWrite.open(kDBFilePath, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    for (int i = 0; i < StudentsList.size(); i++)
    {
        DBFileStreamWrite.write((char*)&StudentsList[i], sizeof(Student));
    }
    //DBFileStreamWrite.write((char*)&StudentsList, sizeof(Student) * StudentsList.size());
    DBFileStreamWrite.close();
}
