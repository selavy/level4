#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstring>
#include <cassert>
#include <limits>
#include <boost/program_options.hpp>
#include "rapidxml.hpp"
#include "command_line.h"
#include <opencv2/opencv.hpp>

enum class Color {
    Blue,
    Green,
    Red,
    Yellow,
    White
};

Color translate_color(const char* val) {
    if (strncmp(val, "yellow", strlen("yellow")) == 0) {
        return Color::Yellow;
    } else if (strncmp(val, "green", strlen("green")) == 0) {
        return Color::Green;
    } else if (strncmp(val, "red", strlen("red")) == 0) {
        return Color::Red;
    } else if (strncmp(val, "white", strlen("white")) == 0) {
        return Color::White;
    } else if (strncmp(val, "blue", strlen("blue")) == 0) {
        return Color::Blue;
    } else {
        assert(0); // should not get here
        return Color::White;
    }
}

cv::Scalar color_to_scalar(Color color) {
    switch (color) {
    case Color::Blue:
        return CV_RGB(0, 0, 255);
    case Color::Green:
        return CV_RGB(0, 204, 0);
    case Color::Red:
        return CV_RGB(255, 0, 0);
    case Color::Yellow:
        return CV_RGB(204, 204, 0);
    case Color::White:
    default:
        return CV_RGB(255, 255, 255);
    }
}

std::string color_to_string(Color color) {
    switch (color) {
    case Color::Blue:
        return "blue";
    case Color::Green:
        return "green";
    case Color::Red:
        return "red";
    case Color::Yellow:
        return "yellow";
    case Color::White:
    default: // not sure why g++ can't tell that there are no other exit points
        return "white";
    }
}

struct Line {
    Line()
        : x_start(0.0)
        , x_end(0.0)
        , y_start(0.0)
        , y_end(0.0)
        , color(Color::White)
        {}

    Line(double x1, double x2, double y1, double y2, Color c=Color::White)
        : x_start(x1)
        , x_end(x2)
        , y_start(y1)
        , y_end(y2)
        , color(c)
        {}
    
    double x_start;
    double x_end;
    double y_start;
    double y_end;
    Color color;
};
std::ostream& operator<<(std::ostream& os, const Line& line) {
    os << "Line("
       << line.x_start << ", "
       << line.x_end << ", "
       << line.y_start << ", "
       << line.y_end << ", "
       << color_to_string(line.color)
       << ")";
    return os;
}

struct Arc {
    Arc()
        : x_center(0.0)
        , y_center(0.0)
        , radius(0.0)
        , arc_start(0.0)
        , arc_extend(0.0)
        , color(Color::White)
        {}

    Arc(double x, double y, double r, double s, double e, Color c=Color::White)
        : x_center(x)
        , y_center(y)
        , radius(r)
        , arc_start(s)
        , arc_extend(e)
        , color(c)
        {}
    
    double x_center;
    double y_center;
    double radius;
    double arc_start;
    double arc_extend;
    Color color;
};
std::ostream& operator<<(std::ostream& os, const Arc& arc) {
    os << "Arc("
       << arc.x_center << ", "
       << arc.y_center << ", "
       << arc.radius << ", "
       << arc.arc_start << ", "
       << arc.arc_extend << ", "
       << color_to_string(arc.color)
       << ")";
    return os;
}

