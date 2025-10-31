#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <omp.h>
#include <fstream>
#include <algorithm>
#include <random>
#include <cmath>
#include <stack>
#include <iomanip>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Переименованные структуры для BMP файлов чтобы избежать конфликтов
#pragma pack(push, 1)  
struct MyBITMAPFILEHEADER {
    unsigned short bfType;
    unsigned long  bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned long  bfOffBits;
};

struct MyRGBTRIPLE {
    unsigned char rgbtBlue;
    unsigned char rgbtGreen;
    unsigned char rgbtRed;
};
#pragma pack(pop)

struct MyBITMAPINFOHEADER {
    unsigned long biSize;
    long          biWidth;
    long          biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned long biCompression;
    unsigned long biSizeImage;
    long          biXPelsPerMeter;
    long          biYPelsPerMeter;
    unsigned long biClrUsed;
    unsigned long biClrImportant;
};

// Структура для хранения данных изображения BMP
struct BMPImage {
    int width;
    int height;
    int channels;
    vector<uint8_t> data;
    MyRGBTRIPLE** rgbData;

    BMPImage() : rgbData(nullptr) {}
    ~BMPImage() {
        if (rgbData != nullptr) {
            delete[] rgbData[0];
            delete[] rgbData;
        }
    }
};

// Прототипы функций сортировки
template<typename T>
void shellSortSequential(T* arr, int n);

template<typename T>
void shellSortParallel(T* arr, int n);

template<typename T>
void quickSortSequential(T* arr, int n);

template<typename T>
void quickSortParallelSimple(T* arr, int n);

// Вспомогательные функции для быстрой сортировки
template<typename T>
int partition(T* arr, int low, int high);

template<typename T>
void quickSortSequentialHelper(T* arr, int low, int high);

// Прототипы функций для работы с BMP
void BMPRead(MyRGBTRIPLE**& rgb, MyBITMAPFILEHEADER& header, MyBITMAPINFOHEADER& bmiHeader, const char* fin);
void BMPWrite(MyRGBTRIPLE**& rgb, int imWidth, int imHeight, const char* fout);
unsigned char get_row_data_padding(unsigned int width);
unsigned int bmp24b_file_size_calc(unsigned int width, unsigned int height);
bool loadBMP(const string& filename, BMPImage& image);
bool saveBMP(const string& filename, BMPImage& image);
void convertRGBToData(BMPImage& image);
void convertDataToRGB(BMPImage& image);

// Прототипы функций применения фильтров
void applyMedianFilterSequential(BMPImage& input, BMPImage& output, int ksize,
    function<void(uint8_t*, int)> sortFunction, const string& algName);

void applyMedianFilterParallel(BMPImage& input, BMPImage& output, int ksize,
    function<void(uint8_t*, int)> sortFunction, const string& algName);

// Прототипы функций измерения времени
double MeasureFTime(function<double()> func, int expCNT, const string& algorithm_name, int ksize);
double AvgTrustedIntervalMED(double*& times, int cnt);
void bubbleSort(double* mas, int cnt);

// Функции для экспериментальных исследований
void test_median_filters(double* experiments_time, vector<string> algorithm_names,
    const vector<string>& input_files, int ksize, int iterations, int file_index);

