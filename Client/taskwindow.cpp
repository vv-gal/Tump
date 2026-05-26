/*!
 * \file taskwindow.cpp
 * \brief Реализация главного окна с задачей и статистикой.
 */
//! \brief Файл реализации класса окна задач и статистики

#include "taskwindow.h"
//! \brief Подключение заголовочного файла с объявлением класса окна задач

#include <QVBoxLayout>
//! \brief Подключение класса вертикального компоновщика

#include <QHBoxLayout>
//! \brief Подключение класса горизонтального компоновщика

#include <QFormLayout>
//! \brief Подключение класса формы с двумя колонками (метка-поле)

#include <QGroupBox>
//! \brief Подключение класса группы с рамкой для группировки виджетов

TaskWindow::TaskWindow(QWidget *parent) : QWidget(parent) {
    //! \brief Конструктор класса TaskWindow
    //! \param parent Указатель на родительский виджет (передается в QWidget)

    setWindowTitle("Математические задачи");
    //! \brief Устанавливает заголовок окна "Математические задачи"

    setMinimumSize(500, 480);
    //! \brief Устанавливает минимальный размер окна (ширина 500, высота 480)

    QVBoxLayout *main = new QVBoxLayout(this);
    //! \brief Создает главный вертикальный компоновщик для текущего окна
    //! \param this Указывает, что компоновщик принадлежит текущему окну

    lblWelcome = new QLabel("<h2>Добро пожаловать!</h2>", this);
    //! \brief Создает текстовую метку с HTML-заголовком второго уровня

    lblWelcome->setAlignment(Qt::AlignCenter);
    //! \brief Выравнивает содержимое метки по центру

    main->addWidget(lblWelcome);
    //! \brief Добавляет метку приветствия в главный компоновщик

    // Группа задачи
    QGroupBox *grpTask = new QGroupBox("Задача", this);
    //! \brief Создает группу с заголовком "Задача" для размещения элементов задачи

    QVBoxLayout *vTask = new QVBoxLayout(grpTask);
    //! \brief Создает вертикальный компоновщик для группы задачи
    //! \param grpTask Родительский виджет - группа задачи

    QLabel *lblDesc = new QLabel(
        "<b>Функция f(x):</b><br>"
        "f(x) = ln(x+1),          -1 &lt; x &lt; 0<br>"
        "f(x) = x³ - 3x + 1,   0 ≤ x ≤ 2<br>"
        "f(x) = 1/(x-2) - 1,   x &gt; 2<br><br>"
        "Найдите корень уравнения f(x) = 0:", this);
    //! \brief Создает текстовую метку с HTML-форматированием для описания задачи
    //! \details Описывает три функции, составляющих кусочную функцию, и задание

    lblDesc->setTextFormat(Qt::RichText);
    //! \brief Устанавливает формат текста как HTML (Rich Text)

    vTask->addWidget(lblDesc);
    //! \brief Добавляет метку с описанием в вертикальный компоновщик задачи

    comboTask = new QComboBox(this);
    //! \brief Создает выпадающий список для выбора задачи

    comboTask->addItem("Задача 1: корень на [0, 2], первый  (≈ 0.347)");
    //! \brief Добавляет первый пункт в выпадающий список (корень ≈ 0.347)

    comboTask->addItem("Задача 2: корень на [0, 2], второй (≈ 1.532)");
    //! \brief Добавляет второй пункт в выпадающий список (корень ≈ 1.532)

    comboTask->addItem("Задача 3: корень на (2, ∞)          (= 3.0)");
    //! \brief Добавляет третий пункт в выпадающий список (корень = 3.0)

    vTask->addWidget(comboTask);
    //! \brief Добавляет выпадающий список в вертикальный компоновщик задачи

    lblFormula = new QLabel("Введите ответ (число):", this);
    //! \brief Создает текстовую метку с инструкцией для ввода ответа

    vTask->addWidget(lblFormula);
    //! \brief Добавляет метку инструкции в вертикальный компоновщик задачи

    editAnswer = new QLineEdit(this);
    //! \brief Создает однострочное поле для ввода ответа

    editAnswer->setPlaceholderText("Например: 0.347");
    //! \brief Устанавливает текст-подсказку внутри поля ввода

    vTask->addWidget(editAnswer);
    //! \brief Добавляет поле ввода ответа в вертикальный компоновщик задачи

    btnCheck = new QPushButton("Проверить ответ", this);
    //! \brief Создает кнопку для проверки ответа

    vTask->addWidget(btnCheck);
    //! \brief Добавляет кнопку проверки в вертикальный компоновщик задачи

    lblResult = new QLabel("", this);
    //! \brief Создает текстовую метку для отображения результата проверки (изначально пустая)

    lblResult->setAlignment(Qt::AlignCenter);
    //! \brief Выравнивает содержимое метки по центру

    lblResult->setStyleSheet("font-weight: bold; font-size: 14px;");
    //! \brief Устанавливает стиль CSS: жирный шрифт, размер 14 пикселей

    vTask->addWidget(lblResult);
    //! \brief Добавляет метку результата в вертикальный компоновщик задачи

    main->addWidget(grpTask);
    //! \brief Добавляет группу задачи в главный компоновщик

    // Статистика
    QGroupBox *grpStat = new QGroupBox("Статистика", this);
    //! \brief Создает группу с заголовком "Статистика" для размещения элементов статистики

    QVBoxLayout *vStat = new QVBoxLayout(grpStat);
    //! \brief Создает вертикальный компоновщик для группы статистики
    //! \param grpStat Родительский виджет - группа статистики

    lblStat = new QLabel("— не загружена —", this);
    //! \brief Создает текстовую метку для отображения статистики (начальное сообщение)

    lblStat->setAlignment(Qt::AlignCenter);
    //! \brief Выравнивает содержимое метки по центру

    vStat->addWidget(lblStat);
    //! \brief Добавляет метку статистики в вертикальный компоновщик статистики

    btnStat = new QPushButton("Обновить статистику", this);
    //! \brief Создает кнопку для обновления статистики

    vStat->addWidget(btnStat);
    //! \brief Добавляет кнопку обновления статистики в вертикальный компоновщик статистики

    main->addWidget(grpStat);
    //! \brief Добавляет группу статистики в главный компоновщик

    // Кнопки
    QHBoxLayout *hBtns = new QHBoxLayout;
    //! \brief Создает горизонтальный компоновщик для кнопок управления

    btnGraph   = new QPushButton("📈 Графики", this);
    //! \brief Создает кнопку для открытия окна графиков (с иконкой эмодзи)

    btnLogout  = new QPushButton("Выйти", this);
    //! \brief Создает кнопку для выхода из системы

    hBtns->addWidget(btnGraph);
    //! \brief Добавляет кнопку "Графики" в горизонтальный компоновщик

    hBtns->addWidget(btnLogout);
    //! \brief Добавляет кнопку "Выйти" в горизонтальный компоновщик

    main->addLayout(hBtns);
    //! \brief Добавляет горизонтальный компоновщик с кнопками в главный компоновщик

    connect(btnCheck,  &QPushButton::clicked, this, &TaskWindow::onCheckClicked);
    //! \brief Соединяет сигнал клика кнопки проверки со слотом onCheckClicked

    connect(btnStat,   &QPushButton::clicked, this, &TaskWindow::onStatClicked);
    //! \brief Соединяет сигнал клика кнопки статистики со слотом onStatClicked

    connect(btnGraph,  &QPushButton::clicked, this, &TaskWindow::graphRequested);
    //! \brief Соединяет сигнал клика кнопки графиков с сигналом graphRequested

    connect(btnLogout, &QPushButton::clicked, this, &TaskWindow::logoutRequested);
    //! \brief Соединяет сигнал клика кнопки выхода с сигналом logoutRequested

    connect(comboTask, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TaskWindow::onTaskChanged);
    //! \brief Соединяет сигнал изменения индекса выпадающего списка со слотом onTaskChanged
    //! \details QOverload<int>::of используется для разрешения перегруженного сигнала
}

