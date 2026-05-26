/*!
 * \file functionsforserver.cpp
 * \brief Реализация функциональной части сервера.
 */
//! \brief Файл реализации математических функций и обработки команд сервера

#include "functionsforserver.h"
//! \brief Подключение заголовочного файла с объявлениями функций

#include "database.h"
//! \brief Подключение класса для работы с базой данных

#include "emailsender.h"
//! \brief Подключение класса для отправки email

#include <QStringList>
//! \brief Подключение класса для работы со списком строк

#include <QtMath>
//! \brief Подключение математических функций Qt

#include <QDebug>
//! \brief Подключение макросов для отладочного вывода

#include <QVector>
//! \brief Подключение класса для работы с динамическим массивом

#include <QRandomGenerator>
//! \brief Подключение класса для генерации случайных чисел

#include <algorithm>
//! \brief Подключение стандартных алгоритмов (сортировка, поиск)

// ─────────────────────────────────────────────────────────────
// ПАРАМЕТРЫ ФУНКЦИЙ
// ─────────────────────────────────────────────────────────────
//! \brief Блок статических глобальных переменных для хранения параметров функций

static double PARAM_A1 = 1.0;
//! \brief Коэффициент a для первой функции (логарифмической), по умолчанию 1.0

static double PARAM_B1 = 1.0;
//! \brief Коэффициент b для первой функции (логарифмической), по умолчанию 1.0

static double PARAM_C1 = 0.0;
//! \brief Коэффициент c для первой функции (логарифмической), по умолчанию 0.0

static double PARAM_A2 = 1.0;
//! \brief Коэффициент a для второй функции (кубической), по умолчанию 1.0

static double PARAM_B2 = -3.0;
//! \brief Коэффициент b для второй функции (кубической), по умолчанию -3.0

static double PARAM_C2 = 1.0;
//! \brief Коэффициент c для второй функции (кубической), по умолчанию 1.0

static double PARAM_A3 = 1.0;
//! \brief Коэффициент a для третьей функции (гиперболической), по умолчанию 1.0

static double PARAM_B3 = -2.0;
//! \brief Коэффициент b для третьей функции (гиперболической), по умолчанию -2.0

static double PARAM_C3 = -1.0;
//! \brief Коэффициент c для третьей функции (гиперболической), по умолчанию -1.0

// ─────────────────────────────────────────────────────────────
// ВЫЧИСЛЕНИЕ ФУНКЦИИ
// ─────────────────────────────────────────────────────────────
//! \brief Блок вычисления кусочной функции f(x)

double computeF(double x)
{
    //! \brief Вычисляет значение кусочной функции f(x) с текущими параметрами
    //! \param x Аргумент функции
    //! \return Значение функции или NaN

    // f1(x) = a·ln(x + b) + c,  -b < x < 0
    if (x > -PARAM_B1 && x < 0.0)
    {
        //! \brief Первая функция: логарифмическая (область определения: x > -b)

        double arg = x + PARAM_B1;
        //! \brief Вычисляет аргумент логарифма

        if (arg <= 0.0)
            //! \brief Проверяет, что аргумент логарифма положителен
            return qQNaN();
        //! \brief Возвращает NaN (Not a Number) для недопустимых значений

        double result = PARAM_A1 * qLn(arg) + PARAM_C1;
        //! \brief Вычисляет значение: a1 * ln(x + b1) + c1

        return result;
        //! \brief Возвращает результат
    }

    // f2(x) = a·x³ + b·x + c,  0 ≤ x ≤ 2
    if (x >= 0.0 && x <= 2.0)
    {
        //! \brief Вторая функция: кубическая (область определения: [0, 2])

        double result = PARAM_A2 * x * x * x + PARAM_B2 * x + PARAM_C2;
        //! \brief Вычисляет значение: a2 * x³ + b2 * x + c2

        return result;
        //! \brief Возвращает результат
    }

    // f3(x) = a/(x+b) + c,  x > 2
    if (x > 2.0)
    {
        //! \brief Третья функция: гиперболическая (область определения: x > 2)

        double denom = x + PARAM_B3;
        //! \brief Вычисляет знаменатель дроби (x + b3)

        if (qAbs(denom) < 1e-9)
            //! \brief Проверяет, что знаменатель не близок к нулю
            return qQNaN();
        //! \brief Возвращает NaN при делении на ноль

        double result = PARAM_A3 / denom + PARAM_C3;
        //! \brief Вычисляет значение: a3 / (x + b3) + c3

        return result;
        //! \brief Возвращает результат
    }

    return qQNaN();
    //! \brief Возвращает NaN, если x вне области определения всех трех функций
}

