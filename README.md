# Инструкция по запуску проекта
# Tump

> Клиент-серверное приложение для решения математических задач с регистрацией по email, статистикой и визуализацией графиков.

**Стек:** C++ · Qt 6 · SQLite · Docker · MailHog · Qt Test · Doxygen

## Быстрый запуск

```bash
# 1. Клонировать репозиторий
git clone https://github.com/vv-gal/Tump.git

# 2. Поднять сервер + MailHog
docker-compose up -d --build

# 3. Открыть клиент в Qt Creator: Client/Client.pro → Run

# 4. Письма с кодами подтверждения: http://localhost:8025
```

---

## Необходимое ПО

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

- **Windows/macOS:** запусти **Docker Desktop** из меню Start / Launchpad, дождись иконки кита
- **Linux:** `sudo systemctl start docker`

### Шаг 2 — перейди в папку проекта

```bash
cd /d/project_timp/project

```

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

## Документация
 Документация Doxygen находиться в папке docs\html

---




