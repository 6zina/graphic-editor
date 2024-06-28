#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QVector>
#include <QWheelEvent>
#include <QMouseEvent>

class FigureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FigureWidget(QWidget *parent = nullptr);
    void setCoordinates(const QVector<QPointF> &coords); // установка координат фигуры

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QVector<QPointF> m_coords;
    QVector<QPointF> m_originalCoords;
    int m_rotationAngle; // угол поворота фигуры
    QPoint m_lastPos; // последняя позиция мыши
    bool m_isDragging; // флаг перетаскивания фигуры
};

#endif
