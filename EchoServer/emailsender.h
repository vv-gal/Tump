/*!
 * \file emailsender.h
 * \brief Заголовочный файл класса EmailSender — отправка email через SMTP.
 */

#ifndef EMAILSENDER_H
#define EMAILSENDER_H

#include <QString>
#include <QTcpSocket>

/*!
 * \class EmailSender
 * \brief Класс для отправки email сообщений через SMTP.
 */
class EmailSender {
public:
    /*!
     * \brief Отправляет письмо с кодом подтверждения.
     * \param toEmail Адрес получателя
     * \param login   Логин пользователя
     * \param code    Код подтверждения
     * \return true если письмо успешно отправлено
     */
    static bool sendConfirmationCode(const QString &toEmail,
                                     const QString &login,
                                     const QString &code);

    /*!
     * \brief Возвращает хост SMTP в зависимости от окружения.
     * \return "mailhog" (Docker network), "host.docker.internal" (Docker host) или "localhost"
     */
    static QString getSMTPHost();

    // Публичные константы для настройки SMTP
    static const QString SMTP_HOST;
    static const int     SMTP_PORT;
    static const QString FROM_ADDR;
    static const QString FROM_PASS;

private:
    /*!
     * \brief Отправляет SMTP-команду и проверяет ответ.
     * \param sock Ссылка на TCP-сокет для обмена данными
     * \param cmd SMTP-команда для отправки
     * \param expectedCode Ожидаемый код ответа от сервера
     * \return true если сервер вернул ожидаемый код
     */
    static bool smtpCmd(QTcpSocket &sock,
                        const QString &cmd,
                        const QString &expectedCode);

    /*!
     * \brief Читает ответ SMTP-сервера и проверяет код.
     * \param sock Ссылка на TCP-сокет для чтения данных
     * \param expectedCode Ожидаемый код ответа от сервера
     * \return true если ответ содержит ожидаемый код
     */
    static bool smtpRead(QTcpSocket &sock,
                         const QString &expectedCode);
};

#endif // EMAILSENDER_H
