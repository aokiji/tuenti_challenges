#include <iostream>
#include <fstream>
#include <stdint.h>
#include <vector>
#include <sstream>
#include <deque>

struct ZXSpectrum {
	ZXSpectrum(const std::string filename, size_t offset) :
			offset(offset), pc(memory + offset) {
		read(filename, offset);
	}

	void run() {
		while (readAndExecuteInstruction()) {

		}
	}

	void printScreen() {
		char out;
		size_t width = 256, height = 192;
		for (size_t y = 0; y < width; ++y) {
			for (size_t x = 0; x < height; ++x) {
				uint16_t addr = 0x4000;
				addr |= (y & 0xC0) << 5;
				addr |= (y & 0x38) << 2;
				addr |= (y & 0x07) << 8;
				addr += x / 8;
				uint16_t mask = 1 << (7 - (x & 0x7));
				auto pixel = *(memory + addr);
				if ((pixel & mask) == mask) {
					out = '*';
				}
				else {
					out = ' ';
				}
				std::cout.write(&out, 1);
			}
			std::cout << std::endl;
		}
	}

private:
	enum Registers {
		A = 0, F, B, C, D, E, H, L, AF = 16, BC = 18, DE = 20, HL = 22
	};

	bool readAndExecuteInstruction() {
		unsigned char firstByte = *pc;
		++pc;

		if ((firstByte & 0xCF) == 0x01) {
			op_register = static_cast<Registers>(BC
					+ ((firstByte & 0x30) >> 4) * 2);
			readOperand(true);
			ldRegistry();
		} else if ((firstByte & 0xC7) == 0x06) {
			op_register = getSingleRegister((firstByte & 0x38) >> 3);
			readOperand(false);
			ldRegistry();
		} else if ((firstByte & 0xF8) == 0x70) {
			op_register = getSingleRegister(firstByte);
			ldIndRegistryRegistry(HL);
		} else if (firstByte == 0x1A) {
			op_register = A;
			operand = indirectReg(DE, false);
			ldRegistry();
		} else if ((firstByte & 0xCF) == 0x03) {
			op_register = getDoubleRegister((firstByte & 0x30) >> 4);
			inc();
		} else if ((firstByte & 0xC7) == 0x05) {
			op_register = getSingleRegister((firstByte & 0x38) >> 3);
			dec();
		} else if ((firstByte & 0xCF) == 0xC5) {
			op_register = getDoubleRegister((firstByte & 0x30) >> 4);
			push();
		}else if ((firstByte & 0xCF) == 0xC1) {
			op_register = getDoubleRegister((firstByte & 0x30) >> 4);
			pop();
		} else if ((firstByte & 0xC9) == 0x09) {
			op_register = getDoubleRegister((firstByte & 0x30) >> 4);
			add(HL);
		}
		else if (firstByte == 0xC9) {
			// ret
			return false;

		}
		else if (firstByte == 0x20) {
			readOperand(false);
			if (!zero_flag) {
				char dis = operand & 0xFF;
				pc += dis;
			}
		} else {
			std::ostringstream msg;
			msg << "Unknown instruction 0x" << std::hex
					<< (unsigned int) firstByte;
			throw std::runtime_error(msg.str());
		}
		return true;
	}

	Registers getSingleRegister(unsigned char r) {
		auto dis = (r + 1) & 0x07;
		return dis == 0 ? A : static_cast<Registers>(F + dis);
	}

	Registers getDoubleRegister(unsigned char r) {
		if ((r & 0x03) == 0x03) {
			throw std::runtime_error("Unsupported");
		}
		return static_cast<Registers>(BC + (r & 0x03) * 2);
	}

	uint16_t indirectReg(Registers reg, bool extraByte) {
		return readNumber(memory + readRegister(reg), extraByte);
	}

	void read(const std::string filename, size_t offset) {
		std::ifstream file(filename, std::ios::binary);
		size_t size = sizeof(memory) / sizeof(char) - offset;
		file.read((char *) pc, size);
	}

	void inc() {
		storeRegister(op_register, readRegister(op_register) + 1);
	}

	void dec() {
		auto value = readRegister(op_register) - 1;
		zero_flag = value == 0;
		storeRegister(op_register, value);
	}

	void push() {
		auto value = readRegister(op_register);
		stack.push_back(value);
	}

	void pop() {
		storeRegister(op_register, stack.back());
		stack.pop_back();
	}

	void add(Registers reg) {
		auto value = readRegister(reg) + readRegister(op_register);
		storeRegister(reg, value);
	}

	void ldRegistry() {
		storeRegister(op_register, operand);
	}

	void ldIndRegistryRegistry(Registers addreg) {
		auto value = readRegister(op_register);
		auto addr = readRegister(addreg);
		*(memory + addr) = value;
	}

	uint16_t readNumber(unsigned char * store, bool extraByte) {
		uint16_t value;
		value = *store;
		if (extraByte) {
			value += *(store + 1) << 8;
		}
		return value;
	}

	void readOperand(bool extraByte) {
		operand = readNumber(pc, extraByte);
		++pc;
		if (extraByte) {
			++pc;
		}
	}

	uint16_t readRegister(Registers reg) {
		auto offset = reg >= 16 ? reg - 16 : reg;
		return readNumber(registers + offset, reg >= 16);
	}

	void storeRegister(Registers reg, uint16_t value) {
		if (reg < 16) {
			*(registers + reg) = value;
		} else {
			int offset = reg - 16;
			*(registers + offset + 1) = value >> 8;
			*(registers + offset) = value & 0x00FF;
		}
	}

	size_t offset;
	unsigned char memory[1 << 16];
	unsigned char registers[16];
	unsigned char * pc;
	Registers op_register;
	uint16_t operand;
	bool zero_flag;
	std::deque<uint16_t> stack;
};

int main() {
	ZXSpectrum spectrum { "hiddenmsg.prg", 0x8100 };
	spectrum.run();

	spectrum.printScreen();
}
