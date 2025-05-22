# Use the official Ubuntu image as the base image
FROM ubuntu:latest

# Set the working directory in the container
WORKDIR /app

# Install necessary dependencies
RUN apt-get update && apt-get install -y vim clang gdb

# Copy the source code into the container
COPY RandomMemRead.cpp .

RUN clang++ RandomMemRead.cpp -std=c++20 -O3

CMD ["./a.out"]
