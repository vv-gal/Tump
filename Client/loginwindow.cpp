/*!
 * \file loginwindow.cpp
 * \brief Реализация формы авторизации.
 */
//! \brief Файл реализации класса окна авторизации

#include "loginwindow.h"
//! \brief Подключение заголовочного файла с объявлением класса окна авторизации

#include <QVBoxLayout>
//! \brief Подключение класса вертикального компоновщика

#include <QFormLayout>
//! \brief Подключение класса формы с двумя колонками (метка-поле)

#include <QMessageBox>
//! \brief Подключение класса для отображения диалоговых окон сообщений

#include <QDebug>  // Добавлен для отладки
//! \brief Подключение макросов для отладочного вывода в консоль

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {
    //! \brief Конструктор класса LoginWindow
    //! \param parent Указатель на родительский виджет (передается в конструктор QWidget)

    setWindowTitle("Авторизация");
    //! \brief Устанавливает заголовок окна "Авторизация"

    setFixedSize(360, 260);
    //! \brief Устанавливает фиксированный размер окна (ширина 360, высота 260)
    //! \details Пользователь не может изменить размер окна

    QVBoxLayout *main = new QVBoxLayout(this);
    //! \brief Создает главный вертикальный компоновщик для размещения виджетов
    //! \param this Указывает, что компоновщик принадлежит текущему окну

    QLabel *title = new QLabel("<h2>Вход в систему</h2>", this);
    //! \brief Создает текстовую метку с HTML-заголовком второго уровня
    //! \param this Родительский виджет - текущее окно

    title->setAlignment(Qt::AlignCenter);
    //! \brief Выравнивает содержимое метки по центру

    main->addWidget(title);
    //! \brief Добавляет метку-заголовок в главный компоновщик

    QFormLayout *form = new QFormLayout;
    //! \brief Создает компоновщик формы для размещения пар "метка-поле ввода"

    editLogin    = new QLineEdit(this);
    //! \brief Создает однострочное поле ввода для логина
    //! \param this Родительский виджет - текущее окно

    editPassword = new QLineEdit(this);
    //! \brief Создает однострочное поле ввода для пароля
    //! \param this Родительский виджет - текущее окно

    editPassword->setEchoMode(QLineEdit::Password);
    //! \brief Устанавливает режим отображения пароля (символы скрыты звездочками)

    form->addRow("Логин:",  editLogin);
    //! \brief Добавляет строку с меткой "Логин:" и полем ввода логина

    form->addRow("Пароль:", editPassword);
    //! \brief Добавляет строку с меткой "Пароль:" и полем ввода пароля

    main->addLayout(form);
    //! \brief Добавляет компоновщик формы в главный вертикальный компоновщик

    lblStatus = new QLabel("", this);
    //! \brief Создает текстовую метку для отображения статуса авторизации (изначально пустая)
    //! \param this Родительский виджет - текущее окно

    lblStatus->setAlignment(Qt::AlignCenter);
    //! \brief Выравнивает текст статуса по центру

    lblStatus->setStyleSheet("color: red;");
    //! \brief Устанавливает стиль CSS: красный цвет текста для сообщений об ошибках

    main->addWidget(lblStatus);
    //! \brief Добавляет метку статуса в главный компоновщик

    btnLogin      = new QPushButton("Войти", this);
    //! \brief Создает кнопку с текстом "Войти"
    //! \param this Родительский виджет - текущее окно

    btnGoRegister = new QPushButton("Регистрация", this);
    //! \brief Создает кнопку с текстом "Регистрация"
    //! \param this Родительский виджет - текущее окно

    main->addWidget(btnLogin);
    //! \brief Добавляет кнопку "Войти" в главный компоновщик

    main->addWidget(btnGoRegister);
    //! \brief Добавляет кнопку "Регистрация" в главный компоновщик

    connect(btnLogin,      &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    //! \brief Соединяет сигнал clicked кнопки "Войти" со слотом onLoginClicked
    //! \details При нажатии кнопки будет вызван метод обработки авторизации

    connect(btnGoRegister, &QPushButton::clicked, this, &LoginWindow::switchToRegister);
    //! \brief Соединяет сигнал clicked кнопки "Регистрация" с сигналом switchToRegister
    //! \details При нажатии кнопки будет испущен сигнал для перехода к окну регистрации
}

void LoginWindow::onLoginClicked() {
    //! \brief Слот для обработки нажатия кнопки входа
    //! \details Выполняет валидацию введенных данных и испускает сигнал авторизации

    QString login = editLogin->text().trimmed();
    //! \brief Получает текст из поля логина и удаляет пробелы в начале и конце

    QString pass  = editPassword->text();
    //! \brief Получает текст из поля пароля (пробелы не удаляются, т.к. пароль может их содержать)

    if (login.isEmpty() || pass.isEmpty()) {
        //! \brief Проверяет, что оба поля заполнены
        lblStatus->setText("Заполните все поля");
        //! \brief Устанавливает текст ошибки в метку статуса
        return;
        //! \brief Прерывает выполнение функции, если поля не заполнены
    }

    lblStatus->setText("Подключение...");
    //! \brief Устанавливает информационный текст о начале процесса подключения

    emit loginRequested(login, pass);
    //! \brief Испускает сигнал loginRequested с введенными логином и паролем
    //! \details Сигнал будет обработан в главном окне (MainWindow) для отправки на сервер
}
