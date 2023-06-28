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

constexpr float width = 3840;
constexpr float height = 2160;
constexpr std::size_t count = 100000;
constexpr int runs = 100;

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

    const auto avg_duration = std::reduce(durations.begin(), durations.end()) / float(runs);
    std::cout << std::format("[jcv]\tgenerated in ~{:3}\n", avg_duration);
}

void bench_mygal() {
    typedef double scalar_t;

    std::vector<std::chrono::milliseconds> durations;

    for (auto r = 0; r < runs; ++r) {
        std::vector<mygal::Vector2<scalar_t>> points;
        points.reserve(count);

        std::mt19937 rng(r);
        std::uniform_real_distribution<scalar_t> distrib;

        for (auto i = 0; i < count; ++i) {
            points.emplace_back( distrib(rng) * (width - 1.0), distrib(rng) * (height - 1.0) );
        }

        const auto start = std::chrono::steady_clock::now();
        auto algorithm = mygal::FortuneAlgorithm<scalar_t>(points);
        algorithm.construct();
//        algorithm.bound(mygal::Box<scalar_t>{-0.05, -0.05, 1.05, 1.05});
        auto diagram = algorithm.getDiagram();
        const auto end = std::chrono::steady_clock::now();

        auto run_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        durations.push_back(run_duration);

        std::cout << "[mygal]\tfinished run " << r << " in " << run_duration << std::endl;
    }

    const auto avg_duration = std::reduce(durations.begin(), durations.end()) / float(runs);
    std::cout << std::format("[mygal]\tgenerated in ~{:3}\n", avg_duration);
}

int main() {
    //bench_jcv();
    bench_mygal();
}