int main() {
    // Используем существующие BMP файлы
    vector<string> input_files = { "1.bmp", "2.bmp", "3.bmp", "4.bmp" };

    // Проверяем существование файлов
    vector<string> existing_files;
    for (const auto& file : input_files) {
        ifstream test(file, ios::binary);
        if (test.good()) {
            existing_files.push_back(file);
            test.close();
            cout << "Найден файл: " << file << endl;
        }
        else {
            cout << "Файл не найден: " << file << endl;
        }
    }

    if (existing_files.empty()) {
        cout << "Ошибка: входные файлы не найдены. Создайте файлы 11.bmp, 22.bmp, 33.bmp, 44.bmp" << endl;
        return 1;
    }

    int iteration_cnt = 1; // Количество реализаций

    vector<string> algorithm_names = {
        "Сортировка Шелла (посл.)",
        "Сортировка Шелла (парал.) 2 потока",
        "Сортировка Шелла (парал.) 3 потока",
        "Сортировка Шелла (парал.) 4 потока",
        "Быстрая сортировка (посл.)",
        "Быстрая сортировка (парал.) 2 потока",
        "Быстрая сортировка (парал.) 3 потока",
        "Быстрая сортировка (парал.) 4 потока"
    };

    vector<int> ksize_values = { 3, 7, 11 };
    int alg_count = algorithm_names.size();
    int ksize_count = ksize_values.size();

    // Выделяем память для хранения результатов
    double*** experiments_time = new double** [existing_files.size()];
    for (size_t f = 0; f < existing_files.size(); f++) {
        experiments_time[f] = new double* [ksize_count];
        for (int k = 0; k < ksize_count; k++) {
            experiments_time[f][k] = new double[alg_count]();
        }
    }

    // Проводим эксперименты для каждого размера окна
    for (int k_idx = 0; k_idx < ksize_count; k_idx++) {
        int ksize = ksize_values[k_idx];
        cout << "\n----------------------------------------" << endl;
        cout << "РАЗМЕР ОКРЕСТНОСТИ: " << ksize << "x" << ksize << endl;
        cout << "----------------------------------------" << endl;

        for (size_t f = 0; f < existing_files.size(); f++) {
            vector<string> single_file = { existing_files[f] };
            cout << "\nОБРАБОТКА ФАЙЛА: " << existing_files[f] << endl;
            test_median_filters(experiments_time[f][k_idx], algorithm_names, single_file, ksize, iteration_cnt, f);
        }
    }

    // Вывод результатов
    ofstream fout("results.txt");
    fout << fixed << setprecision(6);
    cout << fixed << setprecision(6);

    for (size_t f = 0; f < existing_files.size(); f++) {
        cout << "\n\nФАЙЛ: " << existing_files[f] << endl;
        fout << "\n" << existing_files[f] << endl;

        // Сортировка Шелла (посл.)
        for (int k_idx = 0; k_idx < ksize_count; k_idx++) {
            int ksize = ksize_values[k_idx];
            cout << "Сортировка Шелла(посл.) Ksize = " << ksize << "\t" << experiments_time[f][k_idx][0] << " с" << endl;
            fout << experiments_time[f][k_idx][0] << endl;
        }

        // Сортировка Шелла (парал.) 2 потока
        for (int k_idx = 0; k_idx < ksize_count; k_idx++) {
            int ksize = ksize_values[k_idx];
            cout << "Сортировка Шелла(парал.) 2 потока Ksize = " << ksize << "\t" << experiments_time[f][k_idx][1] << " с" << endl;
            fout << experiments_time[f][k_idx][1] << endl;
        }

        // Сортировка Шелла (парал.) 3 потока
        for (int k_idx = 0; k_idx < ksize_count; k_idx++) {
            int ksize = ksize_values[k_idx];
            cout << "Сортировка Шелла(парал.) 3 потока Ksize = " << ksize << "\t" << experiments_time[f][k_idx][2] << " с" << endl;
            fout << experiments_time[f][k_idx][2] << endl;
        }

        // Сортировка Шелла (парал.) 4 потока
        for (int k_idx = 0; k_idx < ksize_count; k_idx++) {
            int ksize = ksize_values[k_idx];
            cout << "Сортировка Шелла(парал.) 4 потока Ksize = " << ksize << "\t" << experiments_time[f][k_idx][3] << " с" << endl;
            fout << experiments_time[f][k_idx][3] << endl;
        }

        // Быстрая сортировка (посл.)
        for (int k_idx = 0; k_idx < ksize_count; k_idx++) {
            int ksize = ksize_values[k_idx];
            cout << "Быстрая сортировка(посл.) Ksize = " << ksize << "\t" << experiments_time[f][k_idx][4] << " с" << endl;
            fout << experiments_time[f][k_idx][4] << endl;
        }

        // Быстрая сортировка (парал.) 2 потока
        for (int k_idx = 0; k_idx < ksize_count; k_idx++) {
            int ksize = ksize_values[k_idx];
            cout << "Быстрая сортировка(парал.) 2 потока Ksize = " << ksize << "\t" << experiments_time[f][k_idx][5] << " с" << endl;
            fout << experiments_time[f][k_idx][5] << endl;
        }

        // Быстрая сортировка (парал.) 3 потока
        for (int k_idx = 0; k_idx < ksize_count; k_idx++) {
            int ksize = ksize_values[k_idx];
            cout << "Быстрая сортировка(парал.) 3 потока Ksize = " << ksize << "\t" << experiments_time[f][k_idx][6] << " с" << endl;
            fout << experiments_time[f][k_idx][6] << endl;
        }

        // Быстрая сортировка (парал.) 4 потока
        for (int k_idx = 0; k_idx < ksize_count; k_idx++) {
            int ksize = ksize_values[k_idx];
            cout << "Быстрая сортировка(парал.) 4 потока Ksize = " << ksize << "\t" << experiments_time[f][k_idx][7] << " с" << endl;
            fout << experiments_time[f][k_idx][7] << endl;
        }

        cout << endl;
        fout << endl;
    }

    fout.close();

    // Освобождаем память
    for (size_t f = 0; f < existing_files.size(); f++) {
        for (int k = 0; k < ksize_count; k++) {
            delete[] experiments_time[f][k];
        }
        delete[] experiments_time[f];
    }
    delete[] experiments_time;

    cout << "\nРезультаты сохранены в файл 'results.txt'" << endl;

    return 0;
}

