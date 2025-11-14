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
#include <cmath>

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
// ----------------- Утилиты -----------------
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

static unsigned char** alloc_uchar2d(int w, int h) {
    unsigned char** a = new unsigned char* [h];
    a[0] = new unsigned char[(size_t)w * h];
    for (int i = 1; i < h; ++i) a[i] = &a[0][(size_t)w * i];
    return a;
}
static void free_uchar2d(unsigned char**& a) {
    if (!a) return;
    delete[] a[0];
    delete[] a;
    a = nullptr;
}

static double** alloc_double2d(int w, int h) {
    double** a = new double* [h];
    a[0] = new double[(size_t)w * h];
    for (int i = 1; i < h; ++i) a[i] = &a[0][(size_t)w * i];
    return a;
}
static void free_double2d(double**& a) {
    if (!a) return;
    delete[] a[0];
    delete[] a;
    a = nullptr;
}
// ----------------- Конвертация в яркость -----------------
static unsigned char** rgb_to_gray(RGBTRIPLE** img, int w, int h) {
    unsigned char** gray = alloc_uchar2d(w, h);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            RGBTRIPLE& p = img[y][x];
            double val = p.rgbtRed * 0.299 + p.rgbtGreen * 0.587 + p.rgbtBlue * 0.114;
            int vi = (int)round(val);
            if (vi < 0) vi = 0;
            if (vi > 255) vi = 255;
            gray[y][x] = (unsigned char)vi;
        }
    }
    return gray;
}
// ----------------- Вычисление текстурных признаков для пикселя -----------------
struct TFVal { double m2, R, U, E; };

