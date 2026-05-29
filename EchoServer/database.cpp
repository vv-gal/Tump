/*!
 * \file database.cpp
 * \brief Реализация класса DataBase — работа с SQLite через Qt SQL.
 */
//! \brief Файл содержит реализацию методов класса DataBase для работы с базой данных SQLite

#include "database.h"
//! \brief Подключение заголовочного файла с объявлением класса DataBase и структур данных

#include <QCryptographicHash>
//! \brief Подключение класса для криптографического хэширования (используется SHA-256 для паролей)

#include <QRandomGenerator>
//! \brief Подключение класса для генерации случайных чисел (используется для создания кода подтверждения)

#include <QFile>
//! \brief Подключение класса для работы с файлами (проверка существования, чтение/запись)

#include <QTextStream>
//! \brief Подключение класса для текстового ввода/вывода (потоковая работа с текстовыми данными)

#include <QDateTime>
//! \brief Подключение класса для работы с датой и временем (метки времени операций)

#include <QProcessEnvironment>
//! \brief Подключение класса для доступа к переменным окружения (определение окружения выполнения)

#include <QDir>
//! \brief Подключение класса для работы с директориями (создание, проверка существования, навигация)

#include <QFileInfo>
//! \brief Подключение класса для получения информации о файлах (размер, дата, существование)

DataBase *DataBase::p_instance = nullptr;
//! \brief Инициализация статического указателя на единственный экземпляр класса DataBase нулевым значением
//! \details Реализация паттерна Singleton: указатель хранит адрес единственного объекта класса

DataBaseDestroyer DataBase::destroyer;
//! \brief Инициализация статического объекта-уничтожителя для автоматического освобождения памяти
//! \details Объект будет удален при завершении программы, вызывая деструктор DataBase

DataBaseDestroyer::~DataBaseDestroyer() { delete p_instance; }
//! \brief Деструктор класса-уничтожителя
//! \details При разрушении уничтожителя удаляет единственный экземпляр DataBase
//! \warning Вызывается автоматически при завершении программы

DataBase::DataBase() {}
//! \brief Приватный конструктор класса DataBase (паттерн Singleton)
//! \details Конструктор пустой, так как подключение к БД требует параметров и выполняется в init()

DataBase *DataBase::getInstance() {
    //! \brief Статический метод получения единственного экземпляра класса (паттерн Singleton)
    //! \return Указатель на единственный экземпляр DataBase

    if (!p_instance) {
        //! \brief Проверка: существует ли уже экземпляр класса

        p_instance = new DataBase();
        //! \brief Создание нового экземпляра при первом вызове (ленивая инициализация)

        destroyer.initialize(p_instance);
        //! \brief Передача указателя на экземпляр уничтожителю для автоматического удаления
    }

    return p_instance;
    //! \brief Возвращение указателя на единственный экземпляр DataBase
}

