/*!
 * \file tst_funcforserver_test.cpp
 * \brief Модульные тесты функциональной части сервера.
 *
 * Тестирует: parsing(), computeF(), bisect(), checkAnswer().
 */
//! \brief Файл с модульными тестами для функций сервера

#include <QtTest>
//! \brief Подключение заголовочного файла фреймворка Qt Test

#include <QCoreApplication>
//! \brief Подключение класса консольного приложения Qt

#include <QDebug>
//! \brief Подключение макросов для отладочного вывода

#include "../EchoServer/functionsforserver.h"
//! \brief Подключение заголовочного файла с функциями сервера

#include "../EchoServer/database.h"
//! \brief Подключение заголовочного файла с классом базы данных

#include <QtMath>
//! \brief Подключение математических функций Qt

#include <cmath>
//! \brief Подключение стандартных математических функций C++

/*!
 * \class FuncForServer_Test
 * \brief Набор unit-тестов для functionsforserver.h.
 */
class FuncForServer_Test : public QObject {
    //! \brief Класс с набором модульных тестов для функций сервера
    Q_OBJECT
    //! \brief Макрос Qt для поддержки сигналов и слотов

public:
    FuncForServer_Test();
    //! \brief Конструктор класса тестов

    ~FuncForServer_Test();
    //! \brief Деструктор класса тестов

private slots:
    //! \brief Секция приватных слотов (каждый слот - отдельный тест)

    void initTestCase();   ///< Инициализация перед всеми тестами
    //! \brief Выполняется один раз перед запуском всех тестов

    void cleanupTestCase();///< Очистка после всех тестов
    //! \brief Выполняется один раз после завершения всех тестов

    // Тесты парсинга (не изменяют состояние)
    void test_parsing_invalid();
    //! \brief Тест обработки невалидных команд

    void test_parsing_auth();
    //! \brief Тест команды авторизации

    void test_parsing_auth_wrong_format();
    //! \brief Тест команды авторизации с неправильным форматом

    void test_parsing_check_format();
    //! \brief Тест команды проверки с неправильным форматом

    void test_parsing_check_correct();
    //! \brief Тест команды проверки с правильными ответами

    void test_parsing_getparams();
    //! \brief Тест команды получения параметров

    // Тесты, которые меняют параметры - запускаем отдельно и сбрасываем
    void test_parsing_setparams_data();
    //! \brief Предоставляет данные для теста установки параметров

    void test_parsing_setparams();
    //! \brief Тест команды установки параметров

    // Тесты математики (должны быть после сброса параметров)
    void test_computeF_segment1();
    //! \brief Тест вычисления функции на первом сегменте (логарифмическом)

    void test_computeF_segment2();
    //! \brief Тест вычисления функции на втором сегменте (кубическом)

    void test_computeF_segment3();
    //! \brief Тест вычисления функции на третьем сегменте (гиперболическом)

    void test_computeF_boundary();
    //! \brief Тест вычисления функции на границах сегментов

    void test_computeF_invalid();
    //! \brief Тест вычисления функции с недопустимыми значениями

    // Тест bisect
    void test_bisect_segment2_root1();
    //! \brief Тест поиска первого корня на втором сегменте

    void test_bisect_segment2_root2();
    //! \brief Тест поиска второго корня на втором сегменте

    void test_bisect_segment3();
    //! \brief Тест поиска корня на третьем сегменте

    // Тесты checkAnswer
    void test_checkAnswer_task1_correct();
    //! \brief Тест проверки правильных ответов для задачи 1

    void test_checkAnswer_task1_wrong();
    //! \brief Тест проверки неправильных ответов для задачи 1

    void test_checkAnswer_task2_variant1();
    //! \brief Тест проверки ответов для задачи 2 (вариант 1)

    void test_checkAnswer_task2_variant2();
    //! \brief Тест проверки ответов для задачи 2 (вариант 2)

    void test_checkAnswer_task3_correct();
    //! \brief Тест проверки правильных ответов для задачи 3

    void test_checkAnswer_invalid_task();
    //! \brief Тест проверки ответов для несуществующих задач

private:
    void resetDefaultParams();  ///< Сброс параметров к значениям по умолчанию
    //! \brief Вспомогательный метод для сброса параметров функций к стандартным
};

