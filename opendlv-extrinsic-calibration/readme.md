# Adaptive Extrinsic Calibration Tool

This tool performs extrinsic calibration based on a checkerboard pattern. It calculates the translation vector in units of each square side length in the checkerboard pattern and th rotation matrix (to go from the second camera to the first one) following the OpenCV convention (right-hand rule with z as the depth of the camera). The results can be stored in a YAML file or send via Opendlv to other microservices.


## Usage

Build the Docker Image

```bash
docker build -t extrinsic .
```
Then run the image

```bash
docker run -it --net=host -v /tmp/.X11-unix:/tmp/.X11-unix -v ./data:/data -e DISPLAY=$DISPLAY extrinsic /tmp/build/extrinsic -dir1 /data/1/flir-2 -dir2 /data/1/flir-4 -checkersize 4 7 --verbose -write /data/extrnisc.yml --send
```

## Command line arguments

- `--verbose` Enable verbose mode to display images and wait for a key press before proceeding to the next image.
- `--send` Send the calibration data to the specified od4 channel. If not specified, the program will only output the results.
- `-dir1` Path to the directory containing images from the first camera perspective.
- `-dir2` Path to the directory containing images from the second camera perspective.
- `-write` Specify a file path to store the calibration results (specify the filename as well as the extension for example /data/extrnisc1.yml).
- `-od4` Specify the conference id for Opendlv 
- `-checkersize`: Checkerboard size specified as width and height (e.g.,if there exist 5 squres horizontally and 7 vertically then -checkersize 4 7).
- `-conv`: Convolution size for calculating the pixels with subpix accuracy by default this is (11,11).


## Output
The tool outputs the translation vector, rotation matrix and RMSE for calibration . The resulting file can be specified using the -write option.

## Notes
Ensure that the Docker container has access to the X11 display server for graphical output.
Adjust volume mount paths and image details according to your setup.

Feel free to modify the tool or Dockerfile to suit your specific needs. If you encounter any issues or have suggestions, please refer to the author.

## Author

Iman Shahmari
Contact: imanshahmari@gmail.com