void DataBase::init() {
    //! \brief Инициализация подключения к базе данных SQLite
    //! \details Открывает файл БД, создает таблицы при первом запуске, настраивает соединение

    db = QSqlDatabase::addDatabase("QSQLITE");
    //! \brief Добавление нового подключения к базе данных с драйвером SQLite
    //! \details Создает объект QSqlDatabase с именем подключения по умолчанию

    QString dbFileName;
    //! \brief Объявление строковой переменной для хранения пути к файлу базы данных

// Определяем окружение
#ifdef Q_OS_WIN
    //! \brief Условная компиляция: блок выполняется только на операционной системе Windows

    // Windows (Qt Creator запуск)
    dbFileName = "D:/project_timp/project/db/math_server.db";
    //! \brief Установка пути к файлу БД для Windows-окружения (абсолютный путь)
    //! \details При запуске через Qt Creator на Windows используется этот путь

    qDebug() << "Running on Windows, using:" << dbFileName;
    //! \brief Вывод отладочного сообщения о запуске в Windows и используемом пути к БД
#else
    //! \brief Условная компиляция: блок выполняется на всех ОС, кроме Windows (Linux, macOS)

    // Linux (Docker)
    dbFileName = "/app/math_server.db";
    //! \brief Установка пути к файлу БД для Linux/Docker-окружения
    //! \details При запуске в Docker-контейнере БД монтируется по этому пути

    qDebug() << "Running on Linux/Docker, using:" << dbFileName;
    //! \brief Вывод отладочного сообщения о запуске в Linux/Docker и используемом пути к БД
#endif
    //! \brief Конец блока условной компиляции

    db.setDatabaseName(dbFileName);
    //! \brief Установка имени файла базы данных для подключения
    //! \details Передает путь к файлу БД объекту QSqlDatabase

    // Проверяем, существует ли файл базы данных
    bool dbExists = QFileInfo::exists(dbFileName);
    //! \brief Проверка существования файла базы данных на диске
    //! \details QFileInfo::exists() возвращает true, если файл существует

    if (!db.open()) {
        //! \brief Попытка открыть соединение с базой данных
        //! \details Если открыть не удалось, выполняется блок обработки ошибки

        qDebug() << "DB open error:" << db.lastError().text();
        //! \brief Вывод сообщения об ошибке открытия БД с текстом ошибки из драйвера

        qDebug() << "Tried to open:" << dbFileName;
        //! \brief Вывод пути к файлу, который пытались открыть (для отладки)

        return;
        //! \brief Прерывание выполнения функции при ошибке открытия БД
    }

    // Если база данных только что создана (файла не было) - создаём таблицы
    if (!dbExists) {
        //! \brief Проверка: была ли база данных создана только что (файл отсутствовал)

        qDebug() << "Creating new database with tables...";
        //! \brief Вывод сообщения о начале создания таблиц в новой базе данных

        QSqlQuery query;
        //! \brief Создание объекта для выполнения SQL-запросов
        //! \details query будет использован для выполнения DDL-команд создания таблиц

        QString createUsers = R"(
            CREATE TABLE Users (
                login     VARCHAR(50)  NOT NULL PRIMARY KEY,
                password  VARCHAR(64)  NOT NULL,
                email     VARCHAR(100) NOT NULL,
                confirmed INTEGER      NOT NULL DEFAULT 0,
                conf_code VARCHAR(10)  NOT NULL DEFAULT ''
            )
        )";
        //! \brief SQL-запрос для создания таблицы пользователей
        //! \details Поля таблицы:
        //!   - login: уникальный логин пользователя (первичный ключ)
        //!   - password: хэш пароля (SHA-256, 64 символа)
        //!   - email: электронная почта для подтверждения
        //!   - confirmed: флаг подтверждения email (0 - не подтвержден, 1 - подтвержден)
        //!   - conf_code: 6-значный код подтверждения регистрации

        QString createStats = R"(
            CREATE TABLE Stats (
                login   VARCHAR(50) NOT NULL,
                task    INTEGER     NOT NULL,
                correct INTEGER     NOT NULL,
                points  INTEGER     NOT NULL
            )
        )";
        //! \brief SQL-запрос для создания таблицы статистики
        //! \details Поля таблицы:
        //!   - login: логин пользователя (внешний ключ к таблице Users)
        //!   - task: номер задачи (1, 2 или 3)
        //!   - correct: результат решения (1 - правильно, 0 - неправильно)
        //!   - points: начисленные очки (7 за правильный ответ, 0 за неправильный)

        if (!query.exec(createUsers)) {
            //! \brief Выполнение SQL-запроса создания таблицы Users
            //! \details При ошибке выполнения запроса управление переходит в блок if

            qDebug() << "Create Users table error:" << query.lastError().text();
            //! \brief Вывод сообщения об ошибке создания таблицы Users

            return;
            //! \brief Прерывание выполнения функции при ошибке создания таблицы
        }

        if (!query.exec(createStats)) {
            //! \brief Выполнение SQL-запроса создания таблицы Stats

            qDebug() << "Create Stats table error:" << query.lastError().text();
            //! \brief Вывод сообщения об ошибке создания таблицы Stats

            return;
            //! \brief Прерывание выполнения функции при ошибке создания таблицы
        }

        qDebug() << "Tables created successfully!";
        //! \brief Вывод сообщения об успешном создании таблиц в новой базе данных
    } else {
        qDebug() << "Using existing database:" << dbFileName;
        //! \brief Вывод сообщения об использовании существующей базы данных
    }

    qDebug() << "SQLite DB connected OK:" << dbFileName;
    //! \brief Вывод сообщения об успешном подключении к базе данных SQLite
}