// Реализации функций сортировки
template<typename T>
void shellSortSequential(T* arr, int n) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            T temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
    }
}

template<typename T>
void shellSortParallel(T* arr, int n) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
#pragma omp parallel for
        for (int i = gap; i < n; i++) {
            T temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
    }
}

// Вспомогательная функция для разделения массива
template<typename T>
int partition(T* arr, int low, int high) {
    T pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

// Последовательная быстрая сортировка
template<typename T>
void quickSortSequentialHelper(T* arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSortSequentialHelper(arr, low, pi - 1);
        quickSortSequentialHelper(arr, pi + 1, high);
    }
}

template<typename T>
void quickSortSequential(T* arr, int n) {
    quickSortSequentialHelper(arr, 0, n - 1);
}

// Альтернативная версия параллельной быстрой сортировки (проще)
template<typename T>
void quickSortParallelSimple(T* arr, int n) {
    if (n <= 1) return;

    // Используем последовательную версию для маленьких массивов
    if (n < 1000) {
        quickSortSequential(arr, n);
        return;
    }

    int pi = partition(arr, 0, n - 1);

#pragma omp parallel shared(arr)
    {
#pragma omp task
            quickSortParallelSimple(arr, pi);
#pragma omp task
            quickSortParallelSimple(arr + pi + 1, n - pi - 1);
#pragma omp taskwait
    }
}

// Функции для работы с BMP
unsigned char get_row_data_padding(unsigned int width) {
    return (width % 4 == 0) ? 0 : (4 - (width * sizeof(MyRGBTRIPLE)) % 4);
}

unsigned int bmp24b_file_size_calc(unsigned int width, unsigned int height) {
    return sizeof(MyBITMAPFILEHEADER) + sizeof(MyBITMAPINFOHEADER) + height * (width * sizeof(MyRGBTRIPLE) + get_row_data_padding(width));
}

void BMPRead(MyRGBTRIPLE**& rgb, MyBITMAPFILEHEADER& header, MyBITMAPINFOHEADER& bmiHeader, const char* fin) {
    // Открываем файл
    FILE* pFile;
    fopen(&pFile, fin, "rb");
    if (!pFile) {
        cout << "Не удалось открыть файл: " << fin << endl;
        return;
    }

    // Считываем заголовок файла
    fread(&header, sizeof(MyBITMAPFILEHEADER), sizeof(MyBITMAPFILEHEADER), 1, pFile);

    // Проверяем сигнатуру BMP
    if (header.bfType != 0x4D42) { // "BM" в little-endian
        cout << "Неверный формат BMP файла: " << fin << endl;
        fclose(pFile);
        return;
    }

    // Считываем заголовочную часть изображения
    fread(&bmiHeader, sizeof(MyBITMAPINFOHEADER), sizeof(MyBITMAPINFOHEADER), 1, pFile);

    // Проверяем, что это 24-битный BMP
    if (bmiHeader.biBitCount != 24) {
        cout << "Поддерживаются только 24-битные BMP файлы: " << fin << endl;
        fclose(pFile);
        return;
    }

    // Выделяем память под массив RGB
    rgb = new MyRGBTRIPLE * [bmiHeader.biHeight];
    rgb[0] = new MyRGBTRIPLE[bmiHeader.biWidth * bmiHeader.biHeight];
    for (int i = 1; i < bmiHeader.biHeight; i++) {
        rgb[i] = &rgb[0][bmiHeader.biWidth * i];
    }

    // Перемещаемся на начало данных изображения
    fseek(pFile, header.bfOffBits, SEEK_SET);

    // Определяем величину дополнения
    int padding = get_row_data_padding(bmiHeader.biWidth);
    char tmp[3] = { 0,0,0 };

    // Считываем данные изображения
    for (int i = 0; i < bmiHeader.biHeight; i++) {
        fread(&rgb[bmiHeader.biHeight - 1 - i][0], (bmiHeader.biWidth * sizeof(MyRGBTRIPLE)), sizeof(MyRGBTRIPLE), bmiHeader.biWidth, pFile);
        if (padding > 0)
            fread(&tmp[0], sizeof(tmp), sizeof(unsigned char), padding, pFile);
    }

    fclose(pFile);
}

