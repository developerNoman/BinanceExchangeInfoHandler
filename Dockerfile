
# FROM ubuntu:20.04

# RUN apt-get update && apt-get install -y \
#     cmake \
#     g++ \
#     libssl-dev \
#     && rm -rf /var/lib/apt/lists/*

# COPY . /BinanceHandlerProject

# WORKDIR /BinanceHandlerProject

# RUN mkdir built
# WORKDIR /BinanceHandlerProject/built

# RUN cmake .. && make

# CMD ["./main"]
# FROM ubuntu:20.04

# # Install dependencies and add Kitware's APT repository for latest CMake

# RUN apt-get update && apt-get install -y \
#     apt-transport-https \
#     ca-certificates \
#     gnupg \
#     software-properties-common \
#     wget \
#     git  # <- Add git here

# # Add Kitware APT repository key
# RUN wget -O /etc/apt/trusted.gpg.d/kitware.asc https://apt.kitware.com/keys/kitware-archive-latest.asc

# # Add Kitware repository
# RUN apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main' && apt-get update

# # Install required packages
# RUN apt-get install -y \
#     cmake \
#     g++ \
#     libssl-dev \
#     && rm -rf /var/lib/apt/lists/*

# COPY . /BinanceHandlerProject

# WORKDIR /BinanceHandlerProject

# RUN mkdir built
# WORKDIR /BinanceHandlerProject/built

# RUN cmake .. && make

# CMD ["./main"]

# FROM gcc:latest

# RUN apt-get update && apt-get install -y \
#     libssl-dev \
#     cmake \ 
#     build-essential

# COPY . /BinanceHandlerProject

# WORKDIR /BinanceHandlerProject

# RUN mkdir built
# WORKDIR /BinanceHandlerProject/built

# RUN cmake .. && make


# CMD ["./main"]
# FROM gcc:latest

# WORKDIR /app

# RUN apt-get update && apt-get install -y \
#     libssl-dev \
#     cmake \ 
#     build-essential

# COPY . /app/


# RUN mkdir build
# RUN cp config.json query.json build/

# WORKDIR /app/build
# RUN cmake .. && make

# CMD ["./main"]

FROM gcc:latest

WORKDIR /app
RUN apt-get update && apt-get install -y \
    libssl-dev \
    cmake \
    build-essential

COPY . /app/

RUN mkdir -p build && rm -rf build/*


RUN cp config.json query.json build/

WORKDIR /app/build

RUN cmake .. && make VERBOSE=1

CMD ["./main"]
