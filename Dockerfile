FROM alpine:3.12.0

RUN apk add gstreamer gstreamer-tools bash

ENTRYPOINT [ "/bin/bash" ]