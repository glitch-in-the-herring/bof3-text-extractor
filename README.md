# Breath of Fire III Text Extractor
A program that can extract the text and dialogues from an AREAxxx.EMI file found in the game files of Breath of Fire III.  
For absolutely no reason, this program comes in two implementations: Python and C.

## Usage
### Python version:
On Windows:
```
python extractor.py [-h] [-o path] file
```
  
On Linux:
```
python3 extractor.py [-h] [-o path] file
```
By default, if no output file is specified, it will print the text to stdout.

### C version:
The code must be built first, install gcc or mingw if you are using Windows.  
Run `make` in the root directory of the repository to build the executable.
Usage:
```
extractor input output
```