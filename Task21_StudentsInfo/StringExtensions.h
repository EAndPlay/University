#pragma once

// Returns input line
char* ReadLine();

// Returns index of "value" in "inputText"
int strFindIndex(const char* inputText, const char* value);

// Returns array of splitted strings
char** strSplit(const char* input_text, const char* separator);