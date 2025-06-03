rm ./disassemble
rm ./out/*

clang++ -std=c++20 disassemble.cpp -o disassemble

echo "Testing 37: "
./disassemble asm_data/listing_0037_single_register_mov > ./out/0037_out.asm
diff asm_data/listing_0037_single_register_mov.asm ./out/0037_out.asm
echo $"\n"

echo "Testing 38: "
./disassemble asm_data/listing_0038_many_register_mov > ./out/0038_out.asm
diff asm_data/listing_0038_many_register_mov.asm ./out/0038_out.asm
