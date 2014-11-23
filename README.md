REtypedef
=========

## What is this?
REtypedef is an IDA PRO plugin that allows defining custom substitutions for function names. It comes with a default ruleset providing substitutions for many common STL types.

## Example output
### Without REtypedef
```asm
.text:0040142E ; =============== S U B R O U T I N E =======================================
.text:0040142E
.text:0040142E ; Attributes: thunk
.text:0040142E
.text:0040142E public: void __thiscall std::basic_string<char, struct std::char_traits<char>, class std::allocator<char>>::swap(class std::basic_string<char, struct std::char_traits<char>, class std::allocator<char>> &) proc near
.text:0040142E
.text:0040142E _Right          = dword ptr  4
.text:0040142E
.text:0040142E                 jmp     std::basic_string<char,std::char_traits<char>,std::allocator<char>>::swap(std::basic_string<char,std::char_traits<char>,std::allocator<char>> &)
.text:0040142E public: void __thiscall std::basic_string<char, struct std::char_traits<char>, class std::allocator<char>>::swap(class std::basic_string<char, struct std::char_traits<char>, class std::allocator<char>> &) endp
.text:0040142E
.text:00401433
.text:00401433 ; =============== S U B R O U T I N E =======================================
.text:00401433
.text:00401433 ; Attributes: thunk
.text:00401433
.text:00401433 public: class std::basic_string<char, struct std::char_traits<char>, class std::allocator<char>> & __thiscall std::basic_string<char, struct std::char_traits<char>, class std::allocator<char>>::insert(unsigned int, class std::basic_string<char, struct std::char_traits<char>, class std::allocator<char>> const &) proc near
.text:00401433
.text:00401433 _Off            = dword ptr  4
.text:00401433 _Right          = dword ptr  8
.text:00401433
.text:00401433                 jmp     std::basic_string<char,std::char_traits<char>,std::allocator<char>>::insert(uint,std::basic_string<char,std::char_traits<char>,std::allocator<char>> const &)
.text:00401433 public: class std::basic_string<char, struct std::char_traits<char>, class std::allocator<char>> & __thiscall std::basic_string<char, struct std::char_traits<char>, class std::allocator<char>>::insert(unsigned int, class std::basic_string<char, struct std::char_traits<char>, class std::allocator<char>> const &) endp
```

### With REtypedef
```asm
.text:0040142E ; =============== S U B R O U T I N E =======================================
.text:0040142E
.text:0040142E ; Attributes: thunk
.text:0040142E
.text:0040142E public: void __thiscall std::string::swap(class std::string &) proc near
.text:0040142E
.text:0040142E _Right          = dword ptr  4
.text:0040142E
.text:0040142E                 jmp     std::string::swap(std::string &)
.text:0040142E public: void __thiscall std::string::swap(class std::string &) endp
.text:0040142E
.text:00401433
.text:00401433 ; =============== S U B R O U T I N E =======================================
.text:00401433
.text:00401433 ; Attributes: thunk
.text:00401433
.text:00401433 public: class std::string & __thiscall std::string::insert(unsigned int, class std::string const &) proc near
.text:00401433
.text:00401433 _Off            = dword ptr  4
.text:00401433 _Right          = dword ptr  8
.text:00401433
.text:00401433                 jmp     std::string::insert(uint,std::string const &)
.text:00401433 public: class std::string & __thiscall std::string::insert(unsigned int, class std::string const &) endp
```

## Binary distribution
[Download latest binary version from github.](https://github.com/athre0z/REtypedef/releases/latest) Currently only the Windows version of IDA is supported.

## Installation
Place `REtypedef.plX` into the `plugins` directory of your IDA installation.