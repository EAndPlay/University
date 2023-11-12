#include <iostream>
#include <cstring>
#include <locale>
#include <cmath>
#include <limits>
#include <windows.h>

//0 0 0 1 1 1

typedef float ValueType;
//constexpr ValueType valueabs(ValueType value)
//{
//    return typeid(ValueType) == typeid(float) ? fabs(value) : abs(value); 
//}

typedef struct
{
    ValueType X;
    ValueType Y;
} Point2;

typedef struct my_tagVECTOR3
{
    ValueType X;
    ValueType Y;
    ValueType Z;
    
    inline bool operator==(const my_tagVECTOR3& rvalue) const
    {
        return this->X == rvalue.X && this->Y == rvalue.Y && this->Z == rvalue.Z;
        //return std::memcpy((my_tagVECTOR3*)this, &rvalue, sizeof(my_tagVECTOR3));
    }
    
    inline my_tagVECTOR3 operator-(const my_tagVECTOR3& rvalue) const
    {
        return my_tagVECTOR3 {this->X - rvalue.X, this->Y - rvalue.Y, this->Z - rvalue.Z};
        //return std::memcpy((my_tagVECTOR3*)this, &rvalue, sizeof(my_tagVECTOR3));
    }
    
    inline my_tagVECTOR3 operator+(const my_tagVECTOR3& rvalue) const
    {
        return my_tagVECTOR3 {this->X + rvalue.X, this->Y + rvalue.Y, this->Z + rvalue.Z};
    }
    
    inline my_tagVECTOR3 operator*(const my_tagVECTOR3& rvalue) const
    {
        return my_tagVECTOR3 {this->X * rvalue.X, this->Y * rvalue.Y, this->Z * rvalue.Z};
    }
    
    inline my_tagVECTOR3 operator*(const ValueType rvalue) const
    {
        return my_tagVECTOR3 {this->X * rvalue, this->Y * rvalue, this->Z * rvalue};
    }
} Vector3;

typedef struct
{
    Vector3 Point1;
    Vector3 Point2;
} Line3;

const ValueType Epsilon = 1.0e-4;

//Скалярное произведение векторов
ValueType vectorDot(Vector3&, Vector3&);

void InitializeCoefficients(Line3&);

