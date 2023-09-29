#include <iostream>
#include <locale>

int main()
{
    setlocale(LC_ALL, "ru");

    std::cout << "������� ����������� ����� �����: ";
    int value;

    std::cin >> value;
    if (std::cin.fail())
    {
        std::cout << "������������ ��� �������� ������" << std::endl;
        system("pause");
        return 0;
    }

    if (value <= 0)
    {
        std::cout << "������� ������������� �����" << std::endl;
        system("pause");
        return 0;
    }

    //������� ���� �������� ���� ���������
    for (int i = value; i > 1; i--)
    {
        bool is_found = true;

        //������� ��������� ����� i
        for (int j = i; j > 1; j--)
        {
            //���� ������� �� ������� � ����� ����� ����� 0
            //�� � ��� ���� ����� ��������
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