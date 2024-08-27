FROM ubuntu:latest

WORKDIR /app
RUN apt-get update && apt-get install -y \
    libssl-dev \
    cmake \
    build-essential \
    git

RUN git clone --branch v1.1.0 https://github.com/Tencent/rapidjson.git /app/rapidjson