void BMPWrite(MyRGBTRIPLE**& rgb, int imWidth, int imHeight, const char* fout) {
    FILE* oFile;
    fopen(&oFile, fout, "wb");
    if (!oFile) {
        cout << "Не удалось создать файл: " << fout << endl;
        return;
    }

    // Создаем заголовки BMP
    MyBITMAPFILEHEADER header = { 0 };
    header.bfType = 0x4D42; // "BM"
    header.bfSize = bmp24b_file_size_calc(imWidth, imHeight);
    header.bfOffBits = 54;

    MyBITMAPINFOHEADER bmiHeader = { 0 };
    bmiHeader.biSize = 40;
    bmiHeader.biWidth = imWidth;
    bmiHeader.biHeight = imHeight;
    bmiHeader.biPlanes = 1;
    bmiHeader.biBitCount = 24;
    bmiHeader.biSizeImage = header.bfSize - sizeof(MyBITMAPINFOHEADER) - sizeof(MyBITMAPFILEHEADER);

    // Записываем заголовки
    fwrite(&header, sizeof(MyBITMAPFILEHEADER), 1, oFile);
    fwrite(&bmiHeader, sizeof(MyBITMAPINFOHEADER), 1, oFile);

    // Записываем данные изображения
    int padding = get_row_data_padding(bmiHeader.biWidth);
    char tmp[3] = { 0,0,0 };

    for (int i = 0; i < bmiHeader.biHeight; i++) {
        fwrite(&(rgb[bmiHeader.biHeight - i - 1][0]), sizeof(MyRGBTRIPLE), bmiHeader.biWidth, oFile);
        if (padding > 0)
            fwrite(&tmp[0], sizeof(unsigned char), padding, oFile);
    }

    fclose(oFile);
}

bool loadBMP(const string& filename, BMPImage& image) {
    MyBITMAPFILEHEADER header;
    MyBITMAPINFOHEADER bmiHeader;

    BMPRead(image.rgbData, header, bmiHeader, filename.c_str());

    if (image.rgbData == nullptr) {
        return false;
    }

    image.width = bmiHeader.biWidth;
    image.height = bmiHeader.biHeight;
    image.channels = 3;

    convertRGBToData(image);

    cout << "Загружено изображение: " << filename << " (" << image.width << "x" << image.height << ")" << endl;
    return true;
}

bool saveBMP(const string& filename, BMPImage& image) {
    if (image.rgbData == nullptr) {
        convertDataToRGB(image);
    }

    BMPWrite(image.rgbData, image.width, image.height, filename.c_str());
    cout << "Сохранено изображение: " << filename << endl;
    return true;
}

void convertRGBToData(BMPImage& image) {
    int dataSize = image.width * image.height * image.channels;
    image.data.resize(dataSize);

    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            int dataIdx = (y * image.width + x) * image.channels;
            int rgbY = image.height - 1 - y;
            image.data[dataIdx] = image.rgbData[rgbY][x].rgbtBlue;
            image.data[dataIdx + 1] = image.rgbData[rgbY][x].rgbtGreen;
            image.data[dataIdx + 2] = image.rgbData[rgbY][x].rgbtRed;
        }
    }
}

