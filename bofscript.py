from sys import argv
import re

codes = {0x01 : '\n', 0xff : ' ', 0x5d : '!', 0x90 : '"', 0x8e : "'", 0x3c : ',', 0x3d : '-', 0x3e : '.', 0x5c : '?', 0x00 : '\n', 0x02: '\n'}
party_members = {b'\x04\x00' : b'Ryu', b'\x04\x01' : b'Nina', b'\x04\x02' : b'Garr', b'\x04\x03': b'Teepo', b'\x04\x04' : b'Rei', b'\x04\x05' : b'Momo', b'\x04\x06' : b'Peco'}
alpha = list(range(65, 91)) + list(range(97, 123))

def main() -> int:
    try:
        area_file = open(argv[1], 'rb')
    except IndexError:
        print("No file was given!")
        return 1
    except FileNotFoundError:
        print("File not found!")
        return 2

    area_file.seek(8)
    if area_file.read(8) != b'MATH_TBL':
        print("Not a valid .EMI file!")
        return 3

    area_file.seek(0)
    chunk = area_file.read(512)
    prepad_chunks_found = [False, False]
    final_chunk =  False
    i = 0
    j = 0
    
    while len(chunk) > 0 and not final_chunk:
        if not (prepad_chunks_found[0] and prepad_chunks_found[1]):
            if chunk == b'\x00' * 136 + b'\x5f' * 376 and not prepad_chunks_found[1]:
                prepad_chunks_found[0] = True
            elif chunk == b'\x5f' * 512 and prepad_chunks_found[0]:
                prepad_chunks_found[1] = True
        else:
            output = ''
            if re.search(b'_{2,}$', chunk):
                final_chunk = True

            for byte, party_member in party_members.items():
                chunk = re.sub(byte, party_member, chunk)
            
            for byte in chunk:
                if byte in alpha:
                    output += chr(byte)
                elif byte in codes:
                    output += codes[byte]
                    
            print(output)
                
        i += 1
        address = area_file.seek(512 * i)
        chunk = area_file.read(512)
   
    
    area_file.close()
    return 0

if __name__ == "__main__":
    main()
