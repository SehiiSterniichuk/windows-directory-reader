#include <iostream>
#include <map>
#include <string>
#include <windows.h>
#include <strsafe.h>
#include <stdio.h>
#include "vector"

#define CONSOLE_CAPACITY 80//максимальне число символів для найбільшого результату гістограми
using namespace std;

const int interval = 1024 * 50;//інтервал
map<string, int> statistic;//string - інтервал, int - лічильник файлів, що входять у цей інтервал
long maxCounter = 0;//максимальна кількість файлів, що потрапляла у проміжок

int dwordToInt(DWORD high, DWORD low);

void countSizeToRange(int size);/*знаходимо проміжок якому належить розмір файлу
 * та інкрементуємо лічильник проміжку*/

void countFile(WIN32_FIND_DATAA file) {//дізнаємося розмір файлу
    int size = dwordToInt(file.nFileSizeHigh, file.nFileSizeLow);
    countSizeToRange(size);
}

void scanAllFilesInDirectory(const string &path) {
    //функція сканує всі файли за даним шляхом
    string directoryPath = path;
    directoryPath += "\\*"; // шлях до папки
    CHAR *curDir = new CHAR[MAX_PATH]{'0'};
    StringCchCopy(((STRSAFE_LPSTR) curDir), MAX_PATH, ((STRSAFE_LPSTR) directoryPath.c_str()));
    /*StringCchCopy - Копіює один рядок в інший.
     * Розмір буфера призначення надається функції,
     * щоб гарантувати, що вона не записує кінець цього буфера.*/
    WIN32_FIND_DATAA file_data;
    HANDLE h_file = FindFirstFileA(curDir, &file_data);
    /*FindFirstFileA - Шукає в каталозі файл або підкаталог з іменем, яке відповідає певному імені
     * (або частковому імені, якщо використовуються символи підстановки).
     *
     * Якщо функції не вдається або не вдається знайти файли з рядка пошуку в параметрі curDir,
     * повертається значення INVALID_HANDLE_VALUE, а вміст file_data є невизначеним.*/
    if (INVALID_HANDLE_VALUE == h_file) {
        cout << "Error in " << directoryPath << endl;
        return;
    }
    do {
        if (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {//перевіряємо чи це папка
            if (strcmp(file_data.cFileName, ".") &&
                strcmp(file_data.cFileName, "..")) { // Пропускаємо папки "." та ".."
                string next = path + '\\' + file_data.cFileName;  // створюємо шлях до папки
                scanAllFilesInDirectory(next); //рекурсивно заходимо в неї
            }
        } else {//інакше це файл
            countFile(file_data);//рахуємо в якому проміжку належить розмір файлу
        }
    } while (FindNextFileA(h_file, &file_data) != 0);
    /* FindNextFileA - Продовжує пошук файлу
     * Якщо функція виконується успішно, повертається значення відмінне від нуля,
     * а параметр lpFindFileData містить інформацію про наступний знайдений файл або каталог.
     * Якщо функція не працює, повертається значення дорівнює нулю,
     * а вміст lpFindFileData є невизначеним.*/
}


void printResult();

int main() {
    string path = "C:\\Program Files\\Sublime Text 3";
    scanAllFilesInDirectory(path);
    printResult();
    return 0;
}

int dwordToInt(DWORD high, DWORD low) {
    return (int) (static_cast<__int64>(high) << 32 | low);
}

string rangeToString(int start, int finish) {
    return to_string(start) + "-" + to_string(finish);
}

string getRange(int value) {//знаходимо проміжок якому належить даний розмір
    if (value <= interval) {
        return rangeToString(1, interval);
    }
    int start, finish;
    if (value % interval == 0) {
        start = value - interval + 1;
        return rangeToString(start, value);
    }
    int diff = value - value % interval;
    start = diff + 1;
    finish = diff + interval;
    return rangeToString(start, finish);
}

void countSizeToRange(int size) {//функція для підрахунку кількості файлів, що належать кожному проміжку
    string key = getRange(size); //обраховуємо проміжок якому він належить - це і є ключ.
    // А далі додаємо у мапу або інкрементуємо значення лічильника, якщо у цьому проміжку вже хтось був
    int counter;
    if (!statistic.contains(key)) {
        counter = 1;
        statistic.insert(pair(key, counter));
    } else {
        auto node = statistic.find(key);
        counter = node->second + 1;
        node->second = counter;
    }
    if (counter > maxCounter) { maxCounter = counter; }
}

void printStarLine(int stars) {
    for (int i = 0; i < stars; ++i) { cout << "*"; }
    cout << endl;
}

void printResult() {
    if (maxCounter == 0) { return; }
    double coeff = (double) CONSOLE_CAPACITY / ((double) maxCounter);
    for (auto &node: statistic) {
        int size = node.second;
        if (size != 0) {
            cout << node.first << endl;
            printStarLine((size * coeff));
        }
    }
}