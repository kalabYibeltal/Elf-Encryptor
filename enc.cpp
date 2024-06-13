#include <iostream>
#include <elfio/elfio.hpp>
#include <bitset>
#include <random>
#include <iomanip> 

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

        std::vector<uint32_t> enc_instructions_vec;

        std::vector<section*> new_sections;

        Elf_Half sec_num = reader.sections.size();
        for (int i = 0; i < sec_num; ++i) {
            section* psec = reader.sections[i];
            // I want to print out each instruction in the .text section
            uint32_t k_code_32 = 0x54640911;
            uint16_t k_code_16 = 0x5464;
            if (psec->get_name() == ".text") {
                    const char* p = psec->get_data();
                    uint64_t base_address = psec->get_address();
                    Elf_Xword size = psec->get_size();
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
                                uint32_t enc_instruction = instruction ^ k_code_32 ^ (base_address + i ); 

                                enc_instructions_vec.push_back(enc_instruction); // store the encrypted instructions in a vector
                                // std::cout << "encrypted instruction :" ; 
                                // std::cout << std::hex << std::setw(8) << std::setfill('0') << enc_instruction << std::endl;

                                i += 4;
                            }
                        } else {
                            // std::cout << "16-bit Instruction: ";
                            // std::cout << std::hex << std::setw(4) << std::setfill('0') << first_half << std::endl;
                            // std::cout << "address :" ; 
                            // std::cout << std::hex << std::setw(8) << std::setfill('0') << base_address + i << std::endl;

                            // each enc_instruction will be written in another elf file 
                            uint32_t enc_instruction = first_half ^ k_code_16   ^ (base_address + i);
                            
                            enc_instructions_vec.push_back(enc_instruction); // store the encrypted instructions in a vector
                            // std::cout << "encrypted instruction :" ; 
                            // std::cout << std::hex << std::setw(8) << std::setfill('0') << enc_instruction << std::endl;

                            i += 2;
                        }
                        // std::cout <<"\n--------------------------------------------" <<std::endl;

                    }
                }
        
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
                    }
                }
            }
            
    }

    for (int i = 0; i < sec_num; ++i) {
        section* psec = reader.sections[i];
        uint16_t k_code_ptr_16 = 0x5810;
        uint32_t k_code_ptr_32 = 0x58105464;
        if (psec->get_name() == ".rodata") {
    
                int jumpt_adds_index = 0;

                const char* p = psec->get_data();
                uint64_t base_address = psec->get_address();
                Elf_Xword size = psec->get_size();


                // jump table encryption
                for (int i = 0; i < size; ) {
                    // std::cout << base_address + i  << std::endl;
                    if (jumpt_adds[jumpt_adds_index][0] <= (base_address + i)  && (base_address + i) < jumpt_adds[jumpt_adds_index][0] + ( jumpt_adds[jumpt_adds_index][1] * 4 ) ) {

                        uint16_t code_ptr = *(uint16_t*)(p + i);
                        // std::cout << base_address + i << "  "<<  code_ptr << std::endl ;

                        uint16_t enc_code_ptr = k_code_ptr_16 ^ code_ptr ;
                        // std::cout << "encrypted code ptr :" ; 
                        // std::cout << std::hex << std::setw(4) << std::setfill('0') << enc_code_ptr << std::endl ;
                        // std::cout << p[i];
                        i += 2; 
                    }
                    else if (jumpt_adds[jumpt_adds_index][0] + ( jumpt_adds[jumpt_adds_index][1] * 4 )  == (base_address + i) ) {
                        jumpt_adds_index += 1;
                        if (jumpt_adds_index == jumpt_adds.size()){
                            break;
                        }
                        // std::cout << base_address + i << "  " << std::endl ;
                        // std::cout << "-----------------------------------------" << std::endl ;
                        if ((base_address + i) ==  jumpt_adds[jumpt_adds_index][0]){
                                i += -2;
                        }
                    }

                    i += 2;
                }
            }

        if (psec->get_name() == ".data") {


                int globcp_adds_index = 0;

                const char* p = psec->get_data();
                uint64_t base_address = psec->get_address();
                Elf_Xword size = psec->get_size();

                
                for( int i = 0; i < size; ) {

                    //globcp encryption
                    if (globcp_adds[globcp_adds_index] == (base_address + i) ) {
                        uint32_t globcp = *(uint32_t*)(p + i);

                        uint32_t enc_globcp = globcp ^ k_code_ptr_32;
                        std::cout << "encrypted globcp :"  << enc_globcp; 
                        
                        std::cout << std::hex << std::setw(8) << std::setfill('0') << enc_globcp << std::endl ;
                        // std::cout << p[i];
                        i += 4; 
                        globcp_adds_index += 1;
                        if (globcp_adds_index == globcp_adds.size()){
                            break;
                        }

                    }
                
                }


        }
    
     }

    // Copy all sections and segments
    // for ( int i = 0; i < reader.sections.size(); ++i ) {
    //     section* orig_sec = reader.sections[i];
    //     section* new_sec = writer.sections.add( orig_sec->get_name() );
        
    //     new_sec->set_type( orig_sec->get_type() );
    //     new_sec->set_flags( orig_sec->get_flags() );
    //     new_sec->set_info( orig_sec->get_info() );
    //     new_sec->set_addr_align( orig_sec->get_addr_align() );
    //     new_sec->set_data( orig_sec->get_data(), orig_sec->get_size() );

      for (const auto& sec : reader.sections) {
        section* new_sec = writer.sections.add(sec->get_name());
        new_sec->set_type(sec->get_type());
        new_sec->set_flags(sec->get_flags());
        new_sec->set_info(sec->get_info());
        new_sec->set_link(sec->get_link());
        new_sec->set_addr_align(sec->get_addr_align());
        new_sec->set_entry_size(sec->get_entry_size());
        new_sec->set_data(sec->get_data(), sec->get_size());

        // if ( orig_sec->get_name() == ".text" ) {
        //     // Modify the .text section
        //     char* enc_instructions = reinterpret_cast<char*>(enc_instructions_vec.data());
        //     Elf_Xword size = enc_instructions_vec.size() * sizeof(uint32_t);
        //     new_sec->set_data(enc_instructions, size);
        // }
    }