void convertDataToRGB(BMPImage& image) {
    if (image.rgbData != nullptr) {
        delete[] image.rgbData[0];
        delete[] image.rgbData;
    }

    image.rgbData = new MyRGBTRIPLE * [image.height];
    image.rgbData[0] = new MyRGBTRIPLE[image.width * image.height];
    for (int i = 1; i < image.height; i++) {
        image.rgbData[i] = &image.rgbData[0][image.width * i];
    }

    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            int dataIdx = (y * image.width + x) * image.channels;
            int rgbY = image.height - 1 - y;

            image.rgbData[rgbY][x].rgbtBlue = image.data[dataIdx];
            image.rgbData[rgbY][x].rgbtGreen = image.data[dataIdx + 1];
            image.rgbData[rgbY][x].rgbtRed = image.data[dataIdx + 2];
        }
    }
}

// Функция применения медианного фильтра (последовательная версия)
void applyMedianFilterSequential(BMPImage& input, BMPImage& output, int ksize,
    function<void(uint8_t*, int)> sortFunction, const string& algName) {

    int radius = ksize / 2;
    output.width = input.width;
    output.height = input.height;
    output.channels = input.channels;
    output.data.resize(input.width * input.height * input.channels);

    // Создаем массивы для представления изображения в формате [Y][X]
    vector<vector<vector<uint8_t>>> IMAGE(input.height, vector<vector<uint8_t>>(input.width, vector<uint8_t>(input.channels)));
    vector<vector<vector<uint8_t>>> IMAGE2(input.height, vector<vector<uint8_t>>(input.width, vector<uint8_t>(input.channels)));

    // Конвертируем входные данные в формат IMAGE[Y][X][C]
    for (int y = 0; y < input.height; y++) {
        for (int x = 0; x < input.width; x++) {
            for (int c = 0; c < input.channels; c++) {
                int dataIdx = (y * input.width + x) * input.channels + c;
                IMAGE[y][x][c] = input.data[dataIdx];
            }
        }
    }

    int RH = radius;  // Размер ранга по высоте
    int RW = radius;  // Размер ранга по ширине
    int size = (RH * 2 + 1) * (RW * 2 + 1);  // Площадь окна фильтрации
    vector<uint8_t> MEDMAS(size);  // Массив элементов медианы

    // Цикл 1 по Y изображению (Y от 0 до Height)
    for (int Y = 0; Y < input.height; Y++) {
        // Цикл 2 по X изображению (X от 0 до Width)
        for (int X = 0; X < input.width; X++) {
            // Для каждого цветового канала
            for (int channel = 0; channel < input.channels; channel++) {
                int Masind = 0;  // Переменная для индексации элементов массива MEDMAS

                // Заполнение массива MEDMAS элементами изображения IMAGE(X,Y,RH,RW)
                // Цикл A1 (DY от –RH до RH включительно)
                for (int DY = -RH; DY <= RH; DY++) {
                    int KY = Y + DY;
                    // Если KY < 0 то KY=0
                    if (KY < 0) KY = 0;
                    // Если KY > Height - 1 то KY = Height - 1
                    if (KY >= input.height) KY = input.height - 1;

                    // Цикл A2 (DX от –RW до RW включительно)
                    for (int DX = -RW; DX <= RW; DX++) {
                        int KX = X + DX;
                        // Если KX < 0 то KX=0
                        if (KX < 0) KX = 0;
                        // Если KX > Width - 1 то KX = Width - 1
                        if (KX >= input.width) KX = input.width - 1;

                        // MEDMAS[Masind] = IMAGE[KY][KX]
                        MEDMAS[Masind] = IMAGE[KY][KX][channel];
                        Masind = Masind + 1;
                    }
                }

                // Сортировка массива MEDMAS любым алгоритмом
                sortFunction(MEDMAS.data(), size);

                // Запись в выходное изображение IMAGE2[Y][X] значения центрального элемента массива MEDMAS
                IMAGE2[Y][X][channel] = MEDMAS[size / 2];
            }
        }
    }

    // Конвертируем выходные данные обратно в линейный формат
    for (int y = 0; y < input.height; y++) {
        for (int x = 0; x < input.width; x++) {
            for (int c = 0; c < input.channels; c++) {
                int dataIdx = (y * input.width + x) * input.channels + c;
                output.data[dataIdx] = IMAGE2[y][x][c];
            }
        }
    }
}

