/*!
 * \file database.cpp
 * \brief Реализация класса DataBase — работа с SQLite через Qt SQL.
 */

#include "database.h"                           //!< Подключение заголовочного файла класса DataBase
#include <QCryptographicHash>                  //!< Подключение класса для криптографического хеширования (SHA-256)
#include <QRandomGenerator>                    //!< Подключение класса для генерации случайных чисел
#include <QFile>                               //!< Подключение класса для работы с файлами
#include <QTextStream>                         //!< Подключение класса для потокового чтения/записи текста
#include <QDateTime>                           //!< Подключение класса для работы с датой и временем

DataBase *DataBase::p_instance = nullptr;      //!< Инициализация статического указателя на единственный экземпляр класса
DataBaseDestroyer DataBase::destroyer;         //!< Создание статического объекта для автоматического уничтожения Singleton

// Деструктор определён здесь, где DataBase уже полностью объявлен
DataBaseDestroyer::~DataBaseDestroyer() { delete p_instance; }  //!< Деструктор уничтожителя: удаляет экземпляр DataBase

DataBase::DataBase() {}                        //!< Конструктор по умолчанию (приватный для Singleton)

DataBase *DataBase::getInstance() {            //!< Статический метод получения единственного экземпляра (Singleton)
    if (!p_instance) {                         //!< Проверка: существует ли уже экземпляр класса
        p_instance = new DataBase();           //!< Создание нового экземпляра DataBase
        destroyer.initialize(p_instance);      //!< Инициализация уничтожителя указателем на созданный экземпляр
    }
    return p_instance;                         //!< Возврат указателя на единственный экземпляр
}

void DataBase::init() {                        //!< Метод инициализации базы данных
    db = QSqlDatabase::addDatabase("QSQLITE"); //!< Добавление драйвера SQLite для работы с базой данных

    // Путь в /app/data/ — персистентный volume, данные не теряются при перезапуске
    db.setDatabaseName("server.db");           //!< Установка имени файла базы данных (создаётся в рабочей директории)

    if (!db.open()) {                          //!< Попытка открыть соединение с базой данных
        qDebug() << "DB open error:" << db.lastError().text();  //!< Вывод сообщения об ошибке открытия БД
        return;                                //!< Выход из метода при ошибке открытия
    }

    QSqlQuery query(db);                       //!< Создание объекта SQL-запроса для выполнения команд

    if (!query.exec("CREATE TABLE IF NOT EXISTS Users ("  //!< Выполнение SQL-запроса создания таблицы Users
                    "login    VARCHAR(50)  NOT NULL PRIMARY KEY,"  //!< Поле логина (первичный ключ)
                    "password VARCHAR(64)  NOT NULL,"              //!< Поле пароля (хранится хеш SHA-256)
                    "email    VARCHAR(100) NOT NULL,"              //!< Поле email пользователя
                    "confirmed INTEGER DEFAULT 0,"                 //!< Поле статуса подтверждения (0 - не подтверждён, 1 - подтверждён)
                    "conf_code VARCHAR(10) DEFAULT ''"             //!< Поле кода подтверждения (6 цифр)
                    ")")) {
        qDebug() << "Failed to create Users table:" << query.lastError().text();  //!< Вывод ошибки создания таблицы Users
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS Stats ("  //!< Выполнение SQL-запроса создания таблицы Stats
                    "login   VARCHAR(50) NOT NULL,"       //!< Поле логина пользователя
                    "task    INTEGER NOT NULL,"           //!< Поле номера задачи
                    "correct INTEGER NOT NULL,"           //!< Поле правильности ответа (0 - неверно, 1 - верно)
                    "points  INTEGER NOT NULL"            //!< Поле набранных очков (7 за верный ответ, 0 за неверный)
                    ")")) {
        qDebug() << "Failed to create Stats table:" << query.lastError().text();  //!< Вывод ошибки создания таблицы Stats
    }

    qDebug() << "DB initialized OK";           //!< Вывод сообщения об успешной инициализации базы данных
}

static QString hashPassword(const QString &pw) {  //!< Вспомогательная функция хеширования пароля
    return QString(QCryptographicHash::hash(pw.toUtf8(), QCryptographicHash::Sha256).toHex());  //!< Возврат шестнадцатеричной строки хеша SHA-256
}

/*!
 * \brief Регистрирует нового пользователя или обновляет код для незавершённой регистрации.
 *
 * ИСПРАВЛЕНО:
 * 1. Если пользователь существует и уже подтверждён — возвращаем false (логин занят).
 * 2. Если пользователь существует, но ещё НЕ подтверждён — генерируем новый код
 *    и обновляем запись. Это позволяет повторить регистрацию если письмо не пришло.
 * 3. Pending-файл больше НЕ пишется здесь. functionsforserver.cpp читает код
 *    через getConfirmationCode() и сам пишет файл — один код на двоих.
 */