FuncForServer_Test::FuncForServer_Test() {}
//! \brief Конструктор класса тестов (пустой)

FuncForServer_Test::~FuncForServer_Test() {}
//! \brief Деструктор класса тестов (пустой)

void FuncForServer_Test::resetDefaultParams()
{
    //! \brief Сбрасывает параметры функций к значениям по умолчанию
    // Сбрасываем параметры к стандартным значениям
    QStringList defaultParams;
    //! \brief Создает список строк для параметров

    defaultParams << "setparams" << "1" << "1" << "0" << "1" << "-3" << "1" << "1" << "-2" << "-1";
    //! \brief Заполняет список стандартными параметрами

    setFunctionParams(defaultParams);
    //! \brief Вызывает функцию установки параметров

    qDebug() << "Parameters reset to defaults";
    //! \brief Выводит сообщение о сбросе параметров
}

void FuncForServer_Test::initTestCase() {
    //! \brief Инициализация перед запуском всех тестов
    // Создаем QCoreApplication для работы с БД
    static int argc = 1;
    //! \brief Статическая переменная с количеством аргументов командной строки

    static char *argv[] = { const_cast<char*>("Tests"), nullptr };
    //! \brief Статический массив с аргументами командной строки

    if (!QCoreApplication::instance()) {
        //! \brief Проверяет, существует ли экземпляр QCoreApplication
        new QCoreApplication(argc, argv);
        //! \brief Создает экземпляр QCoreApplication для тестов

        qDebug() << "QCoreApplication created for tests";
        //! \brief Выводит сообщение о создании приложения
    }

    // Инициализируем БД
    DataBase *db = DataBase::getInstance();
    //! \brief Получает экземпляр базы данных

    if (db) {
        //! \brief Проверяет, что экземпляр получен
        db->init();
        //! \brief Инициализирует базу данных

        qDebug() << "DB initialized OK";
        //! \brief Выводит сообщение об успешной инициализации БД
    }

    // Сбрасываем параметры функций к стандартным
    resetDefaultParams();
    //! \brief Сбрасывает параметры функций

    qDebug() << "Database initialized for tests";
    //! \brief Выводит сообщение о готовности БД к тестам
}

void FuncForServer_Test::cleanupTestCase() {
    //! \brief Очистка после всех тестов
    // Удаляем файл БД после тестов
    QFile::remove("server.db");
    //! \brief Удаляет файл базы данных

    qDebug() << "Test database removed";
    //! \brief Выводит сообщение об удалении БД
}

// ── Тесты parsing ─────────────────────────────────────────────────────────────
//! \brief Раздел тестов функции парсинга команд

void FuncForServer_Test::test_parsing_invalid() {
    //! \brief Тестирует обработку невалидных команд

    QVERIFY2(parsing("ewqeewqewq") == "error",
             "Неизвестная команда должна возвращать 'error'");
    //! \brief Проверяет, что неизвестная команда возвращает "error"

    QVERIFY2(parsing("") == "error",
             "Пустое сообщение должно возвращать 'error'");
    //! \brief Проверяет, что пустое сообщение возвращает "error"
}

void FuncForServer_Test::test_parsing_auth() {
    //! \brief Тестирует команду авторизации с разным количеством параметров

    // Проверка команды auth с разным количеством параметров
    QVERIFY2(parsing("auth") == "auth-",
             "auth без параметров должен вернуть 'auth-'");
    //! \brief Проверяет: auth без параметров → "auth-"

    QVERIFY2(parsing("auth&user") == "auth-",
             "auth без пароля должен вернуть 'auth-'");
    //! \brief Проверяет: auth без пароля → "auth-"
}

void FuncForServer_Test::test_parsing_auth_wrong_format() {
    //! \brief Тестирует команду авторизации с неправильным форматом

    // auth с недостаточными параметрами
    QVERIFY2(parsing("auth&onlyone") == "auth-",
             "auth с одним полем должен вернуть 'auth-'");
    //! \brief Проверяет: auth с одним параметром → "auth-"

    QVERIFY2(parsing("auth&user&pass&extra") == "auth-",
             "auth с лишними полями должен вернуть 'auth-' (из-за несуществующего пользователя)");
    //! \brief Проверяет: auth с лишними параметрами → "auth-"
}

