FROM ubuntu:latest

RUN apt-get update && apt-get install -y
RUN libssl-dev
RUN cmake 
RUN build-essential
RUN git

WORKDIR /app
