/*!
 * \file taskwindow.cpp
 * \brief Реализация главного окна задач с редактируемыми параметрами функций.
 */

#include "taskwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QScrollArea>

/*!
 * \brief Конструктор класса TaskWindow.
 * \param parent Родительский виджет (по умолчанию nullptr).
 * \details Создаёт и настраивает интерфейс окна задач:
 *          - область параметров функций (коэффициенты a, b, c для трёх сегментов)
 *          - область выбора задачи и ввода ответа
 *          - область отображения статистики
 *          - кнопки для управления (проверка, статистика, графики, выход)
 */
TaskWindow::TaskWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Математические задачи");  //!< Устанавливает заголовок окна
    setMinimumSize(560, 600);                 //!< Задаёт минимальный размер окна

    // Главный скролл, чтобы всё влезло при маленьком окне
    QScrollArea *scroll = new QScrollArea(this);  //!< Создаёт область прокрутки
    scroll->setWidgetResizable(true);              //!< Разрешает изменять размер виджета внутри
    QWidget *inner = new QWidget;                  //!< Внутренний контейнер для содержимого
    scroll->setWidget(inner);                      //!< Устанавливает внутренний виджет

    QVBoxLayout *outerLayout = new QVBoxLayout(this);  //!< Внешний вертикальный layout
    outerLayout->setContentsMargins(0, 0, 0, 0);       //!< Убирает отступы
    outerLayout->addWidget(scroll);                    //!< Добавляет область прокрутки

    QVBoxLayout *main = new QVBoxLayout(inner);  //!< Главный вертикальный layout
    main->setSpacing(8);                         //!< Устанавливает расстояние между элементами

    // --- Приветствие ---
    lblWelcome = new QLabel("<h2>Добро пожаловать!</h2>", inner);  //!< Метка приветствия
    lblWelcome->setAlignment(Qt::AlignCenter);                     //!< Выравнивание по центру
    main->addWidget(lblWelcome);                                   //!< Добавление метки

    // ================================================================
    // Группа: параметры функций
    // ================================================================
    QGroupBox *grpParams = new QGroupBox("Параметры кусочной функции f(x)", inner);  //!< Группа параметров
    QVBoxLayout *vParams = new QVBoxLayout(grpParams);  //!< Вертикальный layout для параметров

    // Подсказка
    QLabel *lblHint = new QLabel(                       //!< Текст-подсказка для пользователя
        "<small>Здесь можно изменить коэффициенты. "
        "После изменения нажмите <b>Применить</b>.</small>", inner);
    lblHint->setWordWrap(true);                         //!< Разрешает перенос текста
    vParams->addWidget(lblHint);                        //!< Добавляет подсказку

    // --- f1 ---
    QGroupBox *grpF1 = new QGroupBox("f\u2081(x) = a\u2081·ln(x + b\u2081) + c\u2081,   при -1 < x < 0", inner);  //!< Группа для первого сегмента
    QHBoxLayout *hF1 = new QHBoxLayout(grpF1);  //!< Горизонтальный layout для коэффициентов f1
    hF1->addWidget(new QLabel("a₁:"));          //!< Метка коэффициента a1
    editA1 = new QLineEdit("1");                //!< Поле ввода a1 (начальное значение 1)
    editA1->setFixedWidth(60);                  //!< Фиксированная ширина 60 пикселей
    hF1->addWidget(editA1);                     //!< Добавляет поле a1
    hF1->addWidget(new QLabel("b₁:"));          //!< Метка коэффициента b1
    editB1 = new QLineEdit("1");                //!< Поле ввода b1 (начальное значение 1)
    editB1->setFixedWidth(60);                  //!< Фиксированная ширина 60 пикселей
    hF1->addWidget(editB1);                     //!< Добавляет поле b1
    hF1->addWidget(new QLabel("c₁:"));          //!< Метка коэффициента c1
    editC1 = new QLineEdit("0");                //!< Поле ввода c1 (начальное значение 0)
    editC1->setFixedWidth(60);                  //!< Фиксированная ширина 60 пикселей
    hF1->addWidget(editC1);                     //!< Добавляет поле c1
    lblFormula1 = new QLabel;                   //!< Метка с текстовым отображением формулы f1
    hF1->addWidget(lblFormula1);                //!< Добавляет метку формулы
    hF1->addStretch();                          //!< Растягивает оставшееся пространство
    vParams->addWidget(grpF1);                  //!< Добавляет группу f1 в параметры

    // --- f2 ---
    QGroupBox *grpF2 = new QGroupBox("f\u2082(x) = a\u2082·x³ + b\u2082·x + c\u2082,   при 0 ≤ x ≤ 2", inner);  //!< Группа для второго сегмента
    QHBoxLayout *hF2 = new QHBoxLayout(grpF2);  //!< Горизонтальный layout для коэффициентов f2
    hF2->addWidget(new QLabel("a₂:"));          //!< Метка коэффициента a2
    editA2 = new QLineEdit("1");                //!< Поле ввода a2 (начальное значение 1)
    editA2->setFixedWidth(60);                  //!< Фиксированная ширина 60 пикселей
    hF2->addWidget(editA2);                     //!< Добавляет поле a2
    hF2->addWidget(new QLabel("b₂:"));          //!< Метка коэффициента b2
    editB2 = new QLineEdit("-3");               //!< Поле ввода b2 (начальное значение -3)
    editB2->setFixedWidth(60);                  //!< Фиксированная ширина 60 пикселей
    hF2->addWidget(editB2);                     //!< Добавляет поле b2
    hF2->addWidget(new QLabel("c₂:"));          //!< Метка коэффициента c2
    editC2 = new QLineEdit("1");                //!< Поле ввода c2 (начальное значение 1)
    editC2->setFixedWidth(60);                  //!< Фиксированная ширина 60 пикселей
    hF2->addWidget(editC2);                     //!< Добавляет поле c2
    lblFormula2 = new QLabel;                   //!< Метка с текстовым отображением формулы f2
    hF2->addWidget(lblFormula2);                //!< Добавляет метку формулы
    hF2->addStretch();                          //!< Растягивает оставшееся пространство
    vParams->addWidget(grpF2);                  //!< Добавляет группу f2 в параметры

    // --- f3 ---
    QGroupBox *grpF3 = new QGroupBox("f\u2083(x) = a\u2083/(x + b\u2083) + c\u2083,   при x > 2", inner);  //!< Группа для третьего сегмента
    QHBoxLayout *hF3 = new QHBoxLayout(grpF3);  //!< Горизонтальный layout для коэффициентов f3
    hF3->addWidget(new QLabel("a₃:"));          //!< Метка коэффициента a3
    editA3 = new QLineEdit("1");                //!< Поле ввода a3 (начальное значение 1)
    editA3->setFixedWidth(60);                  //!< Фиксированная ширина 60 пикселей
    hF3->addWidget(editA3);                     //!< Добавляет поле a3
    hF3->addWidget(new QLabel("b₃:"));          //!< Метка коэффициента b3
    editB3 = new QLineEdit("-2");               //!< Поле ввода b3 (начальное значение -2)
    editB3->setFixedWidth(60);                  //!< Фиксированная ширина 60 пикселей
    hF3->addWidget(editB3);                     //!< Добавляет поле b3
    hF3->addWidget(new QLabel("c₃:"));          //!< Метка коэффициента c3
    editC3 = new QLineEdit("-1");               //!< Поле ввода c3 (начальное значение -1)
    editC3->setFixedWidth(60);                  //!< Фиксированная ширина 60 пикселей
    hF3->addWidget(editC3);                     //!< Добавляет поле c3
    lblFormula3 = new QLabel;                   //!< Метка с текстовым отображением формулы f3
    hF3->addWidget(lblFormula3);                //!< Добавляет метку формулы
    hF3->addStretch();                          //!< Растягивает оставшееся пространство
    vParams->addWidget(grpF3);                  //!< Добавляет группу f3 в параметры

    // Кнопки параметров
    QHBoxLayout *hParamBtns = new QHBoxLayout;          //!< Горизонтальный layout для кнопок управления параметрами
    btnSetParams = new QPushButton("✔ Применить параметры", inner);  //!< Кнопка применения параметров
    btnGetParams = new QPushButton("↻ Загрузить с сервера", inner);  //!< Кнопка загрузки параметров с сервера
    btnSetParams->setToolTip("Отправить введённые параметры на сервер");  //!< Всплывающая подсказка
    btnGetParams->setToolTip("Получить текущие параметры функций с сервера");  //!< Всплывающая подсказка
    hParamBtns->addWidget(btnSetParams);             //!< Добавляет кнопку "Применить"
    hParamBtns->addWidget(btnGetParams);             //!< Добавляет кнопку "Загрузить"
    vParams->addLayout(hParamBtns);                  //!< Добавляет layout с кнопками

    lblParamsStatus = new QLabel("", inner);         //!< Метка для отображения статуса параметров
    lblParamsStatus->setAlignment(Qt::AlignCenter);  //!< Выравнивание по центру
    vParams->addWidget(lblParamsStatus);             //!< Добавляет метку статуса

    main->addWidget(grpParams);  //!< Добавляет группу параметров в главный layout

    // ================================================================
    // Группа: задача
    // ================================================================
    QGroupBox *grpTask = new QGroupBox("Задача", inner);  //!< Группа для выбора задачи
    QVBoxLayout *vTask = new QVBoxLayout(grpTask);        //!< Вертикальный layout для задачи

    lblTask = new QLabel("<b>Найдите корень уравнения f(x) = 0:</b>", inner);  //!< Текст задания
    lblTask->setTextFormat(Qt::RichText);                  //!< Формат текста с HTML-разметкой
    vTask->addWidget(lblTask);                             //!< Добавляет текст задания

    comboTask = new QComboBox(inner);                      //!< Выпадающий список для выбора задачи
    comboTask->addItem("Задача 1: корень на [0, 2], первый");  //!< Пункт 1: задача 1
    comboTask->addItem("Задача 2: корень на [0, 2], второй");  //!< Пункт 2: задача 2
    comboTask->addItem("Задача 3: корень на (2, ∞)");          //!< Пункт 3: задача 3
    vTask->addWidget(comboTask);                           //!< Добавляет выпадающий список

    lblFormula = new QLabel("Введите ответ (число):", inner);  //!< Подпись поля ввода ответа
    vTask->addWidget(lblFormula);                          //!< Добавляет подпись

    editAnswer = new QLineEdit(inner);                     //!< Поле ввода ответа
    editAnswer->setPlaceholderText("Например: 0.347");     //!< Текст-подсказка внутри поля
    vTask->addWidget(editAnswer);                          //!< Добавляет поле ввода

    btnCheck = new QPushButton("Проверить ответ", inner);  //!< Кнопка проверки ответа
    vTask->addWidget(btnCheck);                            //!< Добавляет кнопку

    lblResult = new QLabel("", inner);                     //!< Метка для отображения результата проверки
    lblResult->setAlignment(Qt::AlignCenter);              //!< Выравнивание по центру
    lblResult->setStyleSheet("font-weight: bold; font-size: 14px;");  //!< Стиль: жирный шрифт, размер 14
    vTask->addWidget(lblResult);                           //!< Добавляет метку результата

    main->addWidget(grpTask);  //!< Добавляет группу задачи в главный layout

    // ================================================================
    // Группа: статистика
    // ================================================================
    QGroupBox *grpStat = new QGroupBox("Статистика", inner);  //!< Группа для отображения статистики
    QVBoxLayout *vStat = new QVBoxLayout(grpStat);            //!< Вертикальный layout для статистики

    lblStat = new QLabel("— не загружена —", inner);          //!< Метка с текстом статистики
    lblStat->setAlignment(Qt::AlignCenter);                   //!< Выравнивание по центру
    vStat->addWidget(lblStat);                                //!< Добавляет метку статистики

    btnStat = new QPushButton("Обновить статистику", inner);  //!< Кнопка обновления статистики
    vStat->addWidget(btnStat);                                //!< Добавляет кнопку

    main->addWidget(grpStat);  //!< Добавляет группу статистики в главный layout

    // ================================================================
    // Нижние кнопки
    // ================================================================
    QHBoxLayout *hBtns = new QHBoxLayout;           //!< Горизонтальный layout для нижних кнопок
    btnGraph  = new QPushButton("📈 Графики", inner);  //!< Кнопка открытия окна с графиками
    btnLogout = new QPushButton("Выйти", inner);     //!< Кнопка выхода из системы
    hBtns->addWidget(btnGraph);                     //!< Добавляет кнопку "Графики"
    hBtns->addWidget(btnLogout);                    //!< Добавляет кнопку "Выйти"
    main->addLayout(hBtns);                         //!< Добавляет layout в главный

    // ================================================================
    // Соединения
    // ================================================================
    connect(btnCheck,     &QPushButton::clicked, this, &TaskWindow::onCheckClicked);      //!< Связывает кнопку "Проверить" со слотом проверки
    connect(btnStat,      &QPushButton::clicked, this, &TaskWindow::onStatClicked);       //!< Связывает кнопку "Статистика" со слотом запроса статистики
    connect(btnGraph,     &QPushButton::clicked, this, &TaskWindow::graphRequested);      //!< Связывает кнопку "Графики" с сигналом запроса графиков
    connect(btnLogout,    &QPushButton::clicked, this, &TaskWindow::logoutRequested);     //!< Связывает кнопку "Выйти" с сигналом выхода
    connect(btnSetParams, &QPushButton::clicked, this, &TaskWindow::onSetParamsClicked);  //!< Связывает кнопку "Применить" со слотом отправки параметров
    connect(btnGetParams, &QPushButton::clicked, this, &TaskWindow::onGetParamsClicked);  //!< Связывает кнопку "Загрузить" со слотом запроса параметров

    connect(comboTask, QOverload<int>::of(&QComboBox::currentIndexChanged),  //!< Связывает изменение выбора задачи
            this, &TaskWindow::onTaskChanged);                               //!< со слотом обработки

    // Обновлять формулу при изменении любого поля параметров
    auto refresh = [this](const QString &) { updateFormulaLabels(); };  //!< Лямбда-функция для обновления формул
    connect(editA1, &QLineEdit::textChanged, this, refresh);  //!< Изменение a1 → обновить формулу
    connect(editB1, &QLineEdit::textChanged, this, refresh);  //!< Изменение b1 → обновить формулу
    connect(editC1, &QLineEdit::textChanged, this, refresh);  //!< Изменение c1 → обновить формулу
    connect(editA2, &QLineEdit::textChanged, this, refresh);  //!< Изменение a2 → обновить формулу
    connect(editB2, &QLineEdit::textChanged, this, refresh);  //!< Изменение b2 → обновить формулу
    connect(editC2, &QLineEdit::textChanged, this, refresh);  //!< Изменение c2 → обновить формулу
    connect(editA3, &QLineEdit::textChanged, this, refresh);  //!< Изменение a3 → обновить формулу
    connect(editB3, &QLineEdit::textChanged, this, refresh);  //!< Изменение b3 → обновить формулу
    connect(editC3, &QLineEdit::textChanged, this, refresh);  //!< Изменение c3 → обновить формулу

    updateFormulaLabels();  //!< Первоначальное обновление отображения формул
}