void FuncForServer_Test::test_parsing_check_format() {
    //! \brief Тестирует команду проверки с неправильным форматом

    // check с недостаточными полями
    QVERIFY2(parsing("check") == "check-",
             "check без параметров должен вернуть 'check-'");
    //! \brief Проверяет: check без параметров → "check-"

    QVERIFY2(parsing("check&1") == "check-",
             "check без variant и answer должен вернуть 'check-'");
    //! \brief Проверяет: check без варианта и ответа → "check-"

    QVERIFY2(parsing("check&1&1") == "check-",
             "check без answer должен вернуть 'check-'");
    //! \brief Проверяет: check без ответа → "check-"
}

void FuncForServer_Test::test_parsing_check_correct() {
    //! \brief Тестирует команду проверки с правильными ответами

    // Проверка правильных ответов
    QVERIFY2(parsing("check&1&1&0.0") == "check+",
             "Задача 1: ответ 0.0 должен быть правильным");
    //! \brief Проверяет: задача 1, ответ 0.0 → "check+"

    QVERIFY2(parsing("check&2&1&0.347") == "check+",
             "Задача 2 variant 1: ответ 0.347 должен быть правильным");
    //! \brief Проверяет: задача 2 вариант 1, ответ 0.347 → "check+"

    QVERIFY2(parsing("check&2&2&1.532") == "check+",
             "Задача 2 variant 2: ответ 1.532 должен быть правильным");
    //! \brief Проверяет: задача 2 вариант 2, ответ 1.532 → "check+"

    QVERIFY2(parsing("check&3&1&3.0") == "check+",
             "Задача 3: ответ 3.0 должен быть правильным");
    //! \brief Проверяет: задача 3, ответ 3.0 → "check+"
}

void FuncForServer_Test::test_parsing_getparams() {
    //! \brief Тестирует команду получения параметров

    QString response = parsing("getparams");
    //! \brief Выполняет команду getparams

    QVERIFY2(response.startsWith("params&"),
             "getparams должен вернуть строку, начинающуюся с 'params&'");
    //! \brief Проверяет, что ответ начинается с "params&"

    QStringList parts = response.split("&");
    //! \brief Разбивает ответ на части

    QVERIFY2(parts.size() == 10,
             "getparams должен вернуть 10 параметров (включая команду)");
    //! \brief Проверяет, что получено 10 параметров (команда + 9 коэффициентов)
}

void FuncForServer_Test::test_parsing_setparams_data() {
    //! \brief Предоставляет набор данных для теста установки параметров

    // Подготовка данных для теста setparams
    QTest::addColumn<QString>("paramsString");
    //! \brief Добавляет колонку с строкой параметров

    QTest::addColumn<QString>("expected");
    //! \brief Добавляет колонку с ожидаемым результатом

    QTest::newRow("valid_params") << "setparams&2&1&0&1&-3&1&1&-2&-1" << "params+";
    //! \brief Добавляет тестовые данные с валидными параметрами

    QTest::newRow("invalid_params_short") << "setparams&1&2&3" << "params-";
    //! \brief Добавляет тестовые данные с недостаточным количеством параметров

    QTest::newRow("invalid_params_wrong_type") << "setparams&a&b&c&1&-3&1&1&-2&-1" << "params-";
    //! \brief Добавляет тестовые данные с нечисловыми параметрами
}

void FuncForServer_Test::test_parsing_setparams() {
    //! \brief Тестирует команду установки параметров

    QFETCH(QString, paramsString);
    //! \brief Получает строку параметров из набора данных

    QFETCH(QString, expected);
    //! \brief Получает ожидаемый результат из набора данных

    QString response = parsing(paramsString);
    //! \brief Выполняет команду setparams

    QCOMPARE(response, expected);
    //! \brief Сравнивает полученный ответ с ожидаемым

    // После теста сбрасываем параметры
    if (expected == "params+") {
        //! \brief Если тест был успешным
        resetDefaultParams();
        //! \brief Сбрасывает параметры для чистоты следующих тестов
    }
}