static QString hashPassword(const QString &pw) {
    //! \brief Вспомогательная статическая функция для хэширования пароля
    //! \param pw Исходный пароль в открытом виде (строка)
    //! \return Строка с хэшем пароля в шестнадцатеричном формате (64 символа)

    return QString(QCryptographicHash::hash(
                       pw.toUtf8(), QCryptographicHash::Sha256).toHex());
    //! \brief Преобразование пароля в UTF-8, вычисление SHA-256 хэша
    //! \details toHex() преобразует бинарный хэш в шестнадцатеричную строку
}

bool DataBase::registerUser(const QString &login,
                            //! \brief Метод регистрации нового пользователя в системе
                            //! \param login Логин пользователя (уникальный идентификатор)
                            //! \param password Пароль пользователя (будет захэширован перед сохранением)
                            //! \param email Email пользователя для отправки кода подтверждения
                            //! \return true - регистрация успешна, false - ошибка или пользователь уже подтвержден

                            const QString &password,
                            const QString &email) {

    QString code = QString::number(
        QRandomGenerator::global()->bounded(100000, 1000000));
    //! \brief Генерация случайного 6-значного кода подтверждения
    //! \details Диапазон: от 100000 до 999999 (включительно), исключая коды с ведущими нулями

    if (userExists(login)) {
        //! \brief Проверка существования пользователя с таким логином

        QSqlQuery chk(db);
        //! \brief Создание объекта SQL-запроса для проверки статуса пользователя

        chk.prepare("SELECT confirmed FROM Users WHERE login = :login");
        //! \brief Подготовка SQL-запроса с именованным параметром :login
        //! \details Выбирает только поле confirmed для минимизации передаваемых данных

        chk.bindValue(":login", login);
        //! \brief Привязка значения логина к параметру :login в подготовленном запросе
        //! \details Использование bindValue защищает от SQL-инъекций

        if (chk.exec() && chk.next() && chk.value(0).toInt() == 1) {
            //! \brief Выполнение запроса и проверка: найден ли пользователь и подтвержден ли email
            //! \details chk.next() перемещает курсор на первую запись результата
            //!          value(0).toInt() получает значение поля confirmed

            qDebug() << "User already confirmed:" << login;
            //! \brief Вывод сообщения о том, что пользователь уже подтвержден

            return false;
            //! \brief Возврат false - регистрация невозможна для уже подтвержденного пользователя
        }

        QSqlQuery upd(db);
        //! \brief Создание объекта SQL-запроса для обновления данных пользователя

        upd.prepare("UPDATE Users SET password = :password, email = :email, "
                    "conf_code = :code WHERE login = :login");
        //! \brief Подготовка SQL-запроса обновления пароля, email и кода подтверждения

        upd.bindValue(":password", hashPassword(password));
        //! \brief Привязка хэшированного пароля к параметру :password

        upd.bindValue(":email",    email);
        //! \brief Привязка email к параметру :email

        upd.bindValue(":code",     code);
        //! \brief Привязка нового кода подтверждения к параметру :code

        upd.bindValue(":login",    login);
        //! \brief Привязка логина к параметру :login для условия WHERE

        if (!upd.exec()) {
            //! \brief Выполнение запроса обновления с проверкой успешности

            qDebug() << "Update user error:" << upd.lastError().text();
            //! \brief Вывод сообщения об ошибке выполнения запроса обновления

            return false;
            //! \brief Возврат false при ошибке выполнения
        }
        return true;
        //! \brief Успешное обновление данных для незавершенной регистрации
    }

    QSqlQuery ins(db);
    //! \brief Создание объекта SQL-запроса для вставки нового пользователя

    ins.prepare("INSERT INTO Users (login, password, email, confirmed, conf_code) "
                "VALUES (:login, :password, :email, 0, :code)");
    //! \brief Подготовка SQL-запроса вставки с начальным статусом confirmed = 0

    ins.bindValue(":login",    login);
    //! \brief Привязка логина нового пользователя к параметру :login

    ins.bindValue(":password", hashPassword(password));
    //! \brief Привязка хэшированного пароля к параметру :password

    ins.bindValue(":email",    email);
    //! \brief Привязка email к параметру :email

    ins.bindValue(":code",     code);
    //! \brief Привязка кода подтверждения к параметру :code

    if (!ins.exec()) {
        //! \brief Выполнение запроса вставки с проверкой успешности

        qDebug() << "Insert user error:" << ins.lastError().text();
        //! \brief Вывод сообщения об ошибке выполнения запроса вставки

        return false;
        //! \brief Возврат false при ошибке выполнения
    }
    return true;
    //! \brief Успешная регистрация нового пользователя
}

