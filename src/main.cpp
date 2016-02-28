#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <boost/program_options.hpp>
#include "rapidxml.hpp"
#include "command_line.h"

enum Color {
    Blue,
    Green,
    Red,
    Yellow,
    White
};

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

struct Circle {
    Circle()
        : x_center(0.0)
        , y_center(0.0)
        , radius(0.0)
        , arc_start(0.0)
        , arc_extend(0.0)
        , color(Color::White)
        {}

    Circle(double x, double y, double r, double s, double e, Color c=Color::White)
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

    const string LINE = "Line";
    const string ARC = "Arc";
    for (xml::xml_node<> *node = root->first_node(); node; node = node->next_sibling()) {
        if (node->name() == LINE) {
            cout << "LINE" << endl;
            for (xml::xml_node<> *child = node->first_node(); child; child = child->next_sibling()) {
                cout << "\t" << child->name() << endl;
            }
        } else if (node->name() == ARC) {
            cout << "ARC" << endl;
            for (xml::xml_node<> *child = node->first_node(); child; child = child->next_sibling()) {
                cout << "\t" << child->name() << endl;
            }            
        } else {
            cerr << "Unknown element: " << node->name() << endl;
            ::exit(1);
        }
    }

    return 0;
}
