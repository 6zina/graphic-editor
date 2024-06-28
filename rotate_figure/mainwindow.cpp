#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QMouseEvent>

FigureWidget::FigureWidget(QWidget *parent) : QWidget(parent), m_rotationAngle(0), m_isDragging(false)
{
    setMouseTracking(true); // включаем отслеживание движения мыши
}

void FigureWidget::setCoordinates(const QVector<QPointF> &coords)
{
    m_coords = coords;
    update(); // перерисовываем виджет при изменении координат
}

void FigureWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setPen(Qt::gray);
    painter.setBrush(Qt::gray);

    QRectF boundingRect = QRectF(m_coords.at(0), m_coords.at(0));
    for (int i = 1; i < m_coords.size(); ++i) {
        boundingRect = boundingRect.united(QRectF(m_coords.at(i), m_coords.at(i)));
    }
    QPointF center = boundingRect.center();

    painter.translate(width() / 2, height() / 2);
    painter.scale(0.1, 0.1);
    painter.translate(-center);
    painter.rotate(m_rotationAngle);
    painter.translate(center);

    painter.drawPolygon(m_coords);
}
void FigureWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_lastPos = event->pos();
        m_isDragging = true;
    }
}
void FigureWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging)
    {
        int dx = event->x() - m_lastPos.x();
        int dy = event->y() - m_lastPos.y();

        m_lastPos = event->pos();

        for (int i = 0; i < m_coords.size(); ++i)
        {
            m_coords[i] += QPointF(dx, dy);
        }

        update();
    }
}
void FigureWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
    }
}

void FigureWidget::wheelEvent(QWheelEvent *event)
{
    if (event->buttons() & Qt::MiddleButton) {
        m_rotationAngle += 10;
    } else {
        int numDegrees = event->angleDelta().y() / 10;
        m_rotationAngle += numDegrees;
    }

    update();
}
