# \file Dockerfile
# \brief Dockerfile для сборки и запуска EchoServer (TCP сервер с базой данных SQLite)

FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    qt6-base-dev \
    qt6-tools-dev \
    libsqlite3-dev \
    sqlite3 \
    build-essential \
    cmake \
    make \
    graphviz \
    wget \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN if [ -f "EchoServer.pro" ]; then \
        mv EchoServer.pro echoServer.pro; \
    elif [ -f "echoServer.pro" ]; then \
        echo "echoServer.pro already exists"; \
    else \
        for f in *.pro; do \
            if [ -f "$f" ]; then \
                mv "$f" echoServer.pro; \
                break; \
            fi; \
        done; \
    fi

RUN chmod +x *.sh 2>/dev/null || true

RUN mkdir -p build && \
    cd build && \
    qmake6 ../echoServer.pro || qmake ../echoServer.pro && \
    make -j$(nproc)

# Создаём папку для персистентных данных (БД и pending-файлы)
RUN mkdir -p /app/data

EXPOSE 33333

# Запускаем из /app, чтобы pending-файлы и БД писались в /app/data
CMD ["sh", "-c", "cd /app && ./build/EchoServer"]
