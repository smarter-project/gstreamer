FROM alpine:3.12.0

COPY multicast-rtsp.c .

RUN apk add --no-cache --virtual .build-deps alpine-sdk && \
    apk add --no-cache gstreamer gstreamer-tools gst-plugins-base gst-plugins-good gst-plugins-bad gst-plugins-ugly bash && \
    apk add --no-cache -X http://dl-cdn.alpinelinux.org/alpine/edge/testing gst-rtsp-server-dev && \
    gcc -o multicast-rtsp-server multicast-rtsp.c `pkg-config --cflags --libs gstreamer-rtsp-server-1.0` && \
    apk del .build-deps

ENTRYPOINT [ "/bin/bash" ]