/*!
 * \file graphwindow.cpp
 * \brief Реализация окна с тремя графиками на одной оси.
 */
//! \brief Файл реализации классов PlotWidget и GraphWindow

#include "graphwindow.h"
//! \brief Подключение заголовочного файла с объявлениями классов графического окна

#include <QVBoxLayout>
//! \brief Подключение класса вертикального компоновщика

#include <QHBoxLayout>
//! \brief Подключение класса горизонтального компоновщика

#include <QGroupBox>
//! \brief Подключение класса группы с рамкой для группировки виджетов

#include <QFormLayout>
//! \brief Подключение класса формы с двумя колонками (метка-поле)

#include <QLabel>
//! \brief Подключение класса текстовой метки

#include <QPainter>
//! \brief Подключение класса для рисования на виджетах

#include <QPaintEvent>
//! \brief Подключение класса события перерисовки

#include <QtMath>
//! \brief Подключение математических функций Qt

#include <QDoubleValidator>
//! \brief Подключение класса валидатора для чисел с плавающей точкой

#include <QScrollArea>
//! \brief Подключение класса области с прокруткой

#include <QResizeEvent>
//! \brief Подключение класса события изменения размера

// ==================== PlotWidget ====================
//! \brief Разделительная секция для класса PlotWidget

PlotWidget::PlotWidget(QWidget *parent) : QWidget(parent) {
    //! \brief Конструктор виджета для отображения графиков
    //! \param parent Родительский виджет
    setMinimumSize(600, 450);
    //! \brief Устанавливает минимальный размер виджета (ширина 600, высота 450)

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //! \brief Устанавливает политику растяжения виджета при изменении размера родителя

    setAttribute(Qt::WA_OpaquePaintEvent, false);
    //! \brief Отключает непрозрачность события перерисовки для корректной отрисовки
}

void PlotWidget::setParams(double a1, double b1, double c1,
                           double a2, double b2, double c2,
                           double a3, double b3, double c3,
                           bool show1, bool show2, bool show3) {
    //! \brief Устанавливает параметры всех трех функций и их видимость
    //! \param a1,b1,c1 Коэффициенты первой функции (логарифмической)
    //! \param a2,b2,c2 Коэффициенты второй функции (кубической)
    //! \param a3,b3,c3 Коэффициенты третьей функции (гиперболической)
    //! \param show1 Флаг видимости первого графика
    //! \param show2 Флаг видимости второго графика
    //! \param show3 Флаг видимости третьего графика

    m_a1 = a1; m_b1 = b1; m_c1 = c1;
    //! \brief Сохраняет коэффициенты первой функции
    m_a2 = a2; m_b2 = b2; m_c2 = c2;
    //! \brief Сохраняет коэффициенты второй функции
    m_a3 = a3; m_b3 = b3; m_c3 = c3;
    //! \brief Сохраняет коэффициенты третьей функции

    m_show1 = show1;
    //! \brief Сохраняет флаг видимости первого графика
    m_show2 = show2;
    //! \brief Сохраняет флаг видимости второго графика
    m_show3 = show3;
    //! \brief Сохраняет флаг видимости третьего графика

    update();
    //! \brief Запрашивает перерисовку виджета
}

double PlotWidget::evaluateFunction1(double x) const {
    //! \brief Вычисляет значение первой функции: a·ln(x + b) + c
    //! \param x Аргумент функции
    //! \return Значение функции или NaN при некорректных аргументах

    double arg = x + m_b1;
    //! \brief Вычисляет аргумент логарифма (x + b)

    if (arg <= 0) return NAN;
    //! \brief Проверяет, что аргумент логарифма положителен, иначе возвращает NaN

    return m_a1 * qLn(arg) + m_c1;
    //! \brief Возвращает значение функции: a·ln(arg) + c
}

double PlotWidget::evaluateFunction2(double x) const {
    //! \brief Вычисляет значение второй функции: a·x³ + b·x + c
    //! \param x Аргумент функции
    //! \return Значение кубического многочлена

    return m_a2 * x * x * x + m_b2 * x + m_c2;
    //! \brief Возвращает значение кубической функции
}

