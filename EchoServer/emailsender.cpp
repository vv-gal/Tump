/*!
 * \file emailsender.cpp
 * \brief Реализация класса EmailSender — отправка кода через MailHog.
 */

#include "emailsender.h"                       //!< Подключение заголовочного файла класса EmailSender
#include <QTcpSocket>                          //!< Подключение класса TCP-сокета для SMTP-соединения
#include <QDebug>                              //!< Подключение макросов для отладочного вывода в консоль
#include <QFile>                               //!< Подключение класса для работы с файлами (проверка Docker окружения)

// Настройки для MailHog
const int     EmailSender::SMTP_PORT = 1025;   //!< Порт SMTP-сервера MailHog (1025 по умолчанию)
const QString EmailSender::FROM_ADDR = "test@example.com";  //!< Адрес отправителя (тестовый email для MailHog)
const QString EmailSender::FROM_PASS = "";     //!< Пароль отправителя (MailHog не требует аутентификации)

// Оставляем SMTP_HOST для совместимости, но не используем
const QString EmailSender::SMTP_HOST = "localhost";  //!< Устаревшая константа для обратной совместимости

// Функция определения окружения и получения хоста SMTP
static QString getSmtpHost() {                 //!< Функция автоопределения хоста SMTP в зависимости от окружения
    // Проверяем, запущены ли мы в Docker
    QFile dockerFile("/proc/1/cgroup");        //!< Открытие файла cgroup (существует только в Linux/Docker)
    bool inDocker = false;                     //!< Флаг: true если приложение запущено в Docker контейнере

    if (dockerFile.open(QIODevice::ReadOnly)) { //!< Проверка успешности открытия файла cgroup
        QString content = dockerFile.readAll(); //!< Чтение всего содержимого файла cgroup
        if (content.contains("/docker/") || content.contains("/lxc/")) {  //!< Поиск признаков Docker или LXC в содержимом
            inDocker = true;                    //!< Установка флага Docker при обнаружении признаков
            qDebug() << "SMTP: Running inside Docker container";  //!< Вывод сообщения о запуске в Docker контейнере
        }
        dockerFile.close();                    //!< Закрытие файла cgroup
    }

    // Альтернативный способ проверки: наличие файла .dockerenv
    if (!inDocker && QFile::exists("/.dockerenv")) {  //!< Проверка существования файла .dockerenv (признак Docker)
        inDocker = true;                        //!< Установка флага Docker при обнаружении файла
        qDebug() << "SMTP: Running inside Docker container (dockerenv found)";  //!< Вывод сообщения о запуске в Docker
    }

    if (inDocker) {                            //!< Блок кода для Docker окружения
        // В Docker используем специальное имя хоста для доступа к хостовой машине
        // Проверяем, есть ли MailHog в том же контейнере (через docker-compose)
        QTcpSocket testSocket;                 //!< Создание тестового TCP-сокета для проверки подключения
        testSocket.connectToHost("mailhog", 1025);  //!< Попытка подключения к MailHog как к сервису Docker Compose
        if (testSocket.waitForConnected(2000)) {  //!< Ожидание подключения с таймаутом 2 секунды
            testSocket.close();                 //!< Закрытие тестового сокета при успешном подключении
            qDebug() << "SMTP: Using mailhog service inside Docker network";  //!< Вывод сообщения об использовании Docker сети
            return "mailhog";                  //!< Возврат имени сервиса mailhog для Docker Compose
        }

        // Если MailHog на хосте, используем host.docker.internal
        qDebug() << "SMTP: Using host.docker.internal to reach host MailHog";  //!< Вывод сообщения об использовании хоста
        return "host.docker.internal";         //!< Возврат специального имени хоста для доступа к хостовой машине из Docker
    }

    // В Windows/Qt Creator используем localhost
    qDebug() << "SMTP: Running on host (Windows/Linux), using localhost";  //!< Вывод сообщения о запуске на хостовой системе
    return "localhost";                        //!< Возврат localhost для запуска вне Docker
}

// Динамическое получение хоста SMTP (вместо статической константы)
QString EmailSender::getSMTPHost() {          //!< Метод получения хоста SMTP с кэшированием результата
    static QString host = getSmtpHost();      //!< Статическая переменная для однократного определения хоста
    return host;                              //!< Возврат кэшированного значения хоста SMTP
}

bool EmailSender::sendConfirmationCode(const QString &toEmail,   //!< Метод отправки кода подтверждения на email
                                       const QString &login,     //!< Логин пользователя (включается в тело письма)
                                       const QString &code)      //!< Код подтверждения (6 цифр)
{
    QString smtpHost = getSMTPHost();         //!< Определение хоста SMTP в зависимости от окружения
    qDebug() << "SMTP: Sending email to" << toEmail << "via" << smtpHost << ":" << SMTP_PORT;  //!< Вывод информации об отправке письма

    QTcpSocket socket;                        //!< Создание TCP-сокета для соединения с SMTP-сервером
    socket.connectToHost(smtpHost, SMTP_PORT); //!< Установка соединения с SMTP-сервером (хост и порт)

    if (!socket.waitForConnected(5000)) {     //!< Ожидание подключения с таймаутом 5 секунд
        qDebug() << "SMTP: Connection failed to" << smtpHost << ":" << SMTP_PORT;  //!< Вывод сообщения об ошибке подключения
        return false;                         //!< Возврат false при невозможности подключиться
    }

    // SMTP handshake
    if (!smtpRead(socket, "220")) return false;  //!< Чтение приветствия сервера (ожидание кода 220)
    if (!smtpCmd(socket, "EHLO localhost", "250")) return false;  //!< Отправка команды EHLO (приветствие и запрос возможностей сервера)

    // MailHog не требует авторизации
    if (!smtpCmd(socket, "MAIL FROM:<" + FROM_ADDR + ">", "250")) return false;  //!< Указание отправителя письма (команда MAIL FROM)
    if (!smtpCmd(socket, "RCPT TO:<" + toEmail + ">",    "250")) return false;  //!< Указание получателя письма (команда RCPT TO)
    if (!smtpCmd(socket, "DATA", "354")) return false;    //!< Команда DATA (начало передачи содержимого письма)

    QString body = QString(                       //!< Формирование тела письма с использованием QString::arg
                       "From: TcpServer <%1>\r\n"  //!< Заголовок From (отправитель) с переносом строки CRLF
                       "To: %2\r\n"               //!< Заголовок To (получатель) с переносом строки CRLF
                       "Subject: Код подтверждения регистрации\r\n"  //!< Тема письма с переносом строки CRLF
                       "\r\n"                     //!< Пустая строка между заголовками и телом письма
                       "Здравствуйте, %3!\r\n\r\n" //!< Приветствие пользователя с двумя переносами строк
                       "Ваш код подтверждения: %4\r\n\r\n"  //!< Текст с кодом подтверждения с двумя переносами строк
                       "Введите его в приложении.\r\n"      //!< Инструкция по вводу кода с переносом строки
                       ).arg(FROM_ADDR, toEmail, login, code);  //!< Подстановка параметров в шаблон (отправитель, получатель, логин, код)

    // Отправляем тело письма
    socket.write(body.toUtf8());                  //!< Запись тела письма в сокет в кодировке UTF-8
    if (!socket.waitForBytesWritten(3000)) {      //!< Ожидание завершения записи данных (таймаут 3 секунды)
        qDebug() << "SMTP: Failed to write body"; //!< Вывод сообщения об ошибке записи тела письма
        return false;                             //!< Возврат false при ошибке записи
    }
    qDebug() << "SMTP: Body sent";                //!< Вывод сообщения об успешной отправке тела письма

    // Отправляем завершающую точку и ждём 250 от MailHog
    if (!smtpCmd(socket, ".", "250")) return false;  //!< Отправка завершающей точки (конец данных) и ожидание кода 250

    smtpCmd(socket, "QUIT", "221");               //!< Отправка команды QUIT (завершение SMTP-сессии) и ожидание кода 221

    qDebug() << "SMTP: Email sent successfully to" << toEmail;  //!< Вывод сообщения об успешной отправке письма
    return true;                                  //!< Возврат true (письмо успешно отправлено)
}

// Отправить команду и проверить начало ответа
bool EmailSender::smtpCmd(QTcpSocket &sock,      //!< Ссылка на TCP-сокет для обмена данными
                          const QString &cmd,    //!< SMTP-команда для отправки на сервер
                          const QString &expectedCode)  //!< Ожидаемый код ответа от сервера (например "250" или "354")
{
    sock.write((cmd + "\r\n").toUtf8());         //!< Запись команды в сокет с добавлением CRLF и преобразованием в UTF-8
    if (!sock.waitForBytesWritten(3000)) {       //!< Ожидание завершения записи команды (таймаут 3 секунды)
        qDebug() << "SMTP: Failed to write:" << cmd.left(50);  //!< Вывод сообщения об ошибке записи команды (первые 50 символов)
        return false;                             //!< Возврат false при ошибке записи
    }
    qDebug() << "SMTP: Sent >>" << cmd.left(80);  //!< Вывод отправленной команды (первые 80 символов) для отладки

    return smtpRead(sock, expectedCode);         //!< Вызов функции чтения ответа и проверки кода ответа
}

// Прочитать ответ и проверить начало
bool EmailSender::smtpRead(QTcpSocket &sock,     //!< Ссылка на TCP-сокет для чтения ответа
                           const QString &expectedCode)  //!< Ожидаемый код ответа от сервера (например "220" или "250")
{
    if (!sock.waitForReadyRead(5000)) {          //!< Ожидание готовности данных для чтения (таймаут 5 секунд)
        qDebug() << "SMTP: Timeout waiting for" << expectedCode;  //!< Вывод сообщения о таймауте ожидания ответа
        return false;                             //!< Возврат false при таймауте
    }

    QString response = QString::fromUtf8(sock.readAll());  //!< Чтение всех доступных данных из сокета и преобразование в UTF-8 строку
    qDebug() << "SMTP: Recv <<" << response.trimmed();  //!< Вывод полученного ответа без лишних пробелов и переводов строк

    return response.contains(expectedCode);       //!< Проверка: содержит ли ответ сервера ожидаемый код успеха
}