//     std::vector<section*> new_sections;
//     for (auto* sec : reader.sections) {
//         section* new_sec = writer.sections.add(sec->get_name());
//         new_sec->set_type(sec->get_type());
//         new_sec->set_flags(sec->get_flags());
//         new_sec->set_info(sec->get_info());
//         new_sec->set_link(sec->get_link());
//         new_sec->set_addr_align(sec->get_addr_align());
//         new_sec->set_entry_size(sec->get_entry_size());
//         new_sec->set_data(sec->get_data(), sec->get_size());
//         new_sections.push_back(new_sec);
//     }


//    // Copy segments
//     for (auto* seg : reader.segments) {
//         segment* new_seg = writer.segments.add();
//         new_seg->set_type(seg->get_type());
//         new_seg->set_virtual_address(seg->get_virtual_address());
//         new_seg->set_physical_address(seg->get_physical_address());
//         new_seg->set_flags(seg->get_flags());
//         new_seg->set_align(seg->get_align());

//         // Add sections to segment based on original association
//         for (std::size_t i = 0; i < reader.sections.size(); ++i) {
//             section* sec = reader.sections[i];
//             // A basic attempt to recreate segment sections relationship
//             if (seg->get_physical_address() <= sec->get_address() &&
//                 (seg->get_physical_address() + seg->get_memory_size()) > sec->get_address()) {
                
//                 new_seg->add_section_index(new_sections[i]->get_index(), sec->get_addr_align());
//             }
//         }
//     }


//    for (auto* seg : reader.segments) {
//         segment* new_seg = writer.segments.add();
//         new_seg->set_type(seg->get_type());
//         new_seg->set_virtual_address(seg->get_virtual_address());
//         new_seg->set_physical_address(seg->get_physical_address());
//         new_seg->set_flags(seg->get_flags());
//         new_seg->set_align(seg->get_align());

//         for (auto index : seg->get_sections()) {
//             new_seg->add_section_index(index, reader.sections[index]->get_addr_align());
//         }
//     }

    // for ( int i = 0; i < reader.segments.size(); ++i ) {
    //     segment* orig_seg = reader.segments[i];
    //     segment* new_seg = writer.segments.add();
    //     new_seg->set_type( orig_seg->get_type() );
    //     new_seg->set_flags( orig_seg->get_flags() );
    //     new_seg->set_align( orig_seg->get_align() );

    //     // for ( section* sec : orig_seg->get_sections() ) {
    //     //     new_seg->add_section_index( sec->get_index(), sec->get_addr_align() );
    //     // }

    //    for ( Elf_Half j = 0; j < orig_seg->get_sections_num(); ++j ) {
    //         Elf_Half sec_index = orig_seg->get_section_index_at(j);
    //         section* sec = writer.sections[sec_index];
    //         new_seg->add_section_index( sec->get_index(), sec->get_addr_align() );
    //     }

    // }

    // Write the new ELF file
    writer.save( "encrypted.elf" );

 return 1;
}