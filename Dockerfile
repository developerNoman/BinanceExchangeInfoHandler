FROM ubuntu:latest

WORKDIR /app
RUN apt-get update && apt-get install -y \
    libssl-dev \
    cmake \
    build-essential \
    git



