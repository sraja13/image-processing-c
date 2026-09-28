# Image Processing Tool (C)

A command-line image editor written in C99 that reads and writes PPM images.

## Operations
| Command | What it does |
|---|---|
| `invert` | Inverts every color channel |
| `crop <x1> <y1> <x2> <y2>` | Crops to the rectangle between two corners |
| `zoom_out` | Halves the image size by averaging 2x2 pixel blocks |
| `binarize <threshold>` | Converts to black and white using a grayscale threshold |
| `pointilism` | Paints randomly placed circles to create a pointillism effect |
| `blur <sigma>` | Gaussian blur using a kernel generated from sigma |

## Highlights
- Gaussian blur builds its convolution kernel at runtime from the sigma value and normalizes over pixels near the image edges
- Binary PPM (P6) reading and writing
- Manual memory management for image buffers and kernels
- Includes `img_cmp`, a utility for comparing two images when testing

## Build and run
```bash
make
./project input.ppm output.ppm blur 2.0
./project input.ppm output.ppm crop 10 10 200 150
```
