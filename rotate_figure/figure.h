// figure.h
#ifndef FIGURE_H
#define FIGURE_H

#include <QVector>
#include <QPointF>
#include <QtMath>

// Определение класса Figure для управления геометрической фигурой
class Figure
    {
public:
    // Конструктор, инициализирующий фигуру с заданным набором точек
    Figure(const QVector<QPointF>& points) : m_points(points) {}

    // Метод для перемещения фигуры на заданные смещения по осям X и Y
    void move(double dx, double dy)
    {
        // Перебор всех точек фигуры для их смещения
        for (int i = 0; i < m_points.size(); ++i) {
            m_points[i] += QPointF(dx, dy); // Добавление смещения к каждой точке
        }
    }

    // Метод для вращения фигуры на заданный угол в градусах
    void rotate(double angle)
    {
        double radians = qDegreesToRadians(angle); // Преобразование угла из градусов в радианы
        double sine = qSin(radians); // Вычисление синуса угла
        double cosine = qCos(radians); // Вычисление косинуса угла

        // Перебор всех точек для их вращения вокруг начала координат
        for (int i = 0; i < m_points.size(); ++i) {
            double x = m_points[i].x(); // Текущая координата X точки
            double y = m_points[i].y(); // Текущая координата Y точки
            // Вычисление новых координат точки после вращения
            m_points[i] = QPointF(x * cosine - y * sine, x * sine + y * cosine);
        }
    }

private:
    QVector<QPointF> m_points; // Вектор, хранящий точки, составляющие фигуру
};

#endif // FIGURE_H
