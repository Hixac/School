#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <random>
#include <cmath>
#include <cfloat>
#include <chrono>
#include <algorithm>
#include <utility>

#include <omp.h>
#include <tbb/tbb.h>

using namespace std;
using clk = chrono::high_resolution_clock;
using ms_t = chrono::duration<double, milli>;
using value_t = double;

// -------------------- Утилиты --------------------
static void die(const string& s) { cerr << s << "\n"; exit(1); }

struct CmdOptions {
    vector<int> sizes = { 3000, 5000, 8000 };
    int runs = 100;
    bool quick = false;
} opts;

void parse_args(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        string s = argv[i];
        if (s == "--runs" && i + 1 < argc) { opts.runs = stoi(argv[++i]); continue; }
        if (s == "--quick") { opts.quick = true; opts.runs = 15; opts.sizes = { 512,512,512 }; continue; }
        if (s == "--sizes" && i + 1 < argc) {
            string t = argv[++i];
            vector<int> out;
            string cur;
            for (char c : t) {
                if (c == ',') { if (!cur.empty()) { out.push_back(stoi(cur)); cur.clear(); } }
                else cur.push_back(c);
            }
            if (!cur.empty()) out.push_back(stoi(cur));
            if (out.size() == 3) opts.sizes = out; else die("размеры должны быть 3 целыми числами");
            continue;
        }
        cerr << "Неизвестный аргумент: " << s << "\n";
    }
}

// выделить плоскую матрицу n*n
bool alloc_mat(vector<value_t>& M, size_t n) {
    try { M.assign(n * n, 0.0); }
    catch (bad_alloc&) { return false; }
    return true;
}
// -------------------- Заполнение по формулам --------------------
void fillFormulaA(vector<value_t>& A, size_t n) {
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            size_t idx = i * n + j;
            A[idx] = sin(0.002 * i) * cos(0.003 * j) + 0.0001 * i * j + 0.5 * sin((i + j) * 0.001);
        }
    }
}

void fillFormulaB(vector<value_t>& B, size_t n) {
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            size_t idx = i * n + j;
            B[idx] = cos(0.0015 * i) * sin(0.0025 * j) + 0.0002 * (i - j) + 0.3 * cos((i * j) * 0.0001);
        }
    }
}
void fill_matrices(vector<value_t>& A, vector<value_t>& B, size_t n, int dataset_type = 0, unsigned seed = 12345) {
    fillFormulaA(A, n);
    fillFormulaB(B, n);
}

template<typename F, typename... Args>
double time_it_ms(F&& f, Args&&... args) {
    auto t0 = clk::now();
    std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    auto t1 = clk::now();
    return chrono::duration_cast<ms_t>(t1 - t0).count();
}

