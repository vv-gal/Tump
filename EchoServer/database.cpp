/*!
 * \file database.cpp
 * \brief Реализация класса DataBase — работа с SQLite через Qt SQL.
 *
 * База данных хранится в файле D:/project_timp/project/db/math_server.db
 * Таблицы создаются только при первом запуске.
 */

#include "database.h"
//! \brief Подключение заголовочного файла с объявлением класса DataBase

#include <QCryptographicHash>
//! \brief Подключение класса для криптографического хэширования (SHA-256)

#include <QRandomGenerator>
//! \brief Подключение класса для генерации случайных чисел (код подтверждения)

#include <QFile>
//! \brief Подключение класса для работы с файлами

#include <QTextStream>
//! \brief Подключение класса для текстового ввода/вывода в файлы

#include <QDateTime>
//! \brief Подключение класса для работы с датой и временем

#include <QProcessEnvironment>
//! \brief Подключение класса для доступа к переменным окружения

#include <QDir>
//! \brief Подключение класса для работы с директориями и путями

#include <QFileInfo>
//! \brief Подключение класса для получения информации о файлах

DataBase *DataBase::p_instance = nullptr;
//! \brief Инициализация статического указателя на экземпляр класса нулевым значением

DataBaseDestroyer DataBase::destroyer;
//! \brief Инициализация статического объекта-уничтожителя Singleton

DataBaseDestroyer::~DataBaseDestroyer() { delete p_instance; }
//! \brief Деструктор уничтожителя
//! \details Удаляет единственный экземпляр DataBase при завершении программы
//!          delete p_instance автоматически вызовет деструктор DataBase

DataBase::DataBase() {}
//! \brief Приватный конструктор Singleton
//! \details Объект QSqlDatabase создается позже в методе init()
//!          Конструктор пустой, т.к. подключение к БД требует параметров

DataBase *DataBase::getInstance() {
    //! \brief Статический метод получения единственного экземпляра класса
    //! \return Указатель на единственный экземпляр DataBase

    if (!p_instance) {
        //! \brief Проверяет, существует ли уже экземпляр
        p_instance = new DataBase();
        //! \brief Создает новый экземпляр при первом вызове (ленивая инициализация)

        destroyer.initialize(p_instance);
        //! \brief Передает указатель на экземпляр уничтожителю для автоматического удаления
    }

    return p_instance;
    //! \brief Возвращает указатель на единственный экземпляр
}

