#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPolygonItem>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QVector>
#include <QPointF>
#include <QtMath>
#include <QTime>
#include <QRandomGenerator>
#include <QProcess>

QVector<QVector<int>> globalMatrix;

class Figure : public QGraphicsPolygonItem {
public:
    // Конструктор класса, инициализирующий фигуру с заданными точками и размерами зоны и сектора
    Figure(const QVector<QPointF>& points, int zoneSize, int sectorSize) :
        QGraphicsPolygonItem(QPolygonF(points)), // Инициализация базового класса с полигоном из точек
        zoneSize(zoneSize),
        sectorSize(sectorSize)
    {
        setBrush(QBrush(Qt::blue)); // Установка цвета заливки фигуры
        setPen(QPen(Qt::black)); // Установка цвета контура фигуры
        // Определение прямоугольной зоны, в которой может находиться фигура
        zoneRect = QRectF(0, 0, sectorSize * zoneSize, sectorSize * zoneSize);
        // Поиск случайной позиции в пределах зоны, не пересекающейся с другими фигурами
        do {
            qreal randomX = QRandomGenerator::global()->bounded(zoneRect.width() - boundingRect().width());
            qreal randomY = QRandomGenerator::global()->bounded(zoneRect.height() - boundingRect().height());
            setPos(randomX, randomY);
        } while (collidesWithOtherFigures());
    }

    // Обработчик события отпускания кнопки мыши
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override {
        QGraphicsPolygonItem::mouseReleaseEvent(event); // Вызов метода базового класса
        updateGlobalMatrixForAllFigures(this->scene()); // Обновление глобальной матрицы для всех фигур
    }

    // Обработчик события перемещения мыши с зажатой кнопкой
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override {
        QPointF newPos = event->scenePos(); // Новая позиция курсора
        if (zoneRect.contains(newPos)) { // Проверка, что новая позиция в пределах зоны
            QPointF delta = newPos - m_lastPos; // Вычисление смещения
            moveBy(delta.x(), delta.y()); // Перемещение фигуры

            // Проверка на пересечение с другими фигурами после перемещения
            if (collidesWithOtherFigures()) {
                moveBy(-delta.x(), -delta.y()); // Возврат на исходную позицию при пересечении
                m_lastPos = event->scenePos() - delta; // Корректировка последней позиции
                return; // Прекращение обработки события
            }
            m_lastPos = newPos; // Обновление последней позиции
        }
    }

    // Метод для очистки глобальной матрицы
    void clearGlobalMatrix() {
        for (int i = 0; i < zoneSize; ++i) {
            for (int j = 0; j < zoneSize; ++j) {
                globalMatrix[j][i] = 0;
            }
        }
    }

    // Обработчик события прокрутки колесика мыши
    void wheelEvent(QGraphicsSceneWheelEvent* event) override {
        rotate(event->delta()); // Вызов метода вращения
    }

    // Обработчик события нажатия кнопки мыши
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
        if (event->button() == Qt::MiddleButton) {
            rotate(15); // Вращение на 15 градусов при нажатии средней кнопки мыши
        } else {
            m_lastPos = event->scenePos(); // Запоминание позиции при нажатии
        }
    }

private:
    QPointF m_lastPos; // Последняя позиция курсора
    QRectF zoneRect; // Прямоугольник зоны, в которой может находиться фигура
    int zoneSize; // Размер зоны
    int sectorSize; // Размер сектора

    // Метод для вращения фигуры
    void rotate(int angleDelta) {
        double angle = rotation() + angleDelta / 8; // Вычисление нового угла вращения
        setRotation(angle); // Установка угла вращения

        // Проверка на пересечение с другими фигурами после вращения
        if (collidesWithOtherFigures()) {
            setRotation(rotation() - angleDelta / 8); // Возврат к предыдущему углу при пересечении
            return; // Прекращение обработки
        }
    }

    // Метод для сохранения матрицы в файл
    void saveMatrixToFile(const QVector<QVector<int>>& matrix) {
        QFile file("C:/Users/ladan/OneDrive/Рабочий стол/papkamatrix/matrix.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (const auto& row : matrix) {
                for (int value : row) {
                    out << value << " ";
                }
                out << "\n";
            }
            file.close();
        }
    }
    // Метод для анализа позиции фигуры и обновления глобальной матрицы
    void analyzeFigurePosition(const QPointF& position, Figure* figure) {
        for (int i = 0; i < zoneSize; ++i) {
            for (int j = 0; j < zoneSize; ++j) {
                QGraphicsRectItem* sectorRect = new QGraphicsRectItem(i * sectorSize, j * sectorSize, sectorSize, sectorSize);
                if (figure->collidesWithItem(sectorRect)) {
                    globalMatrix[j][i] = 1; // Отмечаем сектор как занятый
                }
                delete sectorRect; // Удаление временного объекта для освобождения памяти
            }
        }
    }
    // Метод для обновления глобальной матрицы для всех фигур на сцене
    void updateGlobalMatrixForAllFigures(QGraphicsScene* scene) {
        clearGlobalMatrix(); // Очищаем матрицу перед обновлением

        // Проходим по всем элементам сцены
        QList<QGraphicsItem*> items = scene->items();
        for (QGraphicsItem* item : items) {
            Figure* figure = dynamic_cast<Figure*>(item);
            if (figure) {
                analyzeFigurePosition(figure->pos(), figure); // Анализируем позицию каждой фигуры
            }
        }

        saveMatrixToFile(globalMatrix); // Сохраняем обновлённую матрицу в файл
    }
    // Метод для проверки пересечения текущей фигуры с другими фигурами на сцене
    bool collidesWithOtherFigures() {
        // Получаем список всех элементов, с которыми пересекается текущая фигура
        QList<QGraphicsItem *> collidingItems = this->collidingItems();
        for (QGraphicsItem* item : collidingItems) {
            if (item != this && item->type() == Figure::Type) {
                return true; // Возвращаем true, если есть пересечение с другой фигурой
            }
        }
        return false; // Возвращаем false, если пересечений нет
    }
};

