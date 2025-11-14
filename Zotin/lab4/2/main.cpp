#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <vector>
#include <string>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <set>
#include <memory>

#include <omp.h>
#include <tbb/tbb.h>
#include <tbb/blocked_range2d.h>
#include <tbb/global_control.h>

#include "../bmpfilerw.hpp"

// Определение величины дополнения на случай, если ширина изображения не кратна 4
unsigned char get_row_data_padding(unsigned int width) {
    return (width % 4 == 0) ? 0 : (4 - (width * sizeof(RGBTRIPLE)) % 4);
}

// Вычисление размера BMP файла
unsigned int bmp24b_file_size_calc(unsigned int width, unsigned int height) {
    return sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + height * (width * sizeof(RGBTRIPLE) + get_row_data_padding(width));
}


void BMPRead(RGBTRIPLE**& rgb, BITMAPFILEHEADER& header, \
        BITMAPINFOHEADER& bmiHeader, const char* fin)
{
    // Открываем файл BMP
    std::ifstream InFile(fin, std::ios::binary);
    // Считываем заголовок файла
    InFile.read((char*)(&header), sizeof(BITMAPFILEHEADER));
    // Считываем заголовочную часть изображения
    InFile.read((char*)(&bmiHeader), sizeof(BITMAPINFOHEADER));
    // Выделяем память под массив RGB хранящий структуры RGBTRIPLE
    rgb = new RGBTRIPLE * [bmiHeader.biHeight];
    // формируем единую область данных для оптимизации хранения
    rgb[0] = new RGBTRIPLE[bmiHeader.biWidth * bmiHeader.biHeight];
    for (int i = 1; i < bmiHeader.biHeight; i++)
    {   // нестраиваем указатели начала каждой строки
        rgb[i] = &rgb[0][bmiHeader.biWidth * i];
    }
    // перемещаемся на начало данных изображения
    InFile.seekg(header.bfOffBits, std::ios::beg);
    // определяем величину дополнения на случай если ширина изображения не кратна 4
    int padding = get_row_data_padding(bmiHeader.biWidth);
    char tmp[3] = { 0,0,0 };
    // Считываем данные изображения в массив структур RGB 
    for (int i = 0; i < bmiHeader.biHeight; i++)
    {
        InFile.read((char*)(&rgb[bmiHeader.biHeight - 1 - i][0]), bmiHeader.biWidth * sizeof(RGBTRIPLE)); // RGBTRIPLE {Blue Green bRed;}
    if (padding > 0)
        InFile.read((char*)(&tmp[0]), padding);
    }
    // Закрываем файл
    InFile.close();
}

void BMPWrite(RGBTRIPLE**& rgb, int imWidth, int imHeight, const char* fout)
{
    // Открываем файл для записи изображения в формат BMP
    std::ofstream OutFile(fout, std::ios::binary);
    // Создаем заголовочную часть для файла BMP
    BITMAPFILEHEADER header = { 0 };
    header.bfType = ('M' << 8) + 'B';
    header.bfSize = bmp24b_file_size_calc(imWidth, imHeight);;
    header.bfOffBits = 54;
    // Создаем заголовочную часть для данных изображения 
    BITMAPINFOHEADER bmiHeader = { 0 };
    // заполняем необходимыми данными
    bmiHeader.biSize = 40;
    bmiHeader.biWidth = imWidth;
    bmiHeader.biHeight = imHeight;
    bmiHeader.biPlanes = 1;
    bmiHeader.biBitCount = 24;
    bmiHeader.biSizeImage = header.bfSize - sizeof(BITMAPINFOHEADER) - sizeof(BITMAPFILEHEADER);
    //// Записываем заголовок файла
    OutFile.write((char*)(&header), sizeof(BITMAPFILEHEADER));
    //// Записываем заголовочную часть изображения
    OutFile.write((char*)(&bmiHeader), sizeof(BITMAPINFOHEADER));
    // определяем величину дополнения на случай если ширина изображения не кратна 4
    int padding = get_row_data_padding(bmiHeader.biWidth);
    char tmp[3] = { 0,0,0 };
    // Записываем данные изображения из массива структур RGBTRIPLE в файл 
    for (int i = 0; i < bmiHeader.biHeight; i++)
    {
        OutFile.write((char*)&(rgb[bmiHeader.biHeight - i - 1][0]), bmiHeader.biWidth * sizeof(RGBTRIPLE));
        if (padding > 0)
            OutFile.write((char*)(&tmp[0]), padding);
    }
    // закрываем файл
    OutFile.close();
}

using namespace std;
using clk = chrono::high_resolution_clock;
using ms = chrono::duration<double, milli>;

