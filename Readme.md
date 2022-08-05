# Turinga
Turinga is a simple crypto experiment based on the encryption in Enigma. The name is inspired by Alan Turing who was able to break the original Enigma with his colleagues.

## Motivation
The aim is gaining experience in cryptography, C++ and related software skills.

## Goals
I'm trying to achieve several aspects:
- security
- cross system compatibility (Linux/ Windows, focus on Linux)
- efficiency

## Libraries used:
library           | use case        | author               | licence can be found at:
------------------|-----------------|----------------------|-------------------------
duthomhas::CSPRNG | generating Keys | Michael Thomas Greer | lib/CSPRNG/LICENSE_1_0.txt

## Getting Turinga
To use turinga you can download binary files for linux and windows from the releases or compile from source. There are versions using `avx2`, `sse4` or none of them.

### Compiling from source on linux
You need CMake. Then you can compile Turinga by entering the following commands in the directoriy of the `CMakeLists.txt` file.
```
mkdir build && cd build  
cmake ..
make
```
By default the `-march=native` option is set.

## Usage
In order to encrypt or decrypt Turinga need rotors and keys. Both can be generated using the programm.
 purpose                                     | Linux
 --------------------------------------------|---------------------------------------------------------------------------
 get syntax help                             | ./turinga21 help 
 encrypt/ decrypt using given key and rotors | ./turinga21 crypt <input_file> <key_file> <rotors_directory> <output_file>
 generate key                                | ./turinga21 genKey <key_file> <key_length> <name_of_all_possibly_used_rotors>
 generate rotors                             | ./turinga21 genRot <rotor_names> <seed_integer>

For most of these commands there are shortcuts which replace some options by standard values. Use the help command to see them. For more help have a look in the wiki.

### On Windows
To run Turinga on windows you need to replace `./turinga21` by `turinga21.exe` in the commands listed above. Of course you need to adjust the command to the actual name of your executable or vice versa.
 