static inline TFVal compute_tf_from_hist(const vector<int>& hist, int winSize) {
    TFVal res{ 0.0,0.0,0.0,0.0 };
    // Size = winSize (кол-во пикселей в окне)
    int Size = winSize;
    // преобразуем гистограмму к вероятностям
    vector<double> p(256, 0.0);
    for (int i = 0; i < 256; ++i) {
        p[i] = (double)hist[i] / (double)Size;
    }
    // m = среднее значение яркости
    double m = 0.0;
    for (int i = 0; i < 256; ++i) m += p[i] * (double)i;
    // m2 второй момент относительно среднего
    double m2 = 0.0;
    double u = 0.0;
    double e = 0.0;
    for (int i = 0; i < 256; ++i) {
        double diff = (double)i - m;
        m2 += diff * diff * p[i];
        u += p[i] * p[i]; // однородность (U) как сумма квадратов вероятностей
        if (p[i] > 0.0) e += p[i] * (log2(p[i])); // для энтропии
    }
    double R = 1.0 - (1.0 / (1.0 + m2)); // относительная гладкость
    double E = -1.0 * e;
    res.m2 = m2;
    res.R = R;
    res.U = u;
    res.E = E;
    return res;
}
static TFVal compute_tf_pixel_seq(unsigned char** gray, int w, int h, int x, int y, int Rn, int Rm) {
    int K_h = 2 * Rn + 1;
    int K_w = 2 * Rm + 1;
    int winSize = K_h * K_w;
    vector<int> hist(256, 0);
    for (int dy = -Rn; dy <= Rn; ++dy) {
        int yy = clampi(y + dy, 0, h - 1);
        for (int dx = -Rm; dx <= Rm; ++dx) {
            int xx = clampi(x + dx, 0, w - 1);
            unsigned char v = gray[yy][xx];
            hist[v]++;
        }
    }
    return compute_tf_from_hist(hist, winSize);
}
// ----------------- Вычисление карт: последовательный -----------------
static void compute_tf_maps_seq(unsigned char** gray, int w, int h, int Rn, int Rm,
    double** M2, double** Rmap, double** Umap, double** Emap)
{
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            TFVal t = compute_tf_pixel_seq(gray, w, h, x, y, Rn, Rm);
            M2[y][x] = t.m2;
            Rmap[y][x] = t.R;
            Umap[y][x] = t.U;
            Emap[y][x] = t.E;
        }
    }
}
// ----------------- Вычисление карт: OpenMP parallel FOR -----------------
static void compute_tf_maps_openmp(unsigned char** gray, int w, int h, int Rn, int Rm,
    double** M2, double** Rmap, double** Umap, double** Emap, int num_threads)
{
    if (num_threads <= 1) {
        compute_tf_maps_seq(gray, w, h, Rn, Rm, M2, Rmap, Umap, Emap);
        return;
    }
    omp_set_num_threads(num_threads);
#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            TFVal t = compute_tf_pixel_seq(gray, w, h, x, y, Rn, Rm);
            M2[y][x] = t.m2;
            Rmap[y][x] = t.R;
            Umap[y][x] = t.U;
            Emap[y][x] = t.E;
        }
    }
}
// ----------------- oneTBB: установление числа потоков -----------------
static void set_tbb_threads(int threads) {
    static unique_ptr<tbb::global_control> gc;
    gc.reset(new tbb::global_control(tbb::global_control::max_allowed_parallelism, threads));
}
// ----------------- Вычисление карт: oneTBB parallel_for  -----------------
static void compute_tf_maps_tbb(unsigned char** gray, int w, int h, int Rn, int Rm,
    double** M2, double** Rmap, double** Umap, double** Emap, int num_threads)
{
    if (num_threads <= 1) {
        compute_tf_maps_seq(gray, w, h, Rn, Rm, M2, Rmap, Umap, Emap);
        return;
    }
    set_tbb_threads(num_threads);
    tbb::parallel_for(
        tbb::blocked_range2d<int>(0, h, 0, w),
        [&](const tbb::blocked_range2d<int>& r) {
            for (int y = r.rows().begin(); y < r.rows().end(); ++y) {
                for (int x = r.cols().begin(); x < r.cols().end(); ++x) {
                    TFVal t = compute_tf_pixel_seq(gray, w, h, x, y, Rn, Rm);
                    M2[y][x] = t.m2;
                    Rmap[y][x] = t.R;
                    Umap[y][x] = t.U;
                    Emap[y][x] = t.E;
                }
            }
        }
    );
}
// ----------------- Нахождение min/max для каждого TF -----------------
struct MinMax { double mn, mx; };
static MinMax find_minmax(double** arr, int w, int h) {
    double mn = numeric_limits<double>::infinity();
    double mx = -numeric_limits<double>::infinity();
    for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
        double v = arr[y][x];
        if (v < mn) mn = v;
        if (v > mx) mx = v;
    }
    if (mn == numeric_limits<double>::infinity()) mn = 0.0;
    if (mx == -numeric_limits<double>::infinity()) mx = 0.0;
    return { mn, mx };
}
// ----------------- Построение цветной карты по порогам -----------------
static RGBTRIPLE** make_tf_color_map(double** TF, int w, int h, double T1, double T2) {
    RGBTRIPLE** out = alloc_rgb(w, h);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            double v = TF[y][x];
            RGBTRIPLE pix;
            if (v <= T1) {
                pix.rgbtRed = 0;
                pix.rgbtGreen = 255;
                pix.rgbtBlue = 0;
            }
            else if (v < T2) {
                pix.rgbtRed = 255;
                pix.rgbtGreen = 255;
                pix.rgbtBlue = 0;
            }
            else {
                pix.rgbtRed = 255;
                pix.rgbtGreen = 0;
                pix.rgbtBlue = 0;
            }
            out[y][x] = pix;
        }
    }
    return out;
}
// ----------------- Эксперимент -----------------
enum ImplType { IMPL_SEQ = 0, IMPL_OPENMP = 1, IMPL_TBB = 2 };

static double run_average_time_impl_tf(
    RGBTRIPLE** srcOrig, int w, int h,
    int Ksize, ImplType impl, int num_threads,
    int runs,

    RGBTRIPLE*** out_M2_for_last_run,
    RGBTRIPLE*** out_R_for_last_run,
    RGBTRIPLE*** out_U_for_last_run,
    RGBTRIPLE*** out_E_for_last_run,
    double k1, double k2,
    const string& ver_name,
    const string& infile
) {
    int R = (Ksize - 1) / 2;
    double total = 0.0;

    for (int run = 0; run < runs; ++run) {
        unsigned char** gray = rgb_to_gray(srcOrig, w, h);

        // Выделяем карты TF
        double** M2 = alloc_double2d(w, h);
        double** Rmap = alloc_double2d(w, h);
        double** Umap = alloc_double2d(w, h);
        double** Emap = alloc_double2d(w, h);

        auto t0 = clk::now();
        if (impl == IMPL_SEQ) {
            compute_tf_maps_seq(gray, w, h, R, R, M2, Rmap, Umap, Emap);
        }
        else if (impl == IMPL_OPENMP) {
            compute_tf_maps_openmp(gray, w, h, R, R, M2, Rmap, Umap, Emap, num_threads);
        }
        else {
            compute_tf_maps_tbb(gray, w, h, R, R, M2, Rmap, Umap, Emap, num_threads);
        }
        auto t1 = clk::now();
        total += ms(t1 - t0).count();

        // Для последнего прогона — сформировать цветные карты и вернуть их
        if (run == runs - 1) {
            // найти min/max для каждого TF
            MinMax mmM2 = find_minmax(M2, w, h);
            MinMax mmR = find_minmax(Rmap, w, h);
            MinMax mmU = find_minmax(Umap, w, h);
            MinMax mmE = find_minmax(Emap, w, h);

            // ограничим k1/k2 в допустимом диапазоне
            double kk1 = k1; if (kk1 < 0.25) kk1 = 0.25; if (kk1 > 0.4) kk1 = 0.4;
            double kk2 = k2; if (kk2 < 0.6) kk2 = 0.6; if (kk2 > 0.75) kk2 = 0.75;

            double T1M2 = (mmM2.mx - mmM2.mn) * kk1 + mmM2.mn;
            double T2M2 = (mmM2.mx - mmM2.mn) * kk2 + mmM2.mn;
            double T1R = (mmR.mx - mmR.mn) * kk1 + mmR.mn;
            double T2R = (mmR.mx - mmR.mn) * kk2 + mmR.mn;
            double T1U = (mmU.mx - mmU.mn) * kk1 + mmU.mn;
            double T2U = (mmU.mx - mmU.mn) * kk2 + mmU.mn;
            double T1E = (mmE.mx - mmE.mn) * kk1 + mmE.mn;
            double T2E = (mmE.mx - mmE.mn) * kk2 + mmE.mn;

            // Формируем цветные карты
            RGBTRIPLE** cmapM2 = make_tf_color_map(M2, w, h, T1M2, T2M2);
            RGBTRIPLE** cmapR = make_tf_color_map(Rmap, w, h, T1R, T2R);
            RGBTRIPLE** cmapU = make_tf_color_map(Umap, w, h, T1U, T2U);
            RGBTRIPLE** cmapE = make_tf_color_map(Emap, w, h, T1E, T2E);

            if (out_M2_for_last_run) *out_M2_for_last_run = cmapM2; else free_rgb(cmapM2);
            if (out_R_for_last_run)  *out_R_for_last_run = cmapR;  else free_rgb(cmapR);
            if (out_U_for_last_run)  *out_U_for_last_run = cmapU;  else free_rgb(cmapU);
            if (out_E_for_last_run)  *out_E_for_last_run = cmapE;  else free_rgb(cmapE);
        }
        else {
            // Освобождаем временные структуры
            free_double2d(M2); free_double2d(Rmap); free_double2d(Umap); free_double2d(Emap);
        }

        free_uchar2d(gray);
    }
    return total / runs;
}
int main(int argc, char** argv)
{
    setlocale(LC_ALL, "");
    vector<string> inputs = { "100x100.bmp", "200x200.bmp", "300x300.bmp" };

    // Параметры эксперимента
    int runs_texture = 7; 
    vector<int> ksizes = { 5, 7, 9 };
    vector<int> requested_threads = { 2, 3, 4 };

    int maxprocs = omp_get_max_threads();
    omp_set_dynamic(0);
    set<int> unique_threads;
    for (int r : requested_threads)
        unique_threads.insert(min(max(r, 1), maxprocs));
    vector<int> actual_threads(unique_threads.begin(), unique_threads.end());

    cout << "Запуск расчёта текстурных признаков: запусков = " << runs_texture << "\n";
    cout << "Параметры: k1 в [0.25,0.4], k2 в [0.6,0.75]\n";

    // Запрос пользователю k1 и k2
    double k1 = 0.3, k2 = 0.65;
    cout << "Введите k1 (0.25..0.4) [по умолчанию 0.3]: ";
    string s;
    if (getline(cin, s)) {
        if (!s.empty()) {
            try { k1 = stod(s); }
            catch (...) { k1 = 0.3; }
        }
    }
    cout << "Введите k2 (0.6..0.75) [по умолчанию 0.65]: ";
    if (getline(cin, s)) {
        if (!s.empty()) {
            try { k2 = stod(s); }
            catch (...) { k2 = 0.65; }
        }
    }
    if (k1 < 0.25) k1 = 0.25; if (k1 > 0.4) k1 = 0.4;
    if (k2 < 0.6) k2 = 0.6; if (k2 > 0.75) k2 = 0.75;

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
            cout << "\nОкрестность K=" << K << " (радиус R=" << (K - 1) / 2 << ")\n";

            // Последовательная реализация
            {
                RGBTRIPLE** outM2 = nullptr, ** outR = nullptr, ** outU = nullptr, ** outE = nullptr;
                double t_seq = run_average_time_impl_tf(img, w, h, K, IMPL_SEQ, 1, runs_texture,
                    &outM2, &outR, &outU, &outE, k1, k2, "seq", infile);
                cout << "  Посл.: время = " << fixed << setprecision(3) << t_seq << " мс\n";
                // сохраняем
                stringstream ss;
                ss << "output_" << infile << "_seq_M2_K" << K << ".bmp";
                BMPWrite(outM2, w, h, ss.str().c_str()); free_rgb(outM2);
                ss.str(""); ss.clear();
                ss << "output_" << infile << "_seq_R_K" << K << ".bmp";
                BMPWrite(outR, w, h, ss.str().c_str()); free_rgb(outR);
                ss.str(""); ss.clear();
                ss << "output_" << infile << "_seq_U_K" << K << ".bmp";
                BMPWrite(outU, w, h, ss.str().c_str()); free_rgb(outU);
                ss.str(""); ss.clear();
                ss << "output_" << infile << "_seq_E_K" << K << ".bmp";
                BMPWrite(outE, w, h, ss.str().c_str()); free_rgb(outE);
            }

            // OpenMP варианты
            for (int t : actual_threads) {
                RGBTRIPLE** outM2 = nullptr, ** outR = nullptr, ** outU = nullptr, ** outE = nullptr;
                double t_omp = run_average_time_impl_tf(img, w, h, K, IMPL_OPENMP, t, runs_texture,
                    &outM2, &outR, &outU, &outE, k1, k2, "omp", infile);
                cout << "  OpenMP FOR поток=" << setw(2) << t << "  время = " << fixed << setprecision(3) << t_omp << " мс\n";
                stringstream ss;
                ss << "output_" << infile << "_omp_t" << t << "_M2_K" << K << ".bmp";
                BMPWrite(outM2, w, h, ss.str().c_str()); free_rgb(outM2);
                ss.str(""); ss.clear();
                ss << "output_" << infile << "_omp_t" << t << "_R_K" << K << ".bmp";
                BMPWrite(outR, w, h, ss.str().c_str()); free_rgb(outR);
                ss.str(""); ss.clear();
                ss << "output_" << infile << "_omp_t" << t << "_U_K" << K << ".bmp";
                BMPWrite(outU, w, h, ss.str().c_str()); free_rgb(outU);
                ss.str(""); ss.clear();
                ss << "output_" << infile << "_omp_t" << t << "_E_K" << K << ".bmp";
                BMPWrite(outE, w, h, ss.str().c_str()); free_rgb(outE);
            }

            // oneTBB варианты
            for (int t : actual_threads) {
                RGBTRIPLE** outM2 = nullptr, ** outR = nullptr, ** outU = nullptr, ** outE = nullptr;
                double t_tbb = run_average_time_impl_tf(img, w, h, K, IMPL_TBB, t, runs_texture,
                    &outM2, &outR, &outU, &outE, k1, k2, "tbb", infile);
                cout << "  oneTBB parallel_for поток=" << setw(2) << t << "  время = " << fixed << setprecision(3) << t_tbb << " мс\n";
                stringstream ss;
                ss << "output_" << infile << "_tbb_t" << t << "_M2_K" << K << ".bmp";
                BMPWrite(outM2, w, h, ss.str().c_str()); free_rgb(outM2);
                ss.str(""); ss.clear();
                ss << "output_" << infile << "_tbb_t" << t << "_R_K" << K << ".bmp";
                BMPWrite(outR, w, h, ss.str().c_str()); free_rgb(outR);
                ss.str(""); ss.clear();
                ss << "output_" << infile << "_tbb_t" << t << "_U_K" << K << ".bmp";
                BMPWrite(outU, w, h, ss.str().c_str()); free_rgb(outU);
                ss.str(""); ss.clear();
                ss << "output_" << infile << "_tbb_t" << t << "_E_K" << K << ".bmp";
                BMPWrite(outE, w, h, ss.str().c_str()); free_rgb(outE);
            }

        } 
        free_rgb(img);
    }
    return 0;
}
