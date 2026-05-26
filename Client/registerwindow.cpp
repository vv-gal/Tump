/*!
 * \file registerwindow.cpp
 * \brief Реализация формы регистрации с двумя шагами (данные + подтверждение email).
 */
//! \brief Файл реализации класса окна регистрации с двухэтапным процессом

#include "registerwindow.h"
//! \brief Подключение заголовочного файла с объявлением класса окна регистрации

#include <QVBoxLayout>
//! \brief Подключение класса вертикального компоновщика

#include <QFormLayout>
//! \brief Подключение класса формы с двумя колонками (метка-поле)

#include <QHBoxLayout>
//! \brief Подключение класса горизонтального компоновщика

RegisterWindow::RegisterWindow(QWidget *parent) : QWidget(parent) {
    //! \brief Конструктор класса RegisterWindow
    //! \param parent Указатель на родительский виджет (передается в QWidget)

    setWindowTitle("Регистрация");
    //! \brief Устанавливает заголовок окна "Регистрация"

    setFixedSize(380, 320);
    //! \brief Устанавливает фиксированный размер окна (ширина 380, высота 320)
    //! \details Пользователь не может изменить размер окна

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    //! \brief Создает главный вертикальный компоновщик для текущего окна
    //! \param this Указывает, что компоновщик принадлежит текущему окну

    stack = new QStackedWidget(this);
    //! \brief Создает стековый виджет для переключения между страницами
    //! \param this Родительский виджет - текущее окно

    mainLayout->addWidget(stack);
    //! \brief Добавляет стековый виджет в главный компоновщик

    // ── Страница 1: ввод данных ──────────────────────────────────────────────
    //! \brief Создание первой страницы для ввода регистрационных данных

    QWidget *page1 = new QWidget;
    //! \brief Создает виджет первой страницы

    QVBoxLayout *v1 = new QVBoxLayout(page1);
    //! \brief Создает вертикальный компоновщик для первой страницы
    //! \param page1 Родительский виджет - первая страница

    QLabel *title = new QLabel("<h2>Регистрация</h2>", page1);
    //! \brief Создает текстовую метку с HTML-заголовком второго уровня

    title->setAlignment(Qt::AlignCenter);
    //! \brief Выравнивает содержимое метки по центру

    v1->addWidget(title);
    //! \brief Добавляет метку-заголовок в вертикальный компоновщик

    QFormLayout *form = new QFormLayout;
    //! \brief Создает компоновщик формы для размещения пар "метка-поле ввода"

    editLogin    = new QLineEdit(page1);
    //! \brief Создает поле ввода для логина

    editPassword = new QLineEdit(page1);
    //! \brief Создает поле ввода для пароля

    editEmail    = new QLineEdit(page1);
    //! \brief Создает поле ввода для email

    editPassword->setEchoMode(QLineEdit::Password);
    //! \brief Устанавливает режим отображения пароля (символы скрыты звездочками)

    form->addRow("Логин:",  editLogin);
    //! \brief Добавляет строку с меткой "Логин:" и полем ввода логина

    form->addRow("Пароль:", editPassword);
    //! \brief Добавляет строку с меткой "Пароль:" и полем ввода пароля

    form->addRow("Email:",  editEmail);
    //! \brief Добавляет строку с меткой "Email:" и полем ввода email

    v1->addLayout(form);
    //! \brief Добавляет компоновщик формы в вертикальный компоновщик

    lblStatus = new QLabel("", page1);
    //! \brief Создает текстовую метку для отображения статуса регистрации (изначально пустая)

    lblStatus->setAlignment(Qt::AlignCenter);
    //! \brief Выравнивает текст статуса по центру

    lblStatus->setStyleSheet("color: red;");
    //! \brief Устанавливает стиль CSS: красный цвет текста для сообщений об ошибках

    v1->addWidget(lblStatus);
    //! \brief Добавляет метку статуса в вертикальный компоновщик

    btnRegister = new QPushButton("Зарегистрироваться", page1);
    //! \brief Создает кнопку для отправки запроса регистрации

    btnBack     = new QPushButton("← Назад", page1);
    //! \brief Создает кнопку для возврата к окну авторизации

    v1->addWidget(btnRegister);
    //! \brief Добавляет кнопку регистрации в вертикальный компоновщик

    v1->addWidget(btnBack);
    //! \brief Добавляет кнопку "Назад" в вертикальный компоновщик

    connect(btnRegister, &QPushButton::clicked, this, &RegisterWindow::onRegisterClicked);
    //! \brief Соединяет сигнал клика кнопки регистрации со слотом onRegisterClicked

    connect(btnBack,     &QPushButton::clicked, this, &RegisterWindow::switchToLogin);
    //! \brief Соединяет сигнал клика кнопки "Назад" с сигналом switchToLogin

    stack->addWidget(page1);
    //! \brief Добавляет первую страницу в стековый виджет (индекс 0)

    // ── Страница 2: подтверждение кода ──────────────────────────────────────
    //! \brief Создание второй страницы для ввода кода подтверждения

    QWidget *page2 = new QWidget;
    //! \brief Создает виджет второй страницы

    QVBoxLayout *v2 = new QVBoxLayout(page2);
    //! \brief Создает вертикальный компоновщик для второй страницы
    //! \param page2 Родительский виджет - вторая страница

    QLabel *info = new QLabel("<b>На вашу почту отправлен код подтверждения.</b><br>"
                              "Введите его ниже:", page2);
    //! \brief Создает информационную метку с HTML-форматированием (жирный шрифт, перенос строки)

    info->setAlignment(Qt::AlignCenter);
    //! \brief Выравнивает содержимое метки по центру

    info->setWordWrap(true);
    //! \brief Включает перенос слов для длинных сообщений

    v2->addWidget(info);
    //! \brief Добавляет информационную метку в вертикальный компоновщик

    editCode = new QLineEdit(page2);
    //! \brief Создает поле ввода для кода подтверждения

    editCode->setPlaceholderText("6-значный код");
    //! \brief Устанавливает текст-подсказку внутри поля ввода

    editCode->setAlignment(Qt::AlignCenter);
    //! \brief Выравнивает вводимый текст по центру

    v2->addWidget(editCode);
    //! \brief Добавляет поле ввода кода в вертикальный компоновщик

    lblConfirmStatus = new QLabel("", page2);
    //! \brief Создает метку для отображения статуса подтверждения (изначально пустая)

    lblConfirmStatus->setAlignment(Qt::AlignCenter);
    //! \brief Выравнивает текст статуса по центру

    lblConfirmStatus->setStyleSheet("color: red;");
    //! \brief Устанавливает стиль CSS: красный цвет текста для сообщений об ошибках

    v2->addWidget(lblConfirmStatus);
    //! \brief Добавляет метку статуса в вертикальный компоновщик

    btnConfirm = new QPushButton("Подтвердить", page2);
    //! \brief Создает кнопку для отправки кода подтверждения

    v2->addWidget(btnConfirm);
    //! \brief Добавляет кнопку подтверждения в вертикальный компоновщик

    connect(btnConfirm, &QPushButton::clicked, this, &RegisterWindow::onConfirmClicked);
    //! \brief Соединяет сигнал клика кнопки подтверждения со слотом onConfirmClicked

    stack->addWidget(page2);
    //! \brief Добавляет вторую страницу в стековый виджет (индекс 1)
}