// ── Тесты computeF ────────────────────────────────────────────────────────────
//! \brief Раздел тестов функции вычисления f(x)

void FuncForServer_Test::test_computeF_segment1() {
    //! \brief Тестирует вычисление на первом сегменте (логарифмическая функция)

    // Сначала убеждаемся, что параметры сброшены
    resetDefaultParams();
    //! \brief Сбрасывает параметры

    // ln(x+1) при x = -0.5: ln(0.5) ≈ -0.6931471805599453
    double result = computeF(-0.5);
    //! \brief Вычисляет значение функции

    double expected = qLn(0.5);
    //! \brief Вычисляет ожидаемое значение

    QVERIFY2(qAbs(result - expected) < 1e-12,
             QString("computeF(-0.5) = %1, expected = %2")
                 .arg(result, 0, 'f', 15)
                 .arg(expected, 0, 'f', 15)
                 .toLatin1());
    //! \brief Проверяет совпадение с высокой точностью

    // При x = -0.9: ln(0.1) ≈ -2.302585092994046
    result = computeF(-0.9);
    //! \brief Вычисляет значение

    expected = qLn(0.1);
    //! \brief Вычисляет ожидаемое значение

    QVERIFY2(qAbs(result - expected) < 1e-12,
             QString("computeF(-0.9) = %1, expected = %2")
                 .arg(result, 0, 'f', 15)
                 .arg(expected, 0, 'f', 15)
                 .toLatin1());
    //! \brief Проверяет совпадение с высокой точностью
}

void FuncForServer_Test::test_computeF_segment2() {
    //! \brief Тестирует вычисление на втором сегменте (кубическая функция)

    // x³-3x+1 при x=0: 0 - 0 + 1 = 1
    QVERIFY2(qAbs(computeF(0.0) - 1.0) < 1e-9,
             "computeF(0) = 1");
    //! \brief Проверяет значение при x=0

    // при x=0.347: примерно 0 (корень)
    double root1 = computeF(0.347);
    //! \brief Вычисляет значение вблизи корня

    QVERIFY2(qAbs(root1) < 0.001,
             "computeF(0.347) ≈ 0");
    //! \brief Проверяет близость к нулю

    // при x=1: 1 - 3 + 1 = -1
    QVERIFY2(qAbs(computeF(1.0) + 1.0) < 1e-9,
             "computeF(1) = -1");
    //! \brief Проверяет значение при x=1

    // при x=1.532: примерно 0 (корень)
    double root2 = computeF(1.532);
    //! \brief Вычисляет значение вблизи второго корня

    QVERIFY2(qAbs(root2) < 0.001,
             "computeF(1.532) ≈ 0");
    //! \brief Проверяет близость к нулю

    // при x=2: 8 - 6 + 1 = 3
    QVERIFY2(qAbs(computeF(2.0) - 3.0) < 1e-9,
             "computeF(2) = 3");
    //! \brief Проверяет значение при x=2
}

void FuncForServer_Test::test_computeF_segment3() {
    //! \brief Тестирует вычисление на третьем сегменте (гиперболическая функция)

    // 1/(x-2)-1 при x=3: 1/1 - 1 = 0
    QVERIFY2(qAbs(computeF(3.0) - 0.0) < 1e-9,
             "computeF(3) = 0 (корень)");
    //! \brief Проверяет корень при x=3

    // при x=4: 1/2 - 1 = -0.5
    QVERIFY2(qAbs(computeF(4.0) + 0.5) < 1e-9,
             "computeF(4) = -0.5");
    //! \brief Проверяет значение при x=4

    // при x=2.5: 1/0.5 - 1 = 2 - 1 = 1
    QVERIFY2(qAbs(computeF(2.5) - 1.0) < 1e-9,
             "computeF(2.5) = 1");
    //! \brief Проверяет значение при x=2.5
}

