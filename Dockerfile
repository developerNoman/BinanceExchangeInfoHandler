FROM gcc:12

WORKDIR /app
RUN apt-get update && apt-get install -y \
    libssl-dev \
    cmake \
    build-essential \
    git

RUN git clone --branch v1.1.0 https://github.com/Tencent/rapidjson.git /app/rapidjson

COPY . /app/

RUN mkdir -p build && rm -rf build/*

WORKDIR /app/build


RUN cmake -DRAPIDJSON_INCLUDE_DIR=/app/rapidjson/include .. && make VERBOSE=1

CMD ["./main"]

