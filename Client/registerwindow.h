#ifndef REGISTERWINDOW_H
//! \brief Защита от множественного включения заголовочного файла

#define REGISTERWINDOW_H
//! \brief Определение макроса для защиты от повторного включения

#include <QWidget>
//! \brief Подключение базового класса для виджетов Qt

#include <QLineEdit>
//! \brief Подключение класса для однострочного текстового поля ввода

#include <QPushButton>
//! \brief Подключение класса для кнопки

#include <QLabel>
//! \brief Подключение класса для текстовой метки

#include <QStackedWidget>
//! \brief Подключение класса для виджета с стековым переключением страниц

/*!
 * \file registerwindow.h
 * \brief Форма регистрации с подтверждением email (Окно 2 из 3).
 */
//! \brief Файл заголовка, содержащий объявление класса окна регистрации с двухэтапным подтверждением

class RegisterWindow : public QWidget {
    //! \brief Класс окна регистрации с подтверждением по email
    //! \details Реализует двухэтапный процесс: ввод данных и подтверждение кода
    Q_OBJECT
    //! \brief Макрос Qt для поддержки сигналов и слотов

public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    //! \brief Конструктор класса RegisterWindow
    //! \param parent Указатель на родительский виджет (по умолчанию nullptr)
    //! \details Создает две страницы: форму регистрации и форму подтверждения кода

signals:
    void registerRequested(const QString &login,
                           const QString &password,
                           const QString &email);
    //! \brief Сигнал, испускаемый при запросе регистрации нового пользователя
    //! \param login Логин пользователя
    //! \param password Пароль пользователя
    //! \param email Email пользователя для отправки кода подтверждения

    void confirmRequested(const QString &login, const QString &code);
    //! \brief Сигнал, испускаемый при запросе подтверждения регистрации
    //! \param login Логин пользователя
    //! \param code Код подтверждения, полученный пользователем по email

    void switchToLogin();
    //! \brief Сигнал, испускаемый при запросе перехода к окну авторизации
    //! \details Испускается при нажатии кнопки возврата к авторизации

private slots:
    void onRegisterClicked();
    //! \brief Слот для обработки нажатия кнопки регистрации
    //! \details Валидирует введенные данные (логин, пароль, email) и
    //!          испускает сигнал registerRequested при успешной проверке

    void onConfirmClicked();
    //! \brief Слот для обработки нажатия кнопки подтверждения кода
    //! \details Испускает сигнал confirmRequested с логином и введенным кодом

private:
    QStackedWidget *stack;
    //! \brief Указатель на стековый виджет для переключения между страницами
    //! \details Индексы: 0 - форма регистрации, 1 - форма подтверждения кода

    // Страница 1: регистрация
    QLineEdit  *editLogin;
    //! \brief Указатель на поле ввода логина на странице регистрации

    QLineEdit  *editPassword;
    //! \brief Указатель на поле ввода пароля на странице регистрации
    //! \details Обычно настраивается как password mode (echoMode = Password)

    QLineEdit  *editEmail;
    //! \brief Указатель на поле ввода email на странице регистрации

    QPushButton *btnRegister;
    //! \brief Указатель на кнопку для отправки запроса регистрации

    QPushButton *btnBack;
    //! \brief Указатель на кнопку для возврата к окну авторизации
    //! \details При нажатии испускает сигнал switchToLogin

    QLabel     *lblStatus;
    //! \brief Указатель на текстовую метку для отображения статуса на странице регистрации
    //! \details Показывает сообщения об ошибках валидации или успешной отправке кода

    // Страница 2: подтверждение кода
    QLineEdit  *editCode;
    //! \brief Указатель на поле ввода кода подтверждения на второй странице

    QPushButton *btnConfirm;
    //! \brief Указатель на кнопку для подтверждения кода

    QLabel     *lblConfirmStatus;
    //! \brief Указатель на текстовую метку для отображения статуса на странице подтверждения
    //! \details Показывает сообщения об ошибках ввода кода или успешной регистрации

    QString     currentLogin;
    //! \brief Логин пользователя, переданный с первой страницы
    //! \details Сохраняется после успешной регистрации для использования при подтверждении
};

#endif // REGISTERWINDOW_H
//! \brief Конец защиты от множественного включения заголовочного файла
