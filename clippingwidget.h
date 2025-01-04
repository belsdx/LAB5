#ifndef CLIPPINGWIDGET_H
#define CLIPPINGWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QPainter>

enum Algorithm {
    SutherlandCohen,
    LiangBarsky,
    Midpoint
};

class ClippingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClippingWidget(QWidget *parent = nullptr);

    void setAlgorithm(int index);
    void setClippingWindow(int x_min, int y_min, int x_max, int y_max);
    void clearLines();
    void addLine(int x1, int y1, int x2, int y2);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawGrid(QPainter &painter);
    void drawAxes(QPainter &painter);
    void drawClippingWindow(QPainter &painter);
    void drawLines(QPainter &painter);
    void clipLines(QPainter &painter);
    void sutherlandCohenClip(QPainter &painter, int x1, int y1, int x2, int y2);
    void liangBarskyClip(QPainter &painter, int x1, int y1, int x2, int y2);
    void midpointClip(QPainter &painter, int x1, int y1, int x2, int y2);

    Algorithm algorithm;
    QVector<QPair<QPair<int, int>, QPair<int, int>>> lines;
    int xmin, ymin, xmax, ymax;
    int transformX(int x);
    int transformY(int y);
};

#endif // CLIPPINGWIDGET_H
