#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <locale.h>

struct tagPOINT
{
    float x;
    float y;
}
typedef point;

struct tagLINE
{
    point p1;
    point p2;
}
typedef line;

struct tagLINEFUNCTION
{
    line ready_line;
    float k; //coefficient
    float b; //free coefficient
    unsigned char flag; //is vertical
}
typedef line_function;

inline void input_line_coords(line* target_line, const char* pre_input_text);

void init_lf(line_function* lf);

point find_intersection_point(line_function* lf1, line_function* lf2);

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
    printf("Формат ввода координат точек прямой: X1 Y1 X2 Y2\n");
    input_line_coords(&lf1.ready_line, "Первая прямая: ");
    input_line_coords(&lf2.ready_line, "Вторая прямая: ");
    input_line_coords(&lf3.ready_line, "Третья прямая: ");
    init_lf(&lf1);
    init_lf(&lf2);
    init_lf(&lf3);
    if (!check_lfs_validation(all_lfs))
    {
        printf("loh\n");
        return 0;
    }
    point intersect1 = find_intersection_point(&lf1, &lf2);
    point intersect2 = find_intersection_point(&lf1, &lf3);
    point intersect3 = find_intersection_point(&lf2, &lf3);
    if (*(long long*)&intersect1 != *(long long*)&intersect2 
        || *(long long*)&intersect1 != *(long long*)&intersect3 
        || *(long long*)&intersect2 != *(long long*)&intersect3)
    //if (intersect1.x != intersect2.x || intersect1.y != intersect2.y || intersect1.x != intersect3.x || intersect1.y != intersect3.y || intersect2.x != intersect3.x
    //    || intersect2.y != intersect3.y)
    {
        printf("Не все прямые пересекаются в одной точке");
    }
    else
    {
        printf("Пересечение:%5.2f %5.2f", intersect1.x, intersect1.y);
    }
    //printf("%i %i %i\n", intersect1.x == intersect2.x, intersect1.x == intersect3.x, intersect2.x == intersect3.x);
    //printf("inter1:%5.2f %5.2f\n", intersect1.x, intersect1.y);
    //printf("inter2:%5.2f %5.2f\n", intersect2.x, intersect2.y);
    //printf("inter3:%5.2f %5.2f\n", intersect3.x, intersect3.y);
}

void input_line_coords(line* target_line, const char* pre_input_text)
{
    printf(pre_input_text);
    scanf_s("%f %f %f %f", &target_line->p1.x, &target_line->p1.y, &target_line->p2.x, &target_line->p2.y);
    //printf("\n");
}

void init_lf(line_function* lf)
{
    point *p1 = &lf->ready_line.p1, *p2 = &lf->ready_line.p2;
    int delta_x = p1->x - p2->x;

    lf->k = delta_x != 0 ? fabs((p2->y - p1->y) / delta_x) : 0;
    if ((p1->x < p2->x && p2->y < p1->y)
        || (p1->x > p2->x && p1->y < p2->y))
    {
        lf->k *= -1;
    }

    lf->b = p1->y - lf->k * p1->x;

    if (p1->x == p2->x)
    {
        lf->flag = 1;
    }
}

point find_intersection_point(line_function* lf1, line_function* lf2)
{
    float x = (lf1->b - lf2->b) / (lf2->k - lf1->k);
    float y = lf1->k * x + lf1->b;
    //printf("%f %f\n", x, y);
    point point = { x, y };
    return point;
}

unsigned char check_lfs_validation(line_function** lfs_array)
{
    unsigned char result = 1;
    int array_length = _msize(lfs_array) / sizeof(size_t);
    for (int i = 0; i < array_length; i++)
    {
        line_function* i_lf = lfs_array[i];
        line* i_line = &i_lf->ready_line;
        if (*(long long*)&i_line->p1 == *(long long*)&i_line->p2)
        //if (i_line->p1.x == i_line->p2.x && i_line->p1.y == i_line->p2.y)
        {
            printf("Прямая %d не существует\n", i + 1);
            continue;
        }
        for (int j = i + 1; j < array_length; j++)
        {
            line_function* j_lf = lfs_array[j];
            line* j_line = &j_lf->ready_line;
            char* warning_str = 0;
            if (*(long long*)&i_line->p1 == *(long long*)&j_line->p1
                && *(long long*)&i_line->p2 == *(long long*)&j_line->p2)
            {
                warning_str = "равны";
                goto Warn;
            }
            else if (i_lf->k == j_lf->k)
            {
                warning_str = i_lf->b == j_lf->b ? "совпадают" : "параллельны";
                //if (i_lf->b == j_lf->b)
                //{
                //    warning_str = "совпадают";
                //}
                //else// if (i_lf->flag == j_lf->flag)
                //{
                //    warning_str = "параллельны";
                //}
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