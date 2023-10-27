//Лабораторная работа по программированию №1. "Пересечение трёх прямых" Вариант №21. Гонцов А.М 1-43

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <locale.h>

typedef struct tagPOINT
{
    float x;
    float y;
} point;

typedef struct tagLINE
{
    point p1;
    point p2;
} line;

typedef struct tagLINEFUNCTION
{
    line ready_line;
    float k; //коэффициент
    float b; //свободный член
    unsigned char flag; //является ли прямая вертикальной
} line_function;

typedef struct tagINTERSECTION_POINT
{
    point intersection;
    int first_line_num;
    int second_line_num;
} intersection_point;

//Запрашивает коодинаты двух точек прямой
void input_line_coords(line* target_line, const char* pre_input_text);

//Высчитывает коэффициенты прямых k и b
void init_lf(line_function* lf);

//Находит точку пересечения двух прямых
point find_intersection_point(line_function* lf1, line_function* lf2);

//Находит все точки пересечения данных прямых
intersection_point* find_intersections_points(line_function** lfs_array);

unsigned char check_lfs_validation(line_function** lfs_array);

const int kLinesCount = 3;

int main()
{
    setlocale(LC_ALL, "ru");
    line_function lf1, lf2, lf3;
    line_function** all_lfs = malloc(sizeof(size_t) * kLinesCount);
    all_lfs[0] = &lf1;
    all_lfs[1] = &lf2;
    all_lfs[2] = &lf3;
    printf("Формат ввода координат точек прямой: X1 Y1 X2 Y2 (Пример: 1 1 2 2)\n");
    input_line_coords(&lf1.ready_line, "Первая прямая: ");
    input_line_coords(&lf2.ready_line, "Вторая прямая: ");
    input_line_coords(&lf3.ready_line, "Третья прямая: ");
    init_lf(&lf1);
    init_lf(&lf2);
    init_lf(&lf3);

    //Проверка на то, смогут ли далее прямые пересечься
    if (!check_lfs_validation(all_lfs))
    {
        printf("Невозможно найти все пересечения\n");
        system("pause");
        return;
    }

    intersection_point* intersections = find_intersections_points(all_lfs);
    int intersections_count = _msize(intersections) / sizeof(intersection_point);
    if (intersections_count == 1)
    {
        printf("Все прямые пересекаются в точке:%5.2f; %5.2f\n", intersections[0].intersection.x, intersections[0].intersection.y);
    }
    else
    {
        printf("Не все прямые пересекаются в одной точке\n");
        //Выводит все точки в которых пересекаются прямые
        for (int i = 0; i < intersections_count; i++)
        {
            intersection_point inter_point = intersections[i];
            printf("Прямые %d и %d пересекаются в точке:%5.2f;%5.2f\n", inter_point.first_line_num, inter_point.second_line_num, inter_point.intersection.x, inter_point.intersection.y);
        }
    }
    system("pause");
}

void input_line_coords(line* target_line, const char* pre_input_text)
{
    printf(pre_input_text);
    if (!scanf_s("%f %f %f %f", &target_line->p1.x, &target_line->p1.y, &target_line->p2.x, &target_line->p2.y))
    {
        printf("Неправильный тип введённых данных\n");
        system("pause");
        exit(0);
    }
}

void init_lf(line_function* lf)
{
    point *p1 = &lf->ready_line.p1, *p2 = &lf->ready_line.p2;
    //При одинаковой X координате и различными Y прямая помечается как вертикальная
    if (p1->x == p2->x && p1->y != p2->y)
    {
        lf->flag = 1;
        lf->k = 0;
        lf->b = 0;
        return;
    }
    else
    {
        lf->flag = 0;
    }
    int delta_x = p1->x - p2->x;

    lf->k = delta_x != 0 ? fabs((p2->y - p1->y) / delta_x) : 0;
    if (lf->k 
        && ((p1->x < p2->x && p2->y < p1->y) 
            || (p1->x > p2->x && p1->y < p2->y)))
    {
        lf->k *= -1;
    }

    lf->b = p1->y - lf->k * p1->x;
}

point find_intersection_point(line_function* lf1, line_function* lf2)
{
    float x, y;
    if (lf1->flag || lf2->flag)
    {
        line_function *vertical, *non_vertical;
        if (lf1->flag)
        {
            vertical = lf1;
            non_vertical = lf2;
        }
        else
        {
            vertical = lf2;
            non_vertical = lf1;
        }
        x = vertical->ready_line.p1.x;
        y = non_vertical->k * x + non_vertical->b;
    }
    else
    {
        //Высчитывание координат пересечения по формуле y = k * x + b
        x = (lf1->b - lf2->b) / (lf2->k - lf1->k);
        y = lf1->k * x + lf1->b;
    }
    point point = { x, y };
    return point;
}

intersection_point* find_intersections_points(line_function** lfs_array)
{
    intersection_point* intersections = malloc(1);
    int result_size = 0;
    int array_length = _msize(lfs_array) / sizeof(size_t);
    for (int i = 0; i < array_length; i++)
    {
        for (int j = i + 1; j < array_length; j++)
        {
            point intersection = find_intersection_point(lfs_array[i], lfs_array[j]);

            //Проверка на наличие точки с такими же координатами, если такая есть, то точка пропускается
            unsigned char is_copy = 0;
            if (result_size != 0)
            {
                for (int k = 0; k < result_size; k++)
                {
                    point inter_point = intersections[k].intersection;
                    if (*(long long*)&inter_point == *(long long*)&intersection)
                    {
                        is_copy = 1;
                        break;
                    }
                }
            }
            if (is_copy) continue;

            result_size++;
            //Запись новой точки пересечения в список
            intersections = realloc(intersections, sizeof(intersection_point) * result_size);
            intersection_point inter_point = { intersection, i + 1, j + 1 };
            intersections[result_size - 1] = inter_point;
        }
    }
    return intersections;
}

unsigned char check_lfs_validation(line_function** lfs_array)
{
    unsigned char result = 1;
    int array_length = _msize(lfs_array) / sizeof(size_t);
    for (int i = 0; i < array_length; i++)
    {
        line_function* i_lf = lfs_array[i];
        line* i_line = &i_lf->ready_line;
        //Проверка на равность точек
        if (*(long long*)&i_line->p1 == *(long long*)&i_line->p2)
        {
            printf("Прямая %d не существует\n", i + 1);
            result = 0;
            continue;
        }
        for (int j = i + 1; j < array_length; j++)
        {
            line_function* j_lf = lfs_array[j];
            line* j_line = &j_lf->ready_line;
            char* warning_str = 0;
            //Проверка на равность прямых
            if (*(long long*)&i_line->p1 == *(long long*)&j_line->p1
                && *(long long*)&i_line->p2 == *(long long*)&j_line->p2)
            {
                warning_str = "равны";
                goto Warn;
            }
            else if (i_lf->k == j_lf->k)
            {
                warning_str = i_lf->b == j_lf->b ? "совпадают" : "параллельны";
                goto Warn;
            }
            continue;
        Warn:
            printf("Прямые %d и %d ", i + 1, j + 1);
            printf("%s\n", warning_str);
            result = 0;
        }
    }
    return result;
}