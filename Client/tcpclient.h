#ifndef TCPCLIENT_H
//! \brief Защита от множественного включения заголовочного файла

#define TCPCLIENT_H
//! \brief Определение макроса для защиты от повторного включения

#include <QObject>
//! \brief Подключение базового класса для объектов Qt с поддержкой сигналов и слотов

#include <QTcpSocket>
//! \brief Подключение класса для TCP-сокета клиента

#include <QString>
//! \brief Подключение класса для работы со строками в Qt

#include <QAbstractSocket>
//! \brief Подключение абстрактного базового класса для сокетов с определением ошибок

/*!
 * \file tcpclient.h
 * \brief Интерфейс взаимодействия с сервером — улучшённый Singleton.
 *
 * Класс TcpClient реализует улучшённый паттерн Singleton с автоматическим
 * управлением памятью через TcpClientDestroyer.
 * Инкапсулирует всё сетевое взаимодействие клиента с сервером.
 */
//! \brief Файл заголовка, содержащий объявление класса TCP-клиента (Singleton)

class TcpClient;
//! \brief Предварительное объявление класса TcpClient для использования в TcpClientDestroyer

/*!
 * \class TcpClientDestroyer
 * \brief Уничтожитель Singleton TcpClient.
 */
class TcpClientDestroyer {
    //! \brief Класс для автоматического управления памятью Singleton TcpClient
    //! \details Реализует паттерн "Уничтожитель" для корректного удаления
    //!          статического экземпляра TcpClient при завершении программы

private:
    TcpClient *p_instance;
    //! \brief Указатель на экземпляр TcpClient, который необходимо уничтожить

public:
    ~TcpClientDestroyer() { delete p_instance; }
    //! \brief Деструктор уничтожителя
    //! \details Удаляет экземпляр TcpClient при разрушении уничтожителя

    void initialize(TcpClient *p) { p_instance = p; }
    //! \brief Инициализирует уничтожитель указателем на экземпляр TcpClient
    //! \param p Указатель на экземпляр TcpClient для последующего удаления
};

/*!
 * \class TcpClient
 * \brief Клиент TCP соединения. Улучшённый Singleton.
 *
 * Обеспечивает подключение к серверу, отправку запросов
 * и приём ответов. Испускает сигналы при получении данных.
 */
class TcpClient : public QObject {
    //! \brief Класс TCP-клиента, реализующий паттерн Singleton
    //! \details Обеспечивает единственный экземпляр для всего приложения,
    //!          управляет сетевым соединением с сервером
    Q_OBJECT
    //! \brief Макрос Qt для поддержки сигналов и слотов

private:
    static TcpClient        *p_instance;
    //! \brief Статический указатель на единственный экземпляр класса TcpClient

    static TcpClientDestroyer destroyer;
    //! \brief Статический объект для автоматического уничтожения Singleton

    QTcpSocket *mSocket;   ///< Сокет соединения
    //! \brief Указатель на TCP-сокет для сетевого взаимодействия с сервером

    QString     mBuffer;   ///< Буфер входящих данных
    //! \brief Буфер для накопления данных, полученных от сервера
    //! \details Используется для обработки частично полученных сообщений

    TcpClient();
    //! \brief Приватный конструктор (Singleton)
    //! \details Создает QTcpSocket и соединяет его сигналы с приватными слотами

    TcpClient(const TcpClient &);
    //! \brief Запрещенный конструктор копирования (Singleton)
    //! \details Объявлен приватным и не реализован для предотвращения копирования

    TcpClient &operator=(TcpClient &);
    //! \brief Запрещенный оператор присваивания (Singleton)
    //! \details Объявлен приватным и не реализован для предотвращения копирования

    ~TcpClient();
    //! \brief Приватный деструктор (Singleton)
    //! \details Закрывает сокет и освобождает ресурсы

    friend class TcpClientDestroyer;
    //! \brief Объявление дружественного класса для доступа к приватному деструктору

public:
    /*!
     * \brief Возвращает единственный экземпляр TcpClient (Singleton).
     */
    static TcpClient *getInstance();
    //! \brief Статический метод получения единственного экземпляра
    //! \return Указатель на единственный экземпляр TcpClient
    //! \details Создает экземпляр при первом вызове (ленивая инициализация)

    /*!
     * \brief Подключается к серверу.
     * \param host Хост (IP или имя)
     * \param port Порт
     */
    void connectToServer(const QString &host, quint16 port);
    //! \brief Устанавливает TCP-соединение с сервером
    //! \param host Адрес сервера (IP-адрес или доменное имя)
    //! \param port Номер порта сервера

    /*!
     * \brief Отключается от сервера.
     */
    void disconnectFromServer();
    //! \brief Разрывает TCP-соединение с сервером
    //! \details Закрывает сокет, если он открыт

    /*!
     * \brief Отправляет сообщение на сервер (добавляет '\n').
     * \param message Сообщение
     */
    void sendMessage(const QString &message);
    //! \brief Отправляет текстовое сообщение на сервер
    //! \param message Строка сообщения
    //! \details Автоматически добавляет символ новой строки '\n' в конец сообщения
    //!          для обозначения конца команды

    /*!
     * \brief Проверяет, подключён ли клиент.
     */
    bool isConnected() const;
    //! \brief Проверяет состояние соединения
    //! \return true - если сокет существует и соединение установлено,
    //!         false - в противном случае

signals:
    /*!
     * \brief Испускается при получении ответа от сервера.
     * \param response Строка ответа
     */
    void responseReceived(const QString &response);
    //! \brief Сигнал, испускаемый при получении данных от сервера
    //! \param response Полученная строка ответа (без символа новой строки)

    /*!
     * \brief Испускается при подключении к серверу.
     */
    void connected();
    //! \brief Сигнал, испускаемый при успешном подключении к серверу

    /*!
     * \brief Испускается при отключении от сервера.
     */
    void disconnected();
    //! \brief Сигнал, испускаемый при отключении от сервера

    /*!
     * \brief Испускается при ошибке соединения.
     * \param error Описание ошибки
     */
    void errorOccurred(const QString &error);
    //! \brief Сигнал, испускаемый при возникновении сетевой ошибки
    //! \param error Текстовое описание ошибки

private slots:
    void onReadyRead();
    //! \brief Приватный слот для обработки готовности данных для чтения
    //! \details Читает данные из сокета, добавляет в буфер,
    //!          извлекает полные сообщения (разделенные '\n')
    //!          и испускает сигнал responseReceived для каждого сообщения

    void onConnected();
    //! \brief Приватный слот для обработки события подключения к серверу
    //! \details Испускает публичный сигнал connected() уведомляющий о подключении

    void onDisconnected();
    //! \brief Приватный слот для обработки события отключения от сервера
    //! \details Испускает публичный сигнал disconnected() уведомляющий об отключении

    void onError(QAbstractSocket::SocketError error);
    //! \brief Приватный слот для обработки ошибок сокета
    //! \param error Код ошибки из перечисления QAbstractSocket::SocketError
    //! \details Преобразует код ошибки в текст и испускает сигнал errorOccurred
};

#endif // TCPCLIENT_H
//! \brief Конец защиты от множественного включения заголовочного файла
