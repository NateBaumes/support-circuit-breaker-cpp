FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libcurl4-openssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN mkdir build && cd build && \
    cmake .. && \
    make -j4

EXPOSE 8080

CMD ["./build/support_circuit_breaker"]