void FuncForServer_Test::test_computeF_boundary() {
    //! \brief Тестирует вычисление на границах сегментов

    // x=0 лежит на границе [0,2] → x³-3x+1
    QVERIFY2(qAbs(computeF(0.0) - 1.0) < 1e-9, "x=0 -> сегмент 2 (f=1)");
    //! \brief Проверяет, что x=0 относится ко второму сегменту

    // x=2 лежит на границе [0,2] → x³-3x+1
    QVERIFY2(qAbs(computeF(2.0) - 3.0) < 1e-9, "x=2 -> сегмент 2 (f=3)");
    //! \brief Проверяет, что x=2 относится ко второму сегменту

    // x=-0.999 должно быть в сегменте 1 (около -6.9)
    double val = computeF(-0.999);
    //! \brief Вычисляет значение вблизи левой границы

    QVERIFY2(!qIsNaN(val) && val < 0, "x=-0.999 -> сегмент 1");
    //! \brief Проверяет, что значение определено и отрицательно
}

void FuncForServer_Test::test_computeF_invalid() {
    //! \brief Тестирует обработку недопустимых значений x

    // x = -1 — вертикальная асимптота, должно вернуть NaN
    QVERIFY2(qIsNaN(computeF(-1.0)), "computeF(-1) должно быть NaN");
    //! \brief Проверяет, что x=-1 возвращает NaN

    // x = -2 — вне области
    QVERIFY2(qIsNaN(computeF(-2.0)), "computeF(-2) должно быть NaN");
    //! \brief Проверяет, что x=-2 возвращает NaN

    // x = 2.0001 — уже сегмент 3 (дробная функция)
    QVERIFY2(!qIsNaN(computeF(2.0001)), "computeF(2.0001) должно быть определено");
    //! \brief Проверяет, что x=2.0001 возвращает число

    // x = 1.9 — сегмент 2
    QVERIFY2(!qIsNaN(computeF(1.9)), "computeF(1.9) должно быть определено");
    //! \brief Проверяет, что x=1.9 возвращает число
}

// ── Тесты bisect ──────────────────────────────────────────────────────────────
//! \brief Раздел тестов метода бисекции

void FuncForServer_Test::test_bisect_segment2_root1() {
    //! \brief Тестирует поиск первого корня на втором сегменте

    double root = bisect(0.0, 0.7, 1e-6);
    //! \brief Находит корень на интервале [0, 0.7]

    QVERIFY2(!qIsNaN(root), "bisect нашёл корень на [0, 0.7]");
    //! \brief Проверяет, что корень найден

    QVERIFY2(qAbs(root - 0.3473) < 0.001,
             QString("Первый корень ≈ 0.347, получено %1").arg(root).toLatin1());
    //! \brief Проверяет, что корень близок к ожидаемому значению
}

void FuncForServer_Test::test_bisect_segment2_root2() {
    //! \brief Тестирует поиск второго корня на втором сегменте

    double root = bisect(1.0, 2.0, 1e-6);
    //! \brief Находит корень на интервале [1, 2]

    QVERIFY2(!qIsNaN(root), "bisect нашёл корень на [1, 2]");
    //! \brief Проверяет, что корень найден

    QVERIFY2(qAbs(root - 1.5321) < 0.001,
             QString("Второй корень ≈ 1.532, получено %1").arg(root).toLatin1());
    //! \brief Проверяет, что корень близок к ожидаемому значению
}

void FuncForServer_Test::test_bisect_segment3() {
    //! \brief Тестирует поиск корня на третьем сегменте

    double root = bisect(2.05, 5.0, 1e-6);
    //! \brief Находит корень на интервале (2, 5]

    QVERIFY2(!qIsNaN(root), "bisect нашёл корень на (2, 5)");
    //! \brief Проверяет, что корень найден

    QVERIFY2(qAbs(root - 3.0) < 0.001,
             QString("Третий корень = 3.0, получено %1").arg(root).toLatin1());
    //! \brief Проверяет, что корень равен 3.0 с заданной точностью
}

// ── Тесты checkAnswer ─────────────────────────────────────────────────────────
//! \brief Раздел тестов функции проверки ответов