// --------------------------------------------------------------------------

/*!
 * \brief Устанавливает логин текущего пользователя.
 * \param login Логин пользователя.
 * \details Обновляет текст приветствия, отображая имя пользователя.
 */
void TaskWindow::setLogin(const QString &login) {
    mLogin = login;  //!< Сохраняет логин в приватное поле
    lblWelcome->setText("<h2>Добро пожаловать, " + login + "!</h2>");  //!< Обновляет приветствие
}

/*!
 * \brief Обновляет отображение статистики пользователя.
 * \param statString Строка с данными статистики в формате "правильные$всего&очки".
 * \details Формат строки: "&правильные$всего&очки". Разбирает и отображает статистику.
 */
void TaskWindow::updateStats(const QString &statString) {
    QStringList parts = statString.split("&");  //!< Разбивает строку по разделителю "&"
    if (parts.size() >= 3) {                    //!< Проверяет, что получено достаточно частей
        QStringList nm2 = parts[1].split("$");  //!< Разбивает вторую часть по "$"
        int correct = nm2.value(0).toInt();     //!< Количество правильных ответов
        int total   = nm2.value(1).toInt();     //!< Общее количество попыток
        int points  = parts[2].toInt();         //!< Количество очков
        lblStat->setText(QString("Правильных: %1 / %2  |  Очков: %3")  //!< Формирует текст статистики
                             .arg(correct).arg(total).arg(points));
    } else {
        lblStat->setText(statString);  //!< Если формат неверный, отображает исходную строку
    }
}