// ─────────────────────────────────────────────────────────────
// МЕТОД БИСЕКЦИИ
// ─────────────────────────────────────────────────────────────
//! \brief Блок реализации метода бисекции (деления пополам)

double bisect(double lo, double hi, double eps)
{
    //! \brief Находит корень уравнения f(x)=0 методом бисекции
    //! \param lo Левая граница интервала
    //! \param hi Правая граница интервала
    //! \param eps Требуемая точность (по умолчанию 1e-6)
    //! \return Приближенное значение корня или NaN

    double f_lo = computeF(lo);
    //! \brief Вычисляет значение функции на левой границе

    double f_hi = computeF(hi);
    //! \brief Вычисляет значение функции на правой границе

    if (qIsNaN(f_lo) || qIsNaN(f_hi))
    {
        //! \brief Проверяет, что значения функции не являются NaN
        qDebug() << "bisect: NaN at boundaries" << lo << hi;
        //! \brief Выводит сообщение о NaN на границах
        return qQNaN();
        //! \brief Возвращает NaN при ошибке
    }

    if (f_lo * f_hi > 0.0)
    {
        //! \brief Проверяет смену знака на интервале
        qDebug() << "bisect: no sign change" << lo << hi << f_lo << f_hi;
        //! \brief Выводит сообщение об отсутствии смены знака
        return qQNaN();
        //! \brief Возвращает NaN, если нет смены знака
    }

    for (int i = 0; i < 1000; ++i)
    {
        //! \brief Цикл бисекции (максимум 1000 итераций)

        double mid = (lo + hi) / 2.0;
        //! \brief Вычисляет середину интервала

        double f_mid = computeF(mid);
        //! \brief Вычисляет значение функции в середине

        if (qIsNaN(f_mid))
            //! \brief Проверяет, что значение не NaN
            return qQNaN();
        //! \brief Возвращает NaN при ошибке

        if (qAbs(f_mid) < eps)
            //! \brief Проверяет достижение требуемой точности
            return mid;
        //! \brief Возвращает найденный корень

        if (f_lo * f_mid <= 0.0)
        {
            //! \brief Корень находится в левой половине
            hi = mid;
            //! \brief Сдвигает правую границу к середине
            f_hi = f_mid;
            //! \brief Обновляет значение на правой границе
        }
        else
        {
            //! \brief Корень находится в правой половине
            lo = mid;
            //! \brief Сдвигает левую границу к середине
            f_lo = f_mid;
            //! \brief Обновляет значение на левой границе
        }
    }

    double result = (lo + hi) / 2.0;
    //! \brief Вычисляет приближенный корень после максимального числа итераций

    qDebug() << "bisect: max iterations reached, result =" << result;
    //! \brief Выводит предупреждение о достижении лимита итераций

    return result;
    //! \brief Возвращает приближенный корень
}

// ─────────────────────────────────────────────────────────────
// ПОИСК КОРНЕЙ
// ─────────────────────────────────────────────────────────────
//! \brief Блок сканирования интервала для поиска всех корней

