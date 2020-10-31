#pragma once
#include <cmath>
#include <vector>

const double EPS = 1e-6;

struct Point {

    double x, y;

    Point() = default;
    explicit Point(const double& x, const double& y) : x(x), y(y) {}
    Point(const Point& point) : x(point.x), y(point.y) {}

    ~Point() = default;

    Point& operator=(const Point& point) {
        if (this == &point)
            return *this;

        x = point.x;
        y = point.y;

        return *this;
    }

    bool operator==(const Point& point) const {
        return (std::abs(point.x - x) < EPS && std::abs(point.y - y) < EPS);
    }

    bool operator!=(const Point& point) const {
        return !(point == *this);
    }

    double distanceBetweenPoints(const Point& point) const {
        return sqrt(pow(point.x - x, 2) + pow(point.y - y, 2));
    }

    Point averageWithPoint(const Point& point) const {
        return Point((x + point.x) / 2, (y + point.y) / 2);
    }

    void rotate(const Point& point, const double& angle) {
        double xBuffer = x, yBuffer = y;
        double angleBuffer = angle * M_PI / 180;
        x = point.x + (xBuffer - point.x) * cos(angleBuffer) - (yBuffer - point.y) * sin(angleBuffer);
        y = point.y + (xBuffer - point.x) * sin(angleBuffer) + (yBuffer - point.y) * cos(angleBuffer);
    }

    void scale(const Point& center, const double& coefficient) {
        x = coefficient * (x - center.x) + center.x;
        y = coefficient * (y - center.y) + center.y;
    }

};

class Line {
private:
    //Ax + By + C = 0
    double a, b, c;

public:

    Line() = default;

    explicit Line(const double& a, const double& b, const double& c) : a(a), b(b), c(c) {}

    explicit Line(const Point& one, const Point& two) : a(one.y - two.y),
                                                        b(two.x - one.x),
                                                        c(one.x * two.y - two.x * one.y) {
        if (a < 0) {
            a *= -1;
            b *= -1;
            c *= -1;
        }
        if (a > EPS) {
            b /= a;
            c /= a;
            a = 1;
        } else {
            a /= b;
            c /= b;
            b = 1;
        }
    }

    explicit Line(const double& a, const double& c) : a(a), b(-1), c(c) {}

    explicit Line(const Point& point, const double& a) : a(a), b(-1), c(point.y - a * point.x) {}

    Line(const Line& line) : a(line.a), b(line.b), c(line.c) {}

    Line& operator=(const Line& line) {
        if (this == &line)
            return *this;

        a = line.a;
        b = line.b;
        c = line.c;
        return *this;
    }

    ~Line() = default;

    bool operator==(const Line& line) const {
        return (std::abs(a - line.a) < EPS && std::abs(b - line.b) < EPS && std::abs(c - line.c) < EPS);
    }

    bool operator!=(const Line& line) const {
        return !(*this == line);
    }

    Point intersectionOfLines(const Line& line) const {
        if (this == &line)
            throw "Lines are the same!";
        if (std::abs(a * line.b - line.a * b) < EPS)
            throw "Lines are parallel!";

        double y = (a * line.c - line.a * c) / (line.a * b - a * line.b);
        double x = ((a != 0) ? ((-c - b * y) / a) : ((-line.c - line.b * y) / line.a));
        return Point(x, y);
    }

    Line getPerpendicularLine(const Point& point) const {
        return Line(-b, a, b * point.x - a * point.y);
    }

};

class Shape {
public:

    Shape() = default;

    virtual double perimeter() const = 0;
    virtual double area() const = 0;
    virtual bool operator==(const Shape&) const = 0;
    bool operator!=(const Shape& shape) const {
        return !(*this == shape);
    }

    virtual void rotate(const Point&, const double&) = 0;
    virtual void reflex(const Point&) = 0;
    virtual void reflex(const Line&) = 0;
    virtual void scale(const Point&, const double&) = 0;

};

class Polygon : public Shape {
protected:

    std::vector<Point> vertices;

public:

    Polygon(const std::vector<Point>& vertices) : vertices(vertices) {}

    size_t verticesCount() const {
        return vertices.size();
    }

    std::vector<Point> getVertices() const {
        return std::vector<Point>(vertices);
    }