void TaskWindow::setLogin(const QString &login) {
    //! \brief Устанавливает логин текущего пользователя
    //! \param login Логин пользователя

    mLogin = login;
    //! \brief Сохраняет логин в приватный член класса

    lblWelcome->setText("<h2>Добро пожаловать, " + login + "!</h2>");
    //! \brief Обновляет текст приветствия с именем пользователя
}

void TaskWindow::updateStats(const QString &statString) {
    //! \brief Обновляет отображение статистики
    //! \param statString Строка со статистикой от сервера

    // Формат: stat&N$M&P  (N=правильных, M=попыток, P=очков)
    QStringList parts = statString.split("&");
    //! \brief Разбивает строку статистики на части по символу '&'

    if (parts.size() >= 3) {
        //! \brief Проверяет, что строка содержит как минимум 3 части
        QString nm = parts[1]; // "N$M"
        //! \brief Извлекает часть с количеством правильных ответов и попыток

        QStringList nm2 = nm.split("$");
        //! \brief Разбивает строку "N$M" на две части по символу '$'

        int correct = nm2.value(0).toInt();
        //! \brief Извлекает количество правильных ответов (N)

        int total   = nm2.value(1).toInt();
        //! \brief Извлекает общее количество попыток (M)

        int points  = parts[2].toInt();
        //! \brief Извлекает количество набранных очков (P)

        lblStat->setText(QString("Правильных: %1 / %2  |  Очков: %3")
                             .arg(correct).arg(total).arg(points));
        //! \brief Форматирует и отображает статистику в метке
    } else {
        //! \brief Если формат строки не соответствует ожидаемому
        lblStat->setText(statString);
        //! \brief Отображает строку статистики как есть
    }
}