// ------------------ Сортировка ------------------
// Shell sort (последовательная)
template<typename T>
void shell_sort_seq(T* a, size_t N) {
    for (size_t gap = N / 2; gap > 0; gap /= 2) {
        for (size_t i = gap; i < N; ++i) {
            T temp = a[i];
            size_t j = i;
            while (j >= gap && a[j - gap] > temp) {
                a[j] = a[j - gap];
                j -= gap;
            }
            a[j] = temp;
        }
    }
}
// ----------------- Утилиты для изображений -----------------
static inline int clampi(int v, int a, int b) {
    if (v < a) return a;
    if (v > b) return b;
    return v;
}
static RGBTRIPLE** alloc_rgb(int width, int height) {
    RGBTRIPLE** rgb = new RGBTRIPLE * [height];
    rgb[0] = new RGBTRIPLE[(size_t)width * height];
    for (int i = 1; i < height; ++i) rgb[i] = &rgb[0][(size_t)width * i];
    return rgb;
}
static void free_rgb(RGBTRIPLE**& rgb) {
    if (!rgb) return;
    delete[] rgb[0];
    delete[] rgb;
    rgb = nullptr;
}
// ----------------- Вычисление медианы для одного канала -----------------
// Возвращает медианное значение 
static inline unsigned char compute_median_channel_seq(
    RGBTRIPLE** img, int w, int h,
    int x, int y, int R, int channel)
{
    int K = 2 * R + 1;
    int N = K * K;
    // используем вектор unsigned char
    vector<unsigned char> window;
    window.reserve(N);

    for (int dy = -R; dy <= R; ++dy) {
        int yy = clampi(y + dy, 0, h - 1);
        for (int dx = -R; dx <= R; ++dx) {
            int xx = clampi(x + dx, 0, w - 1);
            const RGBTRIPLE& pix = img[yy][xx];
            unsigned char v = (channel == 0 ? pix.rgbtRed :
                (channel == 1 ? pix.rgbtGreen : pix.rgbtBlue));
            window.push_back(v);
        }
    }
        shell_sort_seq(window.data(), window.size());
    int mid = (int)window.size() / 2;
    return window[mid];
}
// ----------------- Медианный фильтр: посл. / openmp / tbb -----------------
// Последовательный медианный фильтр
static RGBTRIPLE** median_filter_seq(RGBTRIPLE** src, int w, int h, int Ksize) {
    int R = (Ksize - 1) / 2;
    RGBTRIPLE** out = alloc_rgb(w, h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            unsigned char r = compute_median_channel_seq(src, w, h, x, y, R, 0);
            unsigned char g = compute_median_channel_seq(src, w, h, x, y, R, 1);
            unsigned char b = compute_median_channel_seq(src, w, h, x, y, R, 2);
            out[y][x].rgbtRed = r;
            out[y][x].rgbtGreen = g;
            out[y][x].rgbtBlue = b;
        }
    }
    return out;
}
// OpenMP-вариант: распараллеливаем внешний цикл по строкам
static RGBTRIPLE** median_filter_openmp(RGBTRIPLE** src, int w, int h, int Ksize, int num_threads) {
    int R = (Ksize - 1) / 2;
    RGBTRIPLE** out = alloc_rgb(w, h);

    if (num_threads <= 1) {
        return median_filter_seq(src, w, h, Ksize);
    }
    omp_set_num_threads(num_threads);
#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            unsigned char r = compute_median_channel_seq(src, w, h, x, y, R, 0);
            unsigned char g = compute_median_channel_seq(src, w, h, x, y, R, 1);
            unsigned char b = compute_median_channel_seq(src, w, h, x, y, R, 2);
            out[y][x].rgbtRed = r;
            out[y][x].rgbtGreen = g;
            out[y][x].rgbtBlue = b;
        }
    }
    return out;
}
// oneTBB-вариант: используем tbb::parallel_for с blocked_range2d
static void set_tbb_threads(int threads) {
    static unique_ptr<tbb::global_control> gc;
    gc.reset(new tbb::global_control(tbb::global_control::max_allowed_parallelism, threads));
}
static RGBTRIPLE** median_filter_tbb(RGBTRIPLE** src, int w, int h, int Ksize, int num_threads) {
    int R = (Ksize - 1) / 2;
    RGBTRIPLE** out = alloc_rgb(w, h);

    if (num_threads <= 1) {
        return median_filter_seq(src, w, h, Ksize);
    }
    set_tbb_threads(num_threads);
    tbb::parallel_for(
        tbb::blocked_range2d<int>(0, h, 0, w),
        [&](const tbb::blocked_range2d<int>& r) {
            for (int y = r.rows().begin(); y < r.rows().end(); ++y) {
                for (int x = r.cols().begin(); x < r.cols().end(); ++x) {
                    unsigned char rr = compute_median_channel_seq(src, w, h, x, y, R, 0);
                    unsigned char gg = compute_median_channel_seq(src, w, h, x, y, R, 1);
                    unsigned char bb = compute_median_channel_seq(src, w, h, x, y, R, 2);
                    out[y][x].rgbtRed = rr;
                    out[y][x].rgbtGreen = gg;
                    out[y][x].rgbtBlue = bb;
                }
            }
        }
    );
    return out;
}
// ----------------- Эксперименты -----------------
enum ImplType { IMPL_SEQ = 0, IMPL_OPENMP = 1, IMPL_TBB = 2 };

