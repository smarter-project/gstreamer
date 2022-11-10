# GStreamer

## Functionality
- https://gstreamer.freedesktop.org/
- Configures gstreamer video server that manages all video devices available on host
- Actors are not granted access to dev/video*, but connect to Gstreamer server and receive/stream video over network

For use in kubernetes we have opted to split out a frontend and backend pipeline. The frontend pipeline is resposible for taking a raw camera or video file stream, and performing non-computationally expensive operations (resize, format convert, or framerate convert). The frontend pipeline tees the processed image feed into n shared memory sinks which the backend can then pickup.

For the backend pipeline, we create a rtsp server which launches a backend pipeline per requested rtsp feed. For instance if a client were to request h264 encoded frames at 1 fps, we would launch a new gstreamer process which reads from the shared mem channel for 1 fps raw frames, then h264 encodes them, and sends them to the client. The backend rtsp server is designed to multicast data to m clients if they request the same feed, as to not duplicate encoding work.

By default all input camera feeds are downsampled to 640x480, as most image processing applications do not require high resolution images.

From the client perspective users simply request frames from the url rtsp://\<hostname\>:8554/video.<raw/h264>.<framerate>. In the k8s yaml, the frontend pipeline can be constructed to read from any arbitrary source, be it an rtsp source, physical camera, or file. The backend pipelines spawned by the rtsp server are defined in the c file. The env variable `H264_GSTREAMER_PIPELINE` is used to define what h264 encode method is to be used by the backend pipelines. By default it is set to `queue ! x264enc tune=zerolatency` in the non-accelerated cpu default case. See our [build workflow](.github/workflows/smarter-org-docker-buildx.yml) for the defaults for the Jetson and Rpi4 builds.

For debugging purposes I find it helps to run this container with the environment variable `GST_DEBUG=3,rtspmedia:5` set. By doing this, you will see output when an rtsp client connects to the server, and any errors encountered when the server launches a backend pipeline.