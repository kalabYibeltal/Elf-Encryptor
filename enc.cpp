#include <iostream>
#include <elfio/elfio.hpp>
#include <bitset>
#include <random>
#include <iomanip> 
#include <map>
// #include <algorithm>

using namespace ELFIO;

int main( )
{
    // Create elfio reader
    elfio reader;
    elfio writer;

    // Load ELF data    
    if ( !reader.load( "indirect-test.elf" ) ) {

        std::cout << "Can't find or process ELF file " << std::endl;
            return 2;
    }

     // Copy the properties of the original ELF file
    writer.create( reader.get_class(), reader.get_encoding() );
    writer.set_type( reader.get_type() );
    writer.set_machine( reader.get_machine() );
    writer.set_os_abi( reader.get_os_abi() );

    writer.set_flags( reader.get_flags() );
    writer.set_entry( reader.get_entry() );
    writer.set_os_abi( reader.get_os_abi() );
    writer.set_abi_version( reader.get_abi_version() );

    // ELFIO::section* orig_symtab = reader.sections[".symtab"];

    // // Create a new symbol section in the new ELF file
    // ELFIO::symbol_section_accessor new_symtab(writer, writer.add_section(".symtab"));

    // // Iterate over each symbol in the original symbol table
    // for (auto symbol : orig_symtab->get_symbol_table()) {
    //     // Add the symbol to the new symbol table
    //     new_symtab.add_symbol(symbol.name, symbol.value, symbol.size, symbol.bind, symbol.type, symbol.other, symbol.shndx);
    // }

    std::cout << "ELF file class : ";
    if ( reader.get_class() == ELFCLASS32 )
    std::cout << "ELF32" << std::endl;
    else
    std::cout << "ELF64" << std::endl;

    std::cout << "ELF file encoding : ";
    if ( reader.get_encoding() == ELFDATA2LSB )
    std::cout << "Little endian" << std::endl;
    else
    std::cout << "Big endian" << std::endl;

    // Print ELF file segments info
    Elf_Half seg_num = reader.segments.size();
    std::cout << "Number of segments: " << seg_num << std::endl;
    for ( int i = 0; i < seg_num; ++i ) {
        const segment* pseg = reader.segments[i];
        
        if (pseg->get_flags() & PF_X) {
            std::cout << "This is a code segment.\n";
        } else if (pseg->get_flags() & PF_R && pseg->get_flags() & PF_W) {
            std::cout << "This is a data segment.\n";
        }

        std::cout << " [" << i << "] 0x" << std::hex
        << pseg->get_flags()
        << "\t0x"
        << pseg->get_virtual_address()
        << "\t0x"
        << pseg->get_file_size()
        << "\t0x"
        << pseg->get_memory_size()
        << std::endl;
        // Access segments's data
        

        // code segment no need for it now

        // if (pseg->get_flags() & PF_X) {
        //     const char* p = reader.segments[i]->get_data();
        //     uint64_t base_address = pseg->get_virtual_address();

        //     // Random device initialization
        //     std::random_device rd;
        //     // Seed with the random device
        //     std::mt19937 gen(rd());
        //     // Define range of random values [0, UINT32_MAX]
        //     std::uniform_int_distribution<uint32_t> distrib(0, UINT32_MAX);

        //     // Generate a random 32-bit unsigned integer
        //     uint32_t k_code = distrib(gen);

        //     // Print the random number in decimal and binary form
        //     std::cout << "Random Number (binary): " << std::bitset<32>(k_code) << std::endl;
            
        //     for (Elf_Xword i = 0; i < 16; i += 4) {

        //         uint32_t instruction = 0;
        //         for (int j = 0; j < 4; ++j) {
        //             instruction |= ((uint8_t)p[i + j]) << (j * 8);
        //         }

        //         std::cout << "0x" << std::hex << (base_address + i) << ": ";
        //         std::cout  << "0x" << std::hex << instruction << " - "
        //                   << std::bitset<32>(instruction) << std::endl;
        //         std::bitset<32> binary(instruction);
               
        //     }
        // }
        // std::cout <<"\n--------------------------------------------" <<std::endl;
    }


    ///////////////////////////////////////////////////

    std::vector<std::array<uint64_t, 2>> jumpt_adds;
    std::vector<uint32_t> globcp_adds;

    

    std::map<std::string, std::vector<std::vector<uint32_t>>> addi_lui;

    Elf_Half sec_num = reader.sections.size();
    for (int i = 0; i < sec_num; ++i) {
        // section* text_sec = reader.sections[".text"];
        section* psec = reader.sections[i];
        // I want to print out each instruction in the .text section
    
        // Get the symbol table sectionf
        

        // Check if this is a symbol table section
        if (psec->get_type() == SHT_SYMTAB) {
            symbol_section_accessor symbols(reader, psec);
            // int jumpt_adds_index = 0;
        
            // Iterate over the symbols in the symbol table
            

            for (unsigned int j = 0; j < symbols.get_symbols_num(); ++j) {
                std::string   name;
                Elf64_Addr    value;
                Elf_Xword     size;
                unsigned char bind;
                unsigned char type;
                Elf_Half      section_index;
                unsigned char other;
                symbols.get_symbol(j, name, value, size, bind, type, section_index, other);

                if (name.find(".SnowflakeIoT") != std::string::npos) {
                    if(name.find("$jtab") != std::string::npos){

                        std::string number_str = name.substr(name.find_last_of('$') + 1);
                        uint32_t number = std::stoi(number_str);  

                        // std::cout << "Found symbol " << name << " at address 0x" << std::hex << value << std::endl;

                        // jumpt_adds[jumpt_adds_index][0] = value;
                        // jumpt_adds[jumpt_adds_index][1] = number;
                        

                        jumpt_adds.push_back({value, number});

                        }

                    else if(name.find("$globcp") != std::string::npos) {

                        // std::cout << "Found symbol " << name << " at address 0x" << std::hex << value << std::endl;

                        globcp_adds.push_back(value);

                    }
                   
                    else if(name.find("$luihi") != std::string::npos) {
                        std::string func_name = name.substr(name.find_last_of('$') + 1);
                        if (addi_lui.find(func_name) != addi_lui.end()) {
                            addi_lui[func_name][1].push_back(value);
                        } else {
            
                            addi_lui[func_name] = std::vector<std::vector<uint32_t>> (3, std::vector<uint32_t>());
                            addi_lui[func_name][1].push_back(value);

                        }

                    }
                    else if(name.find("$addilo") != std::string::npos) {
                        std::string func_name = name.substr(name.find_last_of('$') + 1);
                        if (addi_lui.find(func_name) != addi_lui.end()){
                            addi_lui[func_name][0].push_back(value);
                        } else {
            
                            addi_lui[func_name] = std::vector<std::vector<uint32_t>> (3, std::vector<uint32_t>());
                            addi_lui[func_name][0].push_back(value);

                        }

                    }
                }

                else if (addi_lui.find(name) != addi_lui.end()) {
                    addi_lui[name][2].push_back(value);
                }
            }

       
           
        }
            
    }
    
    
    // for (int i = 0; i < sec_num; ++i) {
        
    uint16_t k_code_ptr_16 = 0x5464;
    uint32_t k_code_ptr_32 = 0x52068860;

    section* text_sec =  reader.sections[".text"];
    std::map<uint32_t, std::vector<uint32_t>> addi_lui_enc;

    uint64_t base_address = text_sec->get_address();
    const char* p = text_sec->get_data();
    Elf_Xword size = text_sec->get_size();

    for (const auto& pair : addi_lui) {
            bool addr_overflow = false;
            uint32_t enc_func_addr  = pair.second[2][0] ^ k_code_ptr_32;
            for (const auto& val : pair.second[0]) {
                uint64_t offset = val - base_address;
                uint32_t instruction = *(uint32_t*)(p + offset);

                // Mask t he last 3 digits of enc_func_addr (12 bits in hexadecimal)
                uint32_t enc_func_addr_masked = enc_func_addr & 0xFFF;
                // Mask the last 5 digits of instruction (20 bits in hexadecimal)
                uint32_t instruction_masked = instruction & 0xFFFFF;

                // addi instruction could be negative (>=0x100)  in that case we need to add 
                // enc_func_addr_masked in 2 or 3 steps

                if (enc_func_addr_masked >= 0x800) {
                    addr_overflow = true;
                }
                // if (enc_func_addr_masked >= 0x800) {
                //     uint32_t enc_func_addr_masked_remainder = enc_func_addr_masked - 0x7FF;
                //     enc_func_addr_masked = 0x7FF;
                //     if (enc_func_addr_masked_remainder >= 0x800) {
                //         enc_func_addr_masked_remainder -= 0x1;
                //         // Shift it left to make room for the last 5 digits of instruction
                //         //the remainder can be mathimatcally proven to be 1
                //         uint32_t temp = 0x1; 
                //         uint32_t enc_instruction = (temp <<= 20) | instruction_masked;
                //         // addi_lui_enc[val] = enc_instruction;
                //         addi_lui_enc[val].push_back(enc_instruction);
                //     }
                //     // Shift it left to make room for the last 5 digits of instruction
                //     enc_func_addr_masked_remainder <<= 20;

                //     // Combine the two masked values to form enc_instruction
                //     uint32_t enc_instruction = enc_func_addr_masked_remainder | instruction_masked;
                
                //     // addi instruction 
                //     // addi_lui_enc[val] = enc_instruction;
                //     addi_lui_enc[val].push_back(enc_instruction);

                // }


                // Shift it left to make room for the last 5 digits of instruction
                enc_func_addr_masked <<= 20;

                // Combine the two masked values to form enc_instruction
                uint32_t enc_instruction = enc_func_addr_masked | instruction_masked;
               
                // addi instruction 
                // addi_lui_enc[val] = enc_instruction;
                addi_lui_enc[val].push_back(enc_instruction);
                // std::cout << "address encrypted addilo instruction " << enc_instruction << "\n";

            }

             for (const auto& val : pair.second[1]) {
                uint64_t offset = val - base_address;
                uint32_t instruction = *(uint32_t*)(p + offset);
                
                uint32_t enc_func_addr_masked = enc_func_addr & 0xFFFFF000;
                if (addr_overflow) {
                    enc_func_addr_masked += 0x1000 ; // when addi immediate is negative 
                    std::cout << enc_func_addr_masked;
                }

                uint32_t instruction_masked = instruction & 0xFFF;

                uint32_t enc_instruction = enc_func_addr_masked | instruction_masked;

                // addi_lui_enc[val] = enc_instruction;
                addi_lui_enc[val].push_back(enc_instruction);
                // std::cout << "address encrypted luihi instruction " << enc_instruction << "\n";

            }
        
            
        }

       
  // encrypt every instruction
    
    // std::vector<uint32_t> enc_instructions_vec;
    std::vector<std::pair<uint32_t, bool>> enc_instructions_vec;
    // true if instruction is compressed instruction

    uint32_t k_code_32 = 0x54640911;
    uint16_t k_code_16 = 0x5464;
    for (int i = 0; i < size;) {
        uint16_t first_half = *(uint16_t*)(p + i);
        if ((first_half & 0x3) == 0x3) {
            // This is a 32-bit instruction
            if (i + 2 < size) {


                uint32_t instruction = first_half | (*(uint16_t*)(p + i + 2) << 16);

                // std::cout << "32-bit Instruction :" ; 
                // std::cout << std::hex << std::setw(8) << std::setfill('0') << instruction << std::endl;
                // std::cout << "address :" ; 
                // std::cout << std::hex << std::setw(8) << std::setfill('0') << base_address + i << std::endl;
                
                // each enc_instruction will be written in another elf file 

                //include the addi and lui encryptions
                if (addi_lui_enc.find(base_address + i) != addi_lui_enc.end()) {
                    // instruction = addi_lui_enc[base_address + i];
                    for (const auto& enc_instr : addi_lui_enc[base_address + i]) {
                        enc_instructions_vec.push_back(std::make_pair(enc_instr, false));
                    }

                } 

                else {
                    
                    // SINCE NO CODE Encryption due to laurens students deed
                    // uint32_t enc_instruction = instruction ^ k_code_32 ^ (base_address + i ); 

                    uint32_t enc_instruction = instruction; 
                    // if (instruction == 0x00000093) {
                    //     std::cout<< (base_address + i )<<"+++++++++++++++++++++++++"<<enc_instruction << std::endl;
                    // }

                    enc_instructions_vec.push_back(std::make_pair(enc_instruction, false)); 
                    // false since the instruction is not compressed
                    // store the encrypted instructions in a vector
                    // std::cout << "encrypted instruction :" ; 
                    // std::cout << std::hex << std::setw(8) << std::setfill('0') << enc_instruction << std::endl;

                }

                i += 4;
            }
        } else {
            // std::cout << "16-bit Instruction: ";
            // std::cout << std::hex << std::setw(4) << std::setfill('0') << first_half << std::endl;
            // std::cout << "address :" ; 
            // std::cout << std::hex << std::setw(8) << std::setfill('0') << base_address + i << std::endl;

            // each enc_instruction will be written in another elf file 
            
        
            // SINCE NO CODE Encryption due to laurens students deed
            // uint32_t enc_instruction = first_half ^ k_code_16   ^ ((base_address + i) & 0xFFFF);
          
           
             uint32_t enc_instruction = first_half;
        

            enc_instructions_vec.push_back(std::make_pair(enc_instruction, true));
            // std::cout << "encrypted instruction :" ; 
            // std::cout << std::hex << std::setw(8) << std::setfill('0') << enc_instruction << std::endl;

            i += 2;
        }
        // std::cout <<"\n--------------------------------------------" <<std::endl;

    }
        


    // encrypt rodata and data sections        
    std::vector<uint32_t> enc_rodata_vec;

    section* psec =  reader.sections[".rodata"];
    if (psec->get_name() == ".rodata") {

        // uint8_t x = *(uint8_t*)(p + 4);
        // std::cout << "code_ptr :" << x << std::endl;

        int jumpt_adds_index = 0;

        const char* p = psec->get_data();
        uint64_t base_address = psec->get_address();
        Elf_Xword size = psec->get_size();

        for (int i = 0; i < size; ) {
            
            if (jumpt_adds.size() > 0 && jumpt_adds[jumpt_adds_index][0] <= (base_address + i)  && (base_address + i) < jumpt_adds[jumpt_adds_index][0] + ( jumpt_adds[jumpt_adds_index][1] * 4 ) ) {

                uint32_t code_ptr = *(uint32_t*)(p + i);
                uint32_t enc_code_ptr = code_ptr ^ k_code_ptr_32;
     
                enc_rodata_vec.push_back(enc_code_ptr);
                i += 4; 

                // //globcp + 4
                // code_ptr = *(uint32_t*)(p + i);
                // enc_code_ptr = code_ptr ^ k_code_ptr_32;
     
                // enc_rodata_vec.push_back(enc_code_ptr);
                // i += 4; 
            }
            else {
                uint32_t code_ptr = *(uint32_t*)(p + i);
                enc_rodata_vec.push_back(code_ptr);
                i += 4;

            }

            if (jumpt_adds.size() > 0 &&  jumpt_adds[jumpt_adds_index][0] + ( jumpt_adds[jumpt_adds_index][1] * 4 )  == (base_address + i) ) {
                jumpt_adds_index += 1;
             
            }


           
            }
    }

    
 
    std::vector<uint32_t> enc_data_vec;
    section* data_sec =  reader.sections[".data"];
    if (data_sec->get_name() == ".data") {


        int globcp_adds_index = 0;

        const char* p = data_sec->get_data();
        uint64_t base_address = data_sec->get_address();
        Elf_Xword size = data_sec->get_size();

        std::sort(globcp_adds.begin(), globcp_adds.end());

        for( int i = 0; i < size; ) {

            //globcp encryption
            
            if (globcp_adds.size() > 0 && globcp_adds[globcp_adds_index] == (base_address + i) ) {
                uint32_t globcp = *(uint32_t*)(p + i);

                uint32_t enc_globcp = globcp ^ k_code_ptr_32;
                // uint32_t enc_globcp = 0x52168972;
                
                // std::cout << std::hex << std::setw(8) << std::setfill('0') << enc_globcp << std::endl ;
                // std::cout << p[i];
                enc_data_vec.push_back(enc_globcp); 
            }
            else {
                enc_data_vec.push_back(*(uint32_t*)(p + i));
            }
            i += 4;
        
        }


        }
    

    // copy segments and sections to a new elf file 
    std::vector<section*> new_sections;
    for (int i = 0; i < sec_num; ++i) {
        // section* text_sec = reader.sections[".text"];
        section* sec = reader.sections[i];

        section* new_sec = writer.sections.add(sec->get_name());
        new_sec->set_type(sec->get_type());
        new_sec->set_flags(sec->get_flags());
        new_sec->set_info(sec->get_info());
        new_sec->set_link(sec->get_link());
        new_sec->set_addr_align(sec->get_addr_align());
        new_sec->set_entry_size(sec->get_entry_size());
       
        new_sections.push_back(new_sec);

        if ( sec->get_name() == ".text" ) {
  
            for (const auto& instr_data : enc_instructions_vec) {
                const char* data = reinterpret_cast<const char*>(&instr_data.first);
                if (instr_data.second) {
                    new_sec->append_data(data, 2);
                } else {
                    new_sec->append_data(data, 4);
                }
            }


        }
        else if (sec->get_name() == ".rodata") { 
            // for (const auto& ro_data : enc_rodata_vec) {
            //     const char* data = reinterpret_cast<const char*>(&ro_data);
            //     new_sec->append_data(data, 4);
            // }


            char* enc_rodata = reinterpret_cast<char*>(enc_rodata_vec.data());
            Elf_Xword size = enc_rodata_vec.size() * sizeof(uint32_t);
            new_sec->set_data(enc_rodata, size);
        }
        else if (sec->get_name() == ".data") {
            char* enc_data = reinterpret_cast<char*>(enc_data_vec.data());
            Elf_Xword size = enc_data_vec.size() * sizeof(uint32_t);
            new_sec->set_data(enc_data, size);
        }
        else {
            new_sec->set_data(sec->get_data(), sec->get_size());
        }

    }


    for ( int i = 0; i < seg_num; ++i ) {
        const segment* seg = reader.segments[i];

        segment* new_seg = writer.segments.add();
        new_seg->set_type(seg->get_type());
        new_seg->set_virtual_address(seg->get_virtual_address());
        new_seg->set_physical_address(seg->get_physical_address());
        new_seg->set_flags(seg->get_flags());
        new_seg->set_align(seg->get_align());

        // Add sections to segment based on original association
        for (std::size_t i = 0; i < reader.sections.size(); ++i) {
            section* sec = reader.sections[i];
            // A basic attempt to recreate segment sections relationship
            if (seg->get_physical_address() <= sec->get_address() &&
                (seg->get_physical_address() + seg->get_memory_size()) > sec->get_address()) {
                
                new_seg->add_section_index(new_sections[i]->get_index(), sec->get_addr_align());
            }
        }
    }

    writer.save( "encrypted.elf" );

 return 1;
}