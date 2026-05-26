/*!
 * \file mainwindow.cpp
 * \brief Реализация главного окна — маршрутизация между формами и сервером.
 */
//! \brief Файл реализации главного окна-контроллера

#include "mainwindow.h"
//! \brief Подключение заголовочного файла главного окна

#include "tcpclient.h"
//! \brief Подключение заголовочного файла TCP-клиента (Singleton)

#include <QVBoxLayout>
//! \brief Подключение класса вертикального компоновщика

#include <QMessageBox>
//! \brief Подключение класса для отображения диалоговых окон сообщений

#include <QInputDialog>
//! \brief Подключение класса для диалогового окна ввода данных

#include <QStatusBar>
//! \brief Подключение класса строки состояния окна

#include <QTimer>
//! \brief Подключение класса таймера для отсроченного выполнения действий

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    //! \brief Конструктор главного окна
    //! \param parent Указатель на родительский виджет (передается в QMainWindow)

    setWindowTitle("TCP Клиент — Математические задачи");
    //! \brief Устанавливает заголовок главного окна

    setMinimumSize(520, 520);
    //! \brief Устанавливает минимальный размер окна (ширина 520, высота 520)

    // Стек форм
    stack    = new QStackedWidget(this);
    //! \brief Создает стековый виджет для переключения между формами
    //! \param this Родительский виджет - главное окно

    loginWin = new LoginWindow(this);
    //! \brief Создает виджет окна авторизации

    regWin   = new RegisterWindow(this);
    //! \brief Создает виджет окна регистрации

    taskWin  = new TaskWindow(this);
    //! \brief Создает виджет окна задач

    graphWin = nullptr; // создаётся по запросу
    //! \brief Инициализирует указатель на окно графиков нулевым значением

    stack->addWidget(loginWin); // 0
    //! \brief Добавляет окно авторизации в стек (индекс 0)

    stack->addWidget(regWin);   // 1
    //! \brief Добавляет окно регистрации в стек (индекс 1)

    stack->addWidget(taskWin);  // 2
    //! \brief Добавляет окно задач в стек (индекс 2)

    setCentralWidget(stack);
    //! \brief Устанавливает стековый виджет в качестве центрального виджета главного окна

    statusBar()->showMessage("Подключение к серверу...");
    //! \brief Отображает сообщение в строке состояния о начале подключения

    setupConnections();
    //! \brief Вызывает метод для настройки всех соединений сигналов и слотов

    connectToServer();
    //! \brief Вызывает метод для подключения к серверу
}

MainWindow::~MainWindow() {}
//! \brief Деструктор главного окна (пустой, т.к. используется автоматическое управление памятью)

void MainWindow::setupConnections() {
    //! \brief Настраивает соединения сигналов и слотов между компонентами

    TcpClient *client = TcpClient::getInstance();
    //! \brief Получает единственный экземпляр TCP-клиента

    // Сетевые сигналы
    connect(client, &TcpClient::responseReceived, this, &MainWindow::onServerResponse);
    //! \brief Соединяет сигнал получения ответа от сервера со слотом обработки ответа

    connect(client, &TcpClient::connected,        this, &MainWindow::onConnected);
    //! \brief Соединяет сигнал подключения к серверу со слотом onConnected

    connect(client, &TcpClient::disconnected,     this, &MainWindow::onDisconnected);
    //! \brief Соединяет сигнал отключения от сервера со слотом onDisconnected

    connect(client, &TcpClient::errorOccurred,    this, &MainWindow::onNetworkError);
    //! \brief Соединяет сигнал сетевой ошибки со слотом onNetworkError

    // Сигналы форм
    connect(loginWin, &LoginWindow::loginRequested,     this, &MainWindow::onLoginRequested);
    //! \brief Соединяет сигнал запроса авторизации со слотом onLoginRequested

    connect(loginWin, &LoginWindow::switchToRegister,   this, [this]{ stack->setCurrentIndex(1); });
    //! \brief Соединяет сигнал переключения к регистрации с лямбдой, переключающей стек на индекс 1

    connect(regWin,   &RegisterWindow::registerRequested, this, &MainWindow::onRegisterRequested);
    //! \brief Соединяет сигнал запроса регистрации со слотом onRegisterRequested

    connect(regWin,   &RegisterWindow::confirmRequested,  this, &MainWindow::onConfirmRequested);
    //! \brief Соединяет сигнал запроса подтверждения регистрации со слотом onConfirmRequested

    connect(regWin,   &RegisterWindow::switchToLogin,   this, [this]{ stack->setCurrentIndex(0); });
    //! \brief Соединяет сигнал переключения к авторизации с лямбдой, переключающей стек на индекс 0

    connect(taskWin,  &TaskWindow::checkRequested,  this, &MainWindow::onCheckRequested);
    //! \brief Соединяет сигнал проверки ответа задачи со слотом onCheckRequested

    connect(taskWin,  &TaskWindow::statRequested,   this, &MainWindow::onStatRequested);
    //! \brief Соединяет сигнал запроса статистики со слотом onStatRequested

    connect(taskWin,  &TaskWindow::graphRequested,  this, &MainWindow::onShowGraph);
    //! \brief Соединяет сигнал запроса графика со слотом onShowGraph

    connect(taskWin,  &TaskWindow::logoutRequested, this, &MainWindow::onLogout);
    //! \brief Соединяет сигнал выхода из системы со слотом onLogout
}