class Zone : public QGraphicsItemGroup {
public:
    // Конструктор класса Zone, инициализирующий зону с заданным размером и размером сектора
    Zone(int zoneSize, int sectorSize) : zoneSize(zoneSize), sectorSize(sectorSize) {
        // Создание сетки из прямоугольников (секторов)
        for (int i = 0; i < zoneSize; i++) {
            for (int j = 0; j < zoneSize; j++) {
                // Создание прямоугольника для каждого сектора
                QGraphicsRectItem* subZone = new QGraphicsRectItem(i * sectorSize, j * sectorSize, sectorSize, sectorSize);
                subZone->setPen(QPen(Qt::black)); // Установка черного контура для сектора
                subZone->setBrush(QBrush(Qt::lightGray)); // Установка светло-серого фона для сектора
                addToGroup(subZone); // Добавление сектора в группу (Zone)
            }
        }
    }

private:
    int zoneSize; // Размер зоны в секторах
    int sectorSize; // Размер каждого сектора в пикселях
};
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QProcess process;
    // Запуск внешнего скрипта Python для генерации координат
    process.start("cmd", QStringList() << "/c" << "creating_coord.py");
    process.waitForFinished(-1); // Ожидание завершения скрипта

    QGraphicsScene scene;
    QGraphicsView view(&scene);

    int zoneSize = 4; // Количество секторов по одной стороне зоны
    int sectorSize = 300; // Размер каждого сектора в пикселях

    // Инициализация генератора случайных чисел
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    // Создание объекта зоны и добавление его в сцену
    Zone* zone = new Zone(zoneSize, sectorSize);
    scene.addItem(zone);

    // Инициализация глобальной матрицы для отслеживания занятости секторов
    globalMatrix.resize(zoneSize);
    for (int i = 0; i < zoneSize; ++i) {
        globalMatrix[i].resize(zoneSize);
    }

    // Чтение данных из файла
    QFile file("output_new.txt");//file_krug,file_kvadrat,file_file_souzka,file_zadinka,output_allfig,output_new,output_fig,output1,allfig
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;

    QTextStream in(&file);
    QString line = in.readLine();

    QVector<QPointF> currentFigurePoints;

    // Чтение координат точек из файла и создание фигур
    while (!in.atEnd()) {
        line = in.readLine();
        if (line == "0,0") {
            if (!currentFigurePoints.isEmpty()) {
                Figure* figure = new Figure(currentFigurePoints, zoneSize, sectorSize);
                scene.addItem(figure);
                currentFigurePoints.clear();
            }
        } else {
            QStringList parts = line.split(",");
            double x = parts[0].toDouble() * 0.1;
            double y = parts[1].toDouble() * 0.1;
            currentFigurePoints.append(QPointF(x, y));
        }
    }

    if (!currentFigurePoints.isEmpty()) {
        Figure* figure = new Figure(currentFigurePoints, zoneSize, sectorSize);
        scene.addItem(figure);
    }

    file.close();

    // Установка размеров и масштаба окна просмотра
    int viewWidth = 800; // Желаемая ширина окна
    int viewHeight = 600; // Желаемая высота окна

    // Вычисление масштаба для соответствия размерам окна
    double scaleX = (double)viewWidth / (zoneSize * sectorSize);
    double scaleY = (double)viewHeight / (zoneSize * sectorSize);

    // Увеличение масштаба для лучшего визуального восприятия
    double scaleAdjustment = 1.1;
    double finalScaleX = scaleX * scaleAdjustment;
    double finalScaleY = scaleY * scaleAdjustment;

    // Применение масштаба к окну просмотра
    view.scale(finalScaleX, finalScaleY);

    // Установка сцены и отображение окна
    view.setScene(&scene);
    view.show();

    return a.exec();
}