/*!
 * \brief Отображает результат проверки ответа пользователя.
 * \param correct true — ответ правильный, false — неправильный.
 * \details Устанавливает цвет текста (зелёный для верного, красный для неверного)
 *          и выводит соответствующее сообщение.
 */
void TaskWindow::showCheckResult(bool correct) {
    if (correct) {
        lblResult->setStyleSheet("color: green; font-weight: bold; font-size: 14px;");  //!< Зелёный цвет для успеха
        lblResult->setText("✓ Верно! Молодец!");  //!< Сообщение о правильном ответе
    } else {
        lblResult->setStyleSheet("color: red; font-weight: bold; font-size: 14px;");  //!< Красный цвет для ошибки
        lblResult->setText("✗ Неверно. Попробуйте ещё раз.");  //!< Сообщение о неправильном ответе
    }
}

/*!
 * \brief Загружает параметры функций, полученные с сервера.
 * \param paramsString Строка с параметрами в формате "params&a1&b1&c1&a2&b2&c2&a3&b3&c3".
 * \details Разбирает полученную строку и заполняет поля ввода коэффициентов.
 *          Если формат неверный, выводит сообщение об ошибке.
 */
void TaskWindow::loadParams(const QString &paramsString) {
    // Формат: params&a1&b1&c1&a2&b2&c2&a3&b3&c3
    QStringList p = paramsString.split("&");  //!< Разбивает строку по разделителю "&"
    if (p.size() < 10) {                      //!< Проверяет, что получено 10 частей (команда + 9 коэффициентов)
        lblParamsStatus->setStyleSheet("color: orange;");  //!< Оранжевый цвет для предупреждения
        lblParamsStatus->setText("Получен неверный формат параметров");  //!< Сообщение об ошибке
        return;  //!< Прерывает выполнение
    }
    // Заполняет поля ввода полученными значениями
    editA1->setText(p[1]);  //!< Устанавливает значение a1
    editB1->setText(p[2]);  //!< Устанавливает значение b1
    editC1->setText(p[3]);  //!< Устанавливает значение c1
    editA2->setText(p[4]);  //!< Устанавливает значение a2
    editB2->setText(p[5]);  //!< Устанавливает значение b2
    editC2->setText(p[6]);  //!< Устанавливает значение c2
    editA3->setText(p[7]);  //!< Устанавливает значение a3
    editB3->setText(p[8]);  //!< Устанавливает значение b3
    editC3->setText(p[9]);  //!< Устанавливает значение c3

    lblParamsStatus->setStyleSheet("color: green;");  //!< Зелёный цвет для успеха
    lblParamsStatus->setText("✓ Параметры загружены с сервера");  //!< Сообщение об успешной загрузке
}