void DataBase::init() {
    //! \brief Инициализирует подключение к базе данных SQLite
    //! \details Создает файл БД при его отсутствии, создает таблицы при первом запуске

    // Используем SQLite вместо PostgreSQL
    db = QSqlDatabase::addDatabase("QSQLITE");
    //! \brief Добавляет новое подключение к БД с драйвером SQLite

    // База данных в указанной папке
    QString dbFileName = "D:/project_timp/project/db/math_server.db";
    //! \brief Полный путь к файлу базы данных

    // Убедимся, что папка существует
    QDir dir;
    //! \brief Создает объект для работы с директориями

    QString dbPath = "D:/project_timp/project/db";
    //! \brief Путь к директории, где будет храниться файл БД

    if (!dir.exists(dbPath)) {
        //! \brief Проверяет, существует ли директория

        dir.mkpath(dbPath);
        //! \brief Создает все недостающие директории в пути

        qDebug() << "Created directory:" << dbPath;
        //! \brief Выводит отладочное сообщение о создании директории
    }

    db.setDatabaseName(dbFileName);
    //! \brief Устанавливает имя файла базы данных для подключения

    // Проверяем, существует ли файл базы данных
    bool dbExists = QFileInfo::exists(dbFileName);
    //! \brief Проверяет наличие файла БД на диске

    if (!db.open()) {
        //! \brief Попытка открыть соединение с БД

        qDebug() << "DB open error:" << db.lastError().text();
        //! \brief Выводит сообщение об ошибке открытия БД

        return;
        //! \brief Прерывает выполнение функции при ошибке
    }

    // Если база данных только что создана (файла не было) - создаём таблицы
    if (!dbExists) {
        //! \brief Проверяет, была ли БД только что создана

        qDebug() << "Creating new database with tables...";
        //! \brief Выводит отладочное сообщение о создании таблиц

        QSqlQuery query;
        //! \brief Создает объект для выполнения SQL-запросов

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
        //! \details Поля:
        //!   - login: уникальный логин пользователя (первичный ключ)
        //!   - password: хэш пароля (SHA-256, 64 символа)
        //!   - email: электронная почта пользователя
        //!   - confirmed: флаг подтверждения email (0 или 1)
        //!   - conf_code: код подтверждения (6 цифр)

        QString createStats = R"(
            CREATE TABLE Stats (
                login   VARCHAR(50) NOT NULL,
                task    INTEGER     NOT NULL,
                correct INTEGER     NOT NULL,
                points  INTEGER     NOT NULL
            )
        )";
        //! \brief SQL-запрос для создания таблицы статистики
        //! \details Поля:
        //!   - login: логин пользователя
        //!   - task: номер задачи (1, 2 или 3)
        //!   - correct: 1 - правильный ответ, 0 - неправильный
        //!   - points: количество набранных очков (7 за правильный ответ)

        if (!query.exec(createUsers)) {
            //! \brief Выполняет запрос создания таблицы Users

            qDebug() << "Create Users table error:" << query.lastError().text();
            //! \brief Выводит сообщение об ошибке при создании таблицы

            return;
            //! \brief Прерывает выполнение функции при ошибке
        }

        if (!query.exec(createStats)) {
            //! \brief Выполняет запрос создания таблицы Stats

            qDebug() << "Create Stats table error:" << query.lastError().text();
            //! \brief Выводит сообщение об ошибке при создании таблицы

            return;
            //! \brief Прерывает выполнение функции при ошибке
        }

        qDebug() << "Tables created successfully!";
        //! \brief Выводит сообщение об успешном создании таблиц
    } else {
        qDebug() << "Using existing database:" << dbFileName;
        //! \brief Выводит сообщение об использовании существующей БД
    }

    qDebug() << "SQLite DB connected OK:" << dbFileName;
    //! \brief Выводит сообщение об успешном подключении к БД
}

static QString hashPassword(const QString &pw) {
    //! \brief Вспомогательная функция для хэширования пароля
    //! \param pw Исходный пароль в открытом виде
    //! \return Строка с хэшем пароля в шестнадцатеричном формате
    //! \details Использует алгоритм SHA-256, возвращает 64 символа

    return QString(QCryptographicHash::hash(
                       pw.toUtf8(), QCryptographicHash::Sha256).toHex());
    //! \brief Преобразует пароль в UTF-8, вычисляет SHA-256 хэш,
    //!        конвертирует результат в шестнадцатеричную строку
}

/*!
 * \brief Регистрирует нового пользователя или обновляет код для незавершённой регистрации.
 * \param login Логин пользователя
 * \param password Пароль пользователя (будет захэширован)
 * \param email Email пользователя
 * \return true - регистрация успешна, false - ошибка или пользователь уже подтвержден
 */
