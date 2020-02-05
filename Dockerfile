FROM ubuntu:16.04

WORKDIR /app
COPY ./build/flowchar /app

ENTRYPOINT [ "./flowchar" ]