bool DataBase::authUser(const QString &login, const QString &password) {
    //! \brief Метод аутентификации пользователя по логину и паролю
    //! \param login Логин пользователя
    //! \param password Пароль пользователя (в открытом виде, будет захэширован для сравнения)
    //! \return true - аутентификация успешна (пароль верен и email подтвержден)

    QSqlQuery q(db);
    //! \brief Создание объекта SQL-запроса для выполнения аутентификации

    q.prepare("SELECT password, confirmed FROM Users WHERE login = :login");
    //! \brief Подготовка SQL-запроса выборки пароля и статуса подтверждения
    //! \details Выбираются только необходимые для аутентификации поля

    q.bindValue(":login", login);
    //! \brief Привязка логина к параметру :login запроса

    if (!q.exec() || !q.next()) return false;
    //! \brief Выполнение запроса и проверка: найден ли пользователь
    //! \details Если запрос не выполнен или записей нет - возвращается false

    return q.value(0).toString() == hashPassword(password)
           //! \brief Сравнение хэша пароля из БД с хэшем введенного пароля
           && q.value(1).toInt() == 1;
    //! \brief Проверка, что email пользователя подтвержден (confirmed = 1)
}

bool DataBase::confirmEmail(const QString &login, const QString &code) {
    //! \brief Метод подтверждения email пользователя с использованием кода
    //! \param login Логин пользователя
    //! \param code Код подтверждения, полученный пользователем по email
    //! \return true - код верный и email подтвержден, false - код неверный

    QSqlQuery q(db);
    //! \brief Создание объекта SQL-запроса для проверки кода

    q.prepare("SELECT conf_code FROM Users WHERE login = :login");
    //! \brief Подготовка SQL-запроса получения кода подтверждения для указанного логина

    q.bindValue(":login", login);
    //! \brief Привязка логина к параметру :login запроса

    if (!q.exec() || !q.next()) return false;
    //! \brief Выполнение запроса: если пользователь не найден - возврат false

    if (q.value(0).toString() != code) return false;
    //! \brief Сравнение переданного кода с сохраненным в базе данных

    QSqlQuery upd(db);
    //! \brief Создание объекта SQL-запроса для обновления статуса подтверждения

    upd.prepare("UPDATE Users SET confirmed = 1 WHERE login = :login");
    //! \brief Подготовка SQL-запроса установки флага confirmed в 1 (подтвержден)

    upd.bindValue(":login", login);
    //! \brief Привязка логина к параметру :login для условия WHERE

    return upd.exec();
    //! \brief Выполнение запроса обновления и возврат результата (true при успехе)
}

