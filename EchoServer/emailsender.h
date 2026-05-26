/*!
 * \file emailsender.h
 * \brief Заголовочный файл класса EmailSender — отправка email через SMTP.
 */

#ifndef EMAILSENDER_H                     //!< Начало защиты от множественного включения заголовочного файла
#define EMAILSENDER_H                     //!< Определение макроса для защиты от повторного включения

#include <QString>                        //!< Подключение класса для работы со строками в Qt
#include <QTcpSocket>                     //!< Подключение класса TCP-сокета для SMTP-соединения
#include <QDebug>                         //!< Подключение макросов для отладочного вывода в консоль

/*!
 * \class EmailSender
 * \brief Класс для отправки email сообщений через SMTP.
 */
class EmailSender {                       //!< Начало определения класса EmailSender
public:                                   //!< Начало секции публичных методов и свойств класса

    /*!
     * \brief Отправляет письмо с кодом подтверждения.
     * \param toEmail Адрес получателя
     * \param login   Логин пользователя
     * \param code    Код подтверждения
     * \return true если письмо успешно отправлено
     */
    static bool sendConfirmationCode(const QString &toEmail,  //!< Статический метод отправки кода подтверждения на email
                                     const QString &login,    //!< Параметр: логин пользователя (для персонализации письма)
                                     const QString &code);    //!< Параметр: код подтверждения (6 цифр для верификации email)

    /*!
     * \brief Возвращает хост SMTP в зависимости от окружения.
     * \return "mailhog" (Docker network), "host.docker.internal" (Docker host) или "localhost"
     */
    static QString getSMTPHost();         //!< Статический метод получения хоста SMTP с автоопределением окружения

    // Публичные константы для настройки SMTP
    static const QString SMTP_HOST;       //!< Константа: хост SMTP-сервера (устаревшая, оставлена для совместимости)
    static const int     SMTP_PORT;       //!< Константа: порт SMTP-сервера (1025 для MailHog)
    static const QString FROM_ADDR;       //!< Константа: адрес отправителя (email, от которого отправляются письма)
    static const QString FROM_PASS;       //!< Константа: пароль отправителя (MailHog не требует аутентификации)

private:                                  //!< Начало секции приватных методов класса
    /*!
     * \brief Отправляет SMTP-команду и проверяет ответ.
     * \param sock Ссылка на TCP-сокет для обмена данными
     * \param cmd SMTP-команда для отправки
     * \param expectedCode Ожидаемый код ответа от сервера
     * \return true если сервер вернул ожидаемый код
     */
    static bool smtpCmd(QTcpSocket &sock,           //!< Статический метод отправки SMTP-команды
                        const QString &cmd,        //!< Параметр: SMTP-команда для отправки (например "EHLO localhost")
                        const QString &expectedCode); //!< Параметр: ожидаемый код ответа (например "250" или "354")

    /*!
     * \brief Читает ответ SMTP-сервера и проверяет код.
     * \param sock Ссылка на TCP-сокет для чтения данных
     * \param expectedCode Ожидаемый код ответа от сервера
     * \return true если ответ содержит ожидаемый код
     */
    static bool smtpRead(QTcpSocket &sock,          //!< Статический метод чтения ответа SMTP-сервера
                         const QString &expectedCode); //!< Параметр: ожидаемый код ответа (например "220" или "250")
};

#endif // EMAILSENDER_H                   //!< Конец защиты от множественного включения заголовочного файла