QVector<double> findRoots(double start, double end, double step)
{
    //! \brief Находит все корни уравнения f(x)=0 на интервале методом сканирования
    //! \param start Начало интервала
    //! \param end Конец интервала
    //! \param step Шаг сканирования
    //! \return Вектор найденных корней

    QVector<double> roots;
    //! \brief Вектор для хранения найденных корней

    double prevX = start;
    //! \brief Предыдущая точка X

    double prevF = computeF(prevX);
    //! \brief Значение функции в предыдущей точке

    for (double x = start + step; x <= end + step/2; x += step)
    {
        //! \brief Цикл сканирования интервала с заданным шагом

        double curF = computeF(x);
        //! \brief Вычисляет значение функции в текущей точке

        if (!qIsNaN(prevF) && !qIsNaN(curF))
        {
            //! \brief Проверяет, что оба значения не NaN

            // Нашли смену знака
            if (prevF * curF <= 0.0)
            {
                //! \brief Обнаружена смена знака - есть корень
                double root = bisect(prevX, x, 1e-6);
                //! \brief Уточняет корень методом бисекции

                if (!qIsNaN(root))
                {
                    //! \brief Проверяет, что корень найден

                    // Проверяем на дубликаты
                    bool duplicate = false;
                    //! \brief Флаг для обнаружения дубликатов

                    for (double r : roots)
                    {
                        //! \brief Цикл проверки существующих корней
                        if (qAbs(r - root) < 1e-4)
                        {
                            //! \brief Если корень уже есть (с точностью 1e-4)
                            duplicate = true;
                            //! \brief Устанавливает флаг дубликата
                            break;
                            //! \brief Прерывает цикл
                        }
                    }

                    if (!duplicate)
                    {
                        //! \brief Если корень уникальный
                        roots.push_back(root);
                        //! \brief Добавляет корень в вектор
                        qDebug() << "Found root:" << root;
                        //! \brief Выводит сообщение о найденном корне
                    }
                }
            }
        }

        prevX = x;
        //! \brief Обновляет предыдущую точку

        prevF = curF;
        //! \brief Обновляет предыдущее значение функции
    }

    return roots;
    //! \brief Возвращает вектор найденных корней
}

// ─────────────────────────────────────────────────────────────
// ПОЛУЧЕНИЕ КОРНЯ ДЛЯ ЗАДАНИЯ
// ─────────────────────────────────────────────────────────────
//! \brief Блок получения правильного корня для конкретной задачи

double getCorrectRoot(int taskNumber, int variant)
{
    //! \brief Возвращает правильный корень для указанной задачи и варианта
    //! \param taskNumber Номер задачи (1-3)
    //! \param variant Номер варианта (для задачи 2: 1 или 2)
    //! \return Значение корня или NaN

    QVector<double> roots;
    //! \brief Вектор для хранения корней

    switch (taskNumber)
    {
        //! \brief Выбор интервала поиска в зависимости от номера задачи

    case 1:
        // Корень на интервале [0, 2] для f1 (x=0)
        // Но f1(0) = ln(1) = 0, это и есть корень
        roots = findRoots(-0.5, 0.5, 0.01);
        //! \brief Ищет корни вокруг 0 для логарифмической функции
        break;

    case 2:
        // Область кубической функции f2(x) = x³ - 3x + 1
        // Корни: ≈0.347, ≈1.532, и третий отрицательный (≈-1.879)
        roots = findRoots(0.0, 2.0, 0.01);
        //! \brief Ищет корни кубической функции на интервале [0, 2]
        break;

    case 3:
        // Область дробной функции f3(x) = 1/(x-2) - 1
        // Корень: x = 3
        roots = findRoots(2.01, 10.0, 0.01);
        //! \brief Ищет корни гиперболической функции на интервале (2, 10]
        break;

    default:
        qDebug() << "Unknown task number:" << taskNumber;
        //! \brief Выводит сообщение о неизвестном номере задачи
        return qQNaN();
        //! \brief Возвращает NaN
    }

    if (roots.isEmpty())
    {
        //! \brief Проверяет, найдены ли корни
        qDebug() << "No roots found for task" << taskNumber;
        //! \brief Выводит сообщение об отсутствии корней
        return qQNaN();
        //! \brief Возвращает NaN
    }

    // Для задачи 2 может быть несколько корней, выбираем по варианту
    if (taskNumber == 2)
    {
        //! \brief Обработка двух вариантов для кубической функции
        // Ожидаемые корни: ~0.347 и ~1.532
        // Сортируем корни для надёжности
        std::sort(roots.begin(), roots.end());
        //! \brief Сортирует корни по возрастанию

        if (variant == 1 && roots.size() >= 1)
        {
            //! \brief Первый вариант: меньший корень
            qDebug() << "Task 2, variant 1, root:" << roots[0];
            //! \brief Выводит выбранный корень
            return roots[0];
            //! \brief Возвращает первый (меньший) корень
        }
        else if (variant == 2 && roots.size() >= 2)
        {
            //! \brief Второй вариант: больший корень
            qDebug() << "Task 2, variant 2, root:" << roots[1];
            //! \brief Выводит выбранный корень
            return roots[1];
            //! \brief Возвращает второй (больший) корень
        }
        else
        {
            //! \brief Если недостаточно корней для выбранного варианта
            qDebug() << "Not enough roots for task 2 variant" << variant;
            //! \brief Выводит предупреждение
            return roots[0]; // fallback
            //! \brief Возвращает первый корень как запасной вариант
        }
    }

    qDebug() << "Task" << taskNumber << "root:" << roots[0];
    //! \brief Выводит найденный корень для задач 1 и 3

    return roots[0];
    //! \brief Возвращает первый (единственный) корень
}