bool DataBase::registerUser(const QString &login,
                            const QString &password,
                            const QString &email) {
    //! \brief Метод регистрации нового пользователя

    QString code = QString::number(
        QRandomGenerator::global()->bounded(100000, 1000000));
    //! \brief Генерирует случайный 6-значный код подтверждения
    //! \details Диапазон: от 100000 до 999999 включительно

    if (userExists(login)) {
        //! \brief Проверяет, существует ли пользователь с таким логином

        QSqlQuery chk(db);
        //! \brief Создает запрос для проверки статуса пользователя

        chk.prepare("SELECT confirmed FROM Users WHERE login = :login");
        //! \brief Подготавливает SQL-запрос с параметром

        chk.bindValue(":login", login);
        //! \brief Привязывает значение логина к параметру запроса

        if (chk.exec() && chk.next() && chk.value(0).toInt() == 1) {
            //! \brief Проверяет, подтвержден ли email пользователя

            qDebug() << "User already confirmed:" << login;
            //! \brief Выводит сообщение о существующем подтвержденном пользователе

            return false;
            //! \brief Возвращает false - регистрация невозможна
        }

        QSqlQuery upd(db);
        //! \brief Создает запрос для обновления данных пользователя

        upd.prepare("UPDATE Users SET password = :password, email = :email, "
                    "conf_code = :code WHERE login = :login");
        //! \brief Подготавливает запрос обновления пароля, email и кода

        upd.bindValue(":password", hashPassword(password));
        //! \brief Сохраняет хэш пароля

        upd.bindValue(":email",    email);
        //! \brief Обновляет email пользователя

        upd.bindValue(":code",     code);
        //! \brief Обновляет код подтверждения

        upd.bindValue(":login",    login);
        //! \brief Указывает логин для WHERE-условия

        if (!upd.exec()) {
            //! \brief Выполняет запрос обновления

            qDebug() << "Update user error:" << upd.lastError().text();
            //! \brief Выводит сообщение об ошибке

            return false;
            //! \brief Возвращает false при ошибке выполнения
        }
        return true;
        //! \brief Успешное обновление данных незавершенной регистрации
    }

    QSqlQuery ins(db);
    //! \brief Создает запрос для вставки нового пользователя

    ins.prepare("INSERT INTO Users (login, password, email, confirmed, conf_code) "
                "VALUES (:login, :password, :email, 0, :code)");
    //! \brief Подготавливает запрос вставки с начальным статусом confirmed = 0

    ins.bindValue(":login",    login);
    //! \brief Привязывает логин нового пользователя

    ins.bindValue(":password", hashPassword(password));
    //! \brief Привязывает хэш пароля

    ins.bindValue(":email",    email);
    //! \brief Привязывает email пользователя

    ins.bindValue(":code",     code);
    //! \brief Привязывает код подтверждения

    if (!ins.exec()) {
        //! \brief Выполняет запрос вставки

        qDebug() << "Insert user error:" << ins.lastError().text();
        //! \brief Выводит сообщение об ошибке

        return false;
        //! \brief Возвращает false при ошибке выполнения
    }
    return true;
    //! \brief Успешная регистрация нового пользователя
}

bool DataBase::authUser(const QString &login, const QString &password) {
    //! \brief Аутентифицирует пользователя по логину и паролю
    //! \param login Логин пользователя
    //! \param password Пароль пользователя (в открытом виде)
    //! \return true - аутентификация успешна, false - неверные данные или email не подтвержден

    QSqlQuery q(db);
    //! \brief Создает объект для выполнения SQL-запроса

    q.prepare("SELECT password, confirmed FROM Users WHERE login = :login");
    //! \brief Подготавливает запрос выборки пароля и статуса подтверждения

    q.bindValue(":login", login);
    //! \brief Привязывает логин к параметру запроса

    if (!q.exec() || !q.next()) return false;
    //! \brief Выполняет запрос, возвращает false если пользователь не найден

    return q.value(0).toString() == hashPassword(password)
           && q.value(1).toInt() == 1;
    //! \brief Проверяет совпадение хэша пароля и факт подтверждения email
}

bool DataBase::confirmEmail(const QString &login, const QString &code) {
    //! \brief Подтверждает email пользователя по коду
    //! \param login Логин пользователя
    //! \param code Код подтверждения
    //! \return true - код верный и email подтвержден, false - код неверный

    QSqlQuery q(db);
    //! \brief Создает запрос для проверки кода

    q.prepare("SELECT conf_code FROM Users WHERE login = :login");
    //! \brief Подготавливает запрос получения кода подтверждения

    q.bindValue(":login", login);
    //! \brief Привязывает логин

    if (!q.exec() || !q.next()) return false;
    //! \brief Возвращает false, если пользователь не найден

    if (q.value(0).toString() != code) return false;
    //! \brief Сравнивает введенный код с сохраненным в БД

    QSqlQuery upd(db);
    //! \brief Создает запрос для обновления статуса

    upd.prepare("UPDATE Users SET confirmed = 1 WHERE login = :login");
    //! \brief Подготавливает запрос установки флага confirmed

    upd.bindValue(":login", login);
    //! \brief Привязывает логин пользователя

    return upd.exec();
    //! \brief Выполняет обновление и возвращает результат
}