void MainWindow::connectToServer() {
    //! \brief Устанавливает соединение с сервером

    // Измените IP при запуске сервера на другом компьютере
    TcpClient::getInstance()->connectToServer("127.0.0.1", 33333);
    //! \brief Получает экземпляр TCP-клиента и подключается к localhost на порту 33333
}

// ── Слоты форм ───────────────────────────────────────────────────────────────
//! \brief Разделитель секции слотов для обработки действий пользователя

void MainWindow::onLoginRequested(const QString &login, const QString &password) {
    //! \brief Слот для обработки запроса авторизации
    //! \param login Логин пользователя
    //! \param password Пароль пользователя

    mPendingLogin = login;
    //! \brief Сохраняет логин как ожидающий подтверждения

    TcpClient::getInstance()->sendMessage("auth&" + login + "&" + password);
    //! \brief Отправляет на сервер команду авторизации с логином и паролем
}

void MainWindow::onRegisterRequested(const QString &login,
                                     const QString &password,
                                     const QString &email) {
    //! \brief Слот для обработки запроса регистрации
    //! \param login Логин нового пользователя
    //! \param password Пароль нового пользователя
    //! \param email Email нового пользователя

    mPendingLogin = login;
    //! \brief Сохраняет логин как ожидающий подтверждения

    mPendingPass  = password;
    //! \brief Сохраняет пароль как ожидающий подтверждения

    mPendingEmail = email;
    //! \brief Сохраняет email как ожидающий подтверждения

    TcpClient::getInstance()->sendMessage("reg&" + login + "&" + password + "&" + email);
    //! \brief Отправляет на сервер команду регистрации с данными пользователя
}

void MainWindow::onConfirmRequested(const QString &login, const QString &code) {
    //! \brief Слот для обработки запроса подтверждения регистрации
    //! \param login Логин пользователя
    //! \param code Код подтверждения из email

    TcpClient::getInstance()->sendMessage("confirm&" + login + "&" + code);
    //! \brief Отправляет на сервер команду подтверждения с логином и кодом
}

void MainWindow::onCheckRequested(int task, int variant, const QString &answer) {
    //! \brief Слот для обработки запроса проверки ответа задачи
    //! \param task Номер задачи
    //! \param variant Номер варианта
    //! \param answer Ответ пользователя

    // Проверяем, что пользователь авторизован
    if (mCurrentLogin.isEmpty()) {
        //! \brief Проверяет, авторизован ли пользователь
        QMessageBox::warning(this, "Ошибка", "Пользователь не авторизован");
        //! \brief Показывает предупреждение, если пользователь не авторизован
        return;
        //! \brief Прерывает выполнение слота
    }

    // Отправляем запрос с логином
    TcpClient::getInstance()->sendMessage(
        QString("check&%1&%2&%3&%4").arg(mCurrentLogin).arg(task).arg(variant).arg(answer));
    //! \brief Отправляет на сервер команду проверки с логином, номером задачи, вариантом и ответом

    qDebug() << "Sending check request: login=" << mCurrentLogin
             << "task=" << task
             << "variant=" << variant
             << "answer=" << answer;
    //! \brief Выводит отладочную информацию об отправленном запросе
}