// ─────────────────────────────────────────────────────────────
// ПРОВЕРКА ОТВЕТА
// ─────────────────────────────────────────────────────────────
//! \brief Блок проверки ответа пользователя

bool checkAnswer(int taskNumber, int variant, const QString &answer)
{
    //! \brief Проверяет правильность ответа пользователя
    //! \param taskNumber Номер задачи
    //! \param variant Номер варианта
    //! \param answer Ответ пользователя в виде строки
    //! \return true - ответ верный, false - неверный

    bool ok = false;
    //! \brief Флаг успешного преобразования строки в число

    double ans = answer.toDouble(&ok);
    //! \brief Преобразует строку в число с плавающей точкой

    if (!ok)
    {
        //! \brief Проверяет успешность преобразования
        qDebug() << "Invalid answer format:" << answer;
        //! \brief Выводит сообщение о неверном формате
        return false;
        //! \brief Возвращает false при ошибке формата
    }

    double correct = getCorrectRoot(taskNumber, variant);
    //! \brief Получает правильный корень для задачи

    if (qIsNaN(correct))
    {
        //! \brief Проверяет, что правильный корень найден
        qDebug() << "No correct root for task" << taskNumber;
        //! \brief Выводит сообщение об отсутствии корня
        return false;
        //! \brief Возвращает false
    }

    bool result = qAbs(ans - correct) < 0.01;
    //! \brief Сравнивает ответ с правильным корнем с точностью 0.01

    qDebug() << "Check answer: task=" << taskNumber
             << "variant=" << variant
             << "answer=" << ans
             << "correct=" << correct
             << "result=" << result;
    //! \brief Выводит подробную информацию о проверке

    return result;
    //! \brief Возвращает результат сравнения
}

// ─────────────────────────────────────────────────────────────
// УСТАНОВКА ПАРАМЕТРОВ
// ─────────────────────────────────────────────────────────────
//! \brief Блок установки параметров функций

