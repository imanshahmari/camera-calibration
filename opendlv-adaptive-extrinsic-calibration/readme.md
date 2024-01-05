# Adaptive Extrinsic Calibration Tool

This tool performs adaptive extrinsic calibration using SIFT or ORB feature matching between images from two camera perspectives. This tool performs adaptive extrinsic calibration using SIFT or ORB feature matching between images from two camera perspectives. It calculates the normalized translation vector and rotation matrix (to go from the second camera to the first one) following the OpenCV convention (right-hand rule with z as the depth of the camera). The results can be stored in a YAML file or send via Opendlv to other microservices.


## Usage

Build the Docker Image

```bash
docker build -t adaptive .
```
Then run the image

```bash
docker run -it --net=host -v /tmp/.X11-unix:/tmp/.X11-unix -v ./data:/data -e DISPLAY=$DISPLAY adaptive /tmp/build/adaptive-extrinsic -dir1 /data/flir-2 -dir2 /data/flir-4 -threshold 0.2 --verbose --sift --send
```

## Command line arguments

`--verbose` Enable verbose mode to display images and wait for a key press before proceeding to the next image.
`--sift` Use SIFT feature detection. If not specified, ORB feature detection will be used by default.
`--send` Send the calibration data to the specified od4 channel. If not specified, the program will only output the results.
`-dir1` Path to the directory containing images from the first camera perspective.
`-dir2` Path to the directory containing images from the second camera perspective.
`-threshold` Threshold used for filtering points of interest in images based on knn.
`-k` Number of neighbors in the knn algorithm.
`-write` Specify a file path to store the calibration results.
`-od4` Specify the conference id for Opendlv 

## Output
The tool outputs the normalized translation vector, fundamental matrix, and other calibration details in YAML format. The resulting file can be specified using the -write option.

Note: The translation vector is normalized and provided in relative units, not in metric measurements.

## Notes
Ensure that the Docker container has access to the X11 display server for graphical output.
Adjust volume mount paths and image details according to your setup.

Feel free to modify the tool or Dockerfile to suit your specific needs. If you encounter any issues or have suggestions, please refer to the author.

## Author

Iman Shahmari
Contact: imanshahmari@gmail.com