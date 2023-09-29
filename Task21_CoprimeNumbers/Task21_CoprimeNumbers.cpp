#include <iostream>
#include <locale>

int main()
{
    setlocale(LC_ALL, "ru");

    std::cout << "Введите натуральное число число: ";
    int value;

    std::cin >> value;
    if (std::cin.fail())
    {
        std::cout << "Неправильный тип введённых данных" << std::endl;
        system("pause");
        return 0;
    }

    if (value <= 0)
    {
        std::cout << "Введено ненатуральное число" << std::endl;
        system("pause");
        return 0;
    }

    //Перебор всех значений ниже введённого
    for (int i = value; i > 1; i--)
    {
        bool is_found = true;

        //Перебор делителей числа i
        for (int j = i; j > 1; j--)
        {
            //Если остаток от деления у обоих чисел равен 0
            //То у них есть общий делитель
            if (i % j == 0 && value % j == 0)
            {
                is_found = false;
                break;
            }
        }

        if (is_found)
            std::cout << i << std::endl;
    }
}