void RegisterWindow::onRegisterClicked() {
    //! \brief Слот для обработки нажатия кнопки регистрации
    //! \details Выполняет валидацию введенных данных и испускает сигнал регистрации

    QString login = editLogin->text().trimmed();
    //! \brief Получает текст из поля логина и удаляет пробелы в начале и конце

    QString pass  = editPassword->text();
    //! \brief Получает текст из поля пароля (пробелы сохраняются)

    QString email = editEmail->text().trimmed();
    //! \brief Получает текст из поля email и удаляет пробелы в начале и конце

    if (login.isEmpty() || pass.isEmpty() || email.isEmpty()) {
        //! \brief Проверяет, что все три поля заполнены
        lblStatus->setText("Заполните все поля");
        //! \brief Устанавливает текст ошибки в метку статуса
        return;
        //! \brief Прерывает выполнение функции, если поля не заполнены
    }

    currentLogin = login;
    //! \brief Сохраняет логин для использования на странице подтверждения

    lblStatus->setText("Отправка запроса...");
    //! \brief Устанавливает информационный текст о отправке запроса

    emit registerRequested(login, pass, email);
    //! \brief Испускает сигнал registerRequested с введенными данными
    //! \details Сигнал будет обработан в главном окне (MainWindow) для отправки на сервер
}

void RegisterWindow::onConfirmClicked() {
    //! \brief Слот для обработки нажатия кнопки подтверждения кода
    //! \details Выполняет валидацию кода и испускает сигнал подтверждения

    QString code = editCode->text().trimmed();
    //! \brief Получает текст из поля кода и удаляет пробелы в начале и конце

    if (code.isEmpty()) {
        //! \brief Проверяет, что поле кода не пустое
        lblConfirmStatus->setText("Введите код");
        //! \brief Устанавливает текст ошибки в метку статуса подтверждения
        return;
        //! \brief Прерывает выполнение функции, если код не введен
    }

    emit confirmRequested(currentLogin, code);
    //! \brief Испускает сигнал confirmRequested с сохраненным логином и введенным кодом
    //! \details Сигнал будет обработан в главном окне (MainWindow) для отправки на сервер
}

// Вызывается из MainWindow при успешной регистрации — переключает на страницу кода
// (MainWindow вызывает stack->setCurrentIndex(1) через сигнал reg+)
//! \brief Комментарий о том, что переключение на страницу кода выполняется из MainWindow
