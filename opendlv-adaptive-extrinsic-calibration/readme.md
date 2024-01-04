docker run -it -v /tmp/.X11-unix:/tmp/.X11-unix -v ./data:/data -e DISPLAY=$DISPLAY adaptive_extrinsic /opt/sources/build/adaptive-extrinsic -dir1 /data/flir-2 -dir2 /data/flir-4 -threshold 0.5 -k 3

