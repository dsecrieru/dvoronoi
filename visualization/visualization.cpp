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
const dvoronoi::fortune::config_t config{ dvoronoi::box_t{ 0, 0, width, height } };
const float edge = 100;
const std::size_t SITES_COUNT = 200;

const uint8_t bg_comp = 60;
const sf::Color bg_col(bg_comp, bg_comp, bg_comp);
const sf::Color site_col(244, 108, 63);
const sf::Color edge_col(247, 220, 104);
const sf::Color triang_col(46, 149, 153);

typedef float scalar_t;
typedef sf::Vector2f point_t;

const scalar_t radius = 3;

//std::random_device rd;
std::mt19937 rng(0/*rd()*/);
std::uniform_real_distribution<scalar_t> distrib;

void populate_or_append(std::vector<point_t>& sites, std::vector<sf::Vector2f>& move_vec, std::size_t sites_count) {
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

void draw_voronoi(sf::RenderWindow& window, const auto& diagram) {
    for (const auto he : diagram.half_edges) {
        if (!he.orig || !he.dest)
            continue;

        std::array<sf::Vertex, 2> line = {
                sf::Vertex({ static_cast<scalar_t>(he.orig->point.x), static_cast<scalar_t>(he.orig->point.y) }, edge_col),
                sf::Vertex({ static_cast<scalar_t>(he.dest->point.x), static_cast<scalar_t>(he.dest->point.y) }, edge_col )
        };
        window.draw(line.data(), 2, sf::Lines);
    }
}

void draw_triang(sf::RenderWindow& window, const auto& diagram) {
    for (auto i = 0; i < diagram.sites.size(); ++i) {
        const auto& orig = diagram.sites[i];

        for (const auto& neighbor_idx : (*diagram.triangulation)[i]) {
            const auto& dest = diagram.sites[neighbor_idx];

            std::array<sf::Vertex, 2> line = {
                    sf::Vertex({static_cast<scalar_t>(orig.point.x), static_cast<scalar_t>(orig.point.y)}, triang_col),
                    sf::Vertex({static_cast<scalar_t>(dest.point.x), static_cast<scalar_t>(dest.point.y)}, triang_col)
            };

            window.draw(line.data(), 2, sf::Lines);
        };
    }
}

int main() {
    std::vector<point_t> sites;
    std::vector<point_t> move_vec;
    populate_or_append(sites, move_vec, SITES_COUNT);

    dvoronoi::fortune::algorithm algo;
    auto diagram = algo.generate(sites, config);
    diagram->generate_delaunay();
    diagram->compute_convex_hull();

    sf::RenderWindow window(sf::VideoMode(width, height), "dvoronoi");

    bool is_paused = true;
    bool do_draw_triang = true;
    bool do_draw_voronoi = true;
    bool add_sites = false;
    bool sub_sites = false;
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
                    if (event.key.code == sf::Keyboard::D)
                        do_draw_triang = !do_draw_triang;
                    if (event.key.code == sf::Keyboard::V)
                        do_draw_voronoi = !do_draw_voronoi;
                    if (event.key.code == sf::Keyboard::PageUp)
                        add_sites = true;
                    if (event.key.code == sf::Keyboard::PageDown)
                        sub_sites = true;
                    if (event.key.code == sf::Keyboard::Space)
                        is_paused = !is_paused;
                    break;

                default:
                    break;
            }
        }

        window.clear(bg_col);

        if (add_sites) {
            populate_or_append(sites, move_vec, 10);
        }

        if (sub_sites && sites.size() > 10) {
            sites.resize(sites.size() - 10);
            move_vec.resize(move_vec.size() - 10);
        }

        if (add_sites || sub_sites) {
            diagram = algo.generate(sites, config);
            diagram->generate_delaunay();
            diagram->compute_convex_hull();

            add_sites = sub_sites = false;
        }

        if (do_draw_triang)
            draw_triang(window, *diagram);

        if (do_draw_voronoi)
            draw_voronoi(window, *diagram);

        for (const auto& p : sites)
            window.draw(get_shape(radius, p.x, p.y, site_col));

        if (index > diagram->convex_hull->size() - 1)
            index = 0;

        auto site = diagram->sites[(*diagram->convex_hull)[index]];
        auto face = site.face;
        window.draw(get_shape(radius, static_cast<scalar_t>(site.point.x), static_cast<scalar_t>(site.point.y), sf::Color::Red));

         auto first_he = face->half_edge;
         auto he = first_he;
         bool missing_ends = false;
         bool missing_he = false;
         do {
             if (!he->orig || !he->dest) {
                 missing_ends = true;
                 he = he->next;
                 continue;
             }

             std::array<sf::Vertex, 2> line = {
                 sf::Vertex({ static_cast<scalar_t>(he->orig->point.x), static_cast<scalar_t>(he->orig->point.y) }, sf::Color::Red),
                 sf::Vertex({ static_cast<scalar_t>(he->dest->point.x), static_cast<scalar_t>(he->dest->point.y) }, sf::Color::Red)
             };
             window.draw(line.data(), 2, sf::Lines);
             he = he->next;
             if (!he)
                 missing_he = true;
         } while (he && he != first_he);

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

            diagram = algo.generate(sites, config);
            diagram->generate_delaunay();
            diagram->compute_convex_hull();
        }

        std::this_thread::sleep_for(10ms);
    }
}
