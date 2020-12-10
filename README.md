## mdec-tools

PlayStation1 MDEC compression tools

Implemented so far:
- Load .png/.jpg images using stb_image
- RGB -> YUV conversion
- Float DCT
- Quantization
- Non-RLE bitstream generation
- Mono output

To be implemented:
- Color support (Cr/Cb blocks with 4:2:0 chroma subsampling)
- RLE
- Different quantization tables 
- Integer implementation to match PSX

## Build
```
cmake -B build/ -S .
cmake --build build/
```

## Author
Jakub Czekański

## License
MIT