bool DataBase::registerUser(const QString &login, const QString &password, const QString &email) {  //!< Метод регистрации пользователя
    // Генерируем новый код
    QString code = QString::number(QRandomGenerator::global()->bounded(100000, 1000000));  //!< Генерация случайного 6-значного кода подтверждения

    if (userExists(login)) {                     //!< Проверка: существует ли пользователь с таким логином
        // Проверяем — может уже подтверждён?
        QSqlQuery chk(db);                      //!< Создание объекта запроса для проверки статуса
        chk.prepare("SELECT confirmed FROM Users WHERE login = :login");  //!< Подготовка SQL-запроса для получения статуса подтверждения
        chk.bindValue(":login", login);         //!< Привязка значения логина к параметру запроса
        if (chk.exec() && chk.next() && chk.value(0).toInt() == 1) {  //!< Если запрос выполнен и пользователь подтверждён
            qDebug() << "User already confirmed:" << login;  //!< Вывод сообщения о подтверждённом пользователе
            return false;                       //!< Возврат false (логин занят, нельзя перерегистрироваться)
        }

        // Пользователь есть, но не подтверждён — обновляем пароль, email и код
        QSqlQuery upd(db);                      //!< Создание объекта запроса для обновления данных
        upd.prepare("UPDATE Users SET password = :password, email = :email, "  //!< Подготовка SQL-запроса обновления
                    "conf_code = :code WHERE login = :login");
        upd.bindValue(":password", hashPassword(password));  //!< Привязка хешированного пароля
        upd.bindValue(":email",    email);      //!< Привязка email пользователя
        upd.bindValue(":code",     code);       //!< Привязка нового кода подтверждения
        upd.bindValue(":login",    login);      //!< Привязка логина пользователя

        if (!upd.exec()) {                      //!< Проверка успешности выполнения запроса обновления
            qDebug() << "Failed to update unconfirmed user:" << upd.lastError().text();  //!< Вывод ошибки обновления
            return false;                       //!< Возврат false при ошибке обновления
        }

        qDebug() << "Re-registered unconfirmed user:" << login << "new code:" << code;  //!< Вывод сообщения о перерегистрации
        return true;                            //!< Возврат true (успешное обновление)
    }

    // Новый пользователь — вставляем
    QSqlQuery query(db);                        //!< Создание объекта запроса для вставки данных
    query.prepare("INSERT INTO Users(login, password, email, confirmed, conf_code) "  //!< Подготовка SQL-запроса вставки
                  "VALUES (:login, :password, :email, 0, :code)");
    query.bindValue(":login",    login);        //!< Привязка логина
    query.bindValue(":password", hashPassword(password));  //!< Привязка хешированного пароля
    query.bindValue(":email",    email);        //!< Привязка email
    query.bindValue(":code",     code);         //!< Привязка кода подтверждения

    if (!query.exec()) {                        //!< Проверка успешности выполнения запроса вставки
        qDebug() << "Failed to insert user:" << query.lastError().text();  //!< Вывод ошибки вставки
        return false;                           //!< Возврат false при ошибке вставки
    }

    qDebug() << "Registered new user:" << login << "code:" << code;  //!< Вывод сообщения об успешной регистрации
    return true;                                //!< Возврат true (успешная регистрация)
}

/*!
 * \brief Возвращает код подтверждения из БД.
 * Используется в functionsforserver.cpp, чтобы не генерировать второй код.
 */
QString DataBase::getConfirmationCode(const QString &login) {  //!< Метод получения кода подтверждения из БД
    QSqlQuery query(db);                        //!< Создание объекта запроса
    query.prepare("SELECT conf_code FROM Users WHERE login = :login");  //!< Подготовка SQL-запроса для получения кода
    query.bindValue(":login", login);           //!< Привязка логина пользователя

    if (!query.exec() || !query.next()) {       //!< Проверка выполнения запроса и наличия результата
        qDebug() << "getConfirmationCode: user not found:" << login;  //!< Вывод сообщения о ненайденном пользователе
        return QString();                       //!< Возврат пустой строки при ошибке
    }

    return query.value(0).toString();           //!< Возврат кода подтверждения из первого поля результата
}

bool DataBase::authUser(const QString &login, const QString &password) {  //!< Метод авторизации пользователя
    QSqlQuery query(db);                        //!< Создание объекта запроса
    query.prepare("SELECT password, confirmed FROM Users WHERE login = :login");  //!< Подготовка SQL-запроса для получения хеша и статуса
    query.bindValue(":login", login);           //!< Привязка логина пользователя

    if (!query.exec()) {                        //!< Проверка успешности выполнения запроса
        qDebug() << "Auth query failed:" << query.lastError().text();  //!< Вывод ошибки выполнения запроса
        return false;                           //!< Возврат false при ошибке запроса
    }
    if (!query.next()) {                        //!< Проверка наличия пользователя в БД
        qDebug() << "User not found:" << login; //!< Вывод сообщения об отсутствии пользователя
        return false;                           //!< Возврат false (пользователь не найден)
    }

    QString storedHash = query.value(0).toString();  //!< Получение сохранённого хеша пароля
    int confirmed      = query.value(1).toInt();     //!< Получение статуса подтверждения email

    bool passwordMatch = (storedHash == hashPassword(password));  //!< Сравнение хеша введённого пароля с сохранённым
    bool isConfirmed   = (confirmed == 1);          //!< Проверка, подтверждён ли email

    if (!passwordMatch)  qDebug() << "Wrong password for:" << login;  //!< Вывод сообщения о неверном пароле
    else if (!isConfirmed) qDebug() << "Email not confirmed for:" << login;  //!< Вывод сообщения о неподтверждённом email

    return passwordMatch && isConfirmed;            //!< Возврат true только если пароль верен И email подтверждён
}

