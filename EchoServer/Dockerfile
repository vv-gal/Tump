# \file Dockerfile
# \brief Dockerfile для сборки и запуска EchoServer.
#        БД (PostgreSQL) находится в отдельном контейнере —
#        этот образ её не содержит и не запускает.

FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    qt6-base-dev \
    qt6-tools-dev \
    libpq-dev \
    build-essential \
    cmake \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

# Нормализуем имя .pro-файла
RUN if [ -f "EchoServer.pro" ]; then mv EchoServer.pro echoServer.pro; fi

RUN mkdir -p build && \
    cd build && \
    qmake6 ../echoServer.pro && \
    make -j$(nproc)

EXPOSE 33333

CMD ["./build/EchoServer"]