void FuncForServer_Test::test_checkAnswer_task1_correct() {
    //! \brief Тестирует правильные ответы для задачи 1

    // Задача 1: корень f1(x) = 0 при x=0
    QVERIFY2(checkAnswer(1, 1, "0.0"), "Задача 1: ответ 0.0 верный");
    //! \brief Проверяет ответ 0.0

    QVERIFY2(checkAnswer(1, 1, "0.001"), "Задача 1: ответ 0.001 в пределах точности");
    //! \brief Проверяет ответ в пределах точности

    QVERIFY2(checkAnswer(1, 1, "-0.001"), "Задача 1: ответ -0.001 в пределах точности");
    //! \brief Проверяет ответ в пределах точности
}

void FuncForServer_Test::test_checkAnswer_task1_wrong() {
    //! \brief Тестирует неправильные ответы для задачи 1

    QVERIFY2(!checkAnswer(1, 1, "0.5"), "Задача 1: ответ 0.5 неверный");
    //! \brief Проверяет, что 0.5 - неверный ответ

    QVERIFY2(!checkAnswer(1, 1, "-0.5"), "Задача 1: ответ -0.5 неверный");
    //! \brief Проверяет, что -0.5 - неверный ответ

    QVERIFY2(!checkAnswer(1, 1, "abc"), "Задача 1: нечисловой ответ неверный");
    //! \brief Проверяет, что нечисловой ответ неверный
}

void FuncForServer_Test::test_checkAnswer_task2_variant1() {
    //! \brief Тестирует ответы для задачи 2, вариант 1

    // Задача 2 variant 1: корень ≈ 0.347
    QVERIFY2(checkAnswer(2, 1, "0.347"), "Задача 2 variant 1: ответ 0.347 верный");
    //! \brief Проверяет правильный ответ

    QVERIFY2(checkAnswer(2, 1, "0.35"), "Задача 2 variant 1: ответ 0.35 в пределах точности");
    //! \brief Проверяет ответ в пределах точности

    QVERIFY2(!checkAnswer(2, 1, "1.532"), "Задача 2 variant 1: ответ 1.532 неверный (это variant 2)");
    //! \brief Проверяет, что ответ для варианта 2 не подходит для варианта 1
}

void FuncForServer_Test::test_checkAnswer_task2_variant2() {
    //! \brief Тестирует ответы для задачи 2, вариант 2

    // Задача 2 variant 2: корень ≈ 1.532
    QVERIFY2(checkAnswer(2, 2, "1.532"), "Задача 2 variant 2: ответ 1.532 верный");
    //! \brief Проверяет правильный ответ

    QVERIFY2(checkAnswer(2, 2, "1.53"), "Задача 2 variant 2: ответ 1.53 в пределах точности");
    //! \brief Проверяет ответ в пределах точности

    QVERIFY2(!checkAnswer(2, 2, "0.347"), "Задача 2 variant 2: ответ 0.347 неверный (это variant 1)");
    //! \brief Проверяет, что ответ для варианта 1 не подходит для варианта 2
}

void FuncForServer_Test::test_checkAnswer_task3_correct() {
    //! \brief Тестирует правильные ответы для задачи 3

    // Задача 3: корень = 3.0
    QVERIFY2(checkAnswer(3, 1, "3.0"), "Задача 3: ответ 3.0 верный");
    //! \brief Проверяет правильный ответ

    QVERIFY2(checkAnswer(3, 1, "3.005"), "Задача 3: ответ 3.005 в пределах точности");
    //! \brief Проверяет ответ в пределах точности

    QVERIFY2(checkAnswer(3, 1, "2.995"), "Задача 3: ответ 2.995 в пределах точности");
    //! \brief Проверяет ответ в пределах точности
}

void FuncForServer_Test::test_checkAnswer_invalid_task() {
    //! \brief Тестирует обработку несуществующих задач

    QVERIFY2(!checkAnswer(99, 1, "0.0"), "Несуществующая задача → false");
    //! \brief Проверяет, что задача 99 возвращает false

    QVERIFY2(!checkAnswer(-1, 1, "0.0"), "Отрицательный номер задачи → false");
    //! \brief Проверяет, что отрицательный номер задачи возвращает false
}

QTEST_APPLESS_MAIN(FuncForServer_Test)
//! \brief Макрос Qt Test для создания главной функции без QApplication
//! \details Создает точку входа для запуска всех тестов

#include "tst_funcforserver_test.moc"
//! \brief Подключение метаобъектного кода для класса тестов (генерируется Qt MOC)