// Функция применения медианного фильтра (параллельная версия)
void applyMedianFilterParallel(BMPImage& input, BMPImage& output, int ksize,
    function<void(uint8_t*, int)> sortFunction, const string& algName) {

    int radius = ksize / 2;
    output.width = input.width;
    output.height = input.height;
    output.channels = input.channels;
    output.data.resize(input.width * input.height * input.channels);

    // Создаем массивы для представления изображения в формате [Y][X]
    vector<vector<vector<uint8_t>>> IMAGE(input.height, vector<vector<uint8_t>>(input.width, vector<uint8_t>(input.channels)));
    vector<vector<vector<uint8_t>>> IMAGE2(input.height, vector<vector<uint8_t>>(input.width, vector<uint8_t>(input.channels)));

    // Конвертируем входные данные в формат IMAGE[Y][X][C]
    for (int y = 0; y < input.height; y++) {
        for (int x = 0; x < input.width; x++) {
            for (int c = 0; c < input.channels; c++) {
                int dataIdx = (y * input.width + x) * input.channels + c;
                IMAGE[y][x][c] = input.data[dataIdx];
            }
        }
    }

    int RH = radius;  // Размер ранга по высоте
    int RW = radius;  // Размер ранга по ширине
    int size = (RH * 2 + 1) * (RW * 2 + 1);  // Площадь окна фильтрации

    // Распараллеливаем обработку по строкам изображения
#pragma omp parallel
    {
        vector<uint8_t> MEDMAS(size);  // Каждый поток имеет свою копию массива медианы

        // Цикл 1 по Y изображению (Y от 0 до Height) - распараллеливаем
#pragma omp for
        for (int Y = 0; Y < input.height; Y++) {
            // Цикл 2 по X изображению (X от 0 до Width)
            for (int X = 0; X < input.width; X++) {
                // Для каждого цветового канала
                for (int channel = 0; channel < input.channels; channel++) {
                    int Masind = 0;  // Переменная для индексации элементов массива MEDMAS

                    // Заполнение массива MEDMAS элементами изображения IMAGE(X,Y,RH,RW)
                    // Цикл A1 (DY от –RH до RH включительно)
                    for (int DY = -RH; DY <= RH; DY++) {
                        int KY = Y + DY;
                        // Если KY < 0 то KY=0
                        if (KY < 0) KY = 0;
                        // Если KY > Height - 1 то KY = Height - 1
                        if (KY >= input.height) KY = input.height - 1;

                        // Цикл A2 (DX от –RW до RW включительно)
                        for (int DX = -RW; DX <= RW; DX++) {
                            int KX = X + DX;
                            // Если KX < 0 то KX=0
                            if (KX < 0) KX = 0;
                            // Если KX > Width - 1 то KX = Width - 1
                            if (KX >= input.width) KX = input.width - 1;

                            // MEDMAS[Masind] = IMAGE[KY][KX]
                            MEDMAS[Masind] = IMAGE[KY][KX][channel];
                            Masind = Masind + 1;
                        }
                    }

                    // Сортировка массива MEDMAS любым алгоритмом
                    sortFunction(MEDMAS.data(), size);

                    // Запись в выходное изображение IMAGE2[Y][X] значения центрального элемента массива MEDMAS
                    IMAGE2[Y][X][channel] = MEDMAS[size / 2];
                }
            }
        }
    }

    // Конвертируем выходные данные обратно в линейный формат
    for (int y = 0; y < input.height; y++) {
        for (int x = 0; x < input.width; x++) {
            for (int c = 0; c < input.channels; c++) {
                int dataIdx = (y * input.width + x) * input.channels + c;
                output.data[dataIdx] = IMAGE2[y][x][c];
            }
        }
    }
}

// Функции измерения времени
void bubbleSort(double* mas, int cnt) {
    bool exchanges;
    do {
        exchanges = false;
        for (int i = 0; i < cnt - 1; i++) {
            if (mas[i] > mas[i + 1]) {
                swap(mas[i], mas[i + 1]);
                exchanges = true;
            }
        }
    } while (exchanges);
}

