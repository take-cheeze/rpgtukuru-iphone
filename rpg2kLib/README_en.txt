** About rpg2kLib **

by take_cheeze( takechi101010 ＠ gmail.com )


- Content
1. First
2. Words
3. Frame
4. TODO
5. Library

1. First
"rpg2kLib" is a library for using the data structure of RPG Maker 2000/2003.
I'm not a good programmer so some of the code is wrong some of the way.

2. Words
- BER Number =
	Effective for many small numbers.
	It isn't designed for signed numbers but can use it by casting to int32_t.
- LDB = LcfDataBase
- LMT = LcfMapTree
- LMU = LcfMapUnit
- LSD = LcfSaveData

3. Frame

Namespaces
- rpg2k     - ROOT. All the unnamespaced things belongs here.
-- debug     - Classes for debugging.
-- define    - Defines of LDB/LMT/LMU/LSD/etc.
-- font      - Font library. No longer using.
-- gamemode  - Game Mode things. Not going to use it directly, but useful codes.
-- input     - Input library. No longer using.
-- model     - LDB/LMT/LMU/LSD and Project class.
-- structure - Libraries for data structure. Event/Array/Stream etc.

String
- Has two types under rpg2k.
- Both of them inherits std::string and has a add of member function.
-- RPG2kString  - Shift_JIS string. You can use toSystem() to convert to SystemString.
-- SystemString - Systems character encoded string. Use toRPG2k() to convert to RPG2kString.
- It is using iconv via rpg2k::Encode class.

Data Type
(Except basic types they are under rpg2k::structure)
(Element class holds either type)
(Descriptor class holds data type and default value)
- Array1D    - One dimentional array. Associative array of Element.
- Array2D    - Two dimentional array. Associative array of Array1D.
- Music      - Play information of BGM. (Just a wrap of Array1D)
- Sound      - Play information of SE. (Just a wrap of Array1D)
- EventState - Information f Event. (Just a wrap of Array1D)
- Event      - Event data. Arra of event command. (= Instruction)
- BerEnum    - Enumration of BER numbers in LMT.
- Binary     - Inherit of std::vector<uint8_t>. You can hold any data.
- int        - Integer type.
- bool       - Boolean type.
- string     - String type. "typedef" of RPG2kString.
- double     - Floating point.

Stream
- Writing stream "StreamWriter" and reading stream "StreamReader".
- Both implemented by pimpl.
- You can chose Binary class and stdio.
- Only advantage is that you can use BER number and suitbale for LDB/LMT/LMU/LSD.

Analzing
- If you DEBUG build it it will be done in rpg2k::structure::Element's deconstructor.
- If you a skipping big data to load fast the analyze won't be correct.
- The result will be outputed to "analyze.txt".
- Array1D/Array2D won't be judged. (It's better to see it yourself)
- The analyze will use rpg2k::debug.

Project
- Class that manages game data(LDB/LMT/LMU/LSD).
- Save data(from 1 - 15) will be loaded at constructor. (only if it exists)

4. TODO
- Search for "// TODO".
- Broken LMTs.
- Configuration file with Boost.Serialization.

5. Library

OpenGL - rendering
iconv  - converting character encoding
boost  - smart pointer

I'm not going to use following libraries 
SDL       - sleep, timer, input, drawing
SDL_mixer - playing BGM/SE
SOIL      - (= Simple OpenGL Image Library) loading image, creating texture