double PlotWidget::evaluateFunction3(double x) const {
    //! \brief Вычисляет значение третьей функции: a/(x+b) + c
    //! \param x Аргумент функции
    //! \return Значение функции или NaN при делении на ноль

    double denom = x + m_b3;
    //! \brief Вычисляет знаменатель дроби (x + b)

    if (qAbs(denom) < 1e-6) return NAN;
    //! \brief Проверяет, что знаменатель не близок к нулю (избегает деления на ноль)

    return m_a3 / denom + m_c3;
    //! \brief Возвращает значение гиперболической функции
}

void PlotWidget::paintEvent(QPaintEvent *) {
    //! \brief Обработчик события перерисовки виджета
    //! \param event Указатель на событие перерисовки (не используется)

    QPainter p(this);
    //! \brief Создает объект рисовальщика для текущего виджета

    p.setRenderHint(QPainter::Antialiasing);
    //! \brief Включает сглаживание линий

    int W = width(), H = height();
    //! \brief Получает текущие ширину и высоту виджета

    int padLeft = 60, padRight = 40, padTop = 40, padBottom = 50;
    //! \brief Определяет отступы от краев виджета (слева, справа, сверху, снизу)

    double xRange = m_xMax - m_xMin;
    //! \brief Вычисляет диапазон значений X для отображения

    // Собираем точки для всех функций
    //! \brief Векторы для хранения точек всех трех функций

    std::vector<std::pair<double, double>> points1, points2, points3;
    //! \brief Создает векторы пар (x, y) для каждого графика

    double yMin = 1e18, yMax = -1e18;
    //! \brief Инициализирует минимальное и максимальное значение Y

    int N = 1000;
    //! \brief Количество точек для построения каждого графика

    for (int i = 0; i <= N; ++i) {
        //! \brief Цикл вычисления точек для всех функций

        double x = m_xMin + xRange * i / N;
        //! \brief Вычисляет текущее значение X с равномерным шагом

        if (m_show1) {
            //! \brief Если первый график видим, вычисляем точки
            double y = evaluateFunction1(x);
            //! \brief Вычисляет значение первой функции

            if (!qIsNaN(y) && !qIsInf(y) && qAbs(y) < 1000) {
                //! \brief Проверяет корректность значения (не NaN, не бесконечность, не слишком большое)
                points1.push_back({x, y});
                //! \brief Добавляет точку в вектор первого графика
                yMin = qMin(yMin, y);
                //! \brief Обновляет минимальное значение Y
                yMax = qMax(yMax, y);
                //! \brief Обновляет максимальное значение Y
            }
        }

        if (m_show2) {
            //! \brief Если второй график видим, вычисляем точки
            double y = evaluateFunction2(x);
            //! \brief Вычисляет значение второй функции

            if (!qIsNaN(y) && !qIsInf(y) && qAbs(y) < 1000) {
                //! \brief Проверяет корректность значения
                points2.push_back({x, y});
                //! \brief Добавляет точку в вектор второго графика
                yMin = qMin(yMin, y);
                //! \brief Обновляет минимальное значение Y
                yMax = qMax(yMax, y);
                //! \brief Обновляет максимальное значение Y
            }
        }

        if (m_show3) {
            //! \brief Если третий график видим, вычисляем точки
            double y = evaluateFunction3(x);
            //! \brief Вычисляет значение третьей функции

            if (!qIsNaN(y) && !qIsInf(y) && qAbs(y) < 1000) {
                //! \brief Проверяет корректность значения
                points3.push_back({x, y});
                //! \brief Добавляет точку в вектор третьего графика
                yMin = qMin(yMin, y);
                //! \brief Обновляет минимальное значение Y
                yMax = qMax(yMax, y);
                //! \brief Обновляет максимальное значение Y
            }
        }
    }

    if (points1.empty() && points2.empty() && points3.empty()) return;
    //! \brief Если нет ни одной точки, прекращает отрисовку

    if (yMin == yMax) {
        //! \brief Если все точки имеют одинаковое значение Y
        yMin -= 1;
        //! \brief Расширяет диапазон вниз
        yMax += 1;
        //! \brief Расширяет диапазон вверх
    }

    double yRange = yMax - yMin;
    //! \brief Вычисляет диапазон значений Y

    yMin -= yRange * 0.1;
    //! \brief Добавляет отступ в 10% снизу
    yMax += yRange * 0.1;
    //! \brief Добавляет отступ в 10% сверху

    yRange = yMax - yMin;
    //! \brief Пересчитывает диапазон с учетом отступов

    // Функции преобразования координат
    //! \brief Лямбда-функция преобразования X из мировых в экранные координаты

    auto px = [&](double x) {
        return padLeft + (x - m_xMin) / xRange * (W - padLeft - padRight);
        //! \brief Преобразует логическую координату X в пиксельную
    };

    auto py = [&](double y) {
        return H - padBottom - (y - yMin) / yRange * (H - padTop - padBottom);
        //! \brief Преобразует логическую координату Y в пиксельную
    };

    // Белый фон
    p.fillRect(rect(), Qt::white);
    //! \brief Заливает весь виджет белым цветом

    // Сетка
    //! \brief Устанавливает перо для рисования сетки (светло-серый цвет)
    p.setPen(QPen(QColor(230, 230, 230), 1));

    // Вертикальные линии
    for (int i = 0; i <= 10; ++i) {
        //! \brief Рисует 11 вертикальных линий сетки
        double x = m_xMin + xRange * i / 10.0;
        //! \brief Вычисляет логическую координату X для линии
        double xPx = px(x);
        //! \brief Преобразует в пиксельную координату
        p.drawLine(QPointF(xPx, padTop), QPointF(xPx, H - padBottom));
        //! \brief Рисует вертикальную линию
    }

    // Горизонтальные линии
    for (int i = 0; i <= 10; ++i) {
        //! \brief Рисует 11 горизонтальных линий сетки
        double y = yMin + yRange * i / 10.0;
        //! \brief Вычисляет логическую координату Y для линии
        double yPx = py(y);
        //! \brief Преобразует в пиксельную координату
        p.drawLine(QPointF(padLeft, yPx), QPointF(W - padRight, yPx));
        //! \brief Рисует горизонтальную линию
    }

    // Оси
    p.setPen(QPen(Qt::black, 2));
    //! \brief Устанавливает черное перо толщиной 2 пикселя для осей
    p.drawLine(QPointF(padLeft, padTop), QPointF(padLeft, H - padBottom));
    //! \brief Рисует вертикальную ось Y
    p.drawLine(QPointF(padLeft, H - padBottom), QPointF(W - padRight, H - padBottom));
    //! \brief Рисует горизонтальную ось X

    // Стрелки
    //! \brief Рисует стрелку на конце оси Y
    p.drawLine(QPointF(padLeft - 5, padTop + 5), QPointF(padLeft, padTop));
    p.drawLine(QPointF(padLeft + 5, padTop + 5), QPointF(padLeft, padTop));

    //! \brief Рисует стрелку на конце оси X
    p.drawLine(QPointF(W - padRight - 5, H - padBottom - 5), QPointF(W - padRight, H - padBottom));
    p.drawLine(QPointF(W - padRight - 5, H - padBottom + 5), QPointF(W - padRight, H - padBottom));

    // Подписи осей
    p.setFont(QFont("Arial", 10, QFont::Bold));
    //! \brief Устанавливает жирный шрифт для подписей осей
    p.drawText(QRectF(W - padRight - 30, H - padBottom - 25, 30, 20), Qt::AlignCenter, "X");
    //! \brief Рисует подпись оси X
    p.drawText(QRectF(padLeft - 25, padTop - 20, 30, 20), Qt::AlignCenter, "Y");
    //! \brief Рисует подпись оси Y

    // Подписи X
    p.setFont(QFont("Arial", 8));
    //! \brief Устанавливает обычный шрифт для цифровых подписей
    for (int i = 0; i <= 10; ++i) {
        //! \brief Цикл для подписей значений на оси X
        double x = m_xMin + xRange * i / 10.0;
        //! \brief Вычисляет значение X
        double xPx = px(x);
        //! \brief Вычисляет пиксельную координату
        QString label = QString::number(x, 'f', 1);
        //! \brief Форматирует число с одним знаком после запятой
        p.drawText(QPointF(xPx - 12, H - padBottom + 15), label);
        //! \brief Рисует подпись под осью X
    }

    // Подписи Y
    for (int i = 0; i <= 10; ++i) {
        //! \brief Цикл для подписей значений на оси Y
        double y = yMin + yRange * i / 10.0;
        //! \brief Вычисляет значение Y
        double yPx = py(y);
        //! \brief Вычисляет пиксельную координату
        QString label = QString::number(y, 'f', 1);
        //! \brief Форматирует число с одним знаком после запятой
        p.drawText(QPointF(padLeft - 35, yPx + 4), label);
        //! \brief Рисует подпись слева от оси Y
    }

    // Линия Y=0
    if (yMin < 0 && yMax > 0) {
        //! \brief Если ноль находится в диапазоне Y, рисует линию Y=0
        p.setPen(QPen(Qt::gray, 1, Qt::DashLine));
        //! \brief Устанавливает серое пунктирное перо
        double y0Px = py(0);
        //! \brief Вычисляет пиксельную координату для Y=0
        p.drawLine(QPointF(padLeft, y0Px), QPointF(W - padRight, y0Px));
        //! \brief Рисует горизонтальную линию на уровне Y=0
    }

    // Рисуем графики
    //! \brief Лямбда-функция для отрисовки набора точек в виде линии
    auto drawPoints = [&](const std::vector<std::pair<double, double>>& points, const QColor& color) {
        if (points.size() < 2) return;
        //! \brief Если меньше 2 точек, нечего рисовать
        p.setPen(QPen(color, 2));
        //! \brief Устанавливает перо с цветом графика и толщиной 2
        for (size_t i = 1; i < points.size(); ++i) {
            //! \brief Цикл по всем точкам для рисования отрезков между соседними
            p.drawLine(QPointF(px(points[i-1].first), py(points[i-1].second)),
                       QPointF(px(points[i].first),   py(points[i].second)));
            //! \brief Рисует отрезок между i-1 и i точками
        }
    };

    drawPoints(points1, QColor(220, 50, 50));
    //! \brief Рисует первый график красным цветом
    drawPoints(points2, QColor(50, 180, 50));
    //! \brief Рисует второй график зеленым цветом
    drawPoints(points3, QColor(50, 50, 220));
    //! \brief Рисует третий график синим цветом

    // Легенда
    //! \brief Отрисовка легенды (условных обозначений) графиков
    int legendX = W - 130;
    //! \brief X-координата верхнего левого угла легенды
    int legendY = padTop + 10;
    //! \brief Y-координата верхнего левого угла легенды
    p.setPen(Qt::black);
    //! \brief Устанавливает черный цвет для текста легенды
    p.setFont(QFont("Arial", 8));
    //! \brief Устанавливает шрифт для легенды

    if (m_show1) {
        //! \brief Если первый график отображается, добавляем его в легенду
        p.setBrush(QColor(220, 50, 50));
        //! \brief Устанавливаем красный цвет заливки
        p.drawRect(legendX, legendY, 12, 12);
        //! \brief Рисуем цветной квадрат
        p.drawText(legendX + 16, legendY + 10, "f₁(x)");
        //! \brief Рисуем подпись
        legendY += 20;
        //! \brief Смещаем позицию для следующего элемента
    }
    if (m_show2) {
        //! \brief Если второй график отображается, добавляем его в легенду
        p.setBrush(QColor(50, 180, 50));
        //! \brief Устанавливаем зеленый цвет заливки
        p.drawRect(legendX, legendY, 12, 12);
        //! \brief Рисуем цветной квадрат
        p.drawText(legendX + 16, legendY + 10, "f₂(x)");
        //! \brief Рисуем подпись
        legendY += 20;
        //! \brief Смещаем позицию для следующего элемента
    }
    if (m_show3) {
        //! \brief Если третий график отображается, добавляем его в легенду
        p.setBrush(QColor(50, 50, 220));
        //! \brief Устанавливаем синий цвет заливки
        p.drawRect(legendX, legendY, 12, 12);
        //! \brief Рисуем цветной квадрат
        p.drawText(legendX + 16, legendY + 10, "f₃(x)");
        //! \brief Рисуем подпись
    }
}