bool setFunctionParams(const QStringList &params)
{
    //! \brief Устанавливает параметры для всех трех функций
    //! \param params Список строк с параметрами [cmd, a1, b1, c1, a2, b2, c2, a3, b3, c3]
    //! \return true - параметры успешно установлены, false - ошибка парсинга

    if (params.size() < 10)
    {
        //! \brief Проверяет количество параметров (должно быть 10: команда + 9 параметров)
        qDebug() << "setFunctionParams: expected 10 params, got" << params.size();
        //! \brief Выводит сообщение о неверном количестве параметров
        return false;
        //! \brief Возвращает false
    }

    bool ok = true;
    //! \brief Флаг успешного парсинга всех параметров

    auto parseDouble = [&](const QString &str, double &target, const QString &name)
    {
        //! \brief Лямбда-функция для парсинга double с отладочным выводом

        bool localOk = false;
        //! \brief Локальный флаг успешности парсинга

        double val = str.toDouble(&localOk);
        //! \brief Преобразует строку в число

        if (!localOk)
        {
            //! \brief Если не удалось преобразовать
            qDebug() << "Failed to parse" << name << ":" << str;
            //! \brief Выводит сообщение об ошибке
            ok = false;
            //! \brief Устанавливает глобальный флаг ошибки
            return;
            //! \brief Прерывает выполнение лямбды
        }

        target = val;
        //! \brief Устанавливает значение целевой переменной

        qDebug() << "Set" << name << "=" << val;
        //! \brief Выводит информацию об установленном параметре
    };

    parseDouble(params[1], PARAM_A1, "PARAM_A1");
    //! \brief Парсит параметр a1 первой функции
    parseDouble(params[2], PARAM_B1, "PARAM_B1");
    //! \brief Парсит параметр b1 первой функции
    parseDouble(params[3], PARAM_C1, "PARAM_C1");
    //! \brief Парсит параметр c1 первой функции

    parseDouble(params[4], PARAM_A2, "PARAM_A2");
    //! \brief Парсит параметр a2 второй функции
    parseDouble(params[5], PARAM_B2, "PARAM_B2");
    //! \brief Парсит параметр b2 второй функции
    parseDouble(params[6], PARAM_C2, "PARAM_C2");
    //! \brief Парсит параметр c2 второй функции

    parseDouble(params[7], PARAM_A3, "PARAM_A3");
    //! \brief Парсит параметр a3 третьей функции
    parseDouble(params[8], PARAM_B3, "PARAM_B3");
    //! \brief Парсит параметр b3 третьей функции
    parseDouble(params[9], PARAM_C3, "PARAM_C3");
    //! \brief Парсит параметр c3 третьей функции

    if (!ok)
        //! \brief Проверяет флаг успешности парсинга
        return false;
    //! \brief Возвращает false при ошибке

    qDebug() << "Function parameters updated successfully";
    //! \brief Выводит сообщение об успешном обновлении параметров

    qDebug() << "f1:" << PARAM_A1 << "*ln(x+" << PARAM_B1 << ")+" << PARAM_C1;
    //! \brief Выводит формулу первой функции

    qDebug() << "f2:" << PARAM_A2 << "*x^3 +" << PARAM_B2 << "*x +" << PARAM_C2;
    //! \brief Выводит формулу второй функции

    qDebug() << "f3:" << PARAM_A3 << "/(x+" << PARAM_B3 << ")+" << PARAM_C3;
    //! \brief Выводит формулу третьей функции

    return true;
    //! \brief Возвращает true при успехе
}

// ─────────────────────────────────────────────────────────────
// ПОЛУЧЕНИЕ ПАРАМЕТРОВ
// ─────────────────────────────────────────────────────────────
//! \brief Блок получения текущих параметров функций

QString getFunctionParams()
{
    //! \brief Возвращает строку с текущими параметрами всех функций
    //! \return Строка в формате "params&a1&b1&c1&a2&b2&c2&a3&b3&c3"

    return QString("params&%1&%2&%3&%4&%5&%6&%7&%8&%9")
        //! \brief Форматирует строку с параметрами
        .arg(PARAM_A1)
        .arg(PARAM_B1)
        .arg(PARAM_C1)
        .arg(PARAM_A2)
        .arg(PARAM_B2)
        .arg(PARAM_C2)
        .arg(PARAM_A3)
        .arg(PARAM_B3)
        .arg(PARAM_C3);
    //! \brief Подставляет все 9 параметров в строку
}

// ─────────────────────────────────────────────────────────────
// ПАРСИНГ ПРОТОКОЛА
// ─────────────────────────────────────────────────────────────
//! \brief Блок парсинга и обработки команд клиента

