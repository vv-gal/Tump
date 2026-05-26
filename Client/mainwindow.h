#ifndef MAINWINDOW_H
//! \brief Защита от множественного включения заголовочного файла

#define MAINWINDOW_H
//! \brief Определение макроса для защиты от повторного включения

#include <QMainWindow>
//! \brief Подключение базового класса для главного окна приложения Qt

#include <QStackedWidget>
//! \brief Подключение класса для виджета с стековым переключением страниц

#include <QPointer>
//! \brief Подключение класса умного указателя Qt, автоматически обнуляющегося при удалении объекта

#include "loginwindow.h"
//! \brief Подключение заголовочного файла окна авторизации

#include "registerwindow.h"
//! \brief Подключение заголовочного файла окна регистрации

#include "taskwindow.h"
//! \brief Подключение заголовочного файла окна заданий

#include "graphwindow.h"
//! \brief Подключение заголовочного файла окна графиков

/*!
 * \file mainwindow.h
 * \brief Главное окно приложения — управляет переключением форм (Controller в MVC).
 *
 * Содержит QStackedWidget с тремя формами:
 *   0 — LoginWindow
 *   1 — RegisterWindow
 *   2 — TaskWindow
 *
 * Взаимодействует с TcpClient (Singleton) для отправки запросов и
 * обрабатывает ответы сервера, переключая формы.
 */
//! \brief Файл заголовка главного окна-контроллера, управляющего навигацией между формами

class MainWindow : public QMainWindow {
    //! \brief Класс главного окна приложения, выступающий в роли контроллера (MVC)
    Q_OBJECT
    //! \brief Макрос Qt для поддержки сигналов и слотов

public:
    explicit MainWindow(QWidget *parent = nullptr);
    //! \brief Конструктор главного окна
    //! \param parent Указатель на родительский виджет (по умолчанию nullptr)

    ~MainWindow();
    //! \brief Деструктор главного окна
    //! \details Освобождает ресурсы и закрывает сетевое соединение при необходимости

private slots:
    // Сигналы форм
    void onLoginRequested(const QString &login, const QString &password);
    //! \brief Слот для обработки запроса авторизации от LoginWindow
    //! \param login Логин пользователя
    //! \param password Пароль пользователя

    void onRegisterRequested(const QString &login, const QString &password, const QString &email);
    //! \brief Слот для обработки запроса регистрации от RegisterWindow
    //! \param login Логин пользователя
    //! \param password Пароль пользователя
    //! \param email Email пользователя

    void onConfirmRequested(const QString &login, const QString &code);
    //! \brief Слот для обработки запроса подтверждения регистрации с кодом
    //! \param login Логин пользователя
    //! \param code Код подтверждения из email

    void onCheckRequested(int task, int variant, const QString &answer);
    //! \brief Слот для обработки запроса проверки ответа на задание
    //! \param task Номер задания
    //! \param variant Номер варианта
    //! \param answer Ответ пользователя

    void onStatRequested(const QString &login);
    //! \brief Слот для обработки запроса статистики пользователя
    //! \param login Логин пользователя, для которого запрашивается статистика

    void onLogout();
    //! \brief Слот для обработки запроса выхода из системы
    //! \details Очищает данные текущей сессии и возвращает на окно авторизации

    void onShowGraph();
    //! \brief Слот для отображения окна с графиками функций

    // Сигналы TcpClient
    void onServerResponse(const QString &response);
    //! \brief Слот для обработки ответа от сервера
    //! \param response Ответ сервера в виде строки

    void onConnected();
    //! \brief Слот для обработки события успешного подключения к серверу

    void onDisconnected();
    //! \brief Слот для обработки события отключения от сервера

    void onNetworkError(const QString &error);

    void onSetParamsRequested(const QString &msg);
    void onGetParamsRequested();
    //! \brief Слот для обработки сетевой ошибки
    //! \param error Текст ошибки

private:
    QStackedWidget  *stack;
    //! \brief Указатель на стековый виджет для переключения между формами
    //! \details Индексы: 0 - LoginWindow, 1 - RegisterWindow, 2 - TaskWindow

    LoginWindow     *loginWin;
    //! \brief Указатель на виджет окна авторизации

    RegisterWindow  *regWin;
    //! \brief Указатель на виджет окна регистрации

    TaskWindow      *taskWin;
    //! \brief Указатель на виджет окна заданий

    QPointer<GraphWindow> graphWin;  // ИСПРАВЛЕНО: используем QPointer вместо сырого указателя
    //! \brief Умный указатель QPointer на окно графиков
    //! \details Автоматически обнуляется при удалении окна, предотвращая висячие указатели

    QString          mCurrentLogin;
    //! \brief Логин текущего авторизованного пользователя

    QString          mPendingLogin;
    //! \brief Логин, ожидающий подтверждения регистрации

    QString          mPendingPass;
    //! \brief Пароль, ожидающий подтверждения регистрации

    QString          mPendingEmail;
    //! \brief Email, ожидающий подтверждения регистрации

    void setupConnections();
    //! \brief Настраивает соединения сигналов и слотов между компонентами
    //! \details Соединяет сигналы окон с соответствующими слотами MainWindow
    //!          и сигналы TcpClient со слотами сетевых событий

    void connectToServer();
    //! \brief Устанавливает соединение с сервером
    //! \details Получает экземпляр TcpClient (Singleton) и инициирует подключение
};

#endif // MAINWINDOW_H
//! \brief Конец защиты от множественного включения заголовочного файла
