#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

/*!
 * \file taskwindow.h
 * \brief Главное окно задачи — выбор задачи, ввод ответа, параметры функций, статистика.
 */

/*!
 * \class TaskWindow
 * \brief Класс окна для работы с задачами.
 * \details Предоставляет интерфейс для выбора номера задачи, ввода ответа,
 *          отображения статистики, а также управления параметрами функций
 *          (коэффициентами a, b, c для каждого сегмента).
 */
class TaskWindow : public QWidget {
    Q_OBJECT  //!< Макрос Qt для поддержки сигналов и слотов

public:
    /*!
     * \brief Конструктор окна задач.
     * \param parent Родительский виджет (по умолчанию nullptr).
     */
    explicit TaskWindow(QWidget *parent = nullptr);

    /*!
     * \brief Устанавливает логин текущего пользователя.
     * \param login Логин пользователя.
     */
    void setLogin(const QString &login);

    /*!
     * \brief Обновляет отображение статистики пользователя.
     * \param statString Строка с данными статистики (например, JSON или форматированный текст).
     */
    void updateStats(const QString &statString);

    /*!
     * \brief Отображает результат проверки ответа.
     * \param correct true — ответ правильный, false — неправильный.
     */
    void showCheckResult(bool correct);

    /*!
     * \brief Загружает параметры функций, полученные с сервера.
     * \param paramsString Строка с параметрами (формат: "params&a1&b1&c1&a2&b2&c2&a3&b3&c3").
     */
    void loadParams(const QString &paramsString);

    /*!
     * \brief Вызывается когда сервер ответил на запрос установки параметров.
     * \param success true — параметры успешно применены, false — ошибка.
     */
    void onParamsApplied(bool success);

signals:
    /*!
     * \brief Сигнал запроса проверки ответа.
     * \param task Номер задачи (1, 2 или 3).
     * \param variant Вариант задачи (для задачи 2: 1 или 2).
     * \param answer Ответ пользователя в виде строки.
     */
    void checkRequested(int task, int variant, const QString &answer);

    /*!
     * \brief Сигнал запроса статистики пользователя.
     * \param login Логин пользователя.
     */
    void statRequested(const QString &login);

    /*!
     * \brief Сигнал запроса отображения графического окна.
     */
    void graphRequested();

    /*!
     * \brief Сигнал выхода из системы (возврат к окну входа).
     */
    void logoutRequested();

    /*!
     * \brief Испускается при запросе применить новые параметры функций на сервере.
     * \param paramsMessage Строка с параметрами в формате "setparams&a1&b1&c1&a2&b2&c2&a3&b3&c3".
     */
    void setParamsRequested(const QString &paramsMessage);

    /*!
     * \brief Испускается при запросе получить текущие параметры функций с сервера.
     */
    void getParamsRequested();

private slots:
    /*!
     * \brief Обработчик нажатия кнопки "Проверить".
     * \details Считывает выбранную задачу, вариант и введённый ответ,
     *          проверяет корректность и испускает сигнал checkRequested().
     */
    void onCheckClicked();

    /*!
     * \brief Обработчик нажатия кнопки "Статистика".
     * \details Испускает сигнал statRequested() с текущим логином.
     */
    void onStatClicked();

    /*!
     * \brief Обработчик изменения выбранной задачи в комбобоксе.
     * \param index Индекс выбранного элемента (0 — задача 1, 1 — задача 2, 2 — задача 3).
     */
    void onTaskChanged(int index);

    /*!
     * \brief Обработчик нажатия кнопки "Применить параметры".
     * \details Собирает значения коэффициентов из полей ввода,
     *          формирует строку параметров и испускает сигнал setParamsRequested().
     */
    void onSetParamsClicked();

    /*!
     * \brief Обработчик нажатия кнопки "Получить параметры".
     * \details Испускает сигнал getParamsRequested() для запроса текущих параметров с сервера.
     */
    void onGetParamsClicked();

    /*!
     * \brief Обновляет отображение формул на основе введённых коэффициентов.
     * \details Формирует текстовое представление функций f1, f2, f3
     *          и отображает их в соответствующих метках.
     */
    void updateFormulaLabels();

private:
    QString     mLogin;  //!< Логин текущего пользователя

    // --- Основные элементы интерфейса ---
    QLabel     *lblWelcome;      //!< Метка с приветствием (логин пользователя)
    QLabel     *lblTask;         //!< Метка "Выберите задачу:"
    QLabel     *lblFormula;      //!< Метка с текущей формулой задачи
    QComboBox  *comboTask;       //!< Выпадающий список для выбора задачи (1, 2 или 3)
    QLineEdit  *editAnswer;      //!< Поле ввода ответа пользователя
    QPushButton *btnCheck;       //!< Кнопка "Проверить" — отправка ответа на сервер
    QPushButton *btnStat;        //!< Кнопка "Статистика" — запрос статистики
    QPushButton *btnGraph;       //!< Кнопка "График" — открытие окна с графиками
    QPushButton *btnLogout;      //!< Кнопка "Выйти" — завершение сессии
    QLabel     *lblResult;       //!< Метка для отображения результата проверки ("Верно"/"Неверно")
    QLabel     *lblStat;         //!< Метка для отображения статистики пользователя

    // --- Параметры функций (коэффициенты a, b, c для каждого сегмента) ---
    // f1: a1 * ln(x + b1) + c1  (первый сегмент: логарифмическая функция)
    QLineEdit *editA1;  //!< Поле ввода коэффициента a1 для функции f1
    QLineEdit *editB1;  //!< Поле ввода коэффициента b1 для функции f1
    QLineEdit *editC1;  //!< Поле ввода коэффициента c1 для функции f1

    // f2: a2 * x^3 + b2 * x + c2  (второй сегмент: кубическая функция)
    QLineEdit *editA2;  //!< Поле ввода коэффициента a2 для функции f2
    QLineEdit *editB2;  //!< Поле ввода коэффициента b2 для функции f2
    QLineEdit *editC2;  //!< Поле ввода коэффициента c2 для функции f2

    // f3: a3 / (x + b3) + c3  (третий сегмент: гиперболическая функция)
    QLineEdit *editA3;  //!< Поле ввода коэффициента a3 для функции f3
    QLineEdit *editB3;  //!< Поле ввода коэффициента b3 для функции f3
    QLineEdit *editC3;  //!< Поле ввода коэффициента c3 для функции f3

    QLabel *lblFormula1;  //!< Метка с текстовым представлением формулы f1
    QLabel *lblFormula2;  //!< Метка с текстовым представлением формулы f2
    QLabel *lblFormula3;  //!< Метка с текстовым представлением формулы f3

    QPushButton *btnSetParams;   //!< Кнопка "Применить параметры" — отправка новых коэффициентов на сервер
    QPushButton *btnGetParams;   //!< Кнопка "Получить параметры" — запрос текущих коэффициентов с сервера
    QLabel      *lblParamsStatus; //!< Метка для отображения статуса применения параметров ("Успешно"/"Ошибка")
};

#endif // TASKWINDOW_H