QString parsing(const QString &message)
{
    //! \brief Разбирает входящее сообщение клиента и формирует ответ
    //! \param message Входящее сообщение в формате "команда&параметры"
    //! \return Строка-ответ сервера

    if (message.isEmpty())
    {
        //! \brief Проверяет, что сообщение не пустое
        qDebug() << "Empty message received";
        //! \brief Выводит сообщение об ошибке
        return "error";
        //! \brief Возвращает "error"
    }

    QStringList parts = message.trimmed().split("&");
    //! \brief Разбивает сообщение на части по разделителю "&"

    if (parts.isEmpty())
    {
        //! \brief Проверяет, что после разбиения есть элементы
        qDebug() << "Empty parts after split";
        //! \brief Выводит сообщение об ошибке
        return "error";
        //! \brief Возвращает "error"
    }

    QString cmd = parts[0].toLower();
    //! \brief Извлекает команду и приводит к нижнему регистру

    qDebug() << "Processing command:" << cmd << "parts count:" << parts.size();
    //! \brief Выводит отладочную информацию о команде

    DataBase *db = DataBase::getInstance();
    //! \brief Получает экземпляр базы данных (Singleton)

    // ───────────────── auth ─────────────────
    if (cmd == "auth")
    {
        //! \brief Обработка команды авторизации

        if (parts.size() < 3)
        {
            //! \brief Проверяет наличие логина и пароля
            qDebug() << "auth: insufficient params";
            //! \brief Выводит сообщение об ошибке
            return "auth-";
            //! \brief Возвращает "auth-" (ошибка)
        }

        QString login = parts[1];
        //! \brief Извлекает логин

        QString password = parts[2];
        //! \brief Извлекает пароль

        qDebug() << "Auth request for:" << login;
        //! \brief Выводит информацию о запросе

        if (db->authUser(login, password))
        {
            //! \brief Проверяет учетные данные в БД
            qDebug() << "Auth success for:" << login;
            //! \brief Выводит сообщение об успехе
            return "auth+&" + login;
            //! \brief Возвращает "auth+&[логин]" (успех)
        }

        qDebug() << "Auth failed for:" << login;
        //! \brief Выводит сообщение о неудаче
        return "auth-";
        //! \brief Возвращает "auth-" (ошибка)
    }

    // ───────────────── reg ─────────────────
    if (cmd == "reg")
    {
        //! \brief Обработка команды регистрации

        if (parts.size() < 4)
        {
            //! \brief Проверяет наличие логина, пароля и email
            qDebug() << "reg: insufficient params";
            //! \brief Выводит сообщение об ошибке
            return "reg-";
            //! \brief Возвращает "reg-" (ошибка)
        }

        QString login = parts[1];
        //! \brief Извлекает логин

        QString password = parts[2];
        //! \brief Извлекает пароль

        QString email = parts[3];
        //! \brief Извлекает email

        qDebug() << "Registration request for:" << login << email;
        //! \brief Выводит информацию о запросе

        if (db->registerUser(login, password, email))
        {
            //! \brief Регистрирует пользователя в БД

            // Получаем код подтверждения из БД (уже сохранён в registerUser)
            QString code = db->getConfirmationCode(login);
            //! \brief Получает код подтверждения, сохранённый в БД при регистрации

            if (code.isEmpty())
            {
                qDebug() << "Failed to get confirmation code from DB for" << login;
                return "reg-";
            }

            qDebug() << "Got confirmation code for" << login << ":" << code;

            // Отправляем email с кодом из БД
            if (EmailSender::sendConfirmationCode(email, login, code))
            {
                //! \brief Отправляет письмо с кодом
                qDebug() << "Confirmation email sent to" << email;
                return "reg+&" + login;
            }
            else
            {
                //! \brief Обработка ошибки отправки email
                qDebug() << "Failed to send email to" << email;
                return "reg-";
            }
        }

        qDebug() << "Registration failed for:" << login;
        //! \brief Выводит сообщение о неудаче регистрации
        return "reg-";
        //! \brief Возвращает "reg-" (ошибка)
    }

    // ───────────────── confirm ─────────────────
    if (cmd == "confirm")
    {
        //! \brief Обработка команды подтверждения email

        if (parts.size() < 3)
        {
            //! \brief Проверяет наличие логина и кода
            qDebug() << "confirm: insufficient params";
            //! \brief Выводит сообщение об ошибке
            return "confirm-";
            //! \brief Возвращает "confirm-" (ошибка)
        }

        QString login = parts[1];
        //! \brief Извлекает логин

        QString code = parts[2];
        //! \brief Извлекает код подтверждения

        qDebug() << "Confirm request for:" << login << "code:" << code;
        //! \brief Выводит информацию о запросе

        // Проверяем код непосредственно в БД
        if (db->confirmEmail(login, code))
        {
            qDebug() << "Email confirmed for" << login;
            return "confirm+";
        }

        qDebug() << "Failed to confirm email for" << login;
        return "confirm-";
    }

    // ───────────────── stat ─────────────────
    if (cmd == "stat")
    {
        //! \brief Обработка команды получения статистики

        if (parts.size() < 2)
        {
            //! \brief Проверяет наличие логина
            qDebug() << "stat: insufficient params";
            //! \brief Выводит сообщение об ошибке
            return "stat&0&0&0";
            //! \brief Возвращает нулевую статистику
        }

        QString login = parts[1];
        //! \brief Извлекает логин

        qDebug() << "Stat request for:" << login;
        //! \brief Выводит информацию о запросе

        return db->getStats(login);
        //! \brief Возвращает статистику из БД
    }

    // ───────────────── check ─────────────────
    if (cmd == "check")
    {
        //! \brief Обработка команды проверки ответа

        // Поддерживаем оба формата:
        // 1. check&task&variant&answer (4 параметра)
        // 2. check&login&task&variant&answer (5 параметров)

        if (parts.size() < 4)
        {
            //! \brief Проверяет минимальное количество параметров
            qDebug() << "check: insufficient params, got" << parts.size();
            //! \brief Выводит сообщение об ошибке
            return "check-";
            //! \brief Возвращает "check-" (ошибка)
        }

        QString login;
        //! \brief Логин пользователя (может быть "unknown")

        int taskNum;
        //! \brief Номер задачи

        int variant;
        //! \brief Номер варианта

        QString answer;
        //! \brief Ответ пользователя

        if (parts.size() == 5)
        {
            //! \brief Формат с логином (5 параметров)
            login = parts[1];
            //! \brief Извлекает логин

            taskNum = parts[2].toInt();
            //! \brief Извлекает номер задачи

            variant = parts[3].toInt();
            //! \brief Извлекает номер варианта

            answer = parts[4];
            //! \brief Извлекает ответ

            qDebug() << "Check request (with login):" << login
                     << "task=" << taskNum
                     << "variant=" << variant
                     << "answer=" << answer;
            //! \brief Выводит информацию о запросе
        }
        else
        {
            //! \brief Формат без логина (4 параметра)
            taskNum = parts[1].toInt();
            //! \brief Извлекает номер задачи

            variant = parts[2].toInt();
            //! \brief Извлекает номер варианта

            answer = parts[3];
            //! \brief Извлекает ответ

            login = "unknown";
            //! \brief Устанавливает логин как "unknown"

            qDebug() << "Check request (without login): task=" << taskNum
                     << "variant=" << variant
                     << "answer=" << answer;
            //! \brief Выводит информацию о запросе
        }

        bool correct = checkAnswer(taskNum, variant, answer);
        //! \brief Проверяет правильность ответа

        // Сохраняем результат в БД только если есть логин
        if (login != "unknown")
        {
            //! \brief Если логин известен
            db->saveResult(login, taskNum, correct);
            //! \brief Сохраняет результат в БД

            qDebug() << "Result saved for" << login;
            //! \brief Выводит сообщение о сохранении
        }
        else
        {
            //! \brief Если логин не указан
            qDebug() << "Skipping save to DB (no login provided)";
            //! \brief Выводит сообщение о пропуске сохранения
        }

        qDebug() << "Check result:" << (correct ? "CORRECT" : "WRONG");
        //! \brief Выводит результат проверки

        return correct ? "check+" : "check-";
        //! \brief Возвращает "check+" при правильном ответе, иначе "check-"
    }

    // ───────────────── getparams ─────────────────
    if (cmd == "getparams")
    {
        //! \brief Обработка команды получения параметров функций
        qDebug() << "Get params request";
        //! \brief Выводит информацию о запросе
        return getFunctionParams();
        //! \brief Возвращает текущие параметры функций
    }

    // ───────────────── setparams ─────────────────
    if (cmd == "setparams")
    {
        //! \brief Обработка команды установки параметров функций
        qDebug() << "Set params request with" << (parts.size() - 1) << "parameters";
        //! \brief Выводит информацию о запросе

        if (setFunctionParams(parts))
        {
            //! \brief Устанавливает новые параметры
            qDebug() << "Params updated successfully";
            //! \brief Выводит сообщение об успехе
            return "params+";
            //! \brief Возвращает "params+" (успех)
        }

        qDebug() << "Failed to update params";
        //! \brief Выводит сообщение о неудаче
        return "params-";
        //! \brief Возвращает "params-" (ошибка)
    }

    qDebug() << "Unknown command:" << cmd;
    //! \brief Выводит сообщение о неизвестной команде

    return "error";
    //! \brief Возвращает "error"
}
