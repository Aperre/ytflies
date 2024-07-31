## YTFlies

### Description
YTFlies is a tool for converting any file into an MP4 format using steganography techniques. This allows files to be disguised as harmless MP4 videos, enabling covert storage and transmission of data in video services like Youtube, Vimeo, Dailymotion etc... 
*This isn't meant to be a real tool, more just for me to learn C++. Sorry for the bad code in advance.*

### Features
- **Steganographic Conversion**: Utilizes steganography to embed any file into an MP4 container.
- **File Preservation**: Preserves the original file's data intact within the MP4 format.
- **Simple CLI Interface**: Easy-to-use command-line interface for quick conversion.
- **Cross-Platform**: Works on Windows, macOS, and Linux operating systems.

### Installation
1. **Clone the repository and build**:
   ```
   git clone https://github.com/Aperre/ytflies.git
   cd yt-flies
   g++ main.cpp -o YTFlies `pkg-config --cflags --libs opencv4`
   ```

2. **Install dependencies**:
  FFmpeg 

### Usage
To convert a file into an MP4 using yt-flies, run the following command:
```
./YTFlies decode path/to/your/file.mp4 [newFileName]
or
./YTFlies encode path/to/your/file.xyz [newFileName]
```
Replace `path/to/your/file.xyz` with the path to the file you want to convert.

### Example
Convert a PDF file named `document.pdf` into an MP4:
```
./YTFlies encode path/to/document.pdf EncodedDocument
```

### Notes
- Ensure the file size does not exceed typical MP4 size limits to avoid compatibility issues.
- Use this tool responsibly and respect legal and ethical boundaries regarding data concealment and transmission.

### License
This project is licensed under the GPL v3.0 License - see the LICENSE file for details.



### Usage:

```
ytflies <encode|decode> <inputfile> [outputfile]
```
