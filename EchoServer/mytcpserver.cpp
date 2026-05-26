/*!
 * \file mytcpserver.cpp
 * \brief Реализация TCP-сервера — приём и обработка сообщений клиентов.
 */
//! \brief Файл реализации TCP-сервера

#include "mytcpserver.h"
//! \brief Подключение заголовочного файла с объявлением класса TCP-сервера

#include "functionsforserver.h"
//! \brief Подключение заголовочного файла с функциями обработки команд

#include <QDebug>
//! \brief Подключение макросов для отладочного вывода

/*!
 * \brief Конструктор: создаёт QTcpServer и начинает слушать порт 33333.
 */
MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent) {
    //! \brief Конструктор TCP-сервера
    //! \param parent Указатель на родительский объект (передается в QObject)

    mTcpServer = new QTcpServer(this);
    //! \brief Создает новый TCP-сервер и устанавливает текущий объект в качестве родителя
    //! \details Родитель обеспечивает автоматическое удаление сервера

    connect(mTcpServer, &QTcpServer::newConnection,
            this,        &MyTcpServer::slotNewConnection);
    //! \brief Соединяет сигнал нового подключения сервера со слотом slotNewConnection

    if (!mTcpServer->listen(QHostAddress::Any, 33333))
        //! \brief Начинает прослушивание всех сетевых интерфейсов на порту 33333
        //! \details QHostAddress::Any означает "все доступные IP-адреса"
        qDebug() << "Server NOT started:" << mTcpServer->errorString();
    //! \brief Выводит сообщение об ошибке, если сервер не запустился
    else
        qDebug() << "Server started on port 33333";
    //! \brief Выводит сообщение об успешном запуске сервера
}

/*!
 * \brief Деструктор: корректно закрывает сервер.
 */
MyTcpServer::~MyTcpServer() {
    //! \brief Деструктор TCP-сервера
    mTcpServer->close();
    //! \brief Закрывает сервер, прекращая прослушивание новых подключений
}

/*!
 * \brief Принимает новое соединение. Отправляет приветствие.
 */
void MyTcpServer::slotNewConnection() {
    //! \brief Слот для обработки нового подключения клиента
    //! \details Вызывается автоматически при появлении нового клиента

    QTcpSocket *socket = mTcpServer->nextPendingConnection();
    //! \brief Получает сокет для нового подключения

    mBuffers[socket] = "";
    //! \brief Создает пустой буфер для нового клиента
    //! \details Буфер используется для накопления частично полученных данных

    connect(socket, &QTcpSocket::readyRead,
            this,   &MyTcpServer::slotServerRead);
    //! \brief Соединяет сигнал готовности данных со слотом чтения

    connect(socket, &QTcpSocket::disconnected,
            this,   &MyTcpServer::slotClientDisconnected);
    //! \brief Соединяет сигнал отключения клиента со слотом очистки

    socket->write("connected\r\n");
    //! \brief Отправляет клиенту приветственное сообщение

    qDebug() << "New client:" << socket->peerAddress().toString();
    //! \brief Выводит информацию о новом клиенте (IP-адрес)
}

/*!
 * \brief Читает данные из сокета, накапливает в буфер, обрабатывает полные сообщения.
 *
 * Сообщения разделяются символом '\n'.
 */
void MyTcpServer::slotServerRead() {
    //! \brief Слот для чтения данных от клиента
    //! \details Обрабатывает входящие сообщения, разбивает по символу новой строки

    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    //! \brief Получает указатель на сокет, отправивший сигнал
    //! \details qobject_cast безопасно преобразует QObject* в QTcpSocket*

    if (!socket) return;
    //! \brief Проверяет, что сокет получен корректно

    mBuffers[socket] += QString::fromUtf8(socket->readAll());
    //! \brief Читает все доступные данные из сокета и добавляет их в буфер
    //! \details Данные преобразуются из UTF-8 в QString

    // Обрабатываем все полные строки (разделитель \n)
    while (mBuffers[socket].contains('\n')) {
        //! \brief Цикл обработки всех полных сообщений в буфере

        int idx     = mBuffers[socket].indexOf('\n');
        //! \brief Находит позицию первого символа новой строки

        QString msg = mBuffers[socket].left(idx).trimmed();
        //! \brief Извлекает сообщение от начала до символа новой строки и удаляет лишние пробелы

        mBuffers[socket] = mBuffers[socket].mid(idx + 1);
        //! \brief Удаляет обработанное сообщение из буфера (включая символ новой строки)

        if (msg.isEmpty()) continue;
        //! \brief Пропускает пустые сообщения

        qDebug() << "Received:" << msg;
        //! \brief Выводит отладочную информацию о полученном сообщении

        QString response = parsing(msg);
        //! \brief Вызывает функцию парсинга для обработки команды и получения ответа

        qDebug() << "Response:" << response;
        //! \brief Выводит отладочную информацию об ответе сервера

        socket->write((response + "\r\n").toUtf8());
        //! \brief Отправляет ответ клиенту с добавлением CRLF (\r\n) в кодировке UTF-8
    }
}

/*!
 * \brief Очищает буфер и закрывает сокет отключившегося клиента.
 */
void MyTcpServer::slotClientDisconnected() {
    //! \brief Слот для обработки отключения клиента
    //! \details Вызывается при разрыве соединения с клиентом

    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    //! \brief Получает указатель на сокет, отправивший сигнал

    if (!socket) return;
    //! \brief Проверяет, что сокет получен корректно

    qDebug() << "Client disconnected:" << socket->peerAddress().toString();
    //! \brief Выводит информацию об отключившемся клиенте (IP-адрес)

    mBuffers.remove(socket);
    //! \brief Удаляет буфер для отключившегося клиента

    socket->deleteLater();
    //! \brief Планирует удаление сокета (безопасное удаление в событийном цикле)
    //! \details Используется вместо delete, чтобы избежать проблем с очередью событий
}