// ==================== GraphWindow ====================
//! \brief Разделительная секция для класса GraphWindow

GraphWindow::GraphWindow(QWidget *parent) : QWidget(parent) {
    //! \brief Конструктор главного окна с графиками
    //! \param parent Родительский виджет
    setWindowTitle("Графики функций - математический клиент");
    //! \brief Устанавливает заголовок окна

    // Устанавливаем нормальный размер окна, который можно изменять
    resize(1100, 700);
    //! \brief Устанавливает начальный размер окна (ширина 1100, высота 700)
    setMinimumSize(800, 500);
    //! \brief Устанавливает минимальный допустимый размер окна

    // Важно: устанавливаем правильные флаги окна
    setWindowFlags(Qt::Window);
    //! \brief Устанавливает флаги окна для отображения как отдельного независимого окна

    // Главный горизонтальный layout
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    //! \brief Создает главный горизонтальный компоновщик
    mainLayout->setSpacing(10);
    //! \brief Устанавливает расстояние между элементами в 10 пикселей
    mainLayout->setContentsMargins(10, 10, 10, 10);
    //! \brief Устанавливает отступы вокруг layout (слева, сверху, справа, снизу)

    // ===== Левая панель с параметрами =====
    QWidget *paramsPanel = new QWidget(this);
    //! \brief Создает виджет для левой панели с параметрами
    paramsPanel->setFixedWidth(280);
    //! \brief Устанавливает фиксированную ширину панели (280 пикселей)
    paramsPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    //! \brief Устанавливает политику размера: фиксированная ширина, расширяемая высота

    QScrollArea *scrollArea = new QScrollArea(this);
    //! \brief Создает область с прокруткой для панели параметров
    scrollArea->setWidget(paramsPanel);
    //! \brief Устанавливает виджет с параметрами внутрь области прокрутки
    scrollArea->setWidgetResizable(true);
    //! \brief Разрешает автоматическое изменение размера виджета внутри области
    scrollArea->setMinimumWidth(300);
    //! \brief Устанавливает минимальную ширину области прокрутки
    scrollArea->setMaximumWidth(320);
    //! \brief Устанавливает максимальную ширину области прокрутки
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //! \brief Отключает горизонтальную полосу прокрутки

    QVBoxLayout *paramsLayout = new QVBoxLayout(paramsPanel);
    //! \brief Создает вертикальный компоновщик для панели параметров
    paramsLayout->setSpacing(15);
    //! \brief Устанавливает расстояние между элементами в 15 пикселей
    paramsLayout->setContentsMargins(5, 5, 5, 5);
    //! \brief Устанавливает отступы вокруг layout

    // ===== Функция 1 =====
    QGroupBox *group1 = new QGroupBox("Функция 1: a·ln(x + b) + c", paramsPanel);
    //! \brief Создает группу для первой функции с заголовком
    QFormLayout *form1 = new QFormLayout(group1);
    //! \brief Создает компоновку формы для группы функции 1
    form1->setSpacing(8);
    //! \brief Устанавливает расстояние между строками формы

    m_a1 = new QLineEdit("1");
    //! \brief Создает поле ввода для коэффициента a1 со значением по умолчанию "1"
    m_b1 = new QLineEdit("1");
    //! \brief Создает поле ввода для коэффициента b1 со значением по умолчанию "1"
    m_c1 = new QLineEdit("0");
    //! \brief Создает поле ввода для коэффициента c1 со значением по умолчанию "0"

    m_a1->setValidator(new QDoubleValidator(-100, 100, 3, this));
    //! \brief Устанавливает валидатор для поля a1 (от -100 до 100, 3 знака после запятой)
    m_b1->setValidator(new QDoubleValidator(-100, 100, 3, this));
    //! \brief Устанавливает валидатор для поля b1
    m_c1->setValidator(new QDoubleValidator(-100, 100, 3, this));
    //! \brief Устанавливает валидатор для поля c1

    form1->addRow("a:", m_a1);
    //! \brief Добавляет строку с меткой "a:" и полем a1
    form1->addRow("b:", m_b1);
    //! \brief Добавляет строку с меткой "b:" и полем b1
    form1->addRow("c:", m_c1);
    //! \brief Добавляет строку с меткой "c:" и полем c1

    m_show1 = new QCheckBox("Показать", paramsPanel);
    //! \brief Создает чекбокс для управления видимостью первого графика
    m_show1->setChecked(true);
    //! \brief Устанавливает чекбокс в отмеченное состояние (график видим)
    form1->addRow("", m_show1);
    //! \brief Добавляет чекбокс в новую строку без метки

    m_formula1 = new QLabel("f₁(x) = 1·ln(x+1) + 0", paramsPanel);
    //! \brief Создает текстовую метку с формулой первой функции
    m_formula1->setStyleSheet("font-family: monospace; font-size: 10px; color: #dc3232;");
    //! \brief Устанавливает стиль CSS: моноширинный шрифт, размер 10, красный цвет
    m_formula1->setWordWrap(true);
    //! \brief Включает перенос слов для длинных формул
    form1->addRow("", m_formula1);
    //! \brief Добавляет метку с формулой в новую строку

    paramsLayout->addWidget(group1);
    //! \brief Добавляет группу функции 1 в вертикальный компоновщик

    // ===== Функция 2 =====
    QGroupBox *group2 = new QGroupBox("Функция 2: a·x³ + b·x + c", paramsPanel);
    //! \brief Создает группу для второй функции (кубическая)
    QFormLayout *form2 = new QFormLayout(group2);
    //! \brief Создает компоновку формы для группы функции 2
    form2->setSpacing(8);
    //! \brief Устанавливает расстояние между строками

    m_a2 = new QLineEdit("1");
    //! \brief Создает поле ввода для a2 со значением по умолчанию "1"
    m_b2 = new QLineEdit("-3");
    //! \brief Создает поле ввода для b2 со значением по умолчанию "-3"
    m_c2 = new QLineEdit("1");
    //! \brief Создает поле ввода для c2 со значением по умолчанию "1"

    m_a2->setValidator(new QDoubleValidator(-100, 100, 3, this));
    //! \brief Устанавливает валидатор для поля a2
    m_b2->setValidator(new QDoubleValidator(-100, 100, 3, this));
    //! \brief Устанавливает валидатор для поля b2
    m_c2->setValidator(new QDoubleValidator(-100, 100, 3, this));
    //! \brief Устанавливает валидатор для поля c2

    form2->addRow("a:", m_a2);
    //! \brief Добавляет строку с меткой "a:" и полем a2
    form2->addRow("b:", m_b2);
    //! \brief Добавляет строку с меткой "b:" и полем b2
    form2->addRow("c:", m_c2);
    //! \brief Добавляет строку с меткой "c:" и полем c2

    m_show2 = new QCheckBox("Показать", paramsPanel);
    //! \brief Создает чекбокс для второго графика
    m_show2->setChecked(true);
    //! \brief Устанавливает чекбокс в отмеченное состояние
    form2->addRow("", m_show2);
    //! \brief Добавляет чекбокс без метки

    m_formula2 = new QLabel("f₂(x) = 1·x³ + (-3)·x + 1", paramsPanel);
    //! \brief Создает метку с формулой второй функции
    m_formula2->setStyleSheet("font-family: monospace; font-size: 10px; color: #32b432;");
    //! \brief Устанавливает стиль: моноширинный, зеленый цвет
    m_formula2->setWordWrap(true);
    //! \brief Включает перенос слов
    form2->addRow("", m_formula2);
    //! \brief Добавляет метку с формулой

    paramsLayout->addWidget(group2);
    //! \brief Добавляет группу функции 2 в вертикальный компоновщик

    // ===== Функция 3 =====
    QGroupBox *group3 = new QGroupBox("Функция 3: a/(x+b) + c", paramsPanel);
    //! \brief Создает группу для третьей функции (гиперболическая)
    QFormLayout *form3 = new QFormLayout(group3);
    //! \brief Создает компоновку формы для группы функции 3
    form3->setSpacing(8);
    //! \brief Устанавливает расстояние между строками

    m_a3 = new QLineEdit("1");
    //! \brief Создает поле ввода для a3 со значением по умолчанию "1"
    m_b3 = new QLineEdit("-2");
    //! \brief Создает поле ввода для b3 со значением по умолчанию "-2"
    m_c3 = new QLineEdit("-1");
    //! \brief Создает поле ввода для c3 со значением по умолчанию "-1"

    m_a3->setValidator(new QDoubleValidator(-100, 100, 3, this));
    //! \brief Устанавливает валидатор для поля a3
    m_b3->setValidator(new QDoubleValidator(-100, 100, 3, this));
    //! \brief Устанавливает валидатор для поля b3
    m_c3->setValidator(new QDoubleValidator(-100, 100, 3, this));
    //! \brief Устанавливает валидатор для поля c3

    form3->addRow("a:", m_a3);
    //! \brief Добавляет строку с меткой "a:" и полем a3
    form3->addRow("b:", m_b3);
    //! \brief Добавляет строку с меткой "b:" и полем b3
    form3->addRow("c:", m_c3);
    //! \brief Добавляет строку с меткой "c:" и полем c3

    m_show3 = new QCheckBox("Показать", paramsPanel);
    //! \brief Создает чекбокс для третьего графика
    m_show3->setChecked(true);
    //! \brief Устанавливает чекбокс в отмеченное состояние
    form3->addRow("", m_show3);
    //! \brief Добавляет чекбокс без метки

    m_formula3 = new QLabel("f₃(x) = 1/(x-2) + (-1)", paramsPanel);
    //! \brief Создает метку с формулой третьей функции
    m_formula3->setStyleSheet("font-family: monospace; font-size: 10px; color: #3232dc;");
    //! \brief Устанавливает стиль: моноширинный, синий цвет
    m_formula3->setWordWrap(true);
    //! \brief Включает перенос слов
    form3->addRow("", m_formula3);
    //! \brief Добавляет метку с формулой

    paramsLayout->addWidget(group3);
    //! \brief Добавляет группу функции 3 в вертикальный компоновщик

    // Кнопка обновления
    m_updateBtn = new QPushButton("Обновить графики", paramsPanel);
    //! \brief Создает кнопку для обновления графиков
    m_updateBtn->setStyleSheet("font-weight: bold; padding: 8px; background-color: #4CAF50; color: white;");
    //! \brief Устанавливает стиль кнопки: жирный шрифт, зеленый фон, белый текст
    m_updateBtn->setMinimumHeight(35);
    //! \brief Устанавливает минимальную высоту кнопки 35 пикселей
    paramsLayout->addWidget(m_updateBtn);
    //! \brief Добавляет кнопку в компоновщик

    paramsLayout->addStretch();
    //! \brief Добавляет растягивающийся элемент, который занимает свободное пространство

    // ===== Правая панель с графиком =====
    QWidget *plotPanel = new QWidget(this);
    //! \brief Создает виджет для правой панели с графиком
    QVBoxLayout *plotLayout = new QVBoxLayout(plotPanel);
    //! \brief Создает вертикальный компоновщик для панели графика
    plotLayout->setContentsMargins(0, 0, 0, 0);
    //! \brief Устанавливает нулевые отступы

    QLabel *plotTitle = new QLabel("<h3>Графики функций</h3>", plotPanel);
    //! \brief Создает заголовок панели графика
    plotTitle->setAlignment(Qt::AlignCenter);
    //! \brief Выравнивает заголовок по центру
    plotLayout->addWidget(plotTitle);
    //! \brief Добавляет заголовок в компоновщик

    m_plot = new PlotWidget(plotPanel);
    //! \brief Создает виджет для отображения графиков
    plotLayout->addWidget(m_plot, 1);
    //! \brief Добавляет виджет графика с коэффициентом растяжения 1

    // Добавляем в главный layout
    mainLayout->addWidget(scrollArea);
    //! \brief Добавляет область прокрутки в главный компоновщик
    mainLayout->addWidget(plotPanel, 1);
    //! \brief Добавляет панель графика с коэффициентом растяжения 1

    // Подключаем сигналы
    //! \brief Соединяет сигналы виджетов со слотами

    connect(m_updateBtn, &QPushButton::clicked, this, &GraphWindow::onUpdateClicked);
    //! \brief При нажатии кнопки вызывается слот onUpdateClicked
    connect(m_show1, &QCheckBox::stateChanged, this, &GraphWindow::onToggleGraph1);
    //! \brief При изменении состояния чекбокса графика 1 вызывается соответствующий слот
    connect(m_show2, &QCheckBox::stateChanged, this, &GraphWindow::onToggleGraph2);
    //! \brief При изменении состояния чекбокса графика 2 вызывается соответствующий слот
    connect(m_show3, &QCheckBox::stateChanged, this, &GraphWindow::onToggleGraph3);
    //! \brief При изменении состояния чекбокса графика 3 вызывается соответствующий слот

    //! \brief Соединения для обновления при нажатии Enter в полях ввода
    connect(m_a1, &QLineEdit::returnPressed, this, &GraphWindow::onUpdateClicked);
    connect(m_b1, &QLineEdit::returnPressed, this, &GraphWindow::onUpdateClicked);
    connect(m_c1, &QLineEdit::returnPressed, this, &GraphWindow::onUpdateClicked);
    connect(m_a2, &QLineEdit::returnPressed, this, &GraphWindow::onUpdateClicked);
    connect(m_b2, &QLineEdit::returnPressed, this, &GraphWindow::onUpdateClicked);
    connect(m_c2, &QLineEdit::returnPressed, this, &GraphWindow::onUpdateClicked);
    connect(m_a3, &QLineEdit::returnPressed, this, &GraphWindow::onUpdateClicked);
    connect(m_b3, &QLineEdit::returnPressed, this, &GraphWindow::onUpdateClicked);
    connect(m_c3, &QLineEdit::returnPressed, this, &GraphWindow::onUpdateClicked);

    // Первоначальное обновление
    onUpdateClicked();
    //! \brief Выполняет первоначальную отрисовку графиков с параметрами по умолчанию
}

