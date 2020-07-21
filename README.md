# GStreamer

## Functionality
- https://gstreamer.freedesktop.org/
- Configures gstreamer video server that manages all video devices available on host
- Actors are not granted access to dev/video*, but connect to Gstreamer server and receive/stream video over network

For use in kubernetes we have opted to split out a frontend and backend pipeline. The frontend pipeline is resposible for taking a raw camera or video file stream, and performing non-computationally expensive operations (resize, format convert, or framerate convert). The frontend pipeline tees the processed image feed into n shared memory sinks which the backend can then pickup.

For the backend pipeline, we create a rtsp server which launches a backend pipeline per requested rtsp feed. For instance if a client were to request h264 encoded frames at 1 fps, we would launch a new gstreamer process which reads from the shared mem channel for 1 fps raw frames, then h264 encodes them, and sends them to the client. The backend rtsp server is designed to multicast data to m clients if they request the same feed, as to not duplicate encoding work.

From the client perspective users simply request frames from the url rtsp://\<hostname\>:8554/video.<raw/h264>.<framerate>. In the k8s yaml, the frontend pipeline can be constructed to read from any arbitrary source, be it an rtsp source, physical camera, or file. The backend pipelines spawned by the rtsp server are defined in the c file.