double AvgTrustedIntervalMED(double*& times, int cnt) {
    if (cnt == 0) return 0.0;
    double avg = 0;
    for (int i = 0; i < cnt; i++) avg += times[i];
    avg /= cnt;

    bubbleSort(times, cnt);
    double med = times[cnt / 2];

    double sd = 0, newAVg = 0;
    int newCnt = 0;
    for (int i = 0; i < cnt; i++) {
        sd += (times[i] - avg) * (times[i] - avg);
    }
    if (cnt > 1) sd /= (cnt - 1.0);
    sd = sqrt(sd);

    for (int i = 0; i < cnt; i++) {
        if (med - sd <= times[i] && times[i] <= med + sd) {
            newAVg += times[i];
            newCnt++;
        }
    }
    if (newCnt == 0) newCnt = 1;
    return newAVg / newCnt;
}

double MeasureFTime(function<double()> func, int expCNT, const string& algorithm_name, int ksize) {
    double* times = new double[expCNT];
    cout << algorithm_name << " Ksize=" << ksize << ":\n";
    for (int i = 0; i < expCNT; i++) {
        auto start = high_resolution_clock::now();
        func();
        auto end = high_resolution_clock::now();
        duration<double> duration = end - start;
        times[i] = duration.count();
        cout << "Реализация " << (i + 1) << ": " << times[i] << " с" << endl;
    }
    double avg_time = AvgTrustedIntervalMED(times, expCNT);
    delete[] times;
    return avg_time;
}

void test_median_filters(double* experiments_time, vector<string> algorithm_names,
    const vector<string>& input_files, int ksize, int iterations, int file_index) {

    vector<function<double()>> filterFunctions;

    for (const auto& input_file : input_files) {
        string base_name = input_file.substr(0, input_file.find('.'));

        // Shell Sort Sequential - используем последовательную версию фильтра
        filterFunctions.push_back([input_file, ksize, base_name]() -> double {
            BMPImage local_input, local_output;
            if (!loadBMP(input_file, local_input)) return 0.0;
            auto start = high_resolution_clock::now();
            applyMedianFilterSequential(local_input, local_output, ksize, [](uint8_t* arr, int n) { shellSortSequential(arr, n); }, "shell_seq");
            auto end = high_resolution_clock::now();

            // Сохраняем только для последовательной сортировки Шелла с простыми названиями
            string output_file = base_name + "_k" + to_string(ksize) + ".bmp";
            saveBMP(output_file, local_output);

            duration<double> duration = end - start;
            return duration.count();
            });

        // Shell Sort Parallel - используем параллельную версию фильтра
        for (int threads : {2, 3, 4}) {
            filterFunctions.push_back([input_file, ksize, threads]() -> double {
                BMPImage local_input, local_output;
                if (!loadBMP(input_file, local_input)) return 0.0;
                omp_set_num_threads(threads);
                auto start = high_resolution_clock::now();
                applyMedianFilterParallel(local_input, local_output, ksize, [](uint8_t* arr, int n) { shellSortParallel(arr, n); }, "shell_par");
                auto end = high_resolution_clock::now();
                duration<double> duration = end - start;
                return duration.count();
                });
        }

        // Quick Sort Sequential - используем последовательную версию фильтра
        filterFunctions.push_back([input_file, ksize]() -> double {
            BMPImage local_input, local_output;
            if (!loadBMP(input_file, local_input)) return 0.0;
            auto start = high_resolution_clock::now();
            applyMedianFilterSequential(local_input, local_output, ksize, [](uint8_t* arr, int n) { quickSortSequential(arr, n); }, "quick_seq");
            auto end = high_resolution_clock::now();
            duration<double> duration = end - start;
            return duration.count();
            });

        // Quick Sort Parallel - используем параллельную версию фильтра
        for (int threads : {2, 3, 4}) {
            filterFunctions.push_back([input_file, ksize, threads]() -> double {
                BMPImage local_input, local_output;
                if (!loadBMP(input_file, local_input)) return 0.0;
                omp_set_num_threads(threads);
                auto start = high_resolution_clock::now();
                applyMedianFilterParallel(local_input, local_output, ksize, [](uint8_t* arr, int n) { quickSortParallelSimple(arr, n); }, "quick_par");
                auto end = high_resolution_clock::now();
                duration<double> duration = end - start;
                return duration.count();
                });
        }
    }

    for (size_t i = 0; i < filterFunctions.size() && i < algorithm_names.size(); i++) {
        experiments_time[i] = MeasureFTime(filterFunctions[i], iterations, algorithm_names[i], ksize);
        cout << "Среднее время: " << experiments_time[i] << " с\n" << endl;
    }
}