// --------------------------------------------------------------------------

/*!
 * \brief Слот обработки нажатия кнопки "Проверить".
 * \details Определяет номер выбранной задачи (индекс + 1),
 *          считывает ответ из поля ввода и испускает сигнал checkRequested().
 */
void TaskWindow::onCheckClicked() {
    int task = comboTask->currentIndex() + 1;  //!< Номер задачи (1, 2 или 3)
    QString answer = editAnswer->text().trimmed();  //!< Ответ пользователя (обрезает пробелы)
    if (answer.isEmpty()) return;  //!< Если ответ пустой, ничего не делает
    emit checkRequested(task, 1, answer);  //!< Испускает сигнал с параметрами (вариант всегда 1)
}

/*!
 * \brief Слот обработки нажатия кнопки "Обновить статистику".
 * \details Испускает сигнал statRequested() с текущим логином пользователя.
 */
void TaskWindow::onStatClicked() {
    emit statRequested(mLogin);  //!< Запрашивает статистику для текущего пользователя
}

/*!
 * \brief Слот обработки изменения выбранной задачи в комбобоксе.
 * \param index Индекс выбранного элемента (не используется).
 * \details Очищает поле результата и поле ввода ответа.
 */
void TaskWindow::onTaskChanged(int /*index*/) {
    lblResult->setText("");   //!< Очищает метку результата
    editAnswer->clear();      //!< Очищает поле ввода ответа
}

