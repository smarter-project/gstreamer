FROM alpine:3.12.0

RUN apk add gstreamer gstreamer-tools gst-plugins-base gst-plugins-good gst-plugins-ugly bash

ENTRYPOINT [ "/bin/bash" ]