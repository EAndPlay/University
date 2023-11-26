#include <memory>

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