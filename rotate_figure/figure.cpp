#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "figure.h"
#include "mainwindow.h" // Предположим, что у вас есть свой класс для отображения фигуры

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file("file_souzka.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;
    QVector<QPointF> coords;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(",");
        double x = parts[0].toDouble();
        double y = parts[1].toDouble();
        coords.append(QPointF(x, y));
    }
    file.close();

    Figure figure(coords);
    figure.rotate(45); // Поворот на 45 градусов

    // Создаем виджет для отображения фигуры
    FigureWidget widget;

    widget.show();

    return a.exec();
}
