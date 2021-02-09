FROM debian:stable

ENV REFRESHED_AT 2020-12-14
ENV DOCKER_RUN 1

# Set up a tools dev directory
RUN mkdir /home/dev

RUN apt-get update && apt-get install -y python3-pip git bc clang-format wget

# Pull the gcc-arm-none-eabi tarball and install it
RUN wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2 \
	&& tar xvf gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2 -C /home/dev \
	&& rm gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2
ENV PATH $PATH:/home/dev/gcc-arm-none-eabi-10-2020-q4-major/bin

RUN pip3 install --upgrade pip
RUN pip3 install nrfutil --user
ENV PATH $PATH:/root/.local/bin

WORKDIR /home/project

# Set cross compiler path variable to find compiler using Makefiles
ENV CROSS_COMPILE /home/dev/gcc-arm-none-eabi-10-2020-q4-major