    double perimeter() const override {
        double result = 0;
        for (size_t i = 0; i < vertices.size(); ++i) {
            if (i != vertices.size() - 1) {
                result += vertices[i].distanceBetweenPoints(vertices[i + 1]);
            } else {
                result += vertices[i].distanceBetweenPoints(vertices[0]);
            }
        }
        return result;
    }
    double area() const override {
        double result = 0;
        for (size_t i = 0; i < vertices.size(); ++i) {
            if (i != vertices.size() - 1) {
                result += vertices[i].x * vertices[i + 1].y;
                result -= vertices[i].y * vertices[i + 1].x;
            } else {
                result += vertices[i].x * vertices[0].y;
                result -= vertices[i].y * vertices[0].x;
            }
        }
        result /= 2.;
        return result;
    }

    bool operator==(const Shape& shape) const override {
        if (Polygon* polygon = (Polygon*) (&shape)) {
            if (vertices.size() == polygon->vertices.size()) {
                std::vector<bool> used(vertices.size(), false);
                for (size_t i = 0; i < vertices.size(); ++i) {
                    for (size_t j = 0; j < polygon->vertices.size(); ++j) {
                        if (vertices[i] == polygon->vertices[j] && !used[j]) {
                            used[j] = true;
                            break;
                        }
                        if (j == polygon->vertices.size() - 1) {
                            return false;
                        }
                    }
                }
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    void rotate(const Point& point, const double& angle) override {
        for (auto& p : vertices) {
            p.rotate(point, angle);
        }
    }

    void reflex(const Point& point) override {
        rotate(point, M_PI);
    }

    void reflex(const Line& line) override {
        for (auto& p : vertices) {
            Line bufferLine = line.getPerpendicularLine(p);
            Point point = line.intersectionOfLines(bufferLine);
            p.rotate(point, M_PI);
        }
    }

    void scale(const Point& center, const double& coefficient) override {
        for (auto& p : vertices) {
            p.scale(center, coefficient);
        }
    }

};

class Rectangle : public Polygon {
public:

    explicit Rectangle(const Point& one, const Point& two, const double& attitude) : Polygon({one, two}) {
        vertices.resize(4);
        std::swap(vertices[1], vertices[2]);

        double angle = 180 * atan(attitude) / M_PI;
        Point center((vertices[0].x + vertices[2].x) / 2, (vertices[0].y + vertices[2].y) / 2);
        vertices[1] = vertices[2];
        vertices[1].rotate(center, 2 * angle);
        vertices[3] = vertices[0];
        vertices[3].rotate(center, 2 * angle);
    }

    Point center() const {
        return Point((vertices[0].x + vertices[2].x) / 2, (vertices[0].y + vertices[2].y) / 2);
    }

    std::pair<Line, Line> diagonals() const {
        return std::pair<Line, Line>(Line(vertices[0], vertices[2]),
                                     Line(vertices[1], vertices[3]));
    }

};

class Ellipse : public Shape {
protected:

    Point focusOne, focusTwo;
    double distance;

public:

    explicit Ellipse(const Point& focusOne, const Point& focusTwo, const double& distance)
        : focusOne(focusOne), focusTwo(focusTwo), distance(distance / 2) {}

    std::pair<Point, Point> focuses() const {
        return std::pair<Point, Point>(focusOne, focusTwo);
    }

    double eccentricity() const {
        return focusOne.distanceBetweenPoints(focusTwo) / (2 * distance);
    }

    Point center() const {
        return Point((focusOne.x + focusTwo.x) / 2, (focusOne.y + focusTwo.y) / 2);
    }

    double perimeter() const override {
        return 4 * distance * std::comp_ellint_2(eccentricity());
    }

    double area() const override {
        double e = eccentricity();
        double a = focusOne.distanceBetweenPoints(focusTwo) / (2 * e);
        double b = focusOne.distanceBetweenPoints(focusTwo) * sqrt(1 - pow(e, 2)) / (2 * e);
        return M_PI * a * b;
    }

    bool operator==(const Shape& shape) const override {
        if (Ellipse* ellipse = (Ellipse*) (&shape)) {
            if (focusOne == ellipse->focusOne && focusTwo == ellipse->focusTwo
                && std::abs(distance - ellipse->distance) < EPS) {

                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    void rotate(const Point& point, const double& angle) override {
        focusOne.rotate(point, angle);
        focusTwo.rotate(point, angle);
    }

    void reflex(const Point& point) override {
        rotate(point, M_PI);
    }

    void reflex(const Line& line) override {
        Line bufferLine = line.getPerpendicularLine(focusOne);
        Point point = line.intersectionOfLines(bufferLine);
        focusOne.rotate(point, M_PI);
        bufferLine = line.getPerpendicularLine(focusTwo);
        point = line.intersectionOfLines(bufferLine);
        focusTwo.rotate(point, M_PI);
    }

    void scale(const Point& center, const double& coefficient) override {
        focusOne.scale(center, coefficient);
        focusTwo.scale(center, coefficient);
        distance *= coefficient;
    }

};

class Circle : public Ellipse {
public:

    explicit Circle(const Point& center, const double& radius)
        : Ellipse(center, center, 2 * radius) {}

    double radius() const {
        return distance;
    }

};

class Square : public Rectangle {
public:

    explicit Square(const Point& one, const Point& two) : Rectangle(one, two, 1) {}

    Circle circumscribedCircle() {
        return Circle(center(), vertices[0].distanceBetweenPoints(vertices[2]) / 2);
    }

    Circle inscribedCircle() {
        return Circle(center(), vertices[0].distanceBetweenPoints(vertices[1]) / 2);
    }

};

class Triangle : public Polygon {
public:

    explicit Triangle(const Point& one, const Point& two, const Point& three) : Polygon({one, two, three}) {}

    Circle circumscribedCircle() {
        Point a = vertices[0], b = vertices[1], c = vertices[2];

        double d = 2 * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));
        double centerX = ((pow(a.x, 2) + pow(a.y, 2)) * (b.y - c.y) +
            (pow(b.x, 2) + pow(b.y, 2)) * (c.y - a.y) +
            (pow(c.x, 2) + pow(c.y, 2)) * (a.y - b.y)) / d;
        double centerY = ((pow(a.x, 2) + pow(a.y, 2)) * (c.x - b.x) +
            (pow(b.x, 2) + pow(b.y, 2)) * (a.x - c.x) +
            (pow(c.x, 2) + pow(c.y, 2)) * (b.x - a.x)) / d;

        Point center(centerX, centerY);

        double aDist = vertices[1].distanceBetweenPoints(vertices[2]);
        double bDist = vertices[0].distanceBetweenPoints(vertices[2]);
        double cDist = vertices[0].distanceBetweenPoints(vertices[1]);

        double r = aDist * bDist * cDist / (4 * area());

        return Circle(center, r);
    }

    Circle inscribedCircle() {
        double a = vertices[1].distanceBetweenPoints(vertices[2]);
        double b = vertices[0].distanceBetweenPoints(vertices[2]);
        double c = vertices[0].distanceBetweenPoints(vertices[1]);

        double centerX = (a * vertices[0].x + b * vertices[1].x + c * vertices[2].x) / perimeter();
        double centerY = (a * vertices[0].y + b * vertices[1].y + c * vertices[2].y) / perimeter();

        Point center(centerX, centerY);

        double p = perimeter() / 2;
        double r = sqrt((p - a) * (p - b) * (p - c) / p);

        return Circle(center, r);
    }

    Point centroid() {
        double medX = (vertices[0].x + vertices[1].x + vertices[2].x) / 3;
        double medY = (vertices[0].y + vertices[1].y + vertices[2].y) / 3;
        return Point(medX, medY);
    }

    Point orthocenter() {
        Point a = vertices[0], b = vertices[1], c = vertices[2];

        double orthocenterX = a.y * a.y * (c.y - b.y) + b.x * c.x * (c.y - b.y) +
            (b.y * b.y * (a.y - c.y) + a.x * c.x * (a.y - c.y)) +
            (c.y * c.y * (b.y - a.y) + a.x * b.x * (b.y - a.y));
        orthocenterX /= (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));

        double orthocenterY = a.x * a.x * (b.x - c.x) + b.y * c.y * (b.x - c.x) +
            (b.x * b.x * (c.x - a.x) + a.y * c.y * (c.x - a.x)) +
            (c.x * c.x * (a.x - b.x) + a.y * b.y * (a.x - b.x));
        orthocenterY /= (a.y * (c.x - b.x) + b.y * (a.x - c.x) + c.y * (b.x - a.x));

        return Point(orthocenterX, orthocenterY);
    }

    Line EulerLine() {
        return Line(centroid(), orthocenter());
    }

    Circle ninePointsCircle() {
        Circle circumscribed = circumscribedCircle();
        return Circle(orthocenter().averageWithPoint(circumscribed.center()), circumscribed.radius() / 2);
    }

};