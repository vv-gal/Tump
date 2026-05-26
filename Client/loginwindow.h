#ifndef LOGINWINDOW_H
//! \brief Защита от множественного включения заголовочного файла

#define LOGINWINDOW_H
//! \brief Определение макроса для защиты от повторного включения

#include <QWidget>
//! \brief Подключение базового класса для виджетов Qt

#include <QLineEdit>
//! \brief Подключение класса для однострочного текстового поля ввода

#include <QPushButton>
//! \brief Подключение класса для кнопки

#include <QLabel>
//! \brief Подключение класса для текстовой метки

/*!
 * \file loginwindow.h
 * \brief Форма авторизации (Окно 1 из 3).
 */
//! \brief Файл заголовка, содержащий объявление класса окна авторизации

class LoginWindow : public QWidget {
    //! \brief Класс окна авторизации, предоставляющий интерфейс для входа в систему
    Q_OBJECT
    //! \brief Макрос Qt для поддержки сигналов и слотов

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    //! \brief Конструктор класса LoginWindow
    //! \param parent Указатель на родительский виджет (по умолчанию nullptr)

signals:
    void loginRequested(const QString &login, const QString &password);
    //! \brief Сигнал, испускаемый при запросе авторизации
    //! \param login Логин пользователя, введенный в поле editLogin
    //! \param password Пароль пользователя, введенный в поле editPassword

    void switchToRegister();
    //! \brief Сигнал, испускаемый при запросе перехода к окну регистрации
    //! \details Испускается при нажатии кнопки btnGoRegister

private slots:
    void onLoginClicked();
    //! \brief Слот для обработки нажатия кнопки входа (btnLogin)
    //! \details Проверяет введенные данные и испускает сигнал loginRequested

private:
    QLineEdit  *editLogin;
    //! \brief Указатель на поле ввода логина пользователя

    QLineEdit  *editPassword;
    //! \brief Указатель на поле ввода пароля пользователя
    //! \details Обычно настраивается как password mode (echoMode = Password)

    QPushButton *btnLogin;
    //! \brief Указатель на кнопку для выполнения авторизации

    QPushButton *btnGoRegister;
    //! \brief Указатель на кнопку для перехода к окну регистрации

    QLabel     *lblStatus;
    //! \brief Указатель на текстовую метку для отображения статуса авторизации
    //! \details Используется для показа сообщений об ошибках или успешном входе
};

#endif // LOGINWINDOW_H
//! \brief Конец защиты от множественного включения заголовочного файла
