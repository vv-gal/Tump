#ifndef DATABASE_H
//! \brief Защита от множественного включения заголовочного файла

#define DATABASE_H
//! \brief Определение макроса для защиты от повторного включения

#include <QObject>
//! \brief Подключение базового класса для объектов Qt с поддержкой сигналов и слотов

#include <QSqlDatabase>
//! \brief Подключение класса для работы с подключением к базе данных

#include <QSqlQuery>
//! \brief Подключение класса для выполнения SQL-запросов

#include <QSqlError>
//! \brief Подключение класса для получения информации об ошибках SQL

#include <QSqlRecord>
//! \brief Подключение класса для работы с записями результата запроса

#include <QVariant>
//! \brief Подключение класса для хранения разнотипных данных

#include <QString>
//! \brief Подключение класса для работы со строками в Qt

#include <QDebug>
//! \brief Подключение макросов для отладочного вывода

/*!
 * \file database.h
 * \brief Заголовочный файл класса DataBase — работа с SQLite базой данных.
 *
 * Реализует паттерн улучшённого Singleton для единственного экземпляра БД.
 */
//! \brief Файл заголовка, содержащий объявление класса для работы с базой данных SQLite

class DataBase;
//! \brief Предварительное объявление класса DataBase для использования в DataBaseDestroyer

/*!
 * \class DataBaseDestroyer
 * \brief Вспомогательный класс для корректного удаления Singleton DataBase.
 */
class DataBaseDestroyer {
    //! \brief Класс для автоматического управления памятью Singleton DataBase
    //! \details Реализует паттерн "Уничтожитель" для корректного удаления
    //!          статического экземпляра DataBase при завершении программы

private:
    DataBase *p_instance; ///< Указатель на экземпляр DataBase
    //! \brief Приватный указатель на экземпляр DataBase, который необходимо уничтожить

public:
    ~DataBaseDestroyer();
    //! \brief Деструктор (определён в database.cpp после полного объявления DataBase)

    void initialize(DataBase *p) { p_instance = p; }
    //! \brief Инициализирует уничтожитель указателем на экземпляр DataBase
    //! \param p Указатель на экземпляр DataBase для последующего удаления
};

/*!
 * \class DataBase
 * \brief Класс для работы с базой данных (SQLite).
 *
 * Реализует улучшённый Singleton. Обеспечивает регистрацию,
 * авторизацию, статистику и подтверждение по email.
 */
class DataBase : public QObject {
    //! \brief Класс для работы с базой данных SQLite, реализующий паттерн Singleton
    //! \details Обеспечивает единственный экземпляр для всего приложения,
    //!          управляет подключением к БД, операциями регистрации,
    //!          авторизации, статистики и подтверждения email
    Q_OBJECT
    //! \brief Макрос Qt для поддержки сигналов и слотов

private:
    static DataBase        *p_instance; ///< Единственный экземпляр
    //! \brief Статический указатель на единственный экземпляр класса DataBase

    static DataBaseDestroyer destroyer; ///< Уничтожитель экземпляра
    //! \brief Статический объект для автоматического уничтожения Singleton

    QSqlDatabase db; ///< Объект базы данных
    //! \brief Объект подключения к базе данных SQLite

    DataBase();
    //! \brief Приватный конструктор (Singleton)
    //! \details Инициализирует объект базы данных, но не открывает подключение

    DataBase(const DataBase &);
    //! \brief Запрещенный конструктор копирования (Singleton)
    //! \details Объявлен приватным и не реализован для предотвращения копирования

    DataBase &operator=(DataBase &);
    //! \brief Запрещенный оператор присваивания (Singleton)
    //! \details Объявлен приватным и не реализован для предотвращения копирования

    ~DataBase() { db.close(); }
    //! \brief Приватный деструктор (Singleton)
    //! \details Закрывает соединение с базой данных перед уничтожением

    friend class DataBaseDestroyer;
    //! \brief Объявление дружественного класса для доступа к приватному деструктору

public:
    /*!
     * \brief Возвращает единственный экземпляр DataBase (Singleton).
     * \return Указатель на DataBase
     */
    static DataBase *getInstance();
    //! \brief Статический метод получения единственного экземпляра
    //! \return Указатель на единственный экземпляр DataBase
    //! \details Создает экземпляр при первом вызове (ленивая инициализация)

    /*!
     * \brief Инициализирует базу данных и создаёт таблицы.
     */
    void init();
    //! \brief Инициализирует подключение к базе данных и создает необходимые таблицы
    //! \details Создает таблицы users, confirm_codes, stats, если они не существуют

    /*!
     * \brief Регистрирует нового пользователя.
     * \param login Логин
     * \param password Пароль
     * \param email Email
     * \return true если успешно
     */
    bool registerUser(const QString &login, const QString &password, const QString &email);
    //! \brief Добавляет нового пользователя в базу данных
    //! \param login Логин пользователя
    //! \param password Пароль пользователя
    //! \param email Email пользователя
    //! \return true - регистрация успешна, false - пользователь уже существует или ошибка

    /*!
     * \brief Авторизует пользователя.
     * \param login Логин
     * \param password Пароль
     * \return true если данные верны и email подтверждён
     */
    bool authUser(const QString &login, const QString &password);
    //! \brief Проверяет учетные данные пользователя
    //! \param login Логин пользователя
    //! \param password Пароль пользователя
    //! \return true - логин и пароль верны, email подтвержден,
    //!         false - неверные данные или email не подтвержден

    /*!
     * \brief Подтверждает email по коду.
     * \param login Логин
     * \param code Код подтверждения
     * \return true если код верен
     */
    bool confirmEmail(const QString &login, const QString &code);
    //! \brief Проверяет код подтверждения email и активирует учетную запись
    //! \param login Логин пользователя
    //! \param code Код подтверждения, отправленный на email
    //! \return true - код верен и email подтвержден, false - код неверен

    /*!
     * \brief Записывает результат проверки задачи в статистику.
     * \param login Логин
     * \param taskNum Номер задачи
     * \param correct Верно ли решено
     */
    void saveResult(const QString &login, int taskNum, bool correct);
    //! \brief Сохраняет результат решения задачи в статистику
    //! \param login Логин пользователя
    //! \param taskNum Номер задачи
    //! \param correct true - задача решена верно, false - неверно
    //! \details Обновляет количество попыток и, если ответ верный, увеличивает счетчик правильных

    /*!
     * \brief Возвращает статистику пользователя.
     * \param login Логин
     * \return Строка вида "3$6&21" (правильные$попытки&очки)
     */
    QString getStats(const QString &login);
    //! \brief Получает статистику пользователя из базы данных
    //! \param login Логин пользователя
    //! \return Строка в формате "правильные$попытки&очки" (например, "3$6&21")
    //! \details Возвращает "0$0&0", если пользователь не найден или ошибка

    /*!
     * \brief Проверяет, существует ли пользователь.
     * \param login Логин
     * \return true если существует
     */
    bool userExists(const QString &login);

    /*!
     * \brief Возвращает код подтверждения из БД.
     * \param login Логин
     * \return Код подтверждения или пустая строка
     */
    QString getConfirmationCode(const QString &login);
    //! \brief Получает код подтверждения для указанного пользователя из базы данных
    //! \brief Проверяет наличие пользователя в базе данных
    //! \param login Логин пользователя
    //! \return true - пользователь существует, false - не найден
};

#endif // DATABASE_H
//! \brief Конец защиты от множественного включения заголовочного файла