void GraphWindow::onUpdateClicked() {
    //! \brief Слот для обновления графиков при изменении параметров
    //! \details Считывает значения из полей ввода, обновляет формулы и перерисовывает графики

    double a1 = m_a1->text().toDouble();
    //! \brief Преобразует текст поля a1 в число
    double b1 = m_b1->text().toDouble();
    //! \brief Преобразует текст поля b1 в число
    double c1 = m_c1->text().toDouble();
    //! \brief Преобразует текст поля c1 в число

    double a2 = m_a2->text().toDouble();
    //! \brief Преобразует текст поля a2 в число
    double b2 = m_b2->text().toDouble();
    //! \brief Преобразует текст поля b2 в число
    double c2 = m_c2->text().toDouble();
    //! \brief Преобразует текст поля c2 в число

    double a3 = m_a3->text().toDouble();
    //! \brief Преобразует текст поля a3 в число
    double b3 = m_b3->text().toDouble();
    //! \brief Преобразует текст поля b3 в число
    double c3 = m_c3->text().toDouble();
    //! \brief Преобразует текст поля c3 в число

    bool show1 = m_show1->isChecked();
    //! \brief Получает состояние чекбокса первого графика
    bool show2 = m_show2->isChecked();
    //! \brief Получает состояние чекбокса второго графика
    bool show3 = m_show3->isChecked();
    //! \brief Получает состояние чекбокса третьего графика

    // Обновляем формулы
    //! \brief Форматирует и обновляет текст формулы первой функции
    m_formula1->setText(QString("f₁(x) = %1·ln(x%2) %3")
                            .arg(a1)
                            .arg(b1 >= 0 ? "+" + QString::number(b1) : QString::number(b1))
                            .arg(c1 >= 0 ? "+" + QString::number(c1) : QString::number(c1)));

    //! \brief Форматирует и обновляет текст формулы второй функции
    m_formula2->setText(QString("f₂(x) = %1·x³ %2·x %3")
                            .arg(a2)
                            .arg(b2 >= 0 ? "+" + QString::number(b2) : QString::number(b2))
                            .arg(c2 >= 0 ? "+" + QString::number(c2) : QString::number(c2)));

    //! \brief Форматирует и обновляет текст формулы третьей функции
    m_formula3->setText(QString("f₃(x) = %1/(x%2) %3")
                            .arg(a3)
                            .arg(b3 >= 0 ? "+" + QString::number(b3) : QString::number(b3))
                            .arg(c3 >= 0 ? "+" + QString::number(c3) : QString::number(c3)));

    //! \brief Передает все параметры в виджет графика для перерисовки
    m_plot->setParams(a1, b1, c1, a2, b2, c2, a3, b3, c3, show1, show2, show3);
}

void GraphWindow::onToggleGraph1(int) {
    //! \brief Слот для переключения видимости первого графика
    //! \param state Состояние чекбокса (не используется)
    onUpdateClicked();
    //! \brief Вызывает обновление графиков
}

void GraphWindow::onToggleGraph2(int) {
    //! \brief Слот для переключения видимости второго графика
    //! \param state Состояние чекбокса (не используется)
    onUpdateClicked();
    //! \brief Вызывает обновление графиков
}

void GraphWindow::onToggleGraph3(int) {
    //! \brief Слот для переключения видимости третьего графика
    //! \param state Состояние чекбокса (не используется)
    onUpdateClicked();
    //! \brief Вызывает обновление графиков
}
