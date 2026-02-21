FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# libs
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libsdl2-dev \
    libsdl2-image-dev \
    libfreeimage-dev \
    libjpeg-dev \
    libpng-dev \
    libgl1-mesa-dev \
    zlib1g-dev \
    libx11-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

# oxygine
RUN cd oxygine-framework && \
    mkdir build && cd build && \
    cmake .. && \
    cmake --build . -j$(nproc)

# game
RUN mkdir build && cd build && \
    cmake .. && \
    cmake --build .

CMD ["./bin/ChessGame"]