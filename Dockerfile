FROM debian:13-slim AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    libcurl4-openssl-dev \
    nlohmann-json3-dev \
    libtinyxml2-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build -j$(nproc)

FROM debian:13-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    libcurl4 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /app/build/securelens /usr/local/bin/
COPY --from=builder /app/build/securelens-train /usr/local/bin/
COPY --from=builder /app/build/securelens-cli /usr/local/bin/
COPY --from=builder /app/models /app/models
COPY --from=builder /app/training/dataset /app/training/dataset

RUN mkdir -p /app/models

EXPOSE 8080

CMD ["securelens"]
