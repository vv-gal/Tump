# TCP Math Server

> Клиент-серверное приложение для решения математических задач с регистрацией по email, статистикой и визуализацией графиков.

**Стек:** C++ · Qt 6 · SQLite · Docker · MailHog · Qt Test · Doxygen

---

## Содержание

- [Быстрый старт](#быстрый-старт)
- [Требования](#требования)
- [Установка зависимостей](#установка-зависимостей)
- [Запуск сервера](#запуск-сервера)
- [Запуск клиента](#запуск-клиента)
- [Запуск тестов](#запуск-тестов)
- [MailHog — просмотр писем](#mailhog--просмотр-писем)
- [Генерация документации](#генерация-документации)
- [Структура проекта](#структура-проекта)
- [Протокол обмена](#протокол-обмена)
- [Частые проблемы](#частые-проблемы)

---

## Быстрый старт

```bash
# 1. Клонировать репозиторий
git clone https://github.com/ВАШ_ЛОГИН/tcp-math-server.git
cd tcp-math-server

# 2. Поднять сервер + MailHog
docker-compose up -d --build

# 3. Открыть клиент в Qt Creator: Client/Client.pro → Run

# 4. Письма с кодами подтверждения: http://localhost:8025
```

---

## Требования

| Инструмент | Версия | Для чего |
|---|---|---|
| [Qt](https://www.qt.io/download) | 6.x (минимум 6.2) | Сборка сервера, клиента, тестов |
| [Docker Desktop](https://www.docker.com/products/docker-desktop/) | любая актуальная | Запуск сервера + MailHog |
| [Git](https://git-scm.com/) | любая | Клонирование репозитория |
| [Doxygen](https://www.doxygen.nl/download.html) *(опционально)* | 1.9+ | Генерация HTML-документации |

> **Windows:** Qt устанавливается через [Qt Online Installer](https://www.qt.io/download-qt-installer).  
> При установке выбрать компоненты: `Qt 6.x` → `MinGW 64-bit` + `Qt Network` + `Qt SQL`.

---

## Установка зависимостей

### Windows

1. Скачать и установить **[Qt Online Installer](https://www.qt.io/download-qt-installer)**
2. При установке выбрать:
   - `Qt 6.x for Desktop` → `MinGW 64-bit`
   - Дополнительные модули: `Qt Network`, `Qt SQL`, `Qt Test`, `Qt PrintSupport`
3. Скачать и установить **[Docker Desktop](https://www.docker.com/products/docker-desktop/)**
4. Перезагрузить компьютер после установки Docker

### Linux (Ubuntu/Debian)

```bash
# Qt 6
sudo apt update
sudo apt install -y qt6-base-dev qt6-tools-dev libqt6sql6-sqlite \
                   libqt6network6 build-essential

# Docker
sudo apt install -y docker.io docker-compose
sudo usermod -aG docker $USER   # чтобы не писать sudo
newgrp docker
```

### macOS

```bash
brew install qt@6 docker
export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"
```

---

## Запуск сервера

Сервер запускается в Docker вместе с MailHog одной командой.

### Шаг 1 — убедись, что Docker запущен

- **Windows/macOS:** запусти **Docker Desktop** из меню Start / Launchpad, дождись иконки кита в трее
- **Linux:** `sudo systemctl start docker`

### Шаг 2 — перейди в папку проекта

```bash
cd путь/до/tcp-math-server
```

> **Windows пример:** `cd C:\Users\Иван\Documents\tcp-math-server`

### Шаг 3 — сборка и запуск (первый раз)

```bash
docker-compose up -d --build
```

Флаги:
- `--build` — пересобрать образ из исходников (нужно при первом запуске или после изменений в коде)
- `-d` — запустить в фоновом режиме

Первая сборка занимает **3–7 минут** (скачивается Ubuntu, устанавливается Qt, компилируется сервер).

### Шаг 4 — последующие запуски (без пересборки)

```bash
docker-compose up -d
```

### Шаг 5 — проверить, что всё работает

```bash
docker-compose ps
```

Ожидаемый вывод — оба контейнера в статусе `Up`:

```
NAME               STATUS
mailhog            Up
tcp_math_server    Up
```

### Полезные команды

```bash
# Логи сервера в реальном времени
docker-compose logs -f server

# Остановить всё
docker-compose down

# Остановить и удалить данные (БД будет сброшена!)
docker-compose down -v

# Пересобрать после изменений в коде
docker-compose up -d --build
```

> **После перезагрузки компьютера** Docker Desktop запускается автоматически (если включён автозапуск), а контейнеры поднимаются сами благодаря `restart: unless-stopped`.

---

## Запуск клиента

### Через Qt Creator (рекомендуется)

1. Открыть **Qt Creator**
2. `Файл` → `Открыть файл или проект` → выбрать `Client/Client.pro`
3. При первом открытии выбрать kit: `Desktop Qt 6.x MinGW 64-bit` → **Настроить проект**
4. Нажать ▶ **Run** (Ctrl+R) или кнопку зелёного треугольника

### Через командную строку

```bash
cd Client
qmake Client.pro
make          # Linux/macOS
mingw32-make  # Windows (MinGW)
./Client      # Linux/macOS
.\Client.exe  # Windows
```

### Настройка адреса сервера

По умолчанию клиент подключается к `localhost:33333`.  
Если сервер запущен на другой машине — открой `Client/mainwindow.cpp` и измени строку:

```cpp
TcpClient::getInstance()->connectToServer("localhost", 33333);
//                                         ↑ заменить на IP сервера
```

---

## Запуск тестов

Тесты находятся в папке `Tests/` и проверяют математические функции и протокол сервера.

### Через Qt Creator

1. `Файл` → `Открыть файл или проект` → `Tests/Tests.pro`
2. Kit: `Desktop Qt 6.x MinGW 64-bit`
3. Run (Ctrl+R)

### Через командную строку

```bash
cd Tests
qmake Tests.pro
make          # Linux/macOS
mingw32-make  # Windows
./Tests       # Linux/macOS
.\Tests.exe   # Windows
```

Ожидаемый результат:

```
PASS   : FuncForServer_Test::test_parsing_invalid()
PASS   : FuncForServer_Test::test_parsing_auth()
PASS   : FuncForServer_Test::test_computeF_segment1()
PASS   : FuncForServer_Test::test_computeF_segment2()
PASS   : FuncForServer_Test::test_computeF_segment3()
PASS   : FuncForServer_Test::test_bisect_root1()
PASS   : FuncForServer_Test::test_bisect_root2()
PASS   : FuncForServer_Test::test_bisect_root3()
PASS   : FuncForServer_Test::test_checkAnswer_correct()
PASS   : FuncForServer_Test::test_checkAnswer_wrong()
Totals: 10 passed, 0 failed, 0 skipped
```

---

## MailHog — просмотр писем

При регистрации нового пользователя сервер отправляет письмо с кодом подтверждения.  
В режиме разработки письма перехватывает **MailHog** — он не пересылает их реально.

1. Убедись, что контейнеры запущены (`docker-compose up -d`)
2. Открой браузер: **http://localhost:8025**
3. После регистрации в клиенте письмо появится в списке
4. Открой письмо, скопируй 6-значный код и введи его в клиенте

---

## Генерация документации

```bash
# Из корневой папки проекта
cd tcp-math-server
doxygen Doxyfile

# Открыть документацию
# Windows:
start docs\doxygen\html\index.html
# Linux/macOS:
xdg-open docs/doxygen/html/index.html
```

Документация включает описание всех классов, методов и исходный код с подсветкой синтаксиса.

---

## Структура проекта

```
tcp-math-server/
├── docker-compose.yml          # Оркестрация: сервер + MailHog
├── Doxyfile                    # Конфигурация Doxygen
├── README.md                   # Этот файл
│
├── EchoServer/                 # TCP-сервер (Qt Console)
│   ├── main.cpp                # Точка входа
│   ├── mytcpserver.h/cpp       # View: TCP-соединения
│   ├── functionsforserver.h/cpp# Controller: протокол + математика
│   ├── database.h/cpp          # Model: SQLite (Singleton)
│   ├── emailsender.h/cpp       # SMTP через MailHog
│   ├── echoServer.pro          # Qt-проект
│   └── Dockerfile              # Docker-образ сервера
│
├── Client/                     # Qt GUI-клиент
│   ├── main.cpp
│   ├── mainwindow.h/cpp        # Контроллер + навигация
│   ├── loginwindow.h/cpp       # Экран входа
│   ├── registerwindow.h/cpp    # Экран регистрации (2 шага)
│   ├── taskwindow.h/cpp        # Экран задачи и статистики
│   ├── graphwindow.h/cpp       # Графики (QCustomPlot)
│   ├── tcpclient.h/cpp         # TCP Singleton
│   ├── qcustomplot.h/cpp       # Библиотека графиков
│   └── Client.pro
│
├── Tests/                      # Unit-тесты (Qt Test)
│   ├── tst_funcforserver_test.cpp
│   └── Tests.pro
│
└── docs/                       # Документация
    ├── WIKI.md
    ├── TestStrategy.md
    ├── UML_ClassDiagram.puml
    └── UseCase.puml
```

---

## Протокол обмена

Все сообщения — текстовые, разделитель `\n`, параметры внутри — `&`.

| Запрос | Ответ (успех) | Ответ (ошибка) |
|---|---|---|
| `auth&login&password` | `auth+&login` | `auth-` |
| `reg&login&password&email` | `reg+&login` | `reg-` |
| `confirm&login&code` | `confirm+` | `confirm-` |
| `stat&login` | `stat&N$M&P` | — |
| `check&task&variant&answer` | `check+` | `check-` |
| `getparams` | `params&a1&b1&...` | — |
| `setparams&a1&b1&...` | `params_set+` | `params_set-` |

> `stat&N$M&P` — N правильных, M попыток, P очков

---

## Частые проблемы

### `docker-compose up` зависает или падает с ошибкой

- Убедись, что **Docker Desktop запущен** (иконка кита в трее)
- Windows: проверь, что включена **виртуализация** в BIOS и **WSL 2** установлен
  ```
  wsl --install
  ```

### Клиент не подключается к серверу

- Проверь, что контейнер запущен: `docker-compose ps`
- Проверь логи: `docker-compose logs server`
- Убедись, что порт 33333 не занят другим приложением:
  ```bash
  # Windows
  netstat -ano | findstr :33333
  # Linux/macOS
  lsof -i :33333
  ```

### Письмо с кодом не приходит

- Открой **http://localhost:8025** — письмо должно быть там
- Если MailHog недоступен: `docker-compose logs mailhog`
- Убедись, что оба контейнера в одной сети: `docker network ls`

### Ошибка сборки Qt Creator: "Kit not found"

- Зайди в `Инструменты` → `Параметры` → `Kits`
- Проверь, что Qt 6 и компилятор MinGW обнаружены
- Если нет — переустанови Qt через Qt Online Installer

### Тесты не компилируются

- Убедись, что `Tests/Tests.pro` открыт как **отдельный** проект, а не как часть общего
- Пути `../EchoServer/` должны быть доступны — `Tests/` должна находиться рядом с `EchoServer/`

---

## Лицензия

Учебный проект. Свободное использование в образовательных целях.