/*!
 * \brief Слот обработки нажатия кнопки "Применить параметры".
 * \details Проверяет, что все коэффициенты являются числами.
 *          Формирует строку команды "setparams&a1&b1&c1&...&c3"
 *          и испускает сигнал setParamsRequested().
 */
void TaskWindow::onSetParamsClicked() {
    // Проверяем, что все поля — числа
    bool ok = true;  //!< Флаг корректности ввода
    QList<QLineEdit*> fields = {editA1,editB1,editC1, editA2,editB2,editC2, editA3,editB3,editC3};  //!< Список всех полей
    for (auto *f : fields) {
        f->text().toDouble(&ok);  //!< Пытается преобразовать текст в число
        if (!ok) {                //!< Если преобразование не удалось
            lblParamsStatus->setStyleSheet("color: red;");  //!< Красный цвет для ошибки
            lblParamsStatus->setText("✗ Все коэффициенты должны быть числами");  //!< Сообщение об ошибке
            f->setFocus();  //!< Устанавливает фокус на ошибочное поле
            return;  //!< Прерывает выполнение
        }
    }

    // Формат команды: setparams&a1&b1&c1&a2&b2&c2&a3&b3&c3
    QString msg = QString("setparams&%1&%2&%3&%4&%5&%6&%7&%8&%9")  //!< Формирует строку команды
                      .arg(editA1->text().trimmed())  //!< Подставляет a1
                      .arg(editB1->text().trimmed())  //!< Подставляет b1
                      .arg(editC1->text().trimmed())  //!< Подставляет c1
                      .arg(editA2->text().trimmed())  //!< Подставляет a2
                      .arg(editB2->text().trimmed())  //!< Подставляет b2
                      .arg(editC2->text().trimmed())  //!< Подставляет c2
                      .arg(editA3->text().trimmed())  //!< Подставляет a3
                      .arg(editB3->text().trimmed())  //!< Подставляет b3
                      .arg(editC3->text().trimmed());  //!< Подставляет c3

    lblParamsStatus->setStyleSheet("color: gray;");  //!< Серый цвет для статуса "отправка"
    lblParamsStatus->setText("Отправка...");  //!< Сообщение о начале отправки
    emit setParamsRequested(msg);  //!< Испускает сигнал с командой установки параметров
}

