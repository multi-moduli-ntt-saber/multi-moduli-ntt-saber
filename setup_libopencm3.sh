
get_libopencm3 (){

git clone https://github.com/libopencm3/libopencm3
cd libopencm3
git checkout 2483e2e358f1a0d6526c63aa0b5ef2dd3358039e
make -j4
cd ../

}

cd unmasked/m3/_16_bit_strategy_A/
get_libopencm3
cd ../../../

cd unmasked/m3/_16_bit_strategy_D/
get_libopencm3
cd ../../../

cd unmasked/m3/_32_bit_strategy_A/
get_libopencm3
cd ../../../

cd unmasked/m4/strategy_A/
get_libopencm3
cd ../../../

cd unmasked/m4/strategy_D/
get_libopencm3
cd ../../../

cd masked/m4/strategy_A/
get_libopencm3
cd ../../../

cd masked/m4/strategy_C/
get_libopencm3
cd ../../../

cd masked/m4/strategy_D/
get_libopencm3
cd ../../../

