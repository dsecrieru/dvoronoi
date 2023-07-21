//
// Created by Daniel Secrieru on 30/05/2023.
//

#include <random>
#include <chrono>
#include <numeric>
#include <iostream>

#define JC_VORONOI_IMPLEMENTATION
#include <jc_voronoi.h>

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

void bench_jcv() {
    std::vector<std::chrono::milliseconds> durations;

    for (auto r = 0; r < runs; ++r) {
        auto *points = (jcv_point *) malloc(sizeof(jcv_point) * (size_t) count);

        std::mt19937 rng(r);
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

        auto run_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        durations.push_back(run_duration);

        jcv_diagram_free(&diagram);
        free(points);

        std::cout << "[jcv]\tfinished run " << r << " in " << run_duration << std::endl;
    }

    const auto avg_duration = std::reduce(durations.begin(), durations.end()) / float(durations.size());
    std::cout << "[jcv]      avg: " << std::setw(3) << avg_duration << std::endl;
}

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
    algorithm.bound(mygal::Box<scalar_t>{ 0.0, height, width, 0.0 });
    auto diagram = algorithm.getDiagram();
    const auto end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

auto bench_dvoronoi(std::size_t run) {
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
    auto diagram = dvoronoi::fortune::generate(sites, {});
    const auto end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

int main() {
    std::vector<std::chrono::milliseconds> mygal_durations;
    std::vector<std::chrono::milliseconds> dvoronoi_durations;

    for (std::size_t r = 0; r < runs; ++r) {
        auto mygal = bench_mygal(r);
        mygal_durations.push_back(mygal);

        auto dvoronoi = bench_dvoronoi(r);
        dvoronoi_durations.push_back(dvoronoi);

        std::cout
            << "finished run " << std::setw(3) << r << " in "
            << "[mygal] " << mygal << "\t" << "[dvoronoi] " << dvoronoi
            << std::endl;
    }

    const auto avg_mygal_duration = std::reduce(mygal_durations.begin(), mygal_durations.end()) / float(mygal_durations.size());
    std::cout << "[mygal]    avg: " << std::setw(3) << avg_mygal_duration << std::endl;

    const auto avg_dvoronoi_duration = std::reduce(dvoronoi_durations.begin(), dvoronoi_durations.end()) / float(dvoronoi_durations.size());
    std::cout << "[dvoronoi] avg: " << std::setw(3) << avg_dvoronoi_duration << std::endl;
}