void MainWindow::onStatRequested(const QString &login) {
    //! \brief Слот для обработки запроса статистики пользователя
    //! \param login Логин пользователя

    TcpClient::getInstance()->sendMessage("stat&" + login);
    //! \brief Отправляет на сервер команду запроса статистики с логином
}

void MainWindow::onLogout() {
    //! \brief Слот для обработки запроса выхода из системы

    mCurrentLogin.clear();
    //! \brief Очищает логин текущего пользователя

    mPendingLogin.clear();
    //! \brief Очищает ожидающий логин

    mPendingPass.clear();
    //! \brief Очищает ожидающий пароль

    mPendingEmail.clear();
    //! \brief Очищает ожидающий email

    stack->setCurrentIndex(0);
    //! \brief Переключает стек на окно авторизации (индекс 0)

    statusBar()->showMessage("Вы вышли из системы.");
    //! \brief Отображает сообщение о выходе в строке состояния
}

void MainWindow::onShowGraph() {
    //! \brief Слот для отображения окна с графиками функций

    // Создаём окно графика как независимое окно (без родителя или с родителем nullptr)
    if (graphWin.isNull()) {
        //! \brief Проверяет, создано ли уже окно графиков
        // Важно: parent = nullptr, чтобы окно было независимым
        graphWin = new GraphWindow(nullptr);
        //! \brief Создает новое окно графиков (без родителя)

        // Настройки окна
        graphWin->setWindowFlags(Qt::Window);  // Обычное окно с заголовком
        //! \brief Устанавливает флаги окна для обычного независимого окна

        graphWin->setAttribute(Qt::WA_DeleteOnClose);
        //! \brief Устанавливает атрибут автоматического удаления окна при закрытии

        // Центрируем окно относительно главного окна
        QRect mainGeometry = this->geometry();
        //! \brief Получает геометрию главного окна

        QPoint center = mainGeometry.center();
        //! \brief Вычисляет центр главного окна

        graphWin->resize(1100, 700);  // Явно задаём размер
        //! \brief Устанавливает размер окна графиков (1100x700)

        graphWin->move(center.x() - graphWin->width() / 2,
                       center.y() - graphWin->height() / 2);
        //! \brief Центрирует окно графиков относительно главного окна
    }

    if (!graphWin.isNull()) {
        //! \brief Проверяет, что указатель на окно графиков не нулевой
        graphWin->show();
        //! \brief Отображает окно графиков

        graphWin->raise();
        //! \brief Поднимает окно на передний план

        graphWin->activateWindow();
        //! \brief Активирует окно (делает его активным)
    }
}

// ── Обработка ответов сервера ────────────────────────────────────────────────
//! \brief Разделитель секции обработки ответов от сервера