// -------------------- Операции: последовательные --------------------
void add_seq(const vector<value_t>& A, const vector<value_t>& B, vector<value_t>& C, size_t n) {
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            size_t idx = i * n + j;
            C[idx] = A[idx] + B[idx];
        }
    }
}
void mul_seq(const vector<value_t>& A, const vector<value_t>& B, vector<value_t>& C, size_t n) {
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            size_t idx = i * n + j;
            C[idx] = A[idx] * B[idx];
        }
    }
}
value_t sum_seq(const vector<value_t>& A, const vector<value_t>& B, size_t n) {
    value_t s = 0;
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            size_t idx = i * n + j;
            s += A[idx] + B[idx];
        }
    }
    return s;
}
pair<value_t, value_t> minmax_seq(const vector<value_t>& A, const vector<value_t>& B, size_t n) {
    value_t mn = DBL_MAX, mx = -DBL_MAX;
    size_t total = n * n;
    for (size_t i = 0; i < total; i++) {
        value_t v = A[i];
        if (v < mn) mn = v; if (v > mx) mx = v;
        v = B[i];
        if (v < mn) mn = v; if (v > mx) mx = v;
    }
    return { mn,mx };
}
// -------------------- OpenMP варианты --------------------
void add_omp_for(const vector<value_t>& A, const vector<value_t>& B, vector<value_t>& C, size_t n, int threads) {
    omp_set_num_threads(threads);
#pragma omp parallel for schedule(static)
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            size_t idx = i * n + j;
            C[idx] = A[idx] + B[idx];
        }
    }
}
void mul_omp_for(const vector<value_t>& A, const vector<value_t>& B, vector<value_t>& C, size_t n, int threads) {
    omp_set_num_threads(threads);
#pragma omp parallel for  schedule(static)
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            size_t idx = i * n + j;
            C[idx] = A[idx] * B[idx];
        }
    }
}
void add_omp_tasks(const vector<value_t>& A, const vector<value_t>& B, vector<value_t>& C, size_t n, int threads) {
    omp_set_num_threads(threads);
#pragma omp parallel
    {
#pragma omp single
        {
            for (size_t i = 0; i < n; i++) {
#pragma omp task firstprivate(i)
                {
                    for (size_t j = 0; j < n; j++) {
                        size_t idx = i * n + j;
                        C[idx] = A[idx] + B[idx];
                    }
                }
            }
#pragma omp taskwait
        }
    }
}
void mul_omp_tasks(const vector<value_t>& A, const vector<value_t>& B, vector<value_t>& C, size_t n, int threads) {
    omp_set_num_threads(threads);
#pragma omp parallel
    {
#pragma omp single
        {
            for (size_t i = 0; i < n; i++) {
#pragma omp task firstprivate(i)
                {
                    for (size_t j = 0; j < n; j++) {
                        size_t idx = i * n + j;
                        C[idx] = A[idx] * B[idx];
                    }
                }
            }
#pragma omp taskwait
        }
    }
}
value_t sum_omp_reduction(const vector<value_t>& A, const vector<value_t>& B, size_t n, int threads) {
    value_t total_sum = 0.0;
    omp_set_num_threads(threads);
#pragma omp parallel for reduction(+:total_sum) schedule(static)
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            size_t idx = i * n + j;
            total_sum += A[idx] + B[idx];
        }
    }
    return total_sum;
}
pair<value_t, value_t> minmax_omp(const vector<value_t>& A, const vector<value_t>& B, size_t n, int threads) {
    value_t global_min = DBL_MAX;
    value_t global_max = -DBL_MAX;
    omp_set_num_threads(threads);

#pragma omp parallel for reduction(min:global_min) reduction(max:global_max) schedule(static)
    for (size_t i = 0; i < n * n; i++) {
        value_t v = A[i];
        if (v < global_min) global_min = v;
        if (v > global_max) global_max = v;

        v = B[i];
        if (v < global_min) global_min = v;
        if (v > global_max) global_max = v;
    }

    return { global_min, global_max };
}
// -------------------- oneTBB варианты --------------------
void set_tbb_threads(int threads) {
    static unique_ptr<tbb::global_control> gc;
    gc.reset(new tbb::global_control(tbb::global_control::max_allowed_parallelism, threads));
}

void add_tbb_task_group(const vector<value_t>& A, const vector<value_t>& B, vector<value_t>& C, size_t n, int threads) {
    set_tbb_threads(threads);
    size_t block = n / threads;
    if (block == 0) block = 1;

    tbb::task_group tg;

    for (int t = 0; t < threads; t++) {
        size_t start = t * block;
        size_t end = (t == threads - 1) ? n : min(n, start + block);

        tg.run([&, start, end, n]() {
            for (size_t i = start; i < end; i++) {
                size_t base = i * n;
                for (size_t j = 0; j < n; j++) {
                    C[base + j] = A[base + j] + B[base + j];
                }
            }
            });
    }
    tg.wait();
}

void mul_tbb_task_group(const vector<value_t>& A, const vector<value_t>& B, vector<value_t>& C, size_t n, int threads) {
    set_tbb_threads(threads); 
    size_t block = n / threads;
    if (block == 0) block = 1;

    tbb::task_group tg;

    for (int t = 0; t < threads; t++) {
        size_t start = t * block;
        size_t end = (t == threads - 1) ? n : min(n, start + block);

        tg.run([&, start, end, n]() {
            for (size_t i = start; i < end; i++) {
                size_t base = i * n;
                for (size_t j = 0; j < n; j++) {
                    C[base + j] = A[base + j] * B[base + j];
                }
            }
            });
    }
    tg.wait();
}