int main()
{
    SetConsoleOutputCP(1251);
    auto *linesList = (Line3*) malloc(sizeof(Line3));
    int linesCount = 0;
    
    std::cout << "Введите точки 4-ёх или более прямых. Чтобы закончить ввод - на пустой строке нажмите Enter." << std::endl << "Формат ввода точек: X1 Y1 Z1 X2 Y2 Z3" << std::endl;

    while (true)
    {
        StartInput:
        std::cout << linesCount + 1 << ": ";
        for (int i = 0; i < 6; ++i)
        {
            std::cin >> *((float*)&linesList[linesCount] + i);
            if (std::cin.fail())
            {
                if (!i && std::cin.fail())
                    goto AfterCycle;
                std::cout << std::endl << "Введён неверный тип данных. Введите точки прямой заного." << std::endl;
                goto StartInput;
            }
        }
        InitializeCoefficients(linesList[linesCount]);
        linesCount++;
        linesList = (Line3*) realloc(linesList, (linesCount + 1) * sizeof(Line3));
        if (linesCount == 2)
            break;
    }
    if (true) {}
    AfterCycle:
    linesList = (Line3*) realloc(linesList, (linesCount) * sizeof(Line3));

    for (int i = 0; i < linesCount; i++)
    {
        auto lineI = linesList[i];
        auto vector3I = lineI.Point2 - lineI.Point1;//Vector3 {lineI.Point2.X - lineI.Point1.X, lineI.Point2.Y - lineI.Point1.Y, lineI.Point2.Z - lineI.Point1.Z};
        auto vector3ILength = std::sqrt(vector3I.X * vector3I.X + vector3I.Y * vector3I.Y + vector3I.Z * vector3I.Z);
        for (int j = i + 1; j < linesCount; j++)
        {
            std::cout << std::endl;
            std::cout << "Прямые " << i + 1 << " и " << j + 1 << ": ";
            auto lineJ = linesList[j];
            
//            auto vector3J = Vector3 {lineJ.Point2.X - lineJ.Point1.X, lineJ.Point2.Y - lineJ.Point1.Y, lineJ.Point2.Z - lineJ.Point1.Z};
//            auto vector3JLength = std::sqrt(vector3J.X * vector3J.X + vector3J.Y * vector3J.Y + vector3J.Z * vector3J.Z);
//            
//            //Скалярное произведение векторов
//            auto multVector = vector3I.X * vector3J.X + vector3I.Y * vector3J.Y + vector3I.Z * vector3J.Z;
//            //Косинус угла между векторами
//            auto multVectorCos = multVector / vector3ILength / vector3JLength;
//            //std::cout << i + 1 << ' ' << j + 1 << " cos = " <<  << std::endl;
//            if (!multVector)
//            {
//                std::cout << "пересекаются (перпендикулярны)";
//                continue;
//            }
//            if (multVectorCos == 1 || lineI.ZLean == lineJ.ZLean && lineI.XYLean == lineJ.XYLean)
//            {
//                std::cout << "параллельны";
//                continue;
//            }

            auto vectorDiffI = lineI.Point2 - lineI.Point1;
            auto vectorDiffJ = lineJ.Point2 - lineJ.Point1;
            auto pointDiff = lineI.Point1 - lineJ.Point1;
            auto dotI = vectorDot(vectorDiffI, vectorDiffI);
            auto dotIJ = vectorDot(vectorDiffI, vectorDiffJ);
            auto dotIp = vectorDot(vectorDiffI, pointDiff);
            auto dotJJ = vectorDot(vectorDiffJ, vectorDiffJ);
            auto dotJp = vectorDot(vectorDiffJ, pointDiff);
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
}
//0 1 1 0 2 2 1 1 1 1 2 2 2 1 1 2 2 2 3 1 1 3 2 2 0 1 1 0 2 2 1 1 1 1 2 2 2 1 1 2 2 2 3 1 1 3 2 2
//0 0 0 1 1 1 ; 1 1 1 0 1 1

ValueType vectorDot(Vector3& lv, Vector3& rv)
{
    return lv.X * rv.X + lv.Y * rv.Y + lv.Z * rv.Z;
}

void InitializeCoefficients(Line3 &line)
{
    auto &p1 = line.Point1, &p2 = line.Point2;
//    auto deltaX = p2.X - p1.X;
//    auto deltaY = p2.Y - p1.Y;
//    auto deltaZ = p2.Z - p1.Z;
//    auto xyLength = std::sqrt(deltaX * deltaX + deltaY * deltaY);
//    line->ZLean = xyLength ? deltaZ / xyLength : 0;
//    line->XYLean = deltaX ? deltaY / deltaX : 0;
//    
////    line->Origin.X = p1.X - line->XYLean * p1.Y;
////    if (!line->XYLean)
////        line->Origin.Y = 0;
////    else
////        line->Origin.Y = p1.Y - line->XYLean * p1.X;
////    
////    std::cout << line->Origin.Y << std::endl;
//    std::cout << line->XYLean << ' ' << line->ZLean << std::endl;
//    //std::cout << "origin: " << line->Origin.X << ' ' << line->Origin.Y << ' ' << line->Origin.Z << std::endl;

//    auto deltaX = p1.X - p2.X;
//    auto deltaY = p1.Y - p2.Y;
//
//    line->XY.Coefficient_K = abs((p2.Y - p1.Y) / deltaX);
//    if (line->XY.Coefficient_K && ((p1.X < p2.X && p2.Y < p1.Y) || (p1.X > p2.X && p1.Y < p2.Y)))
//        line->XY.Coefficient_K *= -1;
//
//    line->XZ.Coefficient_K = abs((p2.Z - p1.Z) / deltaX);
//    if (line->XZ.Coefficient_K && ((p1.X < p2.X && p2.Z < p1.Z) || (p1.X > p2.X && p1.Z < p2.Z)))
//        line->XZ.Coefficient_K *= -1;
//
//    line->YZ.Coefficient_K = abs((p2.Z - p1.Z) / deltaY);
//    if (line->YZ.Coefficient_K && ((p1.Y < p2.Y && p2.Z < p1.Z) || (p1.Y > p2.Y && p1.Z < p2.Z)))
//        line->YZ.Coefficient_K *= -1;
//
//    line->Origin.X = p1.X - line->XY.Coefficient_K * p1.Y;
//    line->Origin.Y = p1.Y - line->XY.Coefficient_K * p1.X;
//    line->Origin.Z = p1.Z - line->XZ.Coefficient_K * p1.X;
//    //std::cout << p1.X << ' ' << (p2.Y - p1.Y) / 1 << ' ' << p1.Y << std::endl;
//    std::cout << line->XY.Coefficient_K << ' ' << line->XZ.Coefficient_K << ' ' << line->YZ.Coefficient_K << std::endl;
    
//    auto lineFunction = (LineFunction3*)(((Vector3*) line) + 2);
//    for (int i = 0; i < Point3Size; ++i)
//    {
//        auto coord1 = *((ValueType*)line + i);
//        auto coord2 = *(&coord1 + Point3Size + 1);
//    }
}