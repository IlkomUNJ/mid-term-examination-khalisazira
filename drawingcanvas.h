#ifndef DRAWINGCANVAS_H
#define DRAWINGCANVAS_H

#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QPainter>
#include <QMouseEvent>
#include <iostream>
#include <iomanip>
#include <QPixmap>
#include "CustomMatrix.h"
#include <array>
#include <vector>

using namespace std;
using Array3x3 = std::array<std::array<bool, 3>, 3>;


class DrawingCanvas : public QWidget
{
    Q_OBJECT
private:
    const int WINDOW_WIDTH=600;
    const int WINDOW_HEIGHT=400;

    string isSegmentPattern(const CustomMatrix& matrix);
    vector<pair<string, std::array<std::array<bool, 3>, 3>>> m_segmentPatterns;

public:
    explicit DrawingCanvas(QWidget *parent = nullptr);

    // Slot to clear all points from the canvas
    void clearPoints();
    void paintLines();
    void segmentDetection();
    bool getIsPaintLinesClicked() const {return isPaintLinesClicked; }

protected:

    void paintEvent(QPaintEvent *event) override;

    // Overridden method to handle mouse clicks
    void mousePressEvent(QMouseEvent *event) override;

private:
    // A vector to store all the points drawn by the user
    QVector<QPoint> m_points;
    QVector<pair<QPoint, string>> m_detectedSegments;

    bool isPaintLinesClicked = false;
};
#endif // DRAWINGCANVAS_H