void MainWindow::onServerResponse(const QString &response) {
    //! \brief Слот для обработки ответа от сервера
    //! \param response Строка ответа от сервера

    statusBar()->showMessage("Ответ: " + response, 3000);
    //! \brief Отображает ответ в строке состояния на 3 секунды

    if (response.startsWith("auth+")) {
        //! \brief Обработка успешной авторизации
        // Успешная авторизация → переключаем на задачу
        QStringList parts = response.split("&");
        //! \brief Разбивает строку ответа на части по символу '&'

        // Безопасный парсинг: используем логин от сервера, если есть
        if (parts.size() > 1 && !parts[1].isEmpty()) {
            //! \brief Проверяет, что в ответе есть логин
            mCurrentLogin = parts[1];
            //! \brief Устанавливает логин из ответа сервера
        } else {
            mCurrentLogin = mPendingLogin;
            //! \brief Использует сохраненный логин, если сервер не вернул
        }

        taskWin->setLogin(mCurrentLogin);
        //! \brief Передает логин в окно задач

        stack->setCurrentIndex(2);
        //! \brief Переключает стек на окно задач (индекс 2)

        // Сразу запрашиваем статистику
        TcpClient::getInstance()->sendMessage("stat&" + mCurrentLogin);
        //! \brief Отправляет запрос статистики для авторизованного пользователя

    } else if (response == "auth-") {
        //! \brief Обработка неудачной авторизации
        QMessageBox::warning(this, "Ошибка", "Неверный логин/пароль,\nлибо email не подтверждён.");
        //! \brief Показывает предупреждение об ошибке авторизации

    } else if (response.startsWith("reg+")) {
        //! \brief Обработка успешной регистрации
        // Успешная регистрация → показываем страницу подтверждения кода
        QMessageBox::information(this, "Регистрация",
                                 "Письмо с кодом отправлено на вашу почту.\nВведите код для завершения регистрации.");
        //! \brief Показывает информационное сообщение о необходимости ввода кода

        // Переключаем внутри RegisterWindow на страницу кода
        QWidget *page1 = regWin->layout()->itemAt(0)->widget();
        //! \brief Получает первый виджет из компоновки окна регистрации

        if (QStackedWidget *sw = qobject_cast<QStackedWidget*>(page1)) {
            //! \brief Пытается привести виджет к QStackedWidget
            sw->setCurrentIndex(1);
            //! \brief Переключает стек на страницу кода (индекс 1)
        } else if (QStackedWidget *sw = qobject_cast<QStackedWidget*>(regWin->layout()->itemAt(1)->widget())) {
            //! \brief Альтернативный способ получения стека
            sw->setCurrentIndex(1);
            //! \brief Переключает стек на страницу кода
        }

    } else if (response == "reg-") {
        //! \brief Обработка неудачной регистрации
        QMessageBox::warning(this, "Ошибка", "Регистрация не удалась.\nЛогин уже занят?");
        //! \brief Показывает предупреждение об ошибке регистрации

    } else if (response == "confirm+") {
        //! \brief Обработка успешного подтверждения регистрации
        QMessageBox::information(this, "Успех", "Email подтверждён!\nТеперь можно войти.");
        //! \brief Показывает сообщение об успешном подтверждении

        stack->setCurrentIndex(0);
        //! \brief Переключает стек на окно авторизации (индекс 0)

    } else if (response == "confirm-") {
        //! \brief Обработка неудачного подтверждения
        QMessageBox::warning(this, "Ошибка", "Неверный код подтверждения.");
        //! \brief Показывает предупреждение о неверном коде

    } else if (response.startsWith("stat&")) {
        //! \brief Обработка ответа со статистикой
        taskWin->updateStats(response);
        //! \brief Передает статистику в окно задач для отображения

    } else if (response == "check+") {
        //! \brief Обработка правильного ответа на задание
        taskWin->showCheckResult(true);
        //! \brief Показывает в окне задач сообщение о правильном ответе

        // Обновляем статистику
        TcpClient::getInstance()->sendMessage("stat&" + mCurrentLogin);
        //! \brief Запрашивает обновленную статистику после правильного ответа

    } else if (response == "check-") {
        //! \brief Обработка неправильного ответа на задание
        taskWin->showCheckResult(false);
        //! \brief Показывает в окне задач сообщение о неправильном ответе

    } else if (response == "connected") {
        //! \brief Обработка сообщения об установке соединения
        statusBar()->showMessage("Соединение установлено.");
        //! \brief Отображает сообщение об установке соединения
    }
}

void MainWindow::onConnected() {
    //! \brief Слот для обработки события подключения к серверу
    statusBar()->showMessage("Подключено к серверу.");
    //! \brief Отображает сообщение об успешном подключении
}

void MainWindow::onDisconnected() {
    //! \brief Слот для обработки события отключения от сервера
    statusBar()->showMessage("Соединение разорвано. Переподключение...");
    //! \brief Отображает сообщение о разрыве соединения и начале переподключения

    QTimer::singleShot(3000, this, [this]{ connectToServer(); });
    //! \brief Запускает таймер на 3 секунды, затем пытается переподключиться
}

void MainWindow::onNetworkError(const QString &error) {
    //! \brief Слот для обработки сетевых ошибок
    //! \param error Текст ошибки

    statusBar()->showMessage("Ошибка: " + error);
    //! \brief Отображает сообщение об ошибке в строке состояния
}
