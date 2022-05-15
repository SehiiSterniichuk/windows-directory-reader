#include <vector>
#include <map>
#include <iostream>
#include <filesystem>
#include <string>

using namespace std;

const int interval = 1024 * 50;//інтервал
map<string, int> statistic;//string - інтервал, int - лічильник файлів, що входять у цей інтервал
map<string, int> files;//string - ім'я файлу, int - його розмір
const int consoleCapacity = 80;//максимальне число символів для найбільшого результату гістограми
long maxCounter = 1;//максимальна кількість файлів, що потрапляла у проміжок

void scanAllFilesInDirectory(const string &path) {
    for (const auto &dirIterator: filesystem::recursive_directory_iterator(path)) {
        /*recursive_directory_iterator — це LegacyInputIterator, який виконує ітерацію
         * над елементами directory_entry каталогу і, рекурсивно, над записами всіх підкаталогів.
         * Порядок ітерації не визначений, за винятком того, що кожен запис у каталозі відвідується лише один раз.*/
        if (filesystem::is_regular_file(dirIterator)) {
            /*true, якщо файл відноситься до звичайного файлу, інакше значення false.*/
            string name = dirIterator.path().filename().string();// ім'я файлу
            int size = dirIterator.file_size();//розмір файлу
            files.insert(pair(name, size));//запам'ятовуємо
        };
    }
}

string getRange(int value);

void mappingFilesToRangeOfSize() {
    for (const auto &file: files) {//проходимося по всіх файлах
        int size = file.second;//розмір
        string key = getRange(size); //обраховуємо проміжок якому він належить - це і є ключ.
        // А далі додаємо у мапу або інкрементуємо значенн лічильника, якщо у цьому проміжку вже хтось був
        if (!statistic.contains(key)) {
            statistic.insert(pair(key, 1));
        } else {
            auto node = statistic.find(key);
            int counter = node->second + 1;
            if (counter > maxCounter) {
                maxCounter = counter;
            }
            node->second = counter;
        }
    }
}

void printResult();

int main() {
    string path = "C:\\Program Files\\Sublime Text 3";
    scanAllFilesInDirectory(path);
    mappingFilesToRangeOfSize();
    printResult();
    return 0;
}

string rangeToString(int start, int finish) {
    return to_string(start) + "-" + to_string(finish);
}

void printStarLine(int stars) {
    for (int i = 0; i < stars; ++i) {
        cout << "*";
    }
    cout << endl;
}

string getRange(int value) {
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

void printResult() {
    double coeff = (double) consoleCapacity / ((double) maxCounter);
    for (auto &node: statistic) {
        int size = node.second;
        if (size != 0) {
            cout << node.first << endl;
            printStarLine((size * coeff));
        }
    }
}