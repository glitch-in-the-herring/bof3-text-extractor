from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument("input", help="Path to the .EMI file", metavar="input")
parser.add_argument("output", help="Path to the .EMI file", metavar="output")

args = parser.parse_args()

printables = {' ' : b'\xff', '!' : b'\x5d', '"' : b'\x90', "'" : b'\x8e', ',' : b'\x3c', '-' : b'\x3d', '.' : b'\x3e', '?' : b'\x5c'}

def main() -> int:
	try:
		input_file = open(args.input, 'r')
	except FileNotFoundError:
		print("Could not open input file!")
		return 1

	try:
		output_file = open(args.output, 'wb')
	except FileNotFoundError:
		print("Could not open output file!")
		return 2

	input_line = input_file.readline()
	while input_line != '':
		print(input_line)
		for char in input_line:
			if char.isalpha():
				output_file.write(bytes([ord(char)]))
			elif char in printables:
				output_file.write(printables[char])

		output_file.write(b'\x01')
		input_line = input_file.readline()

	input_file.close()
	output_file.close()

if __name__ == "__main__":
	main()