void DataBase::saveResult(const QString &login, int taskNum, bool correct) {
    //! \brief Сохраняет результат решения задачи
    //! \param login Логин пользователя
    //! \param taskNum Номер задачи (1, 2 или 3)
    //! \param correct true - ответ правильный, false - неправильный
    //! \details За правильный ответ начисляется 7 очков

    int pts = correct ? 7 : 0;
    //! \brief Определяет количество очков (7 за правильный ответ, 0 за неправильный)

    QSqlQuery ins(db);
    //! \brief Создает запрос для вставки статистики

    ins.prepare("INSERT INTO Stats (login, task, correct, points) "
                "VALUES (:login, :task, :correct, :points)");
    //! \brief Подготавливает запрос вставки записи о попытке

    ins.bindValue(":login",   login);
    //! \brief Привязывает логин пользователя

    ins.bindValue(":task",    taskNum);
    //! \brief Привязывает номер задачи

    ins.bindValue(":correct", correct ? 1 : 0);
    //! \brief Привязывает флаг правильности (1 или 0)

    ins.bindValue(":points",  pts);
    //! \brief Привязывает количество начисленных очков

    if (!ins.exec())
        //! \brief Выполняет запрос вставки

        qDebug() << "saveResult error:" << ins.lastError().text();
    //! \brief Выводит сообщение об ошибке при неудачном выполнении
}

QString DataBase::getStats(const QString &login) {
    //! \brief Получает статистику пользователя
    //! \param login Логин пользователя
    //! \return Строка в формате "правильные$всего&очки"
    //! \details Пример: "5$10&35" - 5 правильных из 10 попыток, 35 очков

    QSqlQuery q(db);
    //! \brief Создает запрос для получения статистики

    q.prepare("SELECT "
              "  SUM(correct) AS correct_count, "
              "  COUNT(*)     AS total_count, "
              "  SUM(points)  AS total_points "
              "FROM Stats WHERE login = :login");
    //! \brief Подготавливает агрегирующий запрос:
    //!   - SUM(correct) - количество правильных ответов
    //!   - COUNT(*) - общее количество попыток
    //!   - SUM(points) - сумма набранных очков

    q.bindValue(":login", login);
    //! \brief Привязывает логин пользователя

    if (!q.exec() || !q.next()) return "0$0&0";
    //! \brief Возвращает нулевую статистику при ошибке или отсутствии данных

    int correct = q.value(0).toInt();
    //! \brief Получает количество правильных ответов

    int total   = q.value(1).toInt();
    //! \brief Получает общее количество попыток

    int points  = q.value(2).toInt();
    //! \brief Получает сумму очков

    return QString("%1$%2&%3").arg(correct).arg(total).arg(points);
    //! \brief Формирует строку статистики в нужном формате
}

bool DataBase::userExists(const QString &login) {
    //! \brief Проверяет существование пользователя в БД
    //! \param login Логин пользователя
    //! \return true - пользователь существует, false - не найден

    QSqlQuery q(db);
    //! \brief Создает запрос для проверки существования

    q.prepare("SELECT 1 FROM Users WHERE login = :login");
    //! \brief Подготавливает запрос (SELECT 1 - легковесная проверка)

    q.bindValue(":login", login);
    //! \brief Привязывает логин

    return q.exec() && q.next();
    //! \brief Возвращает true, если запрос выполнен и есть хотя бы одна запись
}

QString DataBase::getConfirmationCode(const QString &login) {
    //! \brief Получает код подтверждения для пользователя
    //! \param login Логин пользователя
    //! \return Код подтверждения или пустую строку, если пользователь не найден

    QSqlQuery q(db);
    //! \brief Создает запрос для получения кода

    q.prepare("SELECT conf_code FROM Users WHERE login = :login");
    //! \brief Подготавливает запрос выборки кода подтверждения

    q.bindValue(":login", login);
    //! \brief Привязывает логин пользователя

    if (!q.exec() || !q.next()) return QString();
    //! \brief Возвращает пустую строку при ошибке или отсутствии пользователя

    return q.value(0).toString();
    //! \brief Возвращает код подтверждения из БД
}
