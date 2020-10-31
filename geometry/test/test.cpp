#include "geometry.h"

#include <cmath>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>


double distance(const Point& a, const Point& b) {
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y-b.y));
}

bool equals(double a, double b, double eps = 1e-6) {
    return a-b <= eps && b-a <= eps;
}

int main() {

    const int ax = -2, ay = 2, bx = 1, by = 2,
            cx = 3, cy = -1, dx = -1, dy = -2,
            ex = 1, ey = -1, fx = 6, fy = 1;

    Point a(ax, ay);
    Point b(bx, by);
    Point c(cx, cy);
    Point d(dx, dy);
    Point e(ex, ey);
    if (!equals(distance(a,b), 3)) {
        std::cerr << "Test 0 failed.\n";
        return 1;
    }
    if (!equals(distance(a,e), 3*sqrt(2))) {
        std::cerr << "Test 1 failed.\n";
        return 1;
    }

    Line ae(a, e);
    Line ea(e, a);
    Line line1(3, 5);
    Line line2(c, -1.5);

    Point f(fx, fy);
    Polygon abfcd({a, b, f, c, d});
    std::vector<Point> vec = {c, f, b, a, d};
    Polygon cfbad(vec);
    if (abfcd != cfbad) {
        std::cerr << "Test 2 failed. (operator ==)\n";
        return 1;
    }


    Triangle abd(a, b, d);
    Polygon bfced({b, f, c, e, d});
    Polygon abfced({a,b,f,c,e,d});
    if (!equals(abd.area() + bfced.area(), abfced.area())) {
        std::cerr << "Test 3 failed. (area)\n";
        return 1;
    }
    if (!(abd != abfced)) {
        std::cerr << "Test 4 failed. (operator !=)\n";
        return 1;
    }
    if (abfced == abd) {
        std::cerr << "Test 5 failed. (operator ==)\n";
        return 1;
    }

    Polygon newAbfced = abfced;
    newAbfced.rotate(Point(0,0), 50);
    newAbfced.scale(Point(0,0), 3);

    if (!equals(9 * abfced.area(), newAbfced.area())) {
        std::cerr << "Test 6 failed. (rotate, scale or area)\n";
        return 1;
    }
    if (!equals(3 * abfced.perimeter(), newAbfced.perimeter())) {
        std::cerr << "Test 7 failed. (rotate, scale or perimeter)\n";
        return 1;
    }
    auto ve = newAbfced.getVertices();
    std::reverse(ve.begin(), ve.end());
    std::rotate(ve.begin(), ve.begin() + 3, ve.end());
    newAbfced = Polygon(ve);
    newAbfced.scale(a, 1. / 3);
    newAbfced.reflex(line1);
    if (newAbfced == abfced) {
        std::cerr << "Test 8 failed. (transformations or equality)\n";
        return 1;
    }

    Point k(3, 1);
    Polygon bfkce({c, k, f, b, e});

    Rectangle rec_ae1(e, a, 1);
    Square sq_ae(a, e);
    if (!(rec_ae1 == sq_ae)) {
        std::cerr << "Test 8.5 failed. (sometimes circle may be equal to ellipse)\n";
        return 1;
    }
    Circle b3(b, 3);
    Ellipse cf5(c, f, 5);

    std::vector<Shape*> shapes;
    shapes.push_back(&abfced);
    shapes.push_back(&abd);
    shapes.push_back(&sq_ae);
    shapes.push_back(&rec_ae1);
    shapes.push_back(&bfkce);
    shapes.push_back(&b3);
    shapes.push_back(&cf5);

    for (auto & shape : shapes) {
        shape->scale(Point(5,5), 0.5);
    }

    // Ellipse testing
    cf5 = Ellipse(f, c, 5);
    {
        double c = sqrt(13)/2;
        double a = 5./2;
        double e = c/a;
        double per = 4*a*1.34050538; // std::comp_ellint_2
        double b =  a*sqrt(1-e*e);
        const double PI = 3.14159265;
        double ar = PI*a*b;
        //std::cerr << "XXXXXXXXX " << cf5.eccentricity() << ' ' << e << '\n';
        //std::cerr << "XXXXXXXXX " << cf5.perimeter() << ' ' << per << '\n';
        //std::cerr << "XXXXXXXXX " << cf5.area() << ' ' << ar << '\n';

        if (!equals(cf5.eccentricity(), e)) {
            std::cerr << "Test 9.0 failed. (ellipse eccentricity)\n";
            return 1;
        }
        if (!equals(cf5.perimeter(), per)) {
            std::cerr << "Test 9.1 failed. (ellipse perimeter). Consider this: https://en.cppreference.com/w/cpp/numeric/special_functions/comp_ellint_2\n";
            return 1;
        }
        if (!equals(cf5.area(), ar)) {
            std::cerr << "Test 9.2 failed. (ellipse area)\n";
            return 1;
        }
    }

    // Triangle testing
    abd = Triangle(d, b, a);
    {
        bool ok = true;
        Circle incircle = abd.inscribedCircle();
        Circle circumcircle = abd.circumscribedCircle();
        Point inc = incircle.center();
        Point circ = circumcircle.center();
        double r = incircle.radius(), R = circumcircle.radius();
        // Euler theorem
        ok = equals(distance(inc, circ), sqrt(R*R - 2*R*r));
        if (!ok) {
            std::cerr << "Test 10.0 failed. (https://en.wikipedia.org/wiki/Euler's_theorem_in_geometry)\n";
            return 1;
        }

        Circle eulerCircle = abd.ninePointsCircle();
        Line eulerLine = abd.EulerLine();
        Point orc = abd.orthocenter();

        // euler circle center lies in the middle of segment between orthocenter and circumcenter
        ok = equals(distance(orc, eulerCircle.center()), distance(circ, eulerCircle.center()));
        if (!ok) {
            std::cerr << "Test 10.1 failed. (nine-point circle center)\n";
            return 1;
        }
        // Радиус окружности девяти точек равен половине радиуса описанной окружности
        ok = equals(circumcircle.radius() / 2, eulerCircle.radius());
        // Описанная окружность есть образ окружности девяти точек
        // относительно гомотетии с центром в ортоцентре и коэффициентом 2
        Circle anotherCircle = eulerCircle;
        anotherCircle.scale(orc, 2);
        ok = ok && circumcircle == anotherCircle;

        // прямая Эйлера проходит через центроид и центр окружности девяти точек
        ok = ok && Line(eulerCircle.center(), abd.centroid()) == eulerLine;

        if (!ok) {
            std::cerr << "Test 10 failed. (triangle tests)\n";
            return 1;
        }
    }

    return 0;
}