void add_tbb_parallel_for_lambda(const vector<value_t>& A, const vector<value_t>& B, vector<value_t>& C, size_t n, int threads) {
    set_tbb_threads(threads);
    tbb::parallel_for(tbb::blocked_range2d<size_t>(0, n, 0, n),
        [&](const tbb::blocked_range2d<size_t>& r) {
            for (size_t i = r.rows().begin(); i < r.rows().end(); i++) {
                for (size_t j = r.cols().begin(); j < r.cols().end(); j++) {
                    size_t idx = i * n + j;
                    C[idx] = A[idx] + B[idx];
                }
            }
        }
    );
}
void mul_tbb_parallel_for_lambda(const vector<value_t>& A, const vector<value_t>& B, vector<value_t>& C, size_t n, int threads) {
    set_tbb_threads(threads);
    tbb::parallel_for(tbb::blocked_range2d<size_t>(0, n, 0, n),
        [&](const tbb::blocked_range2d<size_t>& r) {
            for (size_t i = r.rows().begin(); i < r.rows().end(); i++) {
                for (size_t j = r.cols().begin(); j < r.cols().end(); j++) {
                    size_t idx = i * n + j;
                    C[idx] = A[idx] * B[idx];
                }
            }
        }
    );
}
struct AddBody {
    const vector<value_t>& A;const vector<value_t>& B;vector<value_t>& C;size_t n;
    AddBody(const vector<value_t>& a, const vector<value_t>& b, vector<value_t>& c, size_t nn)
        : A(a), B(b), C(c), n(nn) {
    }
    void operator()(const tbb::blocked_range2d<size_t>& r) const {
        for (size_t i = r.rows().begin(); i < r.rows().end(); i++) {
            for (size_t j = r.cols().begin(); j < r.cols().end(); j++) {
                size_t idx = i * n + j;
                C[idx] = A[idx] + B[idx];
            }
        }
    }
};
struct MulBody {
    const vector<value_t>& A;const vector<value_t>& B;vector<value_t>& C;size_t n;
    MulBody(const vector<value_t>& a, const vector<value_t>& b, vector<value_t>& c, size_t nn)
        : A(a), B(b), C(c), n(nn) {
    }
    void operator()(const tbb::blocked_range2d<size_t>& r) const {
        for (size_t i = r.rows().begin(); i < r.rows().end(); ++i) {
            for (size_t j = r.cols().begin(); j < r.cols().end(); ++j) {
                size_t idx = i * n + j;
                C[idx] = A[idx] * B[idx];
            }
        }
    }
};
void add_tbb_parallel_for_class(const vector<value_t>& A, const vector<value_t>& B, vector<value_t>& C, size_t n, int threads) {
    set_tbb_threads(threads);
    AddBody body(A, B, C, n);
    tbb::parallel_for(tbb::blocked_range2d<size_t>(0, n, 0, n), body);
}
void mul_tbb_parallel_for_class(const vector<value_t>& A, const vector<value_t>& B, vector<value_t>& C, size_t n, int threads) {
    set_tbb_threads(threads);
    MulBody body(A, B, C, n);
    tbb::parallel_for(tbb::blocked_range2d<size_t>(0, n, 0, n), body);
}
// -------------------- oneTBB редукции --------------------
class SumReducer {
public:
    const vector<value_t>& A;const vector<value_t>& B;value_t sum;size_t n;
    SumReducer(const vector<value_t>& a, const vector<value_t>& b, size_t nn)
        : A(a), B(b), sum(0), n(nn) {
    }
    SumReducer(SumReducer& s, tbb::split)
        : A(s.A), B(s.B), sum(0), n(s.n) {
    }
    void operator()(const tbb::blocked_range2d<size_t>& r) {
        value_t local_sum = 0;
        for (size_t i = r.rows().begin(); i < r.rows().end(); ++i) {
            for (size_t j = r.cols().begin(); j < r.cols().end(); ++j) {
                size_t idx = i * n + j;
                local_sum += A[idx] + B[idx];
            }
        }
        sum += local_sum;
    }
    void join(const SumReducer& s) { sum += s.sum; }
};
value_t sum_tbb_reduce_class(const vector<value_t>& A,const vector<value_t>& B,size_t n, int threads) {
    set_tbb_threads(threads);
    SumReducer sr(A, B, n);
    tbb::parallel_reduce(tbb::blocked_range2d<size_t>(0, n, 0, n), sr);
    return sr.sum;
}
value_t sum_tbb_reduce_lambda(const vector<value_t>& A,const vector<value_t>& B,size_t n, int threads) {
    set_tbb_threads(threads);
    auto res = tbb::parallel_reduce(
        tbb::blocked_range2d<size_t>(0, n, 0, n),
        (value_t)0,
        [&](const tbb::blocked_range2d<size_t>& r, value_t local_sum) -> value_t {
            for (size_t i = r.rows().begin(); i < r.rows().end(); ++i) {
                for (size_t j = r.cols().begin(); j < r.cols().end(); ++j) {
                    size_t idx = i * n + j;
                    local_sum += A[idx] + B[idx];
                }
            }
            return local_sum;
        },
        std::plus<value_t>()
    );
    return res;
}
struct minmax_st {
    value_t v_min;
    value_t v_max;
    minmax_st() : v_min(DBL_MAX), v_max(-DBL_MAX) {}
    minmax_st(value_t mn, value_t mx) : v_min(mn), v_max(mx) {}
};
struct minmax_join {
    minmax_st operator()(const minmax_st& L, const minmax_st& R) const {
        return minmax_st(
            std::min(L.v_min, R.v_min),
            std::max(L.v_max, R.v_max)
        );
    }
};
pair<value_t, value_t> minmax_tbb_struct(const vector<value_t>& A,const vector<value_t>& B,size_t n, int threads) {
    set_tbb_threads(threads);
    minmax_st res = tbb::parallel_reduce(
        tbb::blocked_range2d<size_t>(0, n, 0, n),
        minmax_st(),
        [&](const tbb::blocked_range2d<size_t>& r, minmax_st running) -> minmax_st {
            for (size_t i = r.rows().begin(); i < r.rows().end(); ++i) {
                for (size_t j = r.cols().begin(); j < r.cols().end(); ++j) {
                    size_t idx = i * n + j;
                    value_t v = A[idx];
                    if (v < running.v_min) running.v_min = v;
                    if (v > running.v_max) running.v_max = v;
                    v = B[idx];
                    if (v < running.v_min) running.v_min = v;
                    if (v > running.v_max) running.v_max = v;
                }
            }
            return running;
        },
        minmax_join()
    );
    return { res.v_min, res.v_max };
}
class MinMaxCalc {
public:
    const vector<value_t>* A;
    value_t MinValue;
    value_t MaxValue;

