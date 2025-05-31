#include <iostream>
#include <fstream>
#include <sstream>

enum class InstructionPattern : uint8_t {
    MOV = 0b100010
};

enum class RegFieldByte : uint8_t {
    AL = 0b000,
    CL = 0b001,
    DL = 0b010,
    BL = 0b011,
    AH = 0b100,
    CH = 0b101,
    DH = 0b110,
    BH = 0b111
};

enum class RegFieldWide : uint8_t {
    AX = 0b000,
    CX = 0b001,
    DX = 0b010,
    BX = 0b011,
    SP = 0b100,
    BP = 0b101,
    SI = 0b110,
    DI = 0b111
};

std::string get_instruction(uint8_t instruction_bits) {
    InstructionPattern detected_inst_pattern = static_cast<InstructionPattern>(instruction_bits);
    switch (detected_inst_pattern) {
        case InstructionPattern::MOV:
            return "mov";
        default:
            std::string unknown_pattern_str = "";
            for (int i = 5; i >= 0; --i) { // Iterate for 6 bits (from 5 down to 0)
                unknown_pattern_str += (((instruction_bits >> i) & 0b1) ? '1' : '0');
            }
            return "unknown: " + unknown_pattern_str;
    }
}

bool get_w(uint8_t w_bits) {
    return static_cast<bool>(w_bits);
}

bool get_d(uint8_t d_bits) {
    return static_cast<bool>(d_bits);
}

std::string get_register(bool w, uint8_t reg_bits) {
    if (!w) {
        RegFieldByte detected_reg_field = static_cast<RegFieldByte>(reg_bits);
        switch (detected_reg_field) {
            case RegFieldByte::AL:
                return "al";
            case RegFieldByte::CL:
                return "cl";
            case RegFieldByte::DL:
                return "dl";
            case RegFieldByte::BL:
                return "bl";
            case RegFieldByte::AH:
                return "ah";
            case RegFieldByte::CH:
                return "ch";
            case RegFieldByte::DH:
                return "dh";
            case RegFieldByte::BH:
                return "bh";
            default:
                std::string unknown_pattern_str = "";
                for (int i = 2; i >= 0; --i) { // Iterate for 3 bits (from 2 down to 0)
                    unknown_pattern_str += (((reg_bits >> i) & 0b1) ? '1' : '0');
                }
                return "unknown: " + unknown_pattern_str;
        }
    } else {
        RegFieldWide detected_reg_field = static_cast<RegFieldWide>(reg_bits);
        switch (detected_reg_field) {
            case RegFieldWide::AX:
                return "ax";
            case RegFieldWide::CX:
                return "cx";
            case RegFieldWide::DX:
                return "dx";
            case RegFieldWide::BX:
                return "bx";
            case RegFieldWide::SP:
                return "sp";
            case RegFieldWide::BP:
                return "bp";
            case RegFieldWide::SI:
                return "si";
            case RegFieldWide::DI:
                return "di";
            default:
                std::string unknown_pattern_str = "";
                for (int i = 2; i >= 0; --i) { // Iterate for 3 bits (from 2 down to 0)
                    unknown_pattern_str += (((reg_bits >> i) & 0b1) ? '1' : '0');
                }
                return "unknown: " + unknown_pattern_str;
        }
    }

}

std::string parse_binary_file(std::string input_filepath, std::ifstream& file_in) {
    std::ostringstream oss;
    std::string curr_instruction, operand_1, operand_2, destination, source;
    uint8_t inst_bits, w_bits, d_bits, operand_1_bits, operand_2_bits;
    char raw_byte_opcode, raw_byte_operands;
    long long curr_address = 0;
    bool w, d;
    oss << "; " << input_filepath << ":" << std::endl;
    oss << "bits 16" << std::endl;

    while (file_in.get(raw_byte_opcode)) {
        inst_bits = static_cast<uint8_t>(raw_byte_opcode) >> 2;
        curr_instruction = get_instruction(inst_bits);

        if (curr_instruction.starts_with("unknown: ")) {
            std::cerr << "Error, instruction " << curr_instruction << std::endl;
            return "";
        }
        
        if (curr_instruction == "mov") {
            d_bits = (static_cast<uint8_t>(raw_byte_opcode) >> 1) & 0b1;
            d = get_d(d_bits);
            w_bits = static_cast<uint8_t>(raw_byte_opcode) & 0b1;
            w = get_w(w_bits);
            oss << curr_instruction << " ";
            curr_address++;

            file_in.get(raw_byte_operands);
            operand_1_bits = (static_cast<uint8_t>(raw_byte_operands) >> 3) & 0b111;
            operand_2_bits = static_cast<uint8_t>(raw_byte_operands) & 0b111;

            operand_1 = get_register(w, operand_1_bits);
            if (operand_1.starts_with("unknown: ")) {
                std::cerr << "Error, instruction " << operand_1 << std::endl;
                return "";
            }
            operand_2 = get_register(w, operand_2_bits);
            if (operand_2.starts_with("unknown: ")) {
                std::cerr << "Error, instruction " << operand_2 << std::endl;
                return "";
            }
            if (d) {
                destination = operand_1;
                source = operand_2;
            } else {
                destination = operand_2;
                source = operand_1;
            }

            //Depending on the d value, these might switch

            oss << destination << ", " << source;
        }
        oss << std::endl;
    }
    return oss.str();
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << "<input_filepath>" << std::endl;
        return 1;
    }

    std::string input_filepath = argv[1];
    std::ifstream inFile(input_filepath, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open file '" << input_filepath << "'\n";
        return 1;
    }

    std::string out = parse_binary_file(input_filepath, inFile);
    std::cout << out;

    return 0;
}