static double run_average_time_impl(
    RGBTRIPLE** srcOrig, int w, int h,
    int Ksize, ImplType impl, int num_threads,
    int runs, RGBTRIPLE*** out_image_for_last_run)
{
    double total = 0.0;
    for (int run = 0; run < runs; ++run) {
        auto t0 = clk::now();
        RGBTRIPLE** out = nullptr;
        if (impl == IMPL_SEQ)
            out = median_filter_seq(srcOrig, w, h, Ksize);
        else if (impl == IMPL_OPENMP)
            out = median_filter_openmp(srcOrig, w, h, Ksize, num_threads);
        else
            out = median_filter_tbb(srcOrig, w, h, Ksize, num_threads);
        auto t1 = clk::now();
        total += ms(t1 - t0).count();

        if (run == runs - 1 && out_image_for_last_run)
            *out_image_for_last_run = out;
        else
            free_rgb(out);
    }
    return total / runs;
}

int main(int argc, char** argv)
{
    vector<string> inputs = { "1600x900.bmp", "2000x1200.bmp", "2560x1440.bmp" };
    int runs = 10;
    vector<int> ksizes = { 7, 11, 15 };
    vector<int> requested_threads = { 2, 3, 4 };

    int maxprocs = omp_get_max_threads();
    omp_set_dynamic(0);
    set<int> unique_threads;
    for (int r : requested_threads)
        unique_threads.insert(min(max(r, 1), maxprocs));
    vector<int> actual_threads(unique_threads.begin(), unique_threads.end());

    cout << "Запуск медианного фильтра (Shell sort внутри): runs=" << runs << "\n";

    for (const string& infile : inputs) {
        BITMAPFILEHEADER bfh = { 0 };
        BITMAPINFOHEADER bih = { 0 };
        RGBTRIPLE** img = nullptr;

        try { BMPRead(img, bfh, bih, infile.c_str()); }
        catch (...) { img = nullptr; }

        int w = bih.biWidth, h = bih.biHeight;
        if (!img || w <= 0 || h <= 0 || bih.biBitCount != 24 || bih.biCompression != 0) {
            cerr << "Ошибка при чтении " << infile << "\n";
            if (img) free_rgb(img);
            continue;
        }

        cout << "\n=== Файл: " << infile << " (" << w << "x" << h << ") ===\n";

        for (int K : ksizes) {
            cout << "\nМедианный фильтр (посл.), K=" << K;
            RGBTRIPLE** out_seq = nullptr;
            double t_seq = run_average_time_impl(img, w, h, K, IMPL_SEQ, 1, runs, &out_seq);
            cout << "   Время: " << fixed << setprecision(3) << t_seq << " мс\n";
            if (out_seq) {
                stringstream ss;
                ss << "1output_seq_K" << K << "_" << infile;
                BMPWrite(out_seq, w, h, ss.str().c_str());
                free_rgb(out_seq);
            }

            for (int t : actual_threads) {
                RGBTRIPLE** out_omp = nullptr;
                double t_omp = run_average_time_impl(img, w, h, K, IMPL_OPENMP, t, runs, &out_omp);
                cout << "Медианный фильтр (пар. OpenMP) Поток=" << setw(2) << t << "  Время: " << fixed << setprecision(3) << t_omp << " мс\n";
                if (out_omp) {
                    stringstream ss;
                    ss << "2output_omp_t" << t << "_K" << K << "_" << infile;
                    BMPWrite(out_omp, w, h, ss.str().c_str());
                    free_rgb(out_omp);
                }
            }

            for (int t : actual_threads) {
                RGBTRIPLE** out_tbb = nullptr;
                double t_tbb = run_average_time_impl(img, w, h, K, IMPL_TBB, t, runs, &out_tbb);
                cout << "Медианный фильтр (пар. oneTBB)  Поток=" << setw(2) << t << "  Время: " << fixed << setprecision(3) << t_tbb << " мс\n";
                if (out_tbb) {
                    stringstream ss;
                    ss << "3output_tbb_t" << t << "_K" << K << "_" << infile;
                    BMPWrite(out_tbb, w, h, ss.str().c_str());
                    free_rgb(out_tbb);
                }
            }
        }

        free_rgb(img);
    }
    return 0;
}
