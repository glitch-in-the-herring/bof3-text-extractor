from sys import argv
from argparse import ArgumentParser
import re

parser = ArgumentParser()
parser.add_argument("file", help="Path to the .EMI file", metavar="file")
parser.add_argument("-o", "--output", help="Output to this file. If not specified, will be printed to stdout.", metavar="path")

args = parser.parse_args()

printables = {0x01 : '\n', 0xff : ' ', 0x5d : '!', 0x90 : '"', 0x8e : "'", 0x3c : ',', 0x3d : '-', 0x3e : '.', 0x5c : '?', 0x00 : '\n\n------\n', 0x20 : '\n\n------\n', 0x02: '\n\u25bc\n'}
window_position = {0xe4 : '\u2196', 0xe5 : '\u2197', 0xe6 : '\u2199', 0xe7 : '\u2198'}
controls = [
                {b'\x0c\x03' : b'\xe4', b'\x0c\x04' : b'\xe5', b'\x0c\x05' : b'\xe6', b'\x0c\x06' : b'\xe7'}, 
                {b'\x0d' : b'\xff', b'\x0f[\x00-\x09]' : b'\xff', b'\x0b' : b'--'},
                {b'\x04\x00' : b'Ryu', b'\x04\x01' : b'Nina', b'\x04\x02' : b'Garr', b'\x04\x03': b'Teepo', b'\x04\x04' : b'Rei', b'\x04\x05' : b'Momo', b'\x04\x06' : b'Peco'}
            ]
alpha = list(range(65, 91)) + list(range(97, 123))

def main() -> int:
    try:
        area_file = open(args.file, 'rb')
    except FileNotFoundError:
        print("File not found!")
        return 2

    if args.output:
        output_file = open(args.output, 'w', encoding='utf-8')
        output_address = 0
        output_file.seek(0)

    area_file.seek(8)
    if area_file.read(8) != b'MATH_TBL':
        print("Not a valid .EMI file!")
        return 3

    area_file.seek(0)
    chunk = area_file.read(512)
    dialogue_chunk = b''
    prepad_chunks_found = [False, False]
    final_chunk =  False
    i = 0

    while not final_chunk:
        if not (prepad_chunks_found[0] and prepad_chunks_found[1]):
            if chunk == b'\x00' * 136 + b'\x5f' * 376 and not prepad_chunks_found[1]:
                prepad_chunks_found[0] = True
            elif chunk == b'\x5f' * 512 and prepad_chunks_found[0]:
                prepad_chunks_found[1] = True
        else:
            dialogue_chunk += chunk
            if re.search(b'_{2,}$', chunk):
                final_chunk = True

        i += 1
        area_file.seek(512 * i)
        chunk = area_file.read(512)

    for table in controls:
        for byte, item in table.items():
            dialogue_chunk = re.sub(byte, item, dialogue_chunk)

    output = ''
    for byte in dialogue_chunk:
        if byte in alpha:
            output += chr(byte)
        elif byte in printables:
            output += printables[byte]
        elif byte in window_position:
            output += window_position[byte]

    if args.output:
        output_file.write(str(output))
    else:
        print(output)  
    
    area_file.close()
    if args.output:
        output_file.close()

    return 0

if __name__ == "__main__":
    main()
