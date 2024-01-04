# Intrinsic Calibration Tool

This tool performs intrinsic camera calibration using images of a checkerboard pattern. It calculates the camera matrix and distortion coefficients, storing the results in a YAML file for future use.


## Usage

Build the Docker Image
```bash
docker build -t intrinsic_calibration .
```
Then run the image 

```bash
sudo docker run -it -v /tmp/.X11-unix:/tmp/.X11-unix -v ./data:/data -e DISPLAY=$DISPLAY intrinsic_calibration /opt/intrinsic -imagesdir /data/flir-2 -outputyaml /data/flir-2/intrinsics.yml -checkersize 4 7 -cameraid 2
```
Note if you run the image in verbose mode the program wait for you to press a key before preceeding to the next image.


### Command line arguments
- `--verbose`: Enable verbose mode to display images.
- `--fastcheck`: Enable fast check during chessboard detection.
- `-imagesdir`: Path to the directory containing calibration images.
- `-outputyaml`: Output YAML file to store calibration results.
- `-checkersize`: Checkerboard size specified as width and height (e.g.,if there exist 5 squres horizontally and 7 vertically then -checkersize 4 7).
- `-cameraid`: Camera ID for identification in the output YAML.


## Output

The tool outputs the camera matrix, distortion coefficients, and camera ID in YAML format. The resulting file is saved at the specified output path.


## Notes

Ensure that the Docker container has access to the X11 display server for graphical output.
Adjust volume mount paths and image details according to your setup.

Feel free to modify the tool or Dockerfile to suit your specific needs. If you encounter any issues or have suggestions, please refer to the author.


## Author
Iman Shahmari contact at `imanshahmari@gmail.com`