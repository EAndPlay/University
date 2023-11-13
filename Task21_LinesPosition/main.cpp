#include <iostream>
#include <cmath>
#include <windows.h>

typedef float ValueType;

typedef struct my_tagVECTOR3
{
    ValueType X;
    ValueType Y;
    ValueType Z;

    inline my_tagVECTOR3 operator-(const my_tagVECTOR3& rvalue) const
    {
        return my_tagVECTOR3{ this->X - rvalue.X, this->Y - rvalue.Y, this->Z - rvalue.Z };
    }

    inline my_tagVECTOR3 operator+(const my_tagVECTOR3& rvalue) const
    {
        return my_tagVECTOR3{ this->X + rvalue.X, this->Y + rvalue.Y, this->Z + rvalue.Z };
    }

    inline my_tagVECTOR3 operator*(const ValueType rvalue) const
    {
        return my_tagVECTOR3{ this->X * rvalue, this->Y * rvalue, this->Z * rvalue };
    }
} Vector3;

typedef struct
{
    Vector3 Point1;
    Vector3 Point2;
} Line3;

const ValueType Epsilon = 1.0e-4;

//Скалярное произведение векторов
inline ValueType vectorDot(Vector3&, Vector3&);

void InitializeCoefficients(Line3&);

int main()
{
    SetConsoleOutputCP(1251);
    int linesCount = 0;

    std::cout << "Кол-во прямых: ";
    std::cin >> linesCount;
    if (std::cin.fail())
    {
        std::cout << std::endl << "Введён неверный тип данных. Начните весь ввод заного." << std::endl;
        system("pause");
        return 0;
    }
    auto* linesList = (Line3*)malloc(linesCount * sizeof(Line3));
    std::cout << "Введите точки 4-ёх или более прямых" << std::endl << "Формат ввода точек: X1 Y1 Z1 X2 Y2 Z3" << std::endl;

    for (int i = 0; i < linesCount; i++)
    {
        std::cout << i + 1 << ": ";
        for (int j = 0; j < 6; ++j)
        {
            std::cin >> *((float*)&linesList[i] + j);
            if (std::cin.fail())
            {
                std::cout << std::endl << "Введён неверный тип данных. Начните весь ввод заного." << std::endl;
            }
        }
    }

    linesList = (Line3*)realloc(linesList, (linesCount) * sizeof(Line3));

    int notParallelOrInstersectedLines = 0;
    for (int i = 0; i < linesCount; i++)
    {
        auto lineI = linesList[i];
        for (int j = i + 1; j < linesCount; j++)
        {
            std::cout << std::endl;
            std::cout << "Прямые " << i + 1 << " и " << j + 1 << ": ";
            auto lineJ = linesList[j];

            auto vectorDiffI = lineI.Point2 - lineI.Point1;
            auto vectorDiffJ = lineJ.Point2 - lineJ.Point1;
            auto startDiff = lineI.Point1 - lineJ.Point1;
            auto dotI = vectorDot(vectorDiffI, vectorDiffI);
            auto dotIJ = vectorDot(vectorDiffI, vectorDiffJ);
            auto dotIp = vectorDot(vectorDiffI, startDiff);
            auto dotJJ = vectorDot(vectorDiffJ, vectorDiffJ);
            auto dotJp = vectorDot(vectorDiffJ, startDiff);
            auto dotDiff = dotI * dotJJ - dotIJ * dotIJ;
            if (dotDiff > Epsilon)
            {
                auto s = (dotIJ * dotJp - dotIp * dotJJ) / dotDiff;
                auto t = (dotI * dotJp - dotIp * dotIJ) / dotDiff;
                auto p = lineI.Point1 + vectorDiffI * s;
                auto q = lineJ.Point1 + vectorDiffJ * t;
                auto p1 = p - q;
                if (vectorDot(p1, p1) > Epsilon)
                {
                    notParallelOrInstersectedLines++;
                    std::cout << "скрещиваются";
                    continue;
                }
                std::cout << "пересекаются";
                continue;
            }
            std::cout << "параллельны";
        }
        std::cout << std::endl;
    }
    if (linesCount == notParallelOrInstersectedLines)
        std::cout << "Все прямеы соответсвуют условию." << std::endl;
    system("pause");
}
//0 1 1 0 2 2 1 1 1 1 2 2 2 1 1 2 2 2 3 1 1 3 2 2 0 1 1 0 2 2 1 1 1 1 2 2 2 1 1 2 2 2 3 1 1 3 2 2 0 0 2 0 1 0
//0 0 0 1 1 1 1 1 1 0 1 1

ValueType vectorDot(Vector3& lv, Vector3& rv)
{
    return lv.X * rv.X + lv.Y * rv.Y + lv.Z * rv.Z;
}
