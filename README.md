# Breath of Fire III Text Extractor
A tool that lets you extract the dialogue section from AREAxxx.EMI files found in Breath of Fire III's game files.

## Downloads
| OS | Link|
| --- | --- |
| Windows and Ubuntu: | [![](https://github.com/glitch-in-the-herring/bof3-text-extractor/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/glitch-in-the-herring/bof3-text-extractor/actions) |

## Building
Make sure GCC is installed and simply run `make` to build both the English and Japanese extractors. Use `make extractor` to build the English extractor only, and `make jpextractor` to build the Japanese extractor.

## Usage
### C
English extractor:
```
extractor input output
```

Japanese extractor:
```
jpextractor input output
```

### Python version:
The python version is now deprcated 

On Windows:
```
python extractor.py [-h] [-o path] file
```
  
On Linux:
```
python3 extractor.py [-h] [-o path] file
```
By default, if no output file is specified, it will print the text to stdout.