    MinMaxCalc(const vector<value_t>* arr)
        : A(arr), MinValue(DBL_MAX), MaxValue(-DBL_MAX) {
    }

    MinMaxCalc(MinMaxCalc& x, tbb::split)
        : A(x.A), MinValue(DBL_MAX), MaxValue(-DBL_MAX) {
    }
    void operator()(const tbb::blocked_range<size_t>& r) {
        for (size_t i = r.begin(); i < r.end(); ++i) {
            value_t v = (*A)[i];
            if (v < MinValue) MinValue = v;
            if (v > MaxValue) MaxValue = v;
        }
    }
    void join(const MinMaxCalc& y) {
        if (y.MinValue < MinValue) MinValue = y.MinValue;
        if (y.MaxValue > MaxValue) MaxValue = y.MaxValue;
    }
};
pair<value_t, value_t> minmax_tbb_class(const vector<value_t>& arr, size_t total, int threads) {
    set_tbb_threads(threads);
    MinMaxCalc mmc(&arr);
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, total), mmc);
    return { mmc.MinValue, mmc.MaxValue };
}
// -------------------- Эксперимент --------------------
struct ResultRow {
    string name;
    int threads;
    vector<double> times_ms;
};
double avg_time_runs(function<void()> fn, int runs) {
    vector<double> tv; tv.reserve(runs);
    for (int r = 0; r < runs; r++) {
        double t = time_it_ms(fn);
        tv.push_back(t);
    }
    double sum = 0;
    for (double v : tv) sum += v;
    return sum / tv.size();
}
void run_bench_for_size_dataset(size_t n, int dataset_type, int runs, vector<ResultRow>& rows) {
    vector<value_t> A, B, C;
    if (!alloc_mat(A, n) || !alloc_mat(B, n) || !alloc_mat(C, n)) {
        cerr << "Memory allocation failed for size " << n << "\n"; return;
    }
    fill_matrices(A, B, n, dataset_type, 12345 + (unsigned)n);
    // --- сложение ---
    {
        ResultRow r; r.name = "Сложение (посл.)"; r.threads = 1;
        auto fn = [&]() { add_seq(A, B, C, n); };
        r.times_ms.push_back(avg_time_runs(fn, runs));
        rows.push_back(r);
    }
    for (int th = 2; th <= 4; ++th) {
        ResultRow r; r.name = "Сложение OpenMP FOR"; r.threads = th;
        auto fn = [&]() { add_omp_for(A, B, C, n, th); };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    for (int th = 2; th <= 4; ++th) {
        ResultRow r; r.name = "Сложение OpenMP Tasks"; r.threads = th;
        auto fn = [&]() { add_omp_tasks(A, B, C, n, th); };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    for (int th = 2; th <= 4; ++th) {
        ResultRow r; r.name = "Сложение oneTBB task_group"; r.threads = th;
        auto fn = [&]() { add_tbb_task_group(A, B, C, n, th); };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    for (int th = 2; th <= 4; ++th) {
        ResultRow r; r.name = "Сложение oneTBB parallel_for (lambda)"; r.threads = th;
        auto fn = [&]() { add_tbb_parallel_for_lambda(A, B, C, n, th); };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    for (int th = 2; th <= 4; ++th) {
        ResultRow r; r.name = "Сложение oneTBB parallel_for (class)"; r.threads = th;
        auto fn = [&]() { add_tbb_parallel_for_class(A, B, C, n, th); };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    // --- перемножение ---
    {
        ResultRow r; r.name = "Перемножение (посл.)"; r.threads = 1;
        auto fn = [&]() { mul_seq(A, B, C, n); };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    for (int th = 2; th <= 4; ++th) {
        ResultRow r; r.name = "Перемножение OpenMP FOR"; r.threads = th;
        auto fn = [&]() { mul_omp_for(A, B, C, n, th); };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    for (int th = 2; th <= 4; ++th) {
        ResultRow r; r.name = "Перемножение OpenMP Tasks"; r.threads = th;
        auto fn = [&]() { mul_omp_tasks(A, B, C, n, th); };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    for (int th = 2; th <= 4; ++th) {
        ResultRow r; r.name = "Перемножение oneTBB task_group"; r.threads = th;
        auto fn = [&]() { mul_tbb_task_group(A, B, C, n, th); };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    for (int th = 2; th <= 4; ++th) {
        ResultRow r; r.name = "Перемножение oneTBB parallel_for (lambda)"; r.threads = th;
        auto fn = [&]() { mul_tbb_parallel_for_lambda(A, B, C, n, th); };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    for (int th = 2; th <= 4; ++th) {
        ResultRow r; r.name = "Перемножение oneTBB parallel_for (class)"; r.threads = th;
        auto fn = [&]() { mul_tbb_parallel_for_class(A, B, C, n, th); };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    // --- сумма всех элементов ---
    {
        ResultRow r; r.name = "сумма всех элементов (посл.)"; r.threads = 1;
        auto fn = [&]() { volatile value_t s = sum_seq(A, B, n); (void)s; };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    for (int th = 2; th <= 4; ++th) {
        ResultRow r; r.name = "сумма всех элементов OpenMP FOR (редукция)"; r.threads = th;
        auto fn = [&]() { volatile value_t s = sum_omp_reduction(A, B, n, th); (void)s; };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    for (int th = 2; th <= 4; ++th) {
        ResultRow r; r.name = "сумма всех элементов oneTBB редукция (class)"; r.threads = th;
        auto fn = [&]() { volatile value_t s = sum_tbb_reduce_class(A, B, n, th); (void)s; };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    for (int th = 2; th <= 4; ++th) {
        ResultRow r; r.name = "сумма всех элементов oneTBB редукция (lambda)"; r.threads = th;
        auto fn = [&]() { volatile value_t s = sum_tbb_reduce_lambda(A, B, n, th); (void)s; };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    // --- min+max сумма ---
    {
        ResultRow r; r.name = "сумма minEl+maxEl (посл.)"; r.threads = 1;
        auto fn = [&]() { auto mm = minmax_seq(A, B, n); volatile value_t s = mm.first + mm.second; (void)s; };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    for (int th = 2; th <= 4; ++th) {
        ResultRow r; r.name = "сумма minEl+maxEl OpenMP FOR (редукция)"; r.threads = th;
        auto fn = [&]() { auto mm = minmax_omp(A, B, n, th); volatile value_t s = mm.first + mm.second; (void)s; };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    // oneTBB minmax структура
    for (int th = 2; th <= 4; ++th) {
        ResultRow r; r.name = "сумма minEl+maxEl oneTBB редукция (struct)"; r.threads = th;
        auto fn = [&]() { auto mm = minmax_tbb_struct(A, B, n, th); volatile value_t s = mm.first + mm.second; (void)s; };
        r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
    }
    // oneTBB minmax class (над объединённым массивом)
    {
        vector<value_t> comb; comb.reserve(n * n * 2);
        comb.insert(comb.end(), A.begin(), A.end());
        comb.insert(comb.end(), B.begin(), B.end());
        for (int th = 2; th <= 4; ++th) {
            ResultRow r; r.name = "сумма minEl+maxEl oneTBB редукция (class)"; r.threads = th;
            auto fn = [&]() { auto mm = minmax_tbb_class(comb, comb.size(), th); volatile value_t s = mm.first + mm.second; (void)s; };
            r.times_ms.push_back(avg_time_runs(fn, runs)); rows.push_back(r);
        }
    }
}
int main(int argc, char** argv) {
    parse_args(argc, argv);
    cout.setf(std::ios::fixed); cout << setprecision(3);

    cout << "Запуск benchmark по Заданию 4.1 (OpenMP + Intel oneTBB).\n";
    cout << "Серий прогонов (runs) = " << opts.runs << ", размеры (наборы данных) = "
        << opts.sizes[0] << "x" << opts.sizes[0] << ", " << opts.sizes[1] << "x" << opts.sizes[1]
        << ", " << opts.sizes[2] << "x" << opts.sizes[2] << ".\n\n";

    // Собираем результаты в map: ключ = (name,threads) -> vector<double> times (будет добавлять по одному элементу на каждый размер)
    map<pair<string, int>, vector<double>> table;

    for (size_t si = 0; si < opts.sizes.size(); ++si) {
        size_t n = opts.sizes[si];
        cout << "=== Набор данных: mas1 = " << n << "x" << n << "   mas2 = " << n << "x" << n << " ===\n";
        vector<ResultRow> rows_local;
        run_bench_for_size_dataset(n, 0, opts.runs, rows_local);

        for (auto& r : rows_local) {
            auto key = make_pair(r.name, r.threads);
            if (table.find(key) == table.end()) table[key] = vector<double>();
            table[key].push_back(r.times_ms[0]);
            cout << setw(50) << left << (r.name + (r.threads > 1 ? " , поток=" + to_string(r.threads) : ""))
                << " Время = " << fixed << setprecision(3) << r.times_ms[0] << " мс\n";
        }
        cout << "\n";
    }
    vector<ResultRow> aggregated;
    for (auto& kv : table) {
        ResultRow rr;
        rr.name = kv.first.first;
        rr.threads = kv.first.second;
        rr.times_ms = kv.second; 
        aggregated.push_back(rr);
    }
    return 0;
}
