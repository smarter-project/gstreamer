ARG BASE_IMAGE=ubuntu:focal-20221019

FROM ${BASE_IMAGE}

# Ensure apt won't prompt for selecting options
ENV DEBIAN_FRONTEND=noninteractive

ARG EXTRA_PACKAGES

RUN apt update && \
    apt install -yqq --no-install-recommends \
        libgstreamer1.0-0 \
        gstreamer1.0-plugins-base \
        gstreamer1.0-plugins-good \
        gstreamer1.0-plugins-bad \
        gstreamer1.0-plugins-ugly \
        gstreamer1.0-tools \
        libgstrtspserver-1.0-dev \
        gstreamer1.0-rtsp \
        ${EXTRA_PACKAGES} \
        supervisor && \
    mkdir -p /var/log/supervisord && \
    apt autoremove -y && \
    rm -rf /var/lib/apt/lists/*

# Compile rtsp server
ARG H264_GSTREAMER_PIPELINE="queue ! x264enc tune=zerolatency"
ENV H264_GSTREAMER_PIPELINE=${H264_GSTREAMER_PIPELINE}
COPY multicast-rtsp.c .
RUN apt update && \
    apt install -yqq --no-install-recommends gcc && \
    gcc -O2 -o multicast-rtsp-server multicast-rtsp.c `pkg-config --cflags --libs gstreamer-rtsp-server-1.0` && \
    chmod +x multicast-rtsp-server && \
    cp multicast-rtsp-server /usr/local/bin && \
    apt purge -yqq gcc && \
    apt autoremove -y && \
    rm -rf /var/lib/apt/lists/*

# Supervisord
COPY supervisord.conf /etc/supervisord.conf
ENV VIDEO_ID="0"

CMD [ "supervisord", "-n", "-c", "/etc/supervisord.conf" ]