void TaskWindow::showCheckResult(bool correct) {
    //! \brief Отображает результат проверки ответа
    //! \param correct true - ответ верный, false - ответ неверный

    if (correct) {
        //! \brief Если ответ верный
        lblResult->setStyleSheet("color: green; font-weight: bold; font-size: 14px;");
        //! \brief Устанавливает зеленый цвет текста для успешного результата

        lblResult->setText("✓ Верно! Молодец!");
        //! \brief Устанавливает текст успеха с галочкой
    } else {
        //! \brief Если ответ неверный
        lblResult->setStyleSheet("color: red; font-weight: bold; font-size: 14px;");
        //! \brief Устанавливает красный цвет текста для ошибки

        lblResult->setText("✗ Неверно. Попробуйте ещё раз.");
        //! \brief Устанавливает текст ошибки с крестиком
    }
}

void TaskWindow::onCheckClicked() {
    //! \brief Слот для обработки нажатия кнопки проверки ответа

    int task = comboTask->currentIndex() + 1;
    //! \brief Определяет номер задачи (индекс + 1, т.к. индексация с 0)

    QString answer = editAnswer->text().trimmed();
    //! \brief Получает текст ответа и удаляет пробелы в начале и конце

    if (answer.isEmpty()) return;
    //! \brief Если ответ пустой, прерывает выполнение функции

    emit checkRequested(task, 1, answer);
    //! \brief Испускает сигнал checkRequested с номером задачи, вариантом (1) и ответом
}

void TaskWindow::onStatClicked() {
    //! \brief Слот для обработки нажатия кнопки обновления статистики

    emit statRequested(mLogin);
    //! \brief Испускает сигнал statRequested с текущим логином пользователя
}

void TaskWindow::onTaskChanged(int) {
    //! \brief Слот для обработки изменения выбранной задачи
    //! \param index Индекс выбранного элемента (не используется)

    lblResult->setText("");
    //! \brief Очищает метку с результатом проверки

    editAnswer->clear();
    //! \brief Очищает поле ввода ответа
}