int main(int argc, char **argv) {
    using namespace std;
    namespace xml = rapidxml;
    const auto vm = parse_cmdline(argc, argv);

    if (!vm.count("file")) {
        cerr << "--file required" << endl;
        return 1;
    }

    const auto filename = vm["file"].as<std::string>();
    cout << "File: " << filename << endl;

    ifstream ifs(filename.c_str(), ios::in);
    if (!ifs) {
        cerr << "Unable to open: " << filename << endl;
        ::exit(1);
    }
    string text;
    string line;
    while (getline(ifs, line)) text += line;
    ifs.close();

    std::unique_ptr<char[]> buffer(new char[text.size() + 1]);
    memcpy(buffer.get(), text.c_str(), text.size());
    buffer[text.size()] = 0;

    xml::xml_document<> doc;
    doc.parse<0>(buffer.get());

    cout << "Name of first node is: " << doc.first_node()->name() << endl;
    xml::xml_node<> *root = doc.first_node();

    vector<Line> lines;
    vector<Arc> arcs;
    
    const string LINE = "Line";
    const string ARC = "Arc";
    for (xml::xml_node<> *node = root->first_node(); node; node = node->next_sibling()) {
        if (node->name() == LINE) {
            Line line;
            for (xml::xml_node<> *child = node->first_node(); child; child = child->next_sibling()) {
                const string name = child->name();
                if (name == "XStart") {
                    line.x_start = stod(child->value());
                } else if (name == "XEnd") {
                    line.x_end = stod(child->value());
                } else if (name == "YStart") {
                    line.y_start = stod(child->value());
                } else if (name == "YEnd") {
                    line.y_end = stod(child->value());
                } else if (name == "Color") {
                    line.color = translate_color(child->value());
                } else {
                    cout << "Unknown line child: " << name << endl;
                    assert(0);
                }
            }
            lines.push_back(std::move(line));
        } else if (node->name() == ARC) {
            Arc arc;
            for (xml::xml_node<> *child = node->first_node(); child; child = child->next_sibling()) {
                const string name = child->name();
                if (name == "XCenter") {
                    arc.x_center = stod(child->value());
                } else if (name == "YCenter") {
                    arc.y_center = stod(child->value());
                } else if (name == "Radius") {
                    arc.radius = stod(child->value());
                } else if (name == "ArcStart") {
                    arc.arc_start = stod(child->value());
                } else if (name == "ArcExtend") {
                    arc.arc_extend = stod(child->value());
                } else if (name == "Color") {
                    arc.color = translate_color(child->value());
                } else {
                    cout << "Unknown arc child: " << name << endl;
                    assert(0);
                }
            }
            arcs.push_back(std::move(arc));
        } else {
            cerr << "Unknown element: " << node->name() << endl;
            ::exit(1);
        }
    }

    const size_t WIDTH = 1000;
    const size_t HEIGHT = 1000;
    cv::Mat image(WIDTH, HEIGHT, CV_8UC3, cv::Scalar(0));

    // cv::Point2f start(100, HEIGHT-200);
    // cv::Point2f end(500, HEIGHT-800);
    // cv::line(image, start, end, color_to_scalar(Color::Red));

    // Arc arc(600, 600, 200, 10, 80, Color::Blue);
    // cv::Point2f center(arc.x_center, HEIGHT-arc.y_center);
    // cv::Size axes(arc.radius, arc.radius);
    // cv::ellipse(image,
    //             center,
    //             axes,
    //             0,
    //             arc.arc_start,
    //             arc.arc_start + arc.arc_extend - 180.,
    //             color_to_scalar(arc.color));
    
    
    for (const auto& line: lines) {
        // if (line.color != Color::Blue) continue;
        // cout << line << endl;
        cv::Point2f start(line.x_start, HEIGHT - line.y_start);
        cv::Point2f end(line.x_end, HEIGHT - line.y_end);
        cv::line(image, start, end, color_to_scalar(line.color));
    }
    for (const auto& arc: arcs) {
        // if (arc.color != Color::Blue) continue;
        // cout << arc << endl;
        cv::Point2f center(arc.x_center, HEIGHT - arc.y_center);
        cv::Size axes(arc.radius, arc.radius);
        cv::ellipse(image,
                    center,
                    axes,
                    90,
                    arc.arc_start,
                    360. - arc.arc_start + arc.arc_extend,
                    color_to_scalar(arc.color));
    }
    
    cv::imshow("Image", image);
    cv::waitKey(0);
    
    return 0;
}