/*!
 * \brief Слот обработки нажатия кнопки "Загрузить с сервера".
 * \details Устанавливает статус "Запрос..." и испускает сигнал getParamsRequested().
 */
void TaskWindow::onGetParamsClicked() {
    lblParamsStatus->setStyleSheet("color: gray;");  //!< Серый цвет для статуса "запрос"
    lblParamsStatus->setText("Запрос...");  //!< Сообщение о начале запроса
    emit getParamsRequested();  //!< Испускает сигнал запроса параметров
}

/*!
 * \brief Обновляет отображение формул на основе введённых коэффициентов.
 * \details Для каждого сегмента формирует текстовое представление формулы
 *          с подстановкой текущих значений коэффициентов.
 */
void TaskWindow::updateFormulaLabels() {
    // Показываем текущую формулу рядом с полями параметров
    auto fmt = [](QLineEdit *e) -> QString {  //!< Лямбда-функция форматирования текста
        QString s = e->text().trimmed();      //!< Получает текст из поля
        return s.isEmpty() ? "?" : s;         //!< Если поле пустое, возвращает "?"
    };

    lblFormula1->setText(QString("<small><i>f₁ = %1·ln(x+%2)+%3</i></small>")  //!< Формула f1 в HTML
                             .arg(fmt(editA1)).arg(fmt(editB1)).arg(fmt(editC1)));
    lblFormula2->setText(QString("<small><i>f₂ = %1·x³+%2·x+%3</i></small>")  //!< Формула f2 в HTML
                             .arg(fmt(editA2)).arg(fmt(editB2)).arg(fmt(editC2)));
    lblFormula3->setText(QString("<small><i>f₃ = %1/(x+%2)+%3</i></small>")  //!< Формула f3 в HTML
                             .arg(fmt(editA3)).arg(fmt(editB3)).arg(fmt(editC3)));
}

/*!
 * \brief Слот для обработки ответа сервера на установку параметров.
 * \param success true — параметры успешно применены, false — ошибка.
 * \details Отображает соответствующий статус в метке lblParamsStatus.
 */
void TaskWindow::onParamsApplied(bool success) {
    if (success) {
        lblParamsStatus->setStyleSheet("color: green;");  //!< Зелёный цвет для успеха
        lblParamsStatus->setText("✓ Параметры успешно применены на сервере");  //!< Сообщение об успехе
    } else {
        lblParamsStatus->setStyleSheet("color: red;");  //!< Красный цвет для ошибки
        lblParamsStatus->setText("✗ Ошибка применения параметров");  //!< Сообщение об ошибке
    }
}
