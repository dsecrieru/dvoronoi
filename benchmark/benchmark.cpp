//
// Created by Daniel Secrieru on 30/05/2023.
//

#include <random>
#include <chrono>
#include <numeric>
#include <iostream>

//#define JC_VORONOI_IMPLEMENTATION
#ifdef JC_VORONOI_IMPLEMENTATION
#define JCV_REAL_TYPE double
#define JCV_ATAN2 atan2
#define JCV_SQRT sqrt
#define JCV_FLT_MAX DBL_MAX
#define JCV_PI 3.141592653589793115997963468544185161590576171875
#include <jc_voronoi.h>
#endif

#include <MyGAL/FortuneAlgorithm.h>

#include <dvoronoi/fortune/algorithm.hpp>

constexpr float width = 3840;
constexpr float height = 2160;
constexpr std::size_t count = 100000;
constexpr int runs = 100;

template<typename T>
struct gen_point2d_t {
    T x = T(0), y = T(0);
};

#ifdef JC_VORONOI_IMPLEMENTATION
auto bench_jcv(std::size_t run) {
    auto *points = (jcv_point *) malloc(sizeof(jcv_point) * (size_t) count);

    std::mt19937 rng(run);
    std::uniform_real_distribution<float> distrib;

    for (auto i = 0; i < count; ++i) {
        points[i].x = distrib(rng) * (width - 1.0f);
        points[i].y = distrib(rng) * (height - 1.0f);
    }

    jcv_diagram diagram;
    memset(&diagram, 0, sizeof(jcv_diagram));

    const auto start = std::chrono::steady_clock::now();
    jcv_diagram_generate(count, (const jcv_point *) points, nullptr, 0, &diagram);
    const auto end = std::chrono::steady_clock::now();

    jcv_diagram_free(&diagram);
    free(points);

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}
#endif

auto bench_mygal(std::size_t run) {
    typedef double scalar_t;

    std::vector<mygal::Vector2<scalar_t>> points;
    points.reserve(count);

    std::mt19937 rng(run);
    std::uniform_real_distribution<scalar_t> distrib;

    for (auto i = 0; i < count; ++i) {
        points.emplace_back( distrib(rng) * (width - 1.0), distrib(rng) * (height - 1.0) );
    }

    const auto start = std::chrono::steady_clock::now();
    auto algorithm = mygal::FortuneAlgorithm<scalar_t>(points);
    algorithm.construct();
    algorithm.bound(mygal::Box<scalar_t>{-0.5, -0.5, width + 0.5, height + 0.5});
    auto diagram = algorithm.getDiagram();
    const auto end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

auto bench_dvoronoi(std::size_t run, const auto& config) {
    typedef double scalar_t;
    typedef gen_point2d_t<scalar_t> point2d_t;

    std::vector<point2d_t> sites;
    sites.reserve(count);

    std::mt19937 rng(run);
    std::uniform_real_distribution<scalar_t> distrib;

    for(auto i = 0; i < count; ++i) {
        sites.emplace_back(distrib(rng) * (width - 1.0), distrib(rng) * (height - 1.0));
    }

    const auto start = std::chrono::steady_clock::now();
    auto diagram = dvoronoi::fortune::generate(sites, config);
    const auto end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

int main() {
#ifdef JC_VORONOI_IMPLEMENTATION
    std::vector<std::chrono::milliseconds> jcv_durations;
#endif
    std::vector<std::chrono::milliseconds> mygal_durations;
    std::vector<std::chrono::milliseconds> dvoronoi_durations;

    dvoronoi::fortune::config_t config{ dvoronoi::box_t{-0.5, -0.5, width + 0.5, height + 0.5} };

    for (std::size_t r = 0; r < runs; ++r) {
#ifdef JC_VORONOI_IMPLEMENTATION
        auto jcv = bench_jcv(r);
        jcv_durations.push_back(jcv);
#endif

        auto mygal = bench_mygal(r);
        mygal_durations.push_back(mygal);

        auto dvoronoi = bench_dvoronoi(r, config);
        dvoronoi_durations.push_back(dvoronoi);

        std::cout
            << "finished run " << std::setw(3) << r << " in "
#ifdef JC_VORONOI_IMPLEMENTATION
            << "[jcv] " << jcv << "\t[mygal] " << mygal << "\t[dvoronoi] " << dvoronoi
#else
            << "[mygal] " << mygal << "\t[dvoronoi] " << dvoronoi
#endif
            << std::endl;
    }

#ifdef JC_VORONOI_IMPLEMENTATION
    const auto avg_jcv_duration = std::reduce(jcv_durations.begin(), jcv_durations.end()) / float(jcv_durations.size());
    std::cout << "[jcv]      avg: " << std::setw(3) << avg_jcv_duration << std::endl;
#endif

    const auto avg_mygal_duration = std::reduce(mygal_durations.begin(), mygal_durations.end()) / float(mygal_durations.size());
    std::cout << "[mygal]    avg: " << std::setw(3) << avg_mygal_duration << std::endl;

    const auto avg_dvoronoi_duration = std::reduce(dvoronoi_durations.begin(), dvoronoi_durations.end()) / float(dvoronoi_durations.size());
    std::cout << "[dvoronoi] avg: " << std::setw(3) << avg_dvoronoi_duration << std::endl;

#ifdef JC_VORONOI_IMPLEMENTATION
    const auto jcv_relative = 100.0 * (avg_jcv_duration - avg_dvoronoi_duration) / avg_jcv_duration;
    std::cout << "[jcv-dvoronoi]:   " << jcv_relative << '%' << std::endl;
#endif

    const auto mygal_relative = 100.0 * (avg_mygal_duration - avg_dvoronoi_duration) / avg_mygal_duration;
    std::cout << "[mygal-dvoronoi]: " << mygal_relative << '%' << std::endl;
}
