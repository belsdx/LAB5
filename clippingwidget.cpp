#include "clippingwidget.h"

ClippingWidget::ClippingWidget(QWidget *parent) : QWidget(parent), algorithm(SutherlandCohen) {
    setFixedSize(800, 600);
}

void ClippingWidget::setAlgorithm(int index) {
    algorithm = static_cast<Algorithm>(index);
    update();
}

void ClippingWidget::setClippingWindow(int x_min, int y_min, int x_max, int y_max) {
    xmin = x_min;
    ymin = y_min;
    xmax = x_max;
    ymax = y_max;
    update();
}

void ClippingWidget::clearLines() {
    lines.clear();
    update();
}

void ClippingWidget::addLine(int x1, int y1, int x2, int y2) {
    lines.append(qMakePair(qMakePair(x1, y1), qMakePair(x2, y2)));
    update();
}

void ClippingWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
    drawGrid(painter);
    drawAxes(painter);
    drawClippingWindow(painter);
    drawLines(painter);
    clipLines(painter);
}

void ClippingWidget::drawGrid(QPainter &painter) {
    painter.setPen(QColor(200, 200, 200));
    for (int x = 0; x <= width(); x += 20) {
        painter.drawLine(x, 0, x, height());
    }
    for (int y = 0; y <= height(); y += 20) {
        painter.drawLine(0, y, width(), y);
    }
}

void ClippingWidget::drawAxes(QPainter &painter) {
    painter.setPen(Qt::black);
    painter.drawLine(width() / 2, 0, width() / 2, height());
    painter.drawLine(0, height() / 2, width(), height() / 2);
}

void ClippingWidget::drawClippingWindow(QPainter &painter) {
    painter.setPen(Qt::blue);
    painter.drawRect(transformX(xmin), transformY(ymax), xmax - xmin, ymax - ymin);
}

void ClippingWidget::drawLines(QPainter &painter) {
    painter.setPen(Qt::red);
    for (const auto &line : lines) {
        painter.drawLine(transformX(line.first.first), transformY(line.first.second),
                         transformX(line.second.first), transformY(line.second.second));
    }
}

void ClippingWidget::clipLines(QPainter &painter) {
    painter.setPen(Qt::green);
    for (const auto &line : lines) {
        int x1 = line.first.first, y1 = line.first.second;
        int x2 = line.second.first, y2 = line.second.second;
        switch (algorithm) {
            case SutherlandCohen: sutherlandCohenClip(painter, x1, y1, x2, y2); break;
            case LiangBarsky: liangBarskyClip(painter, x1, y1, x2, y2); break;
            case Midpoint: midpointClip(painter, x1, y1, x2, y2); break;
        }
    }
}

int ClippingWidget::transformX(int x) {
    return x + width() / 2;
}

int ClippingWidget::transformY(int y) {
    return height() / 2 - y;
}

void ClippingWidget::sutherlandCohenClip(QPainter &painter, int x1, int y1, int x2, int y2) {
    const int INSIDE = 0; // 0000
    const int LEFT = 1;   // 0001
    const int RIGHT = 2;  // 0010
    const int BOTTOM = 4; // 0100
    const int TOP = 8;    // 1000

    auto computeCode = [this](int x, int y) {
        int code = INSIDE;
        if (x < xmin) code |= LEFT;
        else if (x > xmax) code |= RIGHT;
        if (y < ymin) code |= BOTTOM;
        else if (y > ymax) code |= TOP;
        return code;
    };

    int code1 = computeCode(x1, y1);
    int code2 = computeCode(x2, y2);
    bool accept = false;

    while (true) {
        if (!(code1 | code2)) {
            accept = true;
            break;
        } else if (code1 & code2) {
            break;
        } else {
            int code_out;
            int x, y;
            if (code1 != 0) {
                code_out = code1;
            } else {
                code_out = code2;
            }

            if (code_out & TOP) {
                x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1);
                y = ymax;
            } else if (code_out & BOTTOM) {
                x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1);
                y = ymin;
            } else if (code_out & RIGHT) {
                y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1);
                x = xmax;
            } else if (code_out & LEFT) {
                y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1);
                x = xmin;
            }

            if (code_out == code1) {
                x1 = x;
                y1 = y;
                code1 = computeCode(x1, y1);
            } else {
                x2 = x;
                y2 = y;
                code2 = computeCode(x2, y2);
            }
        }
    }
    if (accept) {
        painter.drawLine(transformX(x1), transformY(y1), transformX(x2), transformY(y2));
    }
}

void ClippingWidget::liangBarskyClip(QPainter &painter, int x1, int y1, int x2, int y2) {
    float dx = x2 - x1, dy = y2 - y1;
    float p[] = {-dx, dx, -dy, dy};
    float q[] = {x1 - xmin, xmax - x1, y1 - ymin, ymax - y1};

    float u1 = 0.0f, u2 = 1.0f;

    for (int i = 0; i < 4; ++i) {
        if (p[i] == 0) {
            if (q[i] < 0) return; // Line is parallel to clipping window and outside
        } else {
            float u = q[i] / p[i];
            if (p[i] < 0) {
                u1 = std::max(u1, u); // Update u1
            } else {
                u2 = std::min(u2, u); // Update u2
            }
        }
    }

    if (u1 > u2) return; // No intersection

    int nx1 = x1 + u1 * dx;
    int ny1 = y1 + u1 * dy;
    int nx2 = x1 + u2 * dx;
    int ny2 = y1 + u2 * dy;

    painter.drawLine(transformX(nx1), transformY(ny1), transformX(nx2), transformY(ny2));
}

void ClippingWidget::midpointClip(QPainter &painter, int x1, int y1, int x2, int y2) {
    auto isInside = [this](int x, int y) {
        return (x >= xmin && x <= xmax && y >= ymin && y <= ymax);
    };

    auto clip = [this, &painter, isInside](int x1, int y1, int x2, int y2) {
        if (isInside(x1, y1) && isInside(x2, y2)) {
            painter.drawLine(transformX(x1), transformY(y1), transformX(x2), transformY(y2));
        } else if (isInside(x1, y1)) {
            midpointClip(painter, x1, y1, (x1 + x2) / 2, (y1 + y2) / 2);
        } else if (isInside(x2, y2)) {
            midpointClip(painter, (x1 + x2) / 2, (y1 + y2) / 2, x2, y2);
        } else {
            midpointClip(painter, x1, y1, (x1 + x2) / 2, (y1 + y2) / 2);
            midpointClip(painter, (x1 + x2) / 2, (y1 + y2) / 2, x2, y2);
        }
    };

    clip(x1, y1, x2, y2);
}