void DataBase::saveResult(const QString &login, int taskNum, bool correct) {
    //! \brief Метод сохранения результата решения задачи пользователем
    //! \param login Логин пользователя
    //! \param taskNum Номер задачи (1, 2 или 3)
    //! \param correct true - ответ правильный, false - ответ неправильный

    int pts = correct ? 7 : 0;
    //! \brief Определение количества начисляемых очков
    //! \details За правильный ответ начисляется 7 очков, за неправильный - 0

    QSqlQuery ins(db);
    //! \brief Создание объекта SQL-запроса для вставки результата

    ins.prepare("INSERT INTO Stats (login, task, correct, points) "
                "VALUES (:login, :task, :correct, :points)");
    //! \brief Подготовка SQL-запроса вставки записи о попытке решения задачи

    ins.bindValue(":login",   login);
    //! \brief Привязка логина пользователя к параметру :login

    ins.bindValue(":task",    taskNum);
    //! \brief Привязка номера задачи к параметру :task

    ins.bindValue(":correct", correct ? 1 : 0);
    //! \brief Привязка флага правильности (1 - правильно, 0 - неправильно)

    ins.bindValue(":points",  pts);
    //! \brief Привязка количества начисленных очков к параметру :points

    if (!ins.exec())
        //! \brief Выполнение запроса вставки с проверкой успешности

        qDebug() << "saveResult error:" << ins.lastError().text();
    //! \brief Вывод сообщения об ошибке при неудачном выполнении (без прерывания)
}

QString DataBase::getStats(const QString &login) {
    //! \brief Метод получения статистики пользователя
    //! \param login Логин пользователя
    //! \return Строка с статистикой в формате "правильные$всего&очки"

    QSqlQuery q(db);
    //! \brief Создание объекта SQL-запроса для получения статистики

    q.prepare("SELECT "
              "  SUM(correct) AS correct_count, "
              "  COUNT(*)     AS total_count, "
              "  SUM(points)  AS total_points "
              "FROM Stats WHERE login = :login");
    //! \brief Подготовка агрегирующего SQL-запроса
    //! \details SUM(correct) - количество правильных ответов
    //!          COUNT(*) - общее количество попыток
    //!          SUM(points) - сумма набранных очков

    q.bindValue(":login", login);
    //! \brief Привязка логина пользователя к параметру :login

    if (!q.exec() || !q.next()) return "0$0&0";
    //! \brief Выполнение запроса; при ошибке или отсутствии данных - возврат нулевой статистики

    int correct = q.value(0).toInt();
    //! \brief Получение количества правильных ответов из первого поля результата

    int total   = q.value(1).toInt();
    //! \brief Получение общего количества попыток из второго поля результата

    int points  = q.value(2).toInt();
    //! \brief Получение суммы очков из третьего поля результата

    return QString("%1$%2&%3").arg(correct).arg(total).arg(points);
    //! \brief Формирование строки статистики в заданном формате
    //! \details Пример: "5$10&35" = 5 правильных из 10 попыток, 35 очков
}

bool DataBase::userExists(const QString &login) {
    //! \brief Метод проверки существования пользователя в базе данных
    //! \param login Логин пользователя для проверки
    //! \return true - пользователь существует, false - не найден

    QSqlQuery q(db);
    //! \brief Создание объекта SQL-запроса для проверки существования

    q.prepare("SELECT 1 FROM Users WHERE login = :login");
    //! \brief Подготовка оптимизированного SQL-запроса (SELECT 1 - легковесная проверка)
    //! \details Возвращает 1 при наличии записи, иначе пустой результат

    q.bindValue(":login", login);
    //! \brief Привязка логина к параметру :login

    return q.exec() && q.next();
    //! \brief Выполнение запроса и проверка наличия хотя бы одной записи
}

QString DataBase::getConfirmationCode(const QString &login) {
    //! \brief Метод получения кода подтверждения для указанного пользователя
    //! \param login Логин пользователя
    //! \return Строка с кодом подтверждения (6 цифр) или пустая строка, если пользователь не найден

    QSqlQuery q(db);
    //! \brief Создание объекта SQL-запроса для получения кода

    q.prepare("SELECT conf_code FROM Users WHERE login = :login");
    //! \brief Подготовка SQL-запроса выборки кода подтверждения

    q.bindValue(":login", login);
    //! \brief Привязка логина пользователя к параметру :login

    if (!q.exec() || !q.next()) return QString();
    //! \brief Выполнение запроса; при ошибке или отсутствии пользователя - возврат пустой строки

    return q.value(0).toString();
    //! \brief Возврат кода подтверждения из первого поля результата запроса
}
