--- priveos-placeholder Project ---

## Purpose
The purpose of this contract is to server as a placeholder for the real, final privEOS contract on mainnet. Founder and advisor tokens will be locked into this one.

## Build Instructions
 - How to Build -
   - cd to 'build' directory
   - run the command 'cmake ..'
   - run the command 'make'

 - After build -
   - The built smart contract is under the 'priveos-placeholder' directory in the 'build' directory
   - You can then do a 'set contract' action with 'cleos' and point in to the './build/priveos-placeholder' directory

 - Additions to CMake should be done to the CMakeLists.txt in the './src' directory and not in the top level CMakeLists.txt