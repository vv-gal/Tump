/*!
 * \file tcpclient.cpp
 * \brief Реализация TcpClient — улучшённый Singleton для работы с сервером.
 */
//! \brief Файл реализации класса TCP-клиента (Singleton)

#include "tcpclient.h"
//! \brief Подключение заголовочного файла с объявлением класса TCP-клиента

#include <QDebug>
//! \brief Подключение макросов для отладочного вывода в консоль

TcpClient        *TcpClient::p_instance = nullptr;
//! \brief Инициализация статического указателя на экземпляр класса нулевым значением

TcpClientDestroyer TcpClient::destroyer;
//! \brief Инициализация статического объекта-уничтожителя Singleton

TcpClient::TcpClient() : QObject(nullptr) {
    //! \brief Приватный конструктор Singleton
    //! \param nullptr Родительский объект отсутствует (не передается this, т.к. Singleton)

    mSocket = new QTcpSocket(this);
    //! \brief Создает новый TCP-сокет и устанавливает текущий объект в качестве родителя
    //! \details Родитель обеспечивает автоматическое удаление сокета при удалении TcpClient

    connect(mSocket, &QTcpSocket::readyRead,    this, &TcpClient::onReadyRead);
    //! \brief Соединяет сигнал готовности данных сокета со слотом onReadyRead

    connect(mSocket, &QTcpSocket::connected,    this, &TcpClient::onConnected);
    //! \brief Соединяет сигнал подключения сокета со слотом onConnected

    connect(mSocket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    //! \brief Соединяет сигнал отключения сокета со слотом onDisconnected

    // ИСПРАВЛЕНО: используем errorOccurred вместо устаревшего QOverload
    connect(mSocket, &QTcpSocket::errorOccurred, this, &TcpClient::onError);
    //! \brief Соединяет сигнал ошибки сокета со слотом onError
    //! \details Используется современный сигнал errorOccurred вместо устаревшего error
}

TcpClient::~TcpClient() {
    //! \brief Приватный деструктор Singleton
    //! \details Закрывает соединение с хостом, если оно активно

    mSocket->disconnectFromHost();
    //! \brief Отключает сокет от удаленного хоста
    //! \details Инициирует graceful shutdown соединения
}

TcpClient *TcpClient::getInstance() {
    //! \brief Статический метод получения единственного экземпляра класса
    //! \return Указатель на единственный экземпляр TcpClient

    if (!p_instance) {
        //! \brief Проверяет, существует ли уже экземпляр
        p_instance = new TcpClient();
        //! \brief Создает новый экземпляр при первом вызове (ленивая инициализация)

        destroyer.initialize(p_instance);
        //! \brief Передает указатель на экземпляр уничтожителю для автоматического удаления
    }

    return p_instance;
    //! \brief Возвращает указатель на единственный экземпляр
}

void TcpClient::connectToServer(const QString &host, quint16 port) {
    //! \brief Устанавливает соединение с сервером
    //! \param host Адрес сервера (IP-адрес или доменное имя)
    //! \param port Номер порта сервера

    mSocket->connectToHost(host, port);
    //! \brief Инициирует асинхронное подключение к серверу
}

void TcpClient::disconnectFromServer() {
    //! \brief Разрывает соединение с сервером

    mSocket->disconnectFromHost();
    //! \brief Отключает сокет от удаленного хоста
}

void TcpClient::sendMessage(const QString &message) {
    //! \brief Отправляет сообщение на сервер
    //! \param message Текст сообщения для отправки

    if (!isConnected()) {
        //! \brief Проверяет, установлено ли соединение с сервером
        emit errorOccurred("Нет соединения с сервером");
        //! \brief Испускает сигнал об ошибке, если соединение отсутствует
        return;
        //! \brief Прерывает выполнение функции
    }

    mSocket->write((message + "\n").toUtf8());
    //! \brief Добавляет символ новой строки и отправляет сообщение в кодировке UTF-8

    qDebug() << "Sent:" << message;
    //! \brief Выводит отладочную информацию об отправленном сообщении
}

bool TcpClient::isConnected() const {
    //! \brief Проверяет состояние соединения
    //! \return true - если сокет находится в состоянии ConnectedState, иначе false

    return mSocket->state() == QAbstractSocket::ConnectedState;
    //! \brief Сравнивает текущее состояние сокета с состоянием подключения
}

void TcpClient::onReadyRead() {
    //! \brief Слот для обработки готовности данных к чтению
    //! \details Читает данные из сокета, накапливает их в буфере,
    //!          извлекает полные сообщения по разделителю '\n'

    mBuffer += QString::fromUtf8(mSocket->readAll());
    //! \brief Читает все доступные данные из сокета и добавляет их в буфер в кодировке UTF-8

    while (mBuffer.contains('\n')) {
        //! \brief Цикл обработки всех полных сообщений в буфере
        int idx      = mBuffer.indexOf('\n');
        //! \brief Находит позицию первого символа новой строки

        QString resp = mBuffer.left(idx).trimmed();
        //! \brief Извлекает сообщение от начала до символа новой строки и удаляет лишние пробелы

        mBuffer      = mBuffer.mid(idx + 1);
        //! \brief Удаляет обработанное сообщение из буфера (включая символ новой строки)

        if (!resp.isEmpty()) {
            //! \brief Проверяет, что сообщение не пустое
            qDebug() << "Received:" << resp;
            //! \brief Выводит отладочную информацию о полученном сообщении

            emit responseReceived(resp);
            //! \brief Испускает сигнал с полученным ответом
        }
    }
}

void TcpClient::onConnected()    {
    //! \brief Слот для обработки события подключения к серверу
    emit connected();
    //! \brief Испускает публичный сигнал connected() уведомляющий о подключении
}

void TcpClient::onDisconnected() {
    //! \brief Слот для обработки события отключения от сервера
    emit disconnected();
    //! \brief Испускает публичный сигнал disconnected() уведомляющий об отключении
}

void TcpClient::onError(QAbstractSocket::SocketError) {
    //! \brief Слот для обработки ошибок сокета
    //! \param error Код ошибки (не используется, т.к. текст ошибки получаем из сокета)

    emit errorOccurred(mSocket->errorString());
    //! \brief Извлекает текстовое описание ошибки из сокета и испускает сигнал errorOccurred
}