bool DataBase::confirmEmail(const QString &login, const QString &code) {  //!< Метод подтверждения email по коду
    QSqlQuery query(db);                        //!< Создание объекта запроса
    query.prepare("SELECT conf_code FROM Users WHERE login = :login");  //!< Подготовка SQL-запроса для получения кода
    query.bindValue(":login", login);           //!< Привязка логина пользователя

    if (!query.exec() || !query.next()) {       //!< Проверка выполнения запроса и наличия результата
        qDebug() << "User not found for confirmation:" << login;  //!< Вывод сообщения о ненайденном пользователе
        return false;                           //!< Возврат false (пользователь не найден)
    }

    QString storedCode = query.value(0).toString();  //!< Получение сохранённого кода из БД

    if (storedCode != code) {                   //!< Проверка соответствия введённого кода сохранённому
        qDebug() << "Wrong confirmation code for:" << login  //!< Вывод сообщения о неверном коде
                 << "expected:" << storedCode << "got:" << code;
        return false;                           //!< Возврат false (код не совпадает)
    }

    QSqlQuery upd(db);                          //!< Создание объекта запроса для обновления
    upd.prepare("UPDATE Users SET confirmed = 1 WHERE login = :login");  //!< Подготовка SQL-запроса установки статуса
    upd.bindValue(":login", login);             //!< Привязка логина пользователя

    if (upd.exec()) {                           //!< Проверка успешности выполнения обновления
        qDebug() << "Email confirmed for:" << login;  //!< Вывод сообщения об успешном подтверждении
        return true;                            //!< Возврат true (успешное подтверждение)
    }

    qDebug() << "Failed to update confirmation:" << upd.lastError().text();  //!< Вывод ошибки обновления статуса
    return false;                               //!< Возврат false при ошибке обновления
}

void DataBase::saveResult(const QString &login, int taskNum, bool correct) {  //!< Метод сохранения результата решения задачи
    QSqlQuery query(db);                        //!< Создание объекта запроса
    query.prepare("INSERT INTO Stats(login, task, correct, points) "  //!< Подготовка SQL-запроса вставки результата
                  "VALUES (:login, :task, :correct, :points)");
    query.bindValue(":login",   login);         //!< Привязка логина пользователя
    query.bindValue(":task",    taskNum);       //!< Привязка номера задачи
    query.bindValue(":correct", correct ? 1 : 0);  //!< Привязка статуса правильности (1 - верно, 0 - неверно)
    query.bindValue(":points",  correct ? 7 : 0);  //!< Привязка набранных очков (7 за верный ответ, 0 за неверный)

    if (!query.exec())                          //!< Проверка успешности выполнения запроса
        qDebug() << "Failed to save result:" << query.lastError().text();  //!< Вывод ошибки сохранения
    else
        qDebug() << "Saved result for" << login << "task" << taskNum << "correct:" << correct;  //!< Вывод сообщения об успешном сохранении
}

QString DataBase::getStats(const QString &login) {  //!< Метод получения статистики пользователя
    QSqlQuery query(db);                        //!< Создание объекта запроса
    query.prepare("SELECT COUNT(*), SUM(correct), SUM(points) FROM Stats WHERE login = :login");  //!< Подготовка SQL-запроса агрегации статистики
    query.bindValue(":login", login);           //!< Привязка логина пользователя

    if (!query.exec() || !query.next())         //!< Проверка выполнения запроса и наличия результата
        return "stat&0$0&0";                   //!< Возврат строки с нулевой статистикой при ошибке

    int total   = query.value(0).toInt();       //!< Получение общего количества попыток
    int correct = query.value(1).toInt();       //!< Получение количества правильных ответов
    int points  = query.value(2).toInt();       //!< Получение суммы набранных очков

    return QString("stat&%1$%2&%3").arg(correct).arg(total).arg(points);  //!< Форматирование строки статистики в формате "stat&правильные$всего&очки"
}

bool DataBase::userExists(const QString &login) {  //!< Метод проверки существования пользователя
    QSqlQuery query(db);                        //!< Создание объекта запроса
    query.prepare("SELECT COUNT(*) FROM Users WHERE login = :login");  //!< Подготовка SQL-запроса подсчёта пользователей
    query.bindValue(":login", login);           //!< Привязка логина пользователя

    if (!query.exec()) {                        //!< Проверка успешности выполнения запроса
        qDebug() << "userExists query failed:" << query.lastError().text();  //!< Вывод ошибки выполнения запроса
        return false;                           //!< Возврат false при ошибке запроса
    }

    return query.next() && query.value(0).toInt() > 0;  //!< Возврат true если количество пользователей > 0
}
