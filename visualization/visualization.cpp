//
// Created by Daniel Secrieru on 18/06/2023.
//

#include <random>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;
#include <array>
#include <iostream>
#include <unordered_set>

#include <SFML/Graphics.hpp>

#include <dvoronoi/fortune/algorithm.hpp>

const int width = 1280;
const int height = 1024;
const float edge = 100;
const std::size_t SITES_COUNT = 2500;

const uint8_t bg_comp = 65;
const sf::Color bg_col(bg_comp, bg_comp, bg_comp);
const sf::Color site_col(244, 108, 63);
const sf::Color edge_col(46, 149, 153);
//const sf::Color triang_col(247, 220, 104);

typedef float scalar_t;
typedef sf::Vector2f point_t;

void populate(std::vector<point_t>& sites, std::vector<sf::Vector2f>& move_vec, std::size_t sites_count) {
    //std::random_device rd;
    std::mt19937 rng(0/*rd()*/);
    std::uniform_real_distribution<scalar_t> distrib;

    sites.reserve(sites_count);
    move_vec.reserve(sites_count);
    for(auto i = 0; i < sites_count; ++i) {
        sites.emplace_back(edge + distrib(rng) * (width - 2 * edge), edge + distrib(rng) * (height - 2 * edge));
        move_vec.emplace_back(distrib(rng) * 2.0f - 1.0f, distrib(rng) * 2.0f - 1.0f);
    }
}

sf::CircleShape get_shape(scalar_t r, scalar_t x, scalar_t y, const sf::Color fill) {
    sf::CircleShape circle(r);
    circle.setPosition({ x - r, y - r });
    circle.setFillColor(fill);
    return circle;
}

int main() {
    std::vector<point_t> sites;
    std::vector<point_t> move_vec;
    populate(sites, move_vec, SITES_COUNT);

    auto diagram = dvoronoi::fortune::generate(sites, {});

    sf::RenderWindow window(sf::VideoMode(width, height), "dvoronoi");

    bool is_paused = true;
    std::size_t index = 0;

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Q)
                        window.close();
                    if (event.key.code == sf::Keyboard::N)
                        ++index;
                    if (event.key.code == sf::Keyboard::Space)
                        is_paused = !is_paused;
                    break;

                default:
                    break;
            }
        }

        window.clear(bg_col);

        for (const auto& p : sites)
            window.draw(get_shape(2, p.x, p.y, site_col));

        for (const auto he : diagram->half_edges) {
            if (!he.orig || !he.dest)
                continue;

            std::array<sf::Vertex, 2> line = {
                    sf::Vertex({ static_cast<scalar_t>(he.orig->point.x), static_cast<scalar_t>(he.orig->point.y) }, edge_col),
                    sf::Vertex({ static_cast<scalar_t>(he.dest->point.x), static_cast<scalar_t>(he.dest->point.y) }, edge_col )
            };
            window.draw(line.data(), 2, sf::Lines);
        }

        if (index > diagram->sites.size() - 1) {
            window.display();
            std::this_thread::sleep_for(10ms);
            continue;
        }

        auto site = diagram->sites[index];
        auto face = site.face;
        window.draw(get_shape(2, static_cast<scalar_t>(site.point.x), static_cast<scalar_t>(site.point.y), sf::Color::Red));

        // auto first_he = face->half_edge;
        // auto he = first_he;
        // bool missing_ends = false;
        // bool missing_he = false;
        // do {
        //     if (!he->orig || !he->dest) {
        //         missing_ends = true;
        //         he = he->next;
        //         continue;
        //     }

        //     std::array<sf::Vertex, 2> line = {
        //         sf::Vertex({ static_cast<scalar_t>(he->orig->point.x), static_cast<scalar_t>(he->orig->point.y) }, sf::Color::Red),
        //         sf::Vertex({ static_cast<scalar_t>(he->dest->point.x), static_cast<scalar_t>(he->dest->point.y) }, sf::Color::Red)
        //     };
        //     window.draw(line.data(), 2, sf::Lines);
        //     he = he->next;
        //     if (!he)
        //         missing_he = true;
        // } while (he && he != first_he);

        window.display();

        if (!is_paused) {
            for (auto i = 0; i < sites.size(); ++i) {
                sites[i] += move_vec[i];

                if (sites[i].x < edge || sites[i].x > width - edge) {
                    sites[i].x = std::clamp(sites[i].x, edge, width - edge);
                    move_vec[i].x = -move_vec[i].x;
                }

                if (sites[i].y < edge || sites[i].y > height - edge) {
                    sites[i].y = std::clamp(sites[i].y, edge, height - edge);
                    move_vec[i].y = -move_vec[i].y;
                }
            }

            diagram = dvoronoi::fortune::generate(sites, {});
        }

        std::this_thread::sleep_for(10ms);
